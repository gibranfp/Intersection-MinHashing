/**
 * @file iminhash.c
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
 * @brief Functions for computing MinHash functions for set intersection search.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>
#include "mt64.h"
#include "iminhash.h"

/**
 * @Brief Prints head of a hash table structure
 *
 * @param hash_table Hash table structure
 */
void imh_print_head(HashTable *hash_table)
{
     uint i;

     printf("========== Hash table =========\n");
     printf("Table size: %d\n"
            "Tuple size: %d\n"
            "Dimensionality: %d\n"
            "Sublist size: %d\n"
            "Used buckets: ",
            hash_table->table_size, 
            hash_table->tuple_size,
            hash_table->dim,
            hash_table->sublist_size); 
     list_print(&hash_table->used_buckets);

     printf("a: ");
     for (i = 0; i < hash_table->tuple_size; i++)
          printf("%u ", hash_table->a[i]);

     printf("\nb: ");
     for (i = 0; i < hash_table->tuple_size; i++)
          printf("%u ", hash_table->b[i]);
     printf("\n");
}

/**
 * @brief Prints head and content of a hash table structure
 *
 * @param hash_table Hash table structure
 */
void imh_print_table(HashTable *hash_table)
{
     uint i;
     
     for (i = 0; i < hash_table->used_buckets.size; i++) {
          uint bucket_number = hash_table->used_buckets.data[i].item;
          printf("[  %d  ] ", bucket_number);
          list_print(&hash_table->buckets[bucket_number].items);
     }
}

/**
 * @brief Initializes a hash table structure for performing Min-Hash
 *        on a collection of list.
 *
 * @param hash_table Hash table structure
 */
void imh_init_table(HashTable *hash_table)
{
     hash_table->table_size = 0;
     hash_table->tuple_size = 0;
     hash_table->sublist_size = 0; 
     hash_table->dim = 0; 
     hash_table->permutations  = NULL; 
     hash_table->buckets = NULL;
     list_init(&hash_table->used_buckets);
     hash_table->a = NULL;
     hash_table->b = NULL;
}

/**
 * @brief Initializes the randon number generator
 */
void imh_init_rng(unsigned long long seed)
{
     init_genrand64(seed);
}

/**
 * @brief Creates a hash table structure for performing Min-Hash
 *        on a collection of list.
 *
 * @param Number of MinHash values per tuple
 * @param dim Largest item value in the database of lists
 * @param table_size Number of buckets in the hash table
 *
 * @return Hash table structure
 */
HashTable imh_create_table(uint table_size, uint tuple_size, uint dim,
                           uint sublist_size)
{
     uint i;
     HashTable hash_table;

     hash_table.table_size = table_size;
     hash_table.tuple_size = tuple_size; 
     hash_table.dim = dim;
     hash_table.sublist_size = sublist_size; 
     hash_table.permutations = (RandomValue *) malloc(tuple_size * dim *
                                                      sizeof(RandomValue));
    
     hash_table.buckets = (Bucket *) calloc(table_size, sizeof(Bucket));
     list_init(&hash_table.used_buckets);

     // generates array of random values for universal hashing
     hash_table.a = (uint *) malloc(tuple_size * sizeof(uint));
     hash_table.b = (uint *) malloc(tuple_size * sizeof(uint));
     for (i = 0; i < tuple_size; i++) {
          hash_table.a[i] = (unsigned int) (genrand64_int64() & 0xFFFFFFFF);
          hash_table.b[i] = (unsigned int) (genrand64_int64() & 0xFFFFFFFF);
     }
     
     return hash_table;
}

/**
 * @brief Destroys a hash table structure 
 *
 * @param hash_table Hash table structure
 */
void imh_destroy_table(HashTable *hash_table)
{
     free(hash_table->permutations);
     free(hash_table->buckets);
     free(hash_table->a);
     free(hash_table->b);
     list_destroy(&hash_table->used_buckets);
     imh_init_table(hash_table);
}

/**
 * @brief Comparison of random double values for bsearch and qsort. 
 *
 * @param *a First random value to compare
 * @param *b Second random value to compare
 *
 * @return 0 if the random double values are equal, positive if the
 *         first random double value is greater than the second and
 *         negative otherwise.
 */
int imh_random_double_value_compare(const void *a, const void *b)
{
     double a_val = ((RandomValue *)a)->random_double;
     double b_val = ((RandomValue *)b)->random_double;

     if (a_val > b_val)
          return 1;
     else if (a_val < b_val)
          return -1;
     else
          return 0;
}

/**
 * @brief Random double value comparison for bsearch and qsort. 
 *
 * @param *a First random double value to compare
 * @param *b Second random double value to compare
 *
 * @return 0 if the random double values are equal, positive if the
 *         second random double value is greater than the first and
 *         negative otherwise.
 */
