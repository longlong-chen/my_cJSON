#include<stdio.h>
#include <stdlib.h>
#include "cJSON.h"

/* Parse text to JSON, then render back to text, and print! */
void doit(char* text) {
    char* out;
    cJSON* json;
    json = cJSON_Parse(text);
    if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
    else {
        out = cJSON_Print(json);
        /* Test Array size and item */
        if(json->type == cJSON_Array) {
            size_t i = 0, size = 0;
            size = cJSON_GetArraySize(json);
            printf("The array's size is %zu\n", size);
            for(i = 0; i < size; i++) {
                printf("item %zu: ", i + 1);
                printf("%s\n", cJSON_Print(cJSON_GetArrayItem(json, i)));
            }
        }
        if(json->type == cJSON_Object) {
            size_t i = 0, size = 0;
            size = cJSON_GetArraySize(json);
            printf("The array's size is %zu\n", size);
            cJSON* node = node = json->child;
            for(i = 0; i < size; i++) {
                printf("item %zu\n", i + 1);
                printf("%s:", node->string);
                printf("%s\n", cJSON_Print(cJSON_GetObjectItem(json, node->string)));
                node = node->next;
            }
        }
        cJSON_Delete(json);
        printf("%s\n", out);
        free(out);
    }
}

/* Test enum Type */
static void test_enum_type() {
    printf("cJSON_False = %d\n", cJSON_False);
    printf("cJSON_Number = %d\n", cJSON_Number);
    printf("cJSON_Object = %d\n", cJSON_Object);
}

/* Test null / false / true */
static void test_null_false_true_type() {
    char test_null_1[] = "null";
    char test_null_2[] = "    null    ";
    char test_null_3[] = "nul";
    char test_null_4[] = "null??";
    doit(test_null_1);
    doit(test_null_2);
    doit(test_null_3);
    doit(test_null_4);
    char test_false_1[] = "false";
    char test_false_2[] = "    false    ";
    char test_false_3[] = "fals";
    char test_false_4[] = " false??";
    doit(test_false_1);
    doit(test_false_2);
    doit(test_false_3);
    doit(test_false_4);
    char test_true_1[] = "true";
    char test_true_2[] = "    true    ";
    char test_true_3[] = "tru";
    /**
     * require_null_terminated = 0 时，the test is yes,
     * require_null_terminated = 1 时，the test is no.   
    */
    char test_true_4[] = " true??"; 
    doit(test_true_1);
    doit(test_true_2);
    doit(test_true_3);
    doit(test_true_4);
}

static void test_string_type() {
    doit("\"\"");
    doit("\"Hello\"");
    doit("\"Hello\\nWorld\"");
    doit("\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    doit("\"Hello\\u0000World\"");
    doit("\"\\u0024\""); /* Dollar sign U+0024 */  
    doit("\"\\u00A2\""); /* Cents sign U+00A2 */
    doit("\"\\u20AC\""); /* Euro sign U+20AC */
    doit("\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    doit("\"\\ud834\\udd1e\""); /* G clef sign U+1D11E */
}

/**
 * Directly using 'null_logic_value' unit for testing:
 * Error before: [""]
 * Error before: ["Hello"]
 * Error before: ["Hello\nWorld"]
 * Error before: ["\" \\ \/ \b \f \n \r \t"]
 * Error before: ["Hello\u0000World"]
 * Error before: ["\u0024"]
 * Error before: ["\u00A2"]
 * Error before: ["\u20AC"]
 * Error before: ["\uD834\uDD1E"]
 * Error before: ["\ud834\udd1e"]
*/

/**
 * Our target:
 * "Hello"
 * "Hello\nWorld"
 * "\" \\ / \b \f \n \r \t"
 * "HelloWorld"
 * "$"
 * "¢"
 * "€"
 * "𝄞"
 * "𝄞"
*/

