/**
 * @file rng.c
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
 * @brief Interface for diferent random number generators
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "mt64.h"
#include "rng.h"

/**
 * @brief Initializes a random number generator
 *
 * @param Seed with which the random number generator is to be initialized
 */
void rng_init64(unsigned long long seed)
{
     init_genrand64(seed);
}

/**
 * @brief Generates a random number
 *
 * @return Random number
 */
ullong rng_generate_int64()
{
     genrand64_int64();
}

/**
 * @brief Generates a random number
 *
 * @return Random number
 */
double rng_generate_double()
{
     genrand64_real1();
}

/**
 * @brief Initializes a random number generator
 *
 * @param Seed with which the random number generator is to be initialized
 */
void rng_init32(uint seed)
{
     srand(seed);
}

/**
 * @brief Generates a random number
 *
 * @return Random number
 */
uint rng_generate_int32()
{
     rand();
}
