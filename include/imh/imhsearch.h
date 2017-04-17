/**
 * @file imhsearch.h
 * @author Gibran Fuentes-Pineda <gibranfp@unam.mx>
 * @date 2017
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
 * @brief Declaration of functios for Sampled Min-Hashing (SMH)
 */
#ifndef IMHSEARCH_H
#define  IMHSEARCH_H

#include <iminhash.h>

typedef struct HashIndex {
	  uint number_of_tables;
	  HashTable *hash_tables;
} HashIndex;

void imhsearch_print_index_head(HashIndex *);
void imhsearch_print_index_tables(HashIndex *);
HashIndex imhsearch_build(ListDB *, uint, uint, uint, uint);
List imhsearch_query(List *, HashIndex *);
void imhsearch_sort_custom(List *, List *, ListDB *, double (*)(List *, List *));
ListDB imhsearch_query_multi(ListDB *, HashIndex *);
#endif
