/*
* An OnEarth module, sends a static file if the request matches a regexp
* Lucian Plesea
* (C) 2016
*/

#include "mod_sfim.h"
#include <httpd.h>
#include <http_config.h>

static int handler(request_rec *r)
{
    return DECLINED;
}

static void create_dir_config(apr_pool_t *p, char *dummy)
{

}

static void register_hooks(apr_pool_t *p)

{
    ap_hook_handler(handler, NULL, NULL, APR_HOOK_MIDDLE);
}

static const command_rec cmds[] = 
{
    {NULL}
};

module AP_MODULE_DECLARE_DATA sfim_module = {
    STANDARD20_MODULE_STUFF,
    create_dir_config,
    0, // No dir_merge
    0, // No server_config
    0, // No server_merge
    cmds, // configuration directives
    register_hooks // processing hooks
};