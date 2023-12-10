LOG(SLOG_E2vd_vS, "Log1 string \"escaped\"%1$d:%2$.*3$d:%4$.*3$d\n", arg1, arg2, arg3);
LOG(SLOG_WZ5Jy6H, "Strings:\n");
LOG(SLOG_kel8VeH, "\t[%10s]\n", s);
LOG(SLOG_fPjG68g, "\t[%-10s]\n", s);
LOG(SLOG_UHYSt3u, "\t[%*s]\n", 10, s);
LOG(SLOG_ZLkMu00, "\t[%-10.*s]\n", 4, s);
LOG(SLOG_UNznSAv, "\t[%-*.*s]\n", 10, 4, s);

LOG(SLOG_gvU6dNt, "Characters:\t%c %%\n", 'A');

LOG(SLOG_24r_mNH, "Integers:\n");
LOG(SLOG_q8I0gLX, "\tDecimal:    \t%hi %hhd %.6i %i %.0i %+i %i\n",1, 2,   3, 0,   0,  4,-4);
LOG(SLOG_ziN4eZL, "\tHexadecimal:\t%x %x %X %#x\n",5,10,10,  6);
LOG(SLOG_vTFqhpQ, "\tOctal:      \t%o %#o %#o\n",10, 10,  4);

LOG(SLOG_rEHyNNf, "Floating point:\n");
LOG(SLOG_DEFA2RZ, "\tRounding:\t%f %.0f %.32f\n", 1.5, 1.5, 1.3);
LOG(SLOG_5tEUWbJ, "\tPadding:\t%05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
LOG(SLOG__90r39N, "\tScientific:\t%E %e\n", 1.5, 1.5);
LOG(SLOG_1OD4AIp, "\tHexadecimal:\t%a %A\n", 1.5, 1.5);
LOG(SLOG_Oy7HFMV, "\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0/0.0, 1.0/0.0);

LOG(SLOG_B4oJC33, "Variable width control:\n");
LOG(SLOG_j_ZU9JH, "\tright-justified variable width: '%*c'\n", 5, 'x');
LOG(SLOG_jNupQr9, "\tleft-justified variable width : '%*c'\n", -5, 'x');
LOG(SLOG_4HxStPL, "(the last printf printed %d characters)\n", r);

LOG(SLOG_YOkwe50, "Fixed-width types:\n");

LOG(SLOG_LXJBxJr, "\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",val,            val);

