{
    const char testString[] = "test string";

    LOG(SLOG_LXW6vqA, "Log1 string \"escaped\"\n");
    LOG(SLOG_WZ5Jy6H, "Strings:\n");
    LOG(SLOG_zzyOBdb, "\t[%s]\n",   testString);
    LOG(SLOG_QFKwshi, "\t[%5s]\n",  testString);
    LOG(SLOG_CDnithN, "\t[%-5s]\n", testString);

    LOG(SLOG_pFpTmyv, "Characters:\t%c\n", 'A');

    LOG(SLOG_24r_mNH, "Integers:\n");
    LOG(SLOG_xIaKf3T, "\tSizes:      \t%hhx %hx %x %lx %llx\n", 0x12, 0x1234, 0x12345678, 0x1234567890AABBCC, 0x1234567890AABBCC);
    LOG(SLOG_q8I0gLX, "\tDecimal:    \t%hi %hhd %.6i %i %.0i %+i %i\n",1, 2, 3, 0, 0, 4, -4);
    LOG(SLOG_ziN4eZL, "\tHexadecimal:\t%x %x %X %#x\n", 5, 10, 10, 6);
    LOG(SLOG_vTFqhpQ, "\tOctal:      \t%o %#o %#o\n",10 ,10 ,4);

    LOG(SLOG_rEHyNNf, "Floating point:\n");
    LOG(SLOG_DEFA2RZ, "\tRounding:\t%f %.0f %.32f\n", 1.5, 1.5, 1.3);
    LOG(SLOG_5tEUWbJ, "\tPadding:\t%05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
    LOG(SLOG__90r39N, "\tScientific:\t%E %e\n", 1.5, 1.5);
    LOG(SLOG_1OD4AIp, "\tHexadecimal:\t%a %A\n", 1.5, 1.5);
    LOG(SLOG_Oy7HFMV, "\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0/0.0, 1.0/0.0);
    LOG(SLOG_ppdNYAu, "\tLong double:\t%Lf %lf\n", 1.5, 1.5);
}



