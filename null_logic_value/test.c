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



int main() {
    // test_enum_type();
    test_null_false_true_type();
    return 0;
}