int imh_random_double_value_compare_back(const void *a, const void *b)
{
     double a_val = ((RandomValue *)a)->random_double;
     double b_val = ((RandomValue *)b)->random_double;

     if (a_val < b_val)
          return 1;
     else if (a_val > b_val)
          return -1;
     else
          return 0;
}

/**
 * @brief Comparison of random integer values for bsearch and qsort. 
 *
 * @param *a First random integer value to compare
 * @param *b Second random integer value to compare
 *
 * @return 0 if the random integer values are equal, positive if the
 *         first random integer value is greater than the second and
 *         negative otherwise.
 */
int imh_random_int_value_compare(const void *a, const void *b)
{
     int a_val = ((RandomValue *)a)->random_int;
     int b_val = ((RandomValue *)b)->random_int;

     return a_val - b_val;
}

/**
 * @brief Random integer value comparison for bsearch and qsort. 
 *
 * @param *a First random integer value to compare
 * @param *b Second random integer value to compare
 *
 * @return 0 if the random integer values are equal, positive if the
 *         second random integer value is greater than the first and
 *         negative otherwise.
 */
int imh_random_int_value_compare_back(const void *a, const void *b)
{
     int a_val = ((RandomValue *)a)->random_int;
     int b_val = ((RandomValue *)b)->random_int;

     return b_val - a_val;
}

/**
 * @brief Assigns, for each MinHash function, a random positive integer 
 *        and a uniformly distributed U(0,1) number to each possible 
 *        item in the database of lists.
 * 
 * @param dim Largest item value in the database of lists
 * @param tuple_size Number of MinHash values per tuple
 * @param permutations Random positive integers assigned to each possible item 
 */
void imh_generate_permutations(uint dim, uint tuple_size,
                               RandomValue *permutations)
{
     uint i, j;
     ullong int_rnd;
     double dbl_rnd;

     // generates random permutations by assigning a random value to each item
     // of the universal set
     for (i = 0; i < tuple_size; i++) { 
          for (j = 0; j < dim; j++) {
               int_rnd = genrand64_int64();
               dbl_rnd = (int_rnd >> 11) * (1.0 / MAX_SAFE_INT);
               permutations[i * dim + j].random_int = int_rnd;
               permutations[i * dim + j].random_double = dbl_rnd;
          }
     }
}

/**
 * @brief Assigns, for each MinHash function, a random positive integer 
 *        and a uniformly distributed U(0,1) number to each possible 
 *        item in the database of lists.
 * 
 * @param list List to be hashed
 * @param pernum Number of the permutation
 * @param permutation Random permutation
 * @param dim Dimensionality of the data (number of lists)
 */
ullong imh_compute_minhash(List *list, RandomValue *permutation)
{
     uint i;

     // get randomly assigned values for list
     // and find minimum value
     ullong min_int = permutation[list->data[0].item].random_int;
     double min_double = permutation[list->data[0].item].random_double;
     for (i = 1; i < list->size; i++) {
          if (min_double > permutation[list->data[i].item].random_double) {
               min_int = permutation[list->data[i].item].random_int;
               min_double = permutation[list->data[i].item].random_double;
          }
     }
     
     return min_int;
}

/**
 * @brief Universal hashing for getting a hash table index from the
 *        corresponding minhash tuple
 *
 * @param list List to be hashed
 * @param hash_table Hash table structure
 * @param hash_value Hash value
 * @param index Table index
 */
void imh_compute_univhash(List *list, HashTable *hash_table, uint *hash_value,
                          uint *index)
{
     uint i;
     ullong minhash;
     __uint128_t temp_index = 0;
     __uint128_t temp_hv = 0;

     // computes MinHash values
     for (i = 0; i < hash_table->tuple_size; i++) {
          uint perm = i * hash_table->dim;
          minhash = imh_compute_minhash(list, &hash_table->permutations[perm]);
          temp_index += ((ullong) hash_table->a[i]) * minhash;
          temp_hv += ((ullong) hash_table->b[i]) * minhash; 
     }
     
     // computes 2nd-level hash value and index (universal hash functions)
     *hash_value = (temp_hv % LARGEST_PRIME64);   
     *index = (temp_index % LARGEST_PRIME64) % hash_table->table_size;
}

/**
 * @brief Computes 2nd-level hash value of lists using open 
 *        adressing collision resolution and linear probing.
 * @todo Add other probing strategies.
 *
 * @param list List to be hashed
 * @param hash_table Hash table structure
 *
 * @return - index of the hash table
 */ 
