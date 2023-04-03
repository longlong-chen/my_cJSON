#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include <float.h>
#include <math.h>
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

static int pow2gt (int x)	{	--x;	x|=x>>1;	x|=x>>2;	x|=x>>4;	x|=x>>8;	x|=x>>16;	return x+1;	}

/* ??? */
typedef struct {char *buffer; int length; int offset; } printbuffer;

static char* ensure(printbuffer *p,int needed)
{
	char *newbuffer;int newsize;
	if (!p || !p->buffer) return 0;
	needed+=p->offset;
	if (needed<=p->length) return p->buffer+p->offset;

	newsize=pow2gt(needed);
	newbuffer=(char*)cJSON_malloc(newsize);
	if (!newbuffer) {cJSON_free(p->buffer);p->length=0,p->buffer=0;return 0;}
	if (newbuffer) memcpy(newbuffer,p->buffer,p->length);
	cJSON_free(p->buffer);
	p->length=newsize;
	p->buffer=newbuffer;
	return newbuffer+p->offset;
}

static int update(printbuffer* p) {
    char* str;
    if(!p || !p->buffer) return 0;
    str = p->buffer + p->offset;
    return p->offset + strlen(str);
}

/* Delete a cJSON structure. */
void cJSON_Delete(cJSON* c) {
    cJSON* next;
    while(c) {
        next = c->next;
        if (!(c->type&cJSON_IsReference) && c->child) cJSON_Delete(c->child);
        if (!(c->type&cJSON_IsReference) && c->valuestring) cJSON_free(c->valuestring);
        cJSON_free(c);
        c = next;
    }
}

