#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "vkext_flex.h"
#include <stdio.h>
 
#define PHP_MY_EXTENSION_VERSION "1.0"
#define PHP_MY_EXTENSION_EXTNAME "vkflex"
 
extern zend_module_entry vkflex_module_entry;
#define phpext_my_extension_ptr &vkflex_entry
 
// declaration of a custom my_function()
PHP_FUNCTION(vkflex);
 
// list of custom PHP functions provided by this extension
// set {NULL, NULL, NULL} as the last record to mark the end of list
static zend_function_entry my_functions[] = {
    PHP_FE(vkflex, NULL)
    {NULL, NULL, NULL}
};
 
// the following code creates an entry for the module and registers it with Zend.
zend_module_entry vkflex_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_MY_EXTENSION_EXTNAME,
    my_functions,
    NULL, // name of the MINIT function or NULL if not applicable
    NULL, // name of the MSHUTDOWN function or NULL if not applicable
    NULL, // name of the RINIT function or NULL if not applicable
    NULL, // name of the RSHUTDOWN function or NULL if not applicable
    NULL, // name of the MINFO function or NULL if not applicable
#if ZEND_MODULE_API_NO >= 20010901
    PHP_MY_EXTENSION_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
 
ZEND_GET_MODULE(vkflex)
 
// implementation of a custom my_function()
PHP_FUNCTION(vkflex)
{
    char *name, *case_;
    long name_len, case_len;
    long sex, lang, type;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslll", 
            &name, &name_len, &case_, &case_len, &sex, &lang, &type) == FAILURE) {
        RETURN_NULL();
    }
    
    char *result = do_flex(name, name_len, case_, case_len,
        (int)sex, (type == 0 ? "names" : "surnames"), (int)lang);

    RETURN_STRING(result);
    efree(result);
}
