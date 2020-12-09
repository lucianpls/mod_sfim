# mod_sfim [AHTSE](https://github.com/lucianpls/AHTSE)
An apache httpd module that sends back a static file for web requests that match a regexp.
Adds one directive to the apache configuration file.  

# Building
Requires apache httpd and libapr to be available for linking and at runtime.
## SendFileIfMatch file_name regexp mime-type

The file_name file will be sent as the response, with mime-type as the type, if the request matches the regexp.
JSON-P is supported if the type is application/jsonp, in which case the response type will be application/json.
If parameters are present in the request, the regexp will also take them into consideration.
There can be multiple such directives in a given Location or Directory container, the first match will generate the response.
