#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "cJSON.h"

/* The cur location after parsing or operating (error); team up with cJSON_Delete() */
static const char* ep;
const char* cJSON_GetErrorPtr(void) {
    return ep;
}

/* Function pointer */
static void *(*cJSON_malloc)(size_t sz) = malloc;
static void (*cJSON_free)(void *ptr) = free;

/* Internal constructor. */
static cJSON* cJSON_New_Item(void) {
    cJSON* node = (cJSON*)cJSON_malloc(sizeof(cJSON));
    if(node)    memset(node, 0, sizeof(cJSON));
    return node;
}

/* Delete a cJSON structure. */
void cJSON_Delete(cJSON* c) {
    /* only support null / false / true */
    cJSON_free(c);
}

/* Predeclare these prototypes */
static const char* parse_value(cJSON* item, const char* value);
static char* print_value(cJSON* item);

/* Utility to jump whitespace and cr/lf */
static const char* skip(const char* in) {
    while(in && *in && (unsigned char)*in <= 32)    in++;
    return in;
}

/* Parse an object - create a new root, and populate. */
cJSON *cJSON_ParseWithOpts(const char* value, const char** return_parse_end, int require_null_terminated) {
    const char* end = 0;
    cJSON* c = cJSON_New_Item();
    ep = 0;
    if(!c)  return 0;   /* memory fail */

    end = parse_value(c, skip(value));
    /* parse failure, ep is set. */
    if(!end) {
        cJSON_Delete(c);
        return 0;
    }
    /* parse success, but may has appended garbage */
    /* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
    if(require_null_terminated) {
        end = skip(end);
        if(*end) {
            cJSON_Delete(c);
            ep = end;
            return 0;
        }
    }
    if(return_parse_end)    *return_parse_end = end;
    return c;
}

/* Default options for cJSON_Parse */
cJSON *cJSON_Parse(const char *value) {return cJSON_ParseWithOpts(value,0,1);}

/**
 * Parse core - when encountering text, process appropriately.
 * return the cur location after parse 
*/

static const char* parse_value(cJSON* item, const char* value) {
    if(!value)  return 0; /* Fail on null. */
    /**
     * int strncmp(const char* str1, const char* str2, size_t n)
     * str1: the first string to be compared.
     * str2: the second string to be compared.
     * n:    the maximun number of characters to compare.
     * return < 0: str1 < str2
     * return > 0: str1 > str2
     * return = 0: str1 = str2
    */
    if(!strncmp(value, "null", 4))  {
        item->type = cJSON_NULL;
        return value + 4;
    }
    if(!strncmp(value, "false", 5))  {
        item->type = cJSON_False;
        return value + 5;
    }
    if(!strncmp(value, "true", 4))  {
        item->type = cJSON_True;
        return value + 4;
    }
    ep = value; /* the cur location after parsing. */
    return 0; /* failure (including error and others type) */
}

static char* cJSON_strdup(const char* str) {
    size_t len;
    char* copy;

    len = strlen(str) + 1;
    if(!(copy = (char*)cJSON_malloc(len)))  return 0;
    memcpy(copy, str, len);
    return copy;
}

char* cJSON_Print(cJSON* item) {
    return print_value(item);
}

static char* print_value(cJSON* item) {
    char* out = 0;
    if(!item)   return 0;
    switch ((item->type) & 255) {
        case cJSON_NULL:    out = cJSON_strdup("null");  break;
        case cJSON_False:   out = cJSON_strdup("false");  break;
        case cJSON_True:    out = cJSON_strdup("true");  break;
    }
    return out;
}

/**
 * bug record
 * the '*out = 0', strcpy() cannot be used directly
 * that's why cJSON_strdup() is used;
 * char* strcpy(char* destination, const char* source);
 * destination: Pointer to the destination array where the content is to be copied.
 * source: C string to be copied.
 * ps: to avoid overflows, the size of the array pointed by destination shall be long enough to contain the same C string as source (including the terminating null character), and should not overlap in memory with source.
*/