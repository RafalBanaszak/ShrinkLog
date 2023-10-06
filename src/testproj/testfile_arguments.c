//
LOG(STAG_UNKNOWN, "Log1 string \"escaped\"%1$d:%2$.*3$d:%4$.*3$d\n", arg1, arg2, arg3);
LOG(STAG_UNKNOWN, "Strings:\n");
LOG(STAG_UNKNOWN, "\t[%10s]\n", s);
LOG(STAG_UNKNOWN, "\t[%-10s]\n", s);
LOG(STAG_UNKNOWN, "\t[%*s]\n", 10, s);
LOG(STAG_UNKNOWN, "\t[%-10.*s]\n", 4, s);
LOG(STAG_UNKNOWN, "\t[%-*.*s]\n", 10, 4, s);

LOG(STAG_UNKNOWN, "Characters:\t%c %%\n", 'A');

LOG(STAG_UNKNOWN, "Integers:\n");
LOG(STAG_UNKNOWN, "\tDecimal:    \t%i %d %.6i %i %.0i %+i %i\n",1, 2,   3, 0,   0,  4,-4);
LOG(STAG_UNKNOWN, "\tHexadecimal:\t%x %x %X %#x\n",5,10,10,  6);
LOG(STAG_UNKNOWN, "\tOctal:      \t%o %#o %#o\n",10, 10,  4);

LOG(STAG_UNKNOWN, "Floating point:\n");
LOG(STAG_UNKNOWN, "\tRounding:\t%f %.0f %.32f\n", 1.5, 1.5, 1.3);
LOG(STAG_UNKNOWN, "\tPadding:\t%05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
LOG(STAG_UNKNOWN, "\tScientific:\t%E %e\n", 1.5, 1.5);
LOG(STAG_UNKNOWN, "\tHexadecimal:\t%a %A\n", 1.5, 1.5);
LOG(STAG_UNKNOWN, "\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0/0.0, 1.0/0.0);

LOG(STAG_UNKNOWN, "Variable width control:\n");
LOG(STAG_UNKNOWN, "\tright-justified variable width: '%*c'\n", 5, 'x');
LOG(STAG_UNKNOWN, "\tleft-justified variable width : '%*c'\n", -5, 'x');
// LOG(STAG_UNKNOWN, "(the last printf printed %d characters)\n", r);

LOG(STAG_UNKNOWN, "Fixed-width types:\n");

LOG(STAG_UNKNOWN, "\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",val,            val);

