/*
overlay64 -- video overlay module
Copyright (C) 2016 Henning Bekel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAIN_H
#define MAIN_H

#define xstr(s) mstr(s)
#define mstr(s) #s

#include <avr/cpufunc.h>

#define TICKS_PER_USEC F_CPU/1000000UL
#define US(n) n*(TICKS_PER_USEC)

#define TWO_NOPS() _NOP(); _NOP()
#define TEN_NOPS() TWO_NOPS(); TWO_NOPS(); TWO_NOPS(); TWO_NOPS(); TWO_NOPS()
#define ONEHUNDRED_NOPS() TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS()
#define FORTYTWO_NOPS() TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TWO_NOPS()
#define ONEHUNDRED_AND_TEN_NOPS() ONEHUNDRED_NOPS(); TEN_NOPS()

void SetupFont(void);
void SetupVersionString(void);

#endif // MAIN_H