/* Parse the input text to generate a number, and populate the result into item. */
static const char* parse_number(cJSON* item, const char* num) {
    double n = 0, sign = 1, scale = 0; int subscale = 0, signsubscale = 1;
    if(*num == '-') sign = -1, num++; /* Has sign? */
    if(*num == '0') num++;            /* is zero */
    if (*num >= '1' && *num <= '9')	do	n= (n * 10.0) + (*num++ - '0');	while (*num >= '0' && *num <= '9');	/* Number? */
    if (*num == '.' && num[1] >= '0' && num[1] <= '9') {num++;		do	n = (n * 10.0) + (*num++ -'0'), scale--; while (*num >= '0' && *num <= '9');}	/* Fractional part? */
	if (*num == 'e' || *num == 'E')		/* Exponent? */
	{	num++;if (*num == '+') num++;	else if (*num == '-') signsubscale = -1, num++;		/* With sign? */
		while (*num >= '0' && *num <= '9') subscale = (subscale * 10)+(*num++ - '0');	/* Number? */
	}

	n = sign * n * pow(10.0, (scale + subscale * signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */
	
	item->valuedouble = n;
	item->valueint = (int)n;
	item->type = cJSON_Number;
	return num;
}

/* Render the number nicely from the given item into a string. */
static char *print_number(cJSON *item, printbuffer *p) {
    char *str = 0;
    double d = item->valuedouble;
    if(d == 0) {
        if(p) str = ensure(p, 2);
        else str = (char*)cJSON_malloc(2); /* special case for 0. */
        if(str) strcpy(str, "0");
    }
    else if(fabs(((double)item->valueint) - d) <= DBL_EPSILON && d<=INT_MAX && d>=INT_MIN) {
        if(p) str = ensure(p, 21);
        else  str=(char*)cJSON_malloc(21);	/* 2^64+1 can be represented in 21 chars. */
        if(str) sprintf(str,"%d",item->valueint);
    }
    else {
        if(p) str = ensure(p, 64);
        else  str = (char*)cJSON_malloc(64); /* This is a nice tradeoff. */
        if(str) {
            if(fabs(floor(d) - d) <= DBL_EPSILON && fabs(d) < 1.0e60) sprintf(str, "%.0f", d);
			else if(fabs(d) < 1.0e-6 || fabs(d) > 1.0e9)			  sprintf(str, "%e", d);
			else												      sprintf(str, "%f", d);
        }
    }
    return str;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char* print_string_ptr(const char* str, printbuffer* p) {
    const char *ptr;char *ptr2,*out;int len=0,flag=0;unsigned char token;
    /* Check whether there are any special characters such as Spaces and transition */
    for (ptr=str;*ptr;ptr++) flag|=((*ptr>0 && *ptr<32)||(*ptr=='\"')||(*ptr=='\\'))?1:0;
    /* flag = false, directly copy result*/
	if (!flag) {
		len=ptr-str;
		if (p) out=ensure(p,len+3);
		else		out=(char*)cJSON_malloc(len+3);
		if (!out) return 0;
		ptr2=out;*ptr2++='\"'; /* the output is like "xxxxx" */
		strcpy(ptr2,str);
		ptr2[len]='\"';
		ptr2[len+1]=0;
		return out;
	}

    /* to solve: "" */
    if (!str) {
		if (p)	out=ensure(p,3);
		else	out=(char*)cJSON_malloc(3);
		if (!out) return 0;
		strcpy(out,"\"\"");
		return out;
	}
    /* from here, ==> flag = true */
    /* "\u0024" ==> $ */
    ptr=str;while ((token=*ptr) && ++len) {if (strchr("\"\\\b\f\n\r\t",token)) len++; else if (token<32) len+=5;ptr++;}

    if (p)	out=ensure(p,len+3);
	else	out=(char*)cJSON_malloc(len+3);
	if (!out) return 0;

    ptr2=out;ptr=str;
    *ptr2++='\"';
    while(*ptr) {
        if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') *ptr2++=*ptr++;
		else {
			*ptr2++='\\';
			switch (token=*ptr++) {
				case '\\':	*ptr2++='\\';	break;
				case '\"':	*ptr2++='\"';	break;
				case '\b':	*ptr2++='b';	break;
				case '\f':	*ptr2++='f';	break;
				case '\n':	*ptr2++='n';	break;
				case '\r':	*ptr2++='r';	break;
				case '\t':	*ptr2++='t';	break;
				default: sprintf(ptr2,"u%04x",token);ptr2+=5;	break;	/* escape and print */
			}
		}
    }
    *ptr2++='\"';*ptr2++=0;
	return out;
}

/* Invote print_string_ptr (which is useful) on an item. */
static char* print_string(cJSON* item, printbuffer* p) {return print_string_ptr(item->valuestring, p);}

/* Predeclare these prototypes */
static const char* parse_value(cJSON* item, const char* value);
static char* print_value(cJSON* item, int depth, int fmt, printbuffer* p);
static const char* parse_array(cJSON* item, const char* value);
static char* print_array(cJSON* item, int depth, int fmt, printbuffer* p);

/* Utility to jump whitespace and cr/lf */
static const char* skip(const char* in) {
    while(in && *in && (unsigned char)*in <= 32)    in++;
    return in;
}

static unsigned parse_hex4(const char *str)
{
	unsigned h=0;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	h=h<<4;str++;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	h=h<<4;str++;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	h=h<<4;str++;
	if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
	return h;
}

/* Parse the input text into an unescaped cstring, and populate them. */
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC }; /* utf-8 codepoint table */
static const char* parse_string(cJSON* item, const char* str) {
    const char* ptr = str + 1;
    char* ptr2;
    char* out;
    int len = 0;
    unsigned uc, uc2;
    if(*str != '\"') {
        ep = str;
        return 0; /* not a string! */
    }
    /* Skip escaped quotes. */
    while(*ptr != '\"' && *ptr && ++len) {
        if(*ptr++ == '\\') {
            ptr++;
        }
    }
    /* This is how long we need for the string, roughly. */
    out=(char*)cJSON_malloc(len+1);
    if(!out)    return 0;
    ptr = str + 1;
    ptr2 = out;
    while(*ptr != '\"' && *ptr) {
        if(*ptr != '\\') *ptr2++ = *ptr++;
        else {
            ptr++; /* after'\' */
            switch (*ptr) {
                case 'b': *ptr2++ = '\b'; break;
                case 'f': *ptr2++ = '\f'; break;
                case 'n': *ptr2++ = '\n'; break;
                case 'r': *ptr2++ = '\r'; break;
                case 't': *ptr2++ = '\t'; break;
                case 'u': /* transcode utf16 to utf8. */
                    uc = parse_hex4(ptr + 1);   /* get the unicode char. */
                    ptr += 4;
                    if((uc>=0xDC00 && uc<=0xDFFF) || uc==0) break; /* low surrogate, check for invalid. */
                    /* UTF16 surrogate pairs, high surrogate */
                    if(uc>=0xD800 && uc<=0xDBFF) {
                        if(ptr[1] != '\\' || ptr[2] != 'u') break; /* missing second-half of surrogate */
                        uc2 = parse_hex4(ptr + 3);
                        ptr += 6;
                        if (uc2<0xDC00 || uc2>0xDFFF)		break;	/* invalid second-half of surrogate. */
                        /**
                         * the function surrogate pair to codepoint:
                         * codepoint = 0x10000 + (H - 0xD800) x 0x400 + (L - 0xDC00)
                        */
                        uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
                    }
                    len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;
                    switch (len) {
						case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 1: *--ptr2 =(uc | firstByteMark[len]);
					}
                    ptr2+=len;
					break;
                default: *ptr2++=*ptr; break;
            }
            ptr++;
        }
    }
    *ptr2 = 0;
    if(*ptr == '\"') ptr++;
    item->valuestring = out;
    item->type = cJSON_String;
    return ptr;
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
    if(*value == '\"') {
        return parse_string(item, value);
    }
    if(*value == '-' || (*value >= '0' && *value <= '9')) {
        return parse_number(item, value);
    }
    if(*value == '[') {
        return parse_array(item, value);
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
    return print_value(item, 0, 1, 0);
}

static char* print_value(cJSON* item, int depth, int fmt, printbuffer* p) {
    char* out = 0;
    if(!item)   return 0;
    if(p) {
        switch ((item->type)&255) {
            case cJSON_NULL:	{out = ensure(p,5);	if (out) strcpy(out,"null");	break;}
			case cJSON_False:	{out = ensure(p,6);	if (out) strcpy(out,"false");	break;}
			case cJSON_True:	{out = ensure(p,5);	if (out) strcpy(out,"true");	break;}
            case cJSON_Number:	out = print_number(item,p); break;
            case cJSON_String:	out = print_string(item,p); break;
            case cJSON_Array:   out = print_array(item, depth, fmt, p); break;
        }
    } else {
        switch ((item->type) & 255) {
            case cJSON_NULL:    out = cJSON_strdup("null");  break;
            case cJSON_False:   out = cJSON_strdup("false"); break;
            case cJSON_True:    out = cJSON_strdup("true");  break;
            case cJSON_Number:	out = print_number(item,0);  break;
            case cJSON_String:  out = print_string(item, 0); break;
            case cJSON_Array:   out = print_array(item, depth, fmt, 0); break;
        }
    }
    return out;
}

/* Bulild an array from input text. */
static const char* parse_array(cJSON* item, const char* value) {
    cJSON* child;

    /* not an array */
    if(*value != '[') {
        ep = value;
        return 0;
    }

    item->type = cJSON_Array;
    value = skip(value + 1);
    /* empty array. */
    if(*value == ']') {
        return value + 1;
    }

    item->child = child = cJSON_New_Item();
    if(!item->child) return 0; /* memory fail */
    value = skip(parse_value(child, skip(value))); /* skip any spacing, get the value. */
    if(!value) return 0;

    /* recursive call */
    while(*value == ',') {
        cJSON* new_item;
        if(!(new_item = cJSON_New_Item())) return 0; /* memory fail */
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip(parse_value(child, skip(value + 1)));
        if(!value) return 0;
    }

    if(*value == ']') return value + 1; /* end of array */
    ep = value;
    return 0;
}

/* Render an array to text */
static char* print_array(cJSON* item, int depth, int fmt, printbuffer* p) {
    char** entries;
    char *out = 0, *ptr, *ret;
    int len = 5;
    cJSON* child = item->child;
    int numentries = 0, i = 0, fail = 0;
    size_t tmplen = 0;

    /* How many entries in the array? */
    while(child) numentries++, child = child->next;
    /* Explicitly handle numentries == 0 */
    if(!numentries) {
        if(p) out = ensure(p, 3);
        else  out = (char*)cJSON_malloc(3);
        if(out) strcpy(out, "[]");
        return out;
    }

    if(p) {
        /* Compose the output array. */
        i = p->offset;
        ptr = ensure(p, 1);
        if(!ptr) return 0;
        *ptr = '[';
        p->offset++;
        child = item->child;
        while(child && !fail) {
            print_value(child, depth + 1, fmt, p);
            p->offset = update(p);
            if(child->next) {
                len = fmt ? 2 : 1;
                ptr = ensure(p, len + 1);
                if(!ptr) {
                    return 0;
                }
                *ptr++=',';
                if(fmt) *ptr++=' ';
                *ptr = 0;
                p->offset += len;    
            }
            child = child->next;
        }
        ptr = ensure(p, 2);
        if(!ptr) return 0;
        *ptr++=']';
        *ptr = 0;
        out = (p->buffer) + i;
    } else {
        /* Allocate an array to hold the values for each */
        entries = (char**)cJSON_malloc(numentries * sizeof(char*));
        if(!entries) return 0;
        memset(entries, 0, numentries * sizeof(char*));
        /**
         * void* memset(void* ptr, int value, size_t num);
         * Describe: Fill block of memory. Sets the first 'num' bytes of the block of memory pointed by 'ptr' to the specified 'value'(interpreted as an unsigned char).
         * Parameters:
         * ptr: Pointer to the block of memory to fill.
         * value: Value to be set. The value is passed as an int, but the function fills the block of memory using the 'unsigned char' conversion of this 'value'.
         * num: Number of bytes to be set to the 'value'. size_t is an unsigned integral type.
         * Return Value: 'ptr' is returned.
        */
        child = item->child;
        while(child && !fail) {
            ret = print_value(child, depth + 1, fmt, 0);
            entries[i++] = ret;
            if(ret) len += strlen(ret) + 2 + (fmt ? 1 : 0);
            else fail = 1;
            child = child->next;
        }

        /* If we didn't fail, try to malloc the output string */
        if(!fail) out = (char*)cJSON_malloc(len);
        /* If that fails, we fail. */
        if(!out) fail = 1;

        /* Handle failure. */
        if(fail) {
            for(i = 0; i < numentries; i++) {
                if(entries[i]) {
                    cJSON_free(entries[i]);
                }
            }
            cJSON_free(entries);
            return 0;
        }

        /* Compose the output array. */
        *out = '[';
        ptr = out + 1;
        *ptr = 0;
        for(i = 0; i < numentries; i++) {
            tmplen = strlen(entries[i]);
            memcpy(ptr, entries[i], tmplen);
            ptr += tmplen;
            if(i != numentries - 1) {
                *ptr++=',';
                if(fmt) *ptr++ = ' ';
                *ptr = 0;
            }
            cJSON_free(entries[i]);
        }
        cJSON_free(entries);
        *ptr++=']';
        *ptr++=0;
    }
    return out;
}

/* Get Array size/item / object item. */
int cJSON_GetArraySize(cJSON* array) {
    cJSON* c = array->child;
    int i = 0;
    while(c) i++, c = c->next;
    return i;
}

cJSON* cJSON_GetArrayItem(cJSON* array, int item) {
    cJSON* c = array->child;
    while(c && item > 0) item--, c = c->next;
    return c;
}

/**
 * bug record (null_logic_value)
 * the '*out = 0', strcpy() cannot be used directly
 * that's why cJSON_strdup() is used;
 * char* strcpy(char* destination, const char* source);
 * destination: Pointer to the destination array where the content is to be copied.
 * source: C string to be copied.
 * ps: to avoid overflows, the size of the array pointed by destination shall be long enough to contain the same C string as source (including the terminating null character), and should not overlap in memory with source.
*/