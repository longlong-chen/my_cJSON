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

int main() {
    // test_enum_type();
    // test_null_false_true_type();
    test_string_type();
    return 0;
}