uint imh_get_index(List *list, HashTable *hash_table)
{
     uint checked_buckets, index, hash_value;
     
     imh_compute_univhash(list, hash_table, &hash_value, &index);
     if (hash_table->buckets[index].items.size != 0) { 
          if (hash_table->buckets[index].hash_value != hash_value) {
               checked_buckets = 1;
               while (checked_buckets < hash_table->table_size) { 
                    index = ((index + 1) & (hash_table->table_size - 1));
                    if (hash_table->buckets[index].items.size != 0) {
                         if (hash_table->buckets[index].hash_value == hash_value)
                              break;   
                    } else {
                         hash_table->buckets[index].hash_value = hash_value;
                         break;
                    }
                    checked_buckets++;      
               }
               
               if (checked_buckets == hash_table->table_size) {
                    fprintf(stderr,"Error: The hash table is full!\n ");
                    exit(EXIT_FAILURE);
               }
          }
     } else {
          hash_table->buckets[index].hash_value = hash_value; 
     }
     
     return index;
}

/**
 * @brief Computes the number of sublists for each list in the database
 *
 * @param listdb Database of lists
 * @param sublist_size Size of the sublist
 * @param sublist_number Number of sublist for each list in the database
 *
 * @return Total number of sublists
 */ 
uint imh_get_sublist_numbers(ListDB *listdb, uint sublist_size, uint *sublist_number)
{
     uint i, sublistdb_size = 0;

     for (i = 0; i < listdb->size; i++) {
          sublist_number[i] = (uint) floor((double) listdb->lists[i].size / (double) sublist_size);
          sublistdb_size += sublist_number[i];
     }
     
     return sublistdb_size;
}

/**
 * @brief Generates a database of sublists from a database of lists
 *
 * @param sublist_number Number of sublist for each list in the database
 * @param sublistdb_size Total number of sublists
 * @param sublist_size Size of the sublist
 * @param sublistdb_ids IDs of the list from which each sublist was generated
 *
 * @return Database of sublists generated from a database of lists
 */ 
ListDB imh_create_sublistdb_from_listdb(ListDB *listdb,
                                        uint *sublist_number,
                                        uint sublistdb_size,
                                        uint sublist_size,
                                        uint *sublistdb_ids)
{
     uint i, j, k;
     uint sublist_index = 0;

     ListDB sublistdb = listdb_create(sublistdb_size, listdb->dim);
          
     // creates a database of sublists from a given database of lists
     for (i = 0; i < listdb->size; i++) {
          // Randomly permutes the list to create the sublists
          RandomValue *permutation = (RandomValue *) malloc(listdb->lists[i].size
                                                            * sizeof(RandomValue));
          for (j = 0; j < listdb->lists[i].size; j++) {
               permutation[j].random_int = j;
               permutation[j].random_double = genrand64_real1();
          }
          qsort(permutation,
                listdb->lists[i].size,
                sizeof(RandomValue),
                imh_random_double_value_compare_back);
          
          // create database of sublists
          for (j = 0; j < sublist_number[i]; j++) {
               List newsublist;
               list_init(&newsublist);
               for (k = sublist_size * j; k < sublist_size * (j + 1); k++) {
                    list_push(&newsublist,
                              listdb->lists[i].data[permutation[k].random_int]);
               }

               sublistdb.lists[sublist_index] = newsublist;
               sublistdb_ids[sublist_index] = i;
               sublist_index++;
          }

          // the last sublist takes the left elements, if any
          for (j = sublist_size * sublist_number[i]; j < listdb->lists[i].size; j++) {
                    list_push(&sublistdb.lists[sublist_index - 1],
                              listdb->lists[i].data[permutation[k].random_int]);
          }
          
          free(permutation);
     }
     
     listdb_apply_to_all(&sublistdb, list_sort_by_item);
     
     return sublistdb;
}

/**
 * @brief Stores lists in the hash table.
 *
 * @param list List to be hashed
 * @param id ID of the list
 * @param hash_table Hash table
 */ 
void imh_store_list(List *list, uint id, HashTable *hash_table)
{
     uint index;
   
     // get index of the hash table
     index = imh_get_index(list, hash_table);

     if (hash_table->buckets[index].items.size == 0) { // mark used bucket
          Item new_used_bucket = {index, 1};
          list_push(&hash_table->used_buckets, new_used_bucket);
     }

     // store list id in the hash table
     Item new_item = {id, 1};
     list_push(&hash_table->buckets[index].items, new_item);
}

/**
 * @brief Stores lists in the hash table.
 *
 * @param listdb Database of lists to be hashed
 * @param hash_table Hash table
 * @param indices Indices of the used buckets
 */ 
void imh_store_sublistdb(ListDB *sublistdb, uint *sublistdb_ids, HashTable *hash_table)
{
     uint i;   
         
     // hash all lists in the database
     for (i = 0; i < sublistdb->size; i++)
          if (sublistdb ->lists[i].size > 0)
               imh_store_list(&sublistdb->lists[i], sublistdb_ids[i], hash_table);
}
