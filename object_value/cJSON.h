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
#define cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct cJSON{
    struct cJSON *next, *prev; /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *child; /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    
    int type; /* The type of the item, as above. */

    char *valuestring;			/* The item's string, if type==cJSON_String */
    int valueint;				/* The item's number, if type==cJSON_Number */
	double valuedouble;			/* The item's number, if type==cJSON_Number */

    char* string; /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
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

/* Returns the number of items in an array (or object). */
extern int cJSON_GetArraySize(cJSON* array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern cJSON* cJSON_GetArrayItem(cJSON* array, int item);
/* Get item "string" from object. Case insensitive. */
extern cJSON* cJSON_GetObjectItem(cJSON* object, const char* string);

#ifdef __cplusplus
}
#endif

#endif