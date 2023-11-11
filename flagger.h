/* A flag-parsing library - header.
 * Copyright (C) 2023  hcjimmy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	char flag1;
	bool * const yes;
} BoolOneFlag;

typedef struct {
	char flag1;
	char **const arg;
} ArgOneFlag;

typedef struct {
	const char *flag2;
	bool *const occured;
} BoolTwoFlag;

typedef struct {
	const char *flag2;
	char **const arg;
} ArgTwoFlag;


/* Receive list of arguments (NULL terminated) and set modify flags to
 * in arrays (which are respectively NULL and '\0' terminated with the flag values).
 *
 * FlagOne arrays contain 1 character flags to be parsed in the form of "-a" or "-abc"
 * while FlagTwo arrays contain options which are parsed in the form of "--option".
 *
 * If a flag matched, the corresponding value is is set:
 * For bool-flags true, negative bool-flags false, and for arg-flags the next arg.
 *
 * If an arg equals "-abc", there are 2 possibilities:
 * 	1. If there is no flag1 == 'a', the arg it is ignored.
 * 	2. The matching valus are set as described above.
 *		If 'b' or 'c' flags don't exist, an error is reported.
 *
 * 	If two one-flags receive the same argument, an error is reported.
 * 	If a flag (any flag) receives a non-existant argument an error is reported.
 * 	After any error reported error_suffix is printed (without anything
 * 	- not even newline - appended afterwards).
 *
 * If an arg is a non-existant two-flag (eg. "--option" but "option" is not a flag)
 * then the arg is ignored.
 *
 * Each element in the array parsed is set to true if the element in the same in index in argv
 * was parsed (had a valid flag), otherwise false.
 *
 * In other words:
 * If parsed[i] == true, argv[i] was parsed.
 * If parsed[i] == false, argv[i] wasn't parsed.
 *
 * Return value:
 * 	true in case of error occured, otherwise false.
 */
bool flagger(char **argv, bool parsed[],
		BoolOneFlag[], BoolTwoFlag[],
		ArgOneFlag[], ArgTwoFlag[],
		BoolOneFlag negative_ones[],BoolTwoFlag negative_twos[],
		const char* error_suffix);

