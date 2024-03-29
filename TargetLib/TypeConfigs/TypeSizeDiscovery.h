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

#ifndef TYPESIZEDISCOVERY_H
#define TYPESIZEDISCOVERY_H

#include <stddef.h>
#include <stdint.h>

/*
 * This utility file requires to implement in the project a function SlogPutchar with following signature:
 * void SlogPutchar(char byte)
 *
 * If you don't have working communication at this point you can declare empty SlogPutchar function and use debugger
 * to check results. Disable optimizations or add -Wno-unused-variable flag to prevent the
 * 'unused' variables from being removed.
 */
void SlogPutchar(char byte);

static void SlogPutString(const char * const string)
{
    unsigned i = 0;
    while(string[i] != '\0')
    {
        SlogPutchar(string[i++]);
    }
}

static void SlogPutSize(const unsigned char size)
{
    if (size <= 8) SlogPutchar('0' + size);
    else if (size == 16) SlogPutString("16");
    else SlogPutString("err");
}

static void SlogDiscover(void) {
    const size_t short_sz = sizeof(short);
    const size_t int_sz = sizeof(int);
    const size_t long_sz = sizeof(long);
    const size_t long_long_sz = sizeof(long long);
    const size_t double_sz = sizeof(double);
    const size_t long_double_sz = sizeof(long double);
    const size_t wchar_t_sz = sizeof(wchar_t);
    const size_t intmax_t_sz = sizeof(intmax_t);
    const size_t size_t_sz = sizeof(size_t);
    const size_t ptrdiff_t_sz = sizeof(ptrdiff_t);
    const size_t void_ptr_sz = sizeof(void*);

    const unsigned endianness_helper = 0xAA;
    const char* endianness = (endianness_helper & (char)0xFF) ? "little" : "big";

    SlogPutString("endianness: ");  SlogPutString(endianness);                  SlogPutchar('\n');
    SlogPutString("short: ");       SlogPutSize((unsigned char)short_sz);       SlogPutchar('\n');
    SlogPutString("int: ");         SlogPutSize((unsigned char)int_sz);         SlogPutchar('\n');
    SlogPutString("long: ");        SlogPutSize((unsigned char)long_sz);        SlogPutchar('\n');
    SlogPutString("long long: ");   SlogPutSize((unsigned char)long_long_sz);   SlogPutchar('\n');
    SlogPutString("double: ");      SlogPutSize((unsigned char)double_sz);      SlogPutchar('\n');
    SlogPutString("long double: "); SlogPutSize((unsigned char)long_double_sz); SlogPutchar('\n');
    SlogPutString("wchar_t: ");     SlogPutSize((unsigned char)wchar_t_sz);     SlogPutchar('\n');
    SlogPutString("intmax_t: ");    SlogPutSize((unsigned char)intmax_t_sz);    SlogPutchar('\n');
    SlogPutString("size_t: ");      SlogPutSize((unsigned char)size_t_sz);      SlogPutchar('\n');
    SlogPutString("ptrdiff_t: ");   SlogPutSize((unsigned char)ptrdiff_t_sz);   SlogPutchar('\n');
    SlogPutString("void*: ");       SlogPutSize((unsigned char)void_ptr_sz);    SlogPutchar('\n');
}

#endif //TYPESIZEDISCOVERY_H
