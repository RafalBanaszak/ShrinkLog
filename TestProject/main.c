//
// Created by rafal on 08.12.2023.
//

#include "slog/ShringLog.h"

void SlogPutchar(char symbol) {
    putchar(symbol);
}

int main() {
    LOG(SLOG_CcIij9H, "This is a test log without arguments\n");
    LOG(SLOG_eEP1Uf1, "This is a test log with integer arguments of different size %hhx %hx %x %lx %llx\n", 0x12, 0x1234, 0x12345678, 0x1234567890AABBCC, 0x1234567890AABBCC);
    LOG(SLOG_ydj90U0, "This is a test log with float arguments of different size %f %Lf\n", (double)1.3, (double)1.3);
    LOG(SLOG_OgFK75H, "This is a test log with string argument %s\n", "test string");

//    for (int i = 0; i < 250000; ++i) {
//        LOG(SLOG_CcIij9H, "This is a test log without arguments\n");
//        LOG(SLOG_eEP1Uf1, "This is a test log with integer arguments of different size %hhx %hx %x %lx %llx\n", 0x12, 0x1234, 0x12345678, 0x1234567890AABBCC, 0x1234567890AABBCC);
//        LOG(SLOG_ydj90U0, "This is a test log with float arguments of different size %f %Lf\n", (double)1.3, (double)1.3);
//        LOG(SLOG_OgFK75H, "This is a test log with string argument %s\n", "test string");
//    }
    return 0;
}
