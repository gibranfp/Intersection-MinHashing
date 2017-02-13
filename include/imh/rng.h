/**
 * @file types.h
 * @author Gibran Fuentes-Pineda <gibranfp@unam.mx>
 * @date 2016
 *
 * @section GPL
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @brief Declaration of functions for random number generation
 */
#ifndef RNG_H
#define RNG_H
#include <types.h>

void rng_init64(ullong) ;
ullong rng_generate_int64() ;
double rng_generate_double() ;
void rng_init32(uint) ;
uint rng_generate32() ;
#endif
