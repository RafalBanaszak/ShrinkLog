//
// Created by rafal on 08.12.2023.
//

#include "slog/ShringLog.h"

void SlogPutchar(char symbol) {
    putchar(symbol);
}

int main() {
    LOG(SLOG_CcIij9H, "This is a test log without arguments\n");
    LOG(SLOG_fp5nPVd, "This is a test log with integer arguments of different size %hhi %hi %i %li %lli\n", 1, 2, 3, 4, 0xAABBCCDDEEFF1122);
    LOG(SLOG_9zuU5Gh, "This is a test log with float arguments of different size %f %lf\n", (float)1.3, (double)1.3);
    LOG(SLOG_luYeHIw, "This is a test log with float arguments of different size %f\n", (float)1.3);
    LOG(SLOG_up6y8si, "This is a test log with float arguments of different size %lf\n", (double)1.3);
    double d = 1.3;
    float f = 1.3;
    double b = f;
    printf("%lx\n", *(uint64_t*)(&f));
    printf("%lx\n", *(uint64_t*)(&d));
    printf("%lx\n", *(uint64_t*)(&b));
    return 0;
}
