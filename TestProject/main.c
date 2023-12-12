//
// Created by rafal on 08.12.2023.
//

#include "slog/ShringLog.h"

void SlogPutchar(char symbol) {
    putchar(symbol);
}

int main() {
    LOG(SLOG_CcIij9H, "This is a test log without arguments\n");
    LOG(SLOG_fp5nPVd, "This is a test log with integer arguments of different size %hhi %hi %i %li %lli\n", 0x12, 0x1234, 0x12345678, 0x1234567890AABBCC, 0x1234567890AABBCC);
    LOG(SLOG_9zuU5Gh, "This is a test log with float arguments of different size %f %lf\n", (float)1.3, (double)1.3);
    return 0;
}