void test_number_type() {
    doit("0");
    doit("-0");
    doit("-0.0");
    doit("1");
    doit("-1");
    doit("1.5");
    doit("-1.5");
    doit("3.1416");
    doit("1E10");
    doit("1e10");
    doit("1E+10");
    doit("1E-10");
    doit("-1E10");
    doit("-1e10");
    doit("-1E+10");
    doit("-1E-10");
    doit("1.234E+10");
    doit("1.234E-10");
    doit("1e-10000");   /* must underflow */

    doit("1.0000000000000002"); /* the smallest number > 1 */
    doit("4.9406564584124654e-324"); /* minimum denormal */
    doit("-4.9406564584124654e-324");
    doit("2.2250738585072009e-308");  /* Max subnormal double */
    doit("-2.2250738585072009e-308");
    doit("2.2250738585072014e-308"); /* Min normal positive double */
    doit("-2.2250738585072014e-308");
    doit("1.7976931348623157e+308"); /* Max double */
    doit("-1.7976931348623157e+308");
}

/**
 * Directly using 'null_string_value' (including null_logic_type, bootsting) unit for testing:
 * Error before: [0]
 * Error before: [-0]
 * Error before: [-0.0]
 * Error before: [1]
 * Error before: [-1]
 * Error before: [1.5]
 * Error before: [-1.5]
 * Error before: [3.1416]
 * Error before: [1E10]
 * Error before: [1e10]
 * Error before: [1E+10]
 * Error before: [1E-10]
 * Error before: [-1E10]
 * Error before: [-1e10]
 * Error before: [-1E+10]
 * Error before: [-1E-10]
 * Error before: [1.234E+10]
 * Error before: [1.234E-10]
 * Error before: [1e-10000]
 * Error before: [1.0000000000000002]
 * Error before: [4.9406564584124654e-324]
 * Error before: [-4.9406564584124654e-324]
 * Error before: [2.2250738585072009e-308]
 * Error before: [-2.2250738585072009e-308]
 * Error before: [2.2250738585072014e-308]
 * Error before: [-2.2250738585072014e-308]
 * Error before: [1.7976931348623157e+308]
 * Error before: [-1.7976931348623157e+308]
*/

#if 0
/* Our target: */
0
0
0
1
-1
1.500000
-1.500000
3.141600
10000000000
10000000000
10000000000
1.000000e-10
-10000000000
-10000000000
-10000000000
-1.000000e-10
12340000000
1.234000e-10
0
1
0
0
0
0
0
0
1.797693e+308
-1.797693e+308
#endif

/**
 * Our target:
 * The array's size is 0
 * []
 * The array's size is 5
 * item 1: null
 * item 2: false
 * item 3: true
 * item 4: 123
 * item 5: "abc"
 * [null, false, true, 123, "abc"]
 * The array's size is 4
 * item 1: []
 * item 2: [0]
 * item 3: [0, 1]
 * item 4: [0, 1, 2]
 * [[], [0], [0, 1], [0, 1, 2]]
*/
void test_array_type() {
    doit("[ ]");
    doit("[ null , false , true , 123 , \"abc\" ]");
    doit("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
}

void test_object_type() {
    doit("{\n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}");
    doit("{\n		\"Image\": {\n			\"Width\":  800,\n			\"Height\": 600,\n			\"Title\":  \"View from 15th Floor\",\n			\"Thumbnail\": {\n				\"Url\":    \"http:www.example.com/image/481989943\",\n				\"Height\": 125,\n				\"Width\":  \"100\"\n			},\n			\"IDs\": [116, 943, 234, 38793]\n		}\n	}");
}


/* test function "cJSON_strcasecmp" */
#include<ctype.h>
static int cJSON_strcasecmp(const char* s1, const char* s2) {
	if(!s1) return (s1==s2) ? 0 : 1; 
    if(!s2) return 1;
	for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)	if(*s1 == 0)	return 0;
	return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}
void test_function_cJSON_strcasecmp() {
    int test1 = cJSON_strcasecmp("1233", "1233");
    int test2 = cJSON_strcasecmp("1233", "1233455");
    int test3 = cJSON_strcasecmp("1233", "1232");
    int test4 = cJSON_strcasecmp("1233", "1234");
    printf("%d %d %d %d", test1, test2, test3, test4);
    /* 0 -52 1 -1 */
}

int main() {
    // test_enum_type();
    // test_null_false_true_type();
    // test_string_type();
    // test_number_type();
    // test_array_type();
    test_object_type();
    // char test[] = "\"\\u0024\"";
    // printf("%s\n", test);
    // test_function_cJSON_strcasecmp();
    return 0;
}