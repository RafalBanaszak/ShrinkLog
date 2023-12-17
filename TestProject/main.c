//
// Created by rafal on 08.12.2023.
//

#include "slog/ShringLog.h"
#include <math.h>

void SlogPutchar(char symbol) {
    putchar(symbol);
}

int main() {

/* This project has two tasks:
 * 1. Provide a test for ShrinkLog's Encoder and Decoder. Ignore this part if you are looking for an example
 * 2. Provide an example and template for using ShrinkLog in other projects
 */
#define TEST_ENABLE /* Comment this define to disable tests */

/* 1. Test ShrinkLog Encoder and Decoder. */
#ifdef TEST_ENABLE
#include "tf_arguments.h"
#include "nested/tf_simple.h"
#include "nested/nested/tf_simple.h"
#include "tf_comment_final1.h"
#include "tf_comment_final2.h"
#include "tf_comment_initial1.h"
#include "tf_comment_initial2.h"
#include "tf_comment_middle1.h"
#include "tf_comment_middle2.h"
#include "tf_empty.h"
#include "tf_multiline_strings.h"
#endif /* TEST_ENABLE */

/* 2. The example starts here */
    LOG(SLOG_jI3O7Y4, "This is an example log without arguments\n");
    LOG(SLOG_OC2ME5N, "This is an \"advanced\" example log with arguments:\n"
                      "%d %i\n"
                      "%hhx %hx %x %lx %llx\n"
                      "%f %.0f %.32f %05.2f %Lf\n"
                      "%c\n"
                      "[%s] [%5s] [%-5s]\n",
                      42, -1,
                      0x12, 0x1234, 0x12345678, 0x1234567890AABBCC, 0x1234567890AABBCC,
                      M_PI, M_PI, M_PI, M_PI, M_PI,
                      'A',
                      "test_string", "test_string", "test_string");

    return 0;
}
