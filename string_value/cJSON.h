#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * cJSON Types
 * Ulike cJSON, it uses an enum type.
*/
enum{cJSON_False, cJSON_True, cJSON_NULL, cJSON_Number, cJSON_String, cJSON_Array, cJSON_Object};

#define cJSON_IsReference 256

/* The cJSON structure: */
typedef struct cJSON{
    int type; /* The type of the item, as above. */
    char *valuestring;			/* The item's string, if type==cJSON_String */
}cJSON;

/* Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished. */
extern cJSON *cJSON_Parse(const char *value);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
extern cJSON *cJSON_ParseWithOpts(const char *value,const char **return_parse_end,int require_null_terminated);
/* Delete a cJSON entity and all sub-entities */
extern void cJSON_Delete(cJSON* c);
/* For analysing failed parses. This returns a pointer to the parse error. */
extern const char* cJSON_GetErrorPtr(void);
/* Render a cJSON entity to text for  transfer/storage without any formatting. Free the char* when finished. */
extern char* cJSON_Print(cJSON* item);

#ifdef __cplusplus
}
#endif

#endif