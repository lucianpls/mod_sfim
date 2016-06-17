/*
* An OnEarth module, sends a static file if the request matches a regexp
* Lucian Plesea
* (C) 2016
*/

#include <apr.h>
#include <httpd.h>
#include <http_config.h>
#include <http_log.h>
#include <http_protocol.h>
#include <apr_tables.h>
#include <apr_strings.h>
#include <ap_regex.h>

#include "mod_sfim.h"

// Returns 1 if string s ends with the ending string
static int ends_with(const char *ending, const char *s) {
    int ls = strlen(s);
    int le = strlen(ending);
    if (le > ls) return 0;
    return NULL != strstr(s + ls - le, ending);
}

static int send_the_file(request_rec *r, const char *filename, const char *type)
{
    static const char *fmt = "%s: %m";
    apr_status_t stat;
    apr_finfo_t info;
    stat = apr_stat(&info, filename, APR_FINFO_SIZE, r->pool);
    if (APR_SUCCESS != stat) {
        ap_log_error(APLOG_MARK, APLOG_ERR, 0, r->server, fmt, filename, stat);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (info.size > MAX_FILE_SIZE) {
        ap_log_error(APLOG_MARK, APLOG_ERR, 0, r->server, "File %s too large, max is %d", filename, MAX_FILE_SIZE);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    void *buffer = apr_palloc(r->pool, info.size);

    if (!buffer) { // Good luck with this one
        ap_log_error(APLOG_MARK, APLOG_CRIT, 0, r->server, "Memory allocation failure");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    apr_file_t *f;
    stat = apr_file_open(&f, filename, APR_FOPEN_READ | APR_FOPEN_BINARY, 0, r->pool);
    if (APR_SUCCESS != stat) {
        ap_log_error(APLOG_MARK, APLOG_ERR, 0, r->server, fmt, filename, stat);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    apr_size_t read_bytes = info.size;
    stat = apr_file_read(f, buffer, &read_bytes);
    if (APR_SUCCESS != stat) {
        ap_log_error(APLOG_MARK, APLOG_ERR, 0, r->server, fmt, filename, stat);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // Got the content, send it
    ap_set_content_type(r, type);
    ap_rwrite(buffer, read_bytes, r);

    return OK; // Done
}
 

static int handler(request_rec *r)
{
    int i;
    if (r->method_number != M_GET) return DECLINED; // Only GET requests
    sfim_conf *cfg = (sfim_conf *)ap_get_module_config(r->per_dir_config, &sfim_module);
    if (!cfg || cfg->matches == NULL) return DECLINED;
    for (i = 0; i < cfg->matches->nelts; i++) {
        match m = APR_ARRAY_IDX(cfg->matches, i, match);
        // Returns 0 for a match
        if (ap_regexec(m.regexp, r->uri, 0, NULL, 0))
            continue;
        // It did match
        return send_the_file(r, m.filename, m.type);
    }
    return DECLINED;
}

// One directive per call, per folder
static const char *entry_set(cmd_parms *cmd, void *dconf, const char *filename, const char *regexp, const char *type)
{
    sfim_conf *cfg = (sfim_conf *)dconf;
    if (cfg->matches == NULL) // Initialize the table if not already done
        cfg->matches = apr_array_make(cmd->pool, 2, sizeof(match));

    // compile the regexp first
    match *m = (match *)apr_array_push(cfg->matches);

    m->regexp = apr_palloc(cmd->pool, sizeof(ap_regex_t));
    int error = ap_regcomp(m->regexp, regexp, 0);
    if (error) {
        int msize = 2048;
        char *message = apr_palloc(cmd->temp_pool, msize);
        ap_regerror(error, m->regexp, message, msize);
        return message;
    }
    m->filename = apr_pstrdup(cmd->pool, filename);
    m->type = apr_pstrdup(cmd->pool, type);

    return NULL;
}

static void *create_dir_config(apr_pool_t *p, char *dummy)
{
    sfim_conf *c =
        (sfim_conf *)apr_pcalloc(p, sizeof(sfim_conf));
    return c;
}

static void register_hooks(apr_pool_t *p)

{
    ap_hook_handler(handler, NULL, NULL, APR_HOOK_MIDDLE);
}

static const command_rec cmds[] = 
{
    AP_INIT_TAKE3(
        "SendFileIfMatch",
        entry_set, // Callback
        0, // Self pass argument
        ACCESS_CONF,
        "The file to send and the regexp to match against the URI"
    ),
    {NULL}
};

module AP_MODULE_DECLARE_DATA sfim_module = {
    STANDARD20_MODULE_STUFF,
    create_dir_config,
    0, // No dir_merge, no inheritance
    0, // No server_config
    0, // No server_merge
    cmds, // configuration directives
    register_hooks // processing hooks
};
