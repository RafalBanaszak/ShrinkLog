// MIT License
//
// Copyright (c) 2024 ShrinkLog Contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "slog/ShrinkLog.h"
#include "slog/TypeSizeDiscovery.h"
#include <math.h>

/* On the target device this function should implement sending a character over interface to the log collecting PC */
void SlogPutchar(const char byte) {
    putchar(byte);
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
                      M_PI, M_PI, M_PI, M_PI, (long double)M_PI,
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
