//
// Created by rbanasza on 06.01.2024.
//

#ifndef TYPESIZEDISCOVERY_H
#define TYPESIZEDISCOVERY_H

#include <stddef.h>
#include <stdint.h>

/*
 * This utility file requires to implement in the project a function SlogPutchar with following signature:
 * void SlogPutchar(char byte)
 *
 * If you don't have working communication at this point you can declare empty SlogPutchar function and use debugger
 * to check results. Disable optimizations or add -Wno-unused-variable flag to prevent removing of the
 * 'unused' variables.
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
