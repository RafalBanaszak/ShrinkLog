//
// Created by rafal on 08.12.2023.
//

#include "slog/ShrinkLog.h"
#include "slog/TypeSizeDiscovery.h"
#include <math.h>

void SlogPutchar(char symbol) {
    putchar(symbol);
}

int main() {

/* This project has three parts:
 * 1. LOG function demo/example
 * 2. Type size discovery demo/example.
 * 3. Test for ShrinkLog Encoder and Decoder.
 */

#ifdef SLOG_LOG_EXAMPLE
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
#elif SLOG_DISCOVER_EXAMPLE
    SlogDiscover();
#elif SLOG_TEST
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
#endif

    return 0;
}
