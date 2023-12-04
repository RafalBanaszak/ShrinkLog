//
// Created by rafal on 03.11.2023.
//

#ifndef SHRINKLOG_SHRINGLOG_H
#define SHRINKLOG_SHRINGLOG_H

#include "Tags.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

/*
 * The library requires to implement and include here a function putchar with following signature:
 * void putchar(unsigned char byte)
 */

/* #include "include/putchar/header/here.h" */

static inline void Slogputchar(char symbol)
{
    printf("%x", symbol & 0xFF);
}

#if !defined(SLOG_INT_B_SIZE) || SLOG_INT_B_SIZE != 4
#error "Unsupported int size. Check SLOG_INT_B_SIZE definition"
#endif

#if SLOG_INT_B_SIZE == 8 && !defined(SLOG_ENABLE_8B)
#error "Int size equal to 8 but 8 byte mode disabled. Check SLOG_INT_B_SIZE and SLOG_ENABLE_8B defines"
#endif

static inline void SlogPutcharWrapper(unsigned char symbol)
{
#ifdef SLOG_BINARY_MODE
    putchar(symbol);
#else /* Text mode */
    /* Convert byte to hex representation */
    static char halfByteToHex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    putchar(halfByteToHex[(symbol >> 4) & 0x0F]);
    putchar(halfByteToHex[symbol & 0x0F]);
#endif
}

static inline void PrintStaticArgument(va_list args, const uint_fast8_t size)
{
    uint_fast8_t argIt;
#ifdef SLOG_ENABLE_8B
    union
    {
        uint_fast64_t var64;
        uint_fast32_t var32;
        uint_fast16_t var16;
        uint_fast8_t  var8;
    } var;
    #if SLOG_INT_B_SIZE == 8
        var.var64 = va_arg(args, uint64_t);
    #elif SLOG_INT_B_SIZE == 4
        if (size == 8)  { var.var64 = va_arg(args, uint64_t); }
        else            { var.var32 = va_arg(args, uint32_t); }
    #elif SLOG_INT_B_SIZE == 2
        if      (size == 8) { var.var64 = va_arg(args, uint64_t); }
        else if (size == 4) { var.var32 = va_arg(args, uint32_t); }
        else                { var.var16 = va_arg(args, uint16_t); }
    #elif SLOG_INT_B_SIZE == 1
        if      (size == 8) { var.var64 = va_arg(args, uint64_t); }
        else if (size == 4) { var.var32 = va_arg(args, uint32_t); }
        else if (size == 2) { var.var16 = va_arg(args, uint16_t); }
        else                { var.var8  = va_arg(args, uint8_t ); }
    #endif
    for (argIt = 0; argIt < size; ++argIt)
    {
        SlogPutcharWrapper(var.var64 >> (8 * argIt));
    }
#else
    union
    {
        uint_fast32_t var32;
        uint_fast16_t var16;
        uint_fast8_t  var8;
    } var;
#if SLOG_INT_B_SIZE == 4
    var.var32 = va_arg(args, uint32_t);
#elif SLOG_INT_B_SIZE == 2
    if (size == 4)  { var.var32 = va_arg(args, uint32_t); }
        else            { var.var16 = va_arg(args, uint16_t); }
#elif SLOG_INT_B_SIZE == 1
    if      (size == 4) { var.var32 = va_arg(args, uint32_t); }
        else if (size == 2) { var.var16 = va_arg(args, uint16_t); }
        else                { var.var8  = va_arg(args, uint8_t);  }
#endif
    for (argIt = 0; argIt < size; ++argIt)
    {
        SlogPutcharWrapper(var.var32 >> (8 * argIt));
    }
#endif
}

static inline void PrintStringArgument(va_list args)
{
    uint_fast16_t stringIt = 0;
    char* str = (char*)va_arg(args, char*);
    while(str[stringIt] != '\0'){
        putchar(str[stringIt++]);
    }
    SlogPutcharWrapper(str[stringIt]);
}

void LOG(char* tag, char* message, ...){
    uint_fast8_t tagIt = 0;
    va_list args;
    va_start(args, message);

    for (; tagIt < SLOG_ID_BYTE_CNT; ++tagIt){
        SlogPutcharWrapper(tag[tagIt]);
    }

    while(tag[tagIt] != '\0'){
        /* Get static arguments which byte size is known */
        if(tag[tagIt] <= 8){
            PrintStaticArgument(args, tag[tagIt]);
        }
        /* Get dynamic arguments which byte size is NOT known */
        else if (tag[tagIt] == 0x7F) {
            PrintStringArgument(args);
        }
        ++tagIt;
    }
    SlogPutcharWrapper('\0');
#ifndef SLOG_BINARY_MODE
    putchar('\n');
#endif
    va_end(args);
}

#endif //SHRINKLOG_SHRINGLOG_H
