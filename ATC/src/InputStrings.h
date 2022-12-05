#ifndef SRC_INPUTSTRINGS_H_
#define SRC_INPUTSTRINGS_H_

// Time ID PX PY PZ VX VY VZ
// Low load is specifically curated to demonstrate various aspects of the system
// Med and high load were randomly generated

constexpr auto LOW_LOAD = "1 1 0 50000 20000 1000 0 0\n"
		"1 2 50000 0 20000 0 1000 0\n"
		"5 3 0 0 15000 250 250 500\n";

constexpr auto MED_LOAD = "8 1 0 48263 23170 652 1078 0\n"
		"5 2 22858 0 16556 599 936 0\n"
		"26 3 70022 0 21308 525 482 0\n"
		"24 4 0 66436 22822 496 1021 0\n"
		"37 5 0 39728 24535 395 944 0\n"
		"13 6 0 38406 20438 505 734 0\n"
		"17 7 0 95620 18311 966 963 0\n"
		"36 8 0 67006 21960 505 362 0\n"
		"2 9 0 89567 20099 808 545 0\n"
		"23 10 36989 0 21532 820 1226 0\n";

constexpr auto HIGH_LOAD = "53 1 70222 0 16393 900 534 0\n"
		"1 2 0 43499 19036 868 461 0\n"
		"27 3 18093 0 15704 982 365 0\n"
		"15 4 0 84580 15289 492 859 0\n"
		"18 5 0 29453 19133 445 993 0\n"
		"28 6 37590 0 18898 802 585 0\n"
		"29 7 64380 0 16069 641 303 0\n"
		"55 8 75801 0 20148 779 1196 0\n"
		"31 9 5190 0 17901 572 330 0\n"
		"10 10 0 92659 15300 406 278 0\n"
		"38 11 24558 0 21642 257 329 0\n"
		"11 12 9925 0 16691 586 986 0\n"
		"6 13 0 78386 19920 1118 622 0\n"
		"49 14 89727 0 23645 1143 1039 0\n"
		"27 15 93776 0 20390 949 821 0\n";

#endif /* SRC_INPUTSTRINGS_H_ */
