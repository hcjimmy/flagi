/* A flag-parsing library - source file.
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
#include "flagi.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

BoolOneFlag* match_bool_ones(char flag, BoolOneFlag *bool_ones)
{
	while(bool_ones->flag1 != '\0') {
		if(bool_ones->flag1 == flag)
			return bool_ones;
		++bool_ones;
	}

	return NULL;
}

ArgOneFlag* match_arg_ones(char flag, ArgOneFlag *arg_ones)
{
	while(arg_ones->flag1 != '\0') {
		if(arg_ones->flag1 == flag)
			return arg_ones;
		++arg_ones;
	}

	return NULL;
}

BoolTwoFlag* match_bool_twos(char *flag, BoolTwoFlag *bool_twos)
{
	while(bool_twos->flag2 != NULL) {
		if(strcmp(bool_twos->flag2, flag) == 0)
			return bool_twos;
		++bool_twos;
	}

	return NULL;
}

ArgTwoFlag* match_arg_twos(char *flag, ArgTwoFlag *arg_twos)
{
	while(arg_twos->flag2 != NULL) {
		if(strcmp(arg_twos->flag2, flag) == 0)
			return arg_twos;
		++arg_twos;
	}

	return NULL;
}

void report_missing_arg(char flag1)
{
	fprintf(stderr, "Error: -%c needs an argument.\n", flag1);
}

/** Parse a one-flag.
 *
 * next_arg			The next argument the flag is set to if it's an argument flag.
 * parsed_next			*parsed_next indicates if a flag on the same arg received the next arg.
 * 					If parsed an argument, it is set to true, otherwise unmodified.
 * unknown_flag_is_invalid	Indicate whether an unknown flag should be treated as an error
 * 				(since depending on the context it might be legal).
 *
 * Errors reporting:
 * 	All errors are reported to stderr.
 *
 * Return value:
 *	If any error occures: -1.
 *
 *	Otherwise:
 *		If successfully parsed a flag: 0
 *		Else (if an unknown flag is valid): 1
 *
 * 	In other words if an unknown flag is invalid, only -1 and 0 may be returned.
 */
short parse_one_flag(char flag, char *next_arg, bool *parsed_next, bool unknown_flag_is_invalid,
		BoolOneFlag *bool_ones, BoolOneFlag *negative_ones,
		ArgOneFlag *arg_ones)
{
	BoolOneFlag *bool_one_match;
	ArgOneFlag *arg_one_match;

	if((bool_one_match = match_bool_ones(flag, bool_ones)))
		*bool_one_match->yes = true;

	else if((bool_one_match = match_bool_ones(flag, negative_ones)))
		*bool_one_match->yes = false;

	else if((arg_one_match = match_arg_ones(flag, arg_ones))) {

		// If we already parsed the following value.
		if(*parsed_next) {
			fprintf(stderr, "Error: multiple flags given the same arg '%s'.",
					next_arg);
			return -1;
		}

		if(!next_arg) {
			report_missing_arg(flag);
			return -1;
		}

		*arg_one_match->arg = next_arg;
		*parsed_next = true;

	} else {
		// Uknown flag.
		if(unknown_flag_is_invalid) {
			fprintf(stderr, "Error: unknown flag given -%c.", flag);
			return -1;
		}
		return 1;
	}

	return 0;
}

bool flag_ignore(char **argv, bool *parsed,
		BoolOneFlag *bool_ones, BoolTwoFlag *bool_twos,
		ArgOneFlag *arg_ones, ArgTwoFlag *arg_twos,
		BoolOneFlag *negative_ones, BoolTwoFlag *negative_twos)
{

	BoolTwoFlag *bool_two_match;
	ArgTwoFlag *arg_two_match;

	char *arg;

	bool parsed_next;
	short state;

	for(;*argv != NULL; ++argv, ++parsed) {

		// Skip if not a flag.
		if(argv[0][0] != '-' || argv[0][1] == '\0') {
			*parsed = false;
			continue;
		}

		// If it's a two-flag
		if(argv[0][1] == '-') {
			if((bool_two_match = match_bool_twos(*argv + 2, bool_twos)))
				*bool_two_match->occured = true;

			else if ((bool_two_match = match_bool_twos(*argv + 2, negative_twos)))
				*bool_two_match->occured = false;

			else if((arg_two_match = match_arg_twos(*argv + 2, arg_twos))) {

					if(++argv == NULL) {
						fprintf(stderr, "Error: flag --%s needs an argument.",
								arg_two_match->flag2);
						return true;
					}

					*parsed++ = true;
					*arg_two_match->arg = *argv;
			} else {
				*parsed = false;
				continue;
			}

			*parsed = true;
			continue;
		}

		// It's a one-flag.
		parsed_next = false;
		arg = *argv + 1;

		state = parse_one_flag(*arg, argv[1], &parsed_next, false, bool_ones, negative_ones, arg_ones);

		if(state < 0)	// Error occured.
			return true;
		*parsed = (state == 0);
		if(!*parsed)		// If state != 0, it's unknown flag, skip.
			continue;

		while(*++arg != '\0') {
			if(parse_one_flag(*arg, argv[1], &parsed_next, true, bool_ones, negative_ones, arg_ones)) {
				return true;	// Error occured.
			}
		}

		if(parsed_next) {
			++argv;
			*++parsed = true;
		}

	}

	return false;
}
