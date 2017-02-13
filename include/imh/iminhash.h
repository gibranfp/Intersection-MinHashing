/**
 * @file iminhash.h
 * @author Gibran Fuentes Pineda <gibranfp@unam.mx>
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
 * @brief Declaration of structures and functions for Intersection Min-Hashing
 */
#ifndef IMINHASH_H
#define IMINHASH_H

#include "listdb.h"

typedef struct RandomValue
{
     ullong random_int;
     double random_double;
} RandomValue;

typedef struct Bucket{
     ullong hash_value;
     List items;
} Bucket;

typedef struct HashTable {
	  uint table_size; 
	  uint tuple_size; 
	  uint dim;
	  uint sublist_size;
	  RandomValue *permutations;
	  Bucket *buckets;
	  List used_buckets;
	  uint *a;
	  uint *b;
	  double *weights;
} HashTable;

typedef struct HashIndex {
	  uint number_of_tables;
	  HashTable *hash_tables;
} HashIndex;

/************************ Function prototypes ************************/
void imh_print_head(HashTable *);
void imh_print_table(HashTable *);
void imh_rng_init(unsigned long long);
void imh_init_table(HashTable *);
HashTable imh_create_table(uint, uint, uint, uint);
void imh_destroy_table(HashTable *);
int imh_random_double_value_compare(const void *, const void *);
int imh_random_double_value_compare_back(const void *, const void *);
int imh_random_int_value_compare(const void *, const void *);
int imh_random_int_value_compare_back(const void *, const void *);
void imh_generate_permutations(uint, uint, RandomValue *);
ullong imh_compute_minhash(List *, RandomValue *);
ListDB imh_create_sublistdb_from_listdb(ListDB *, uint *, uint, uint *);
int imh_random_value_double_compare_back(const void *, const void *);
void imh_compute_univhash(List *, HashTable *, uint *, uint *);
uint imh_get_index(List *, HashTable *);
uint imh_get_sublist_numbers(ListDB *, uint, uint *);
uint imh_store_list(List *, uint, HashTable *);
void imh_store_listdb(ListDB *, HashTable *, uint *);
#endif
