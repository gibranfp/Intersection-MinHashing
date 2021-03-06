/**
 * @file imhsearch.c
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
 * @brief Functions to perform intersection search via Min-Hashing
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "array_lists.h"
#include "listdb.h"
#include "imhsearch.h"

/**
 * @Brief Prints head of a hash index structure
 *
 * @param hash_index Hash index structure
 */
void imhsearch_print_index_head(HashIndex *hash_index)
{
     printf("========== Hash Index =========\n");
     printf("Number of tables: %u\n"
            "Table size: %d\n"
            "Tuple size: %d\n"
            "Dimensionality: %d\n"
            "Sublist size: %d\n",
            hash_index->number_of_tables,
            hash_index->hash_tables[0].table_size, 
            hash_index->hash_tables[0].tuple_size,
            hash_index->hash_tables[0].dim,
            hash_index->hash_tables[0].sublist_size); 
}

/**
 * @Brief Prints each tables of a hash index structure
 *
 * @param hash_index Hash index structure 
 */
void imhsearch_print_index_tables(HashIndex *hash_index)
{
     uint i;
     for (i = 0; i < hash_index->number_of_tables; i++) {
          printf("========== Table %u =========== \n", i);
          imh_print_table(&hash_index->hash_tables[i]);
     }
}

/**
 * @brief Creates a hash index and stores a database of lists in each hash table
 *
 * @param listdb Database of lists to be hashed
 * @param number_of_tables Number of tables
 * @param tuple_size Number of hash values per tuple
 * @param table_size Number of buckets in the hash table
 *
 * @returns Hash index
 */
HashIndex imhsearch_build(ListDB *listdb, uint number_of_tables, uint tuple_size,
                          uint table_size, uint sublist_size)
{
     // Generates sublists
     uint *sublist_number = (uint *) malloc(listdb->size * sizeof(uint));
     uint sublistdb_size = imh_get_sublist_numbers(listdb,
                                                   sublist_size,
                                                   sublist_number);
     uint *sublistdb_ids = (uint *) malloc(sublistdb_size * sizeof(uint));
     ListDB sublistdb = imh_create_sublistdb_from_listdb(listdb,
                                                         sublist_number,
                                                         sublistdb_size,
                                                         sublist_size,
                                                         sublistdb_ids);

     // Creates hash index
     HashIndex hash_index;
     hash_index.number_of_tables = number_of_tables;
     hash_index.hash_tables = (HashTable *) malloc(number_of_tables * sizeof(HashTable));

     // Stores lists in each hash table 
     uint i;
     for (i = 0; i < number_of_tables; i++) {
          hash_index.hash_tables[i] = imh_create_table(table_size,
                                                       tuple_size,
                                                       listdb->dim,
                                                       sublist_size);
          imh_generate_permutations(listdb->dim, tuple_size, hash_index.hash_tables[i].permutations);
          imh_store_sublistdb(&sublistdb, sublistdb_ids, &hash_index.hash_tables[i]);
     }

     return hash_index;
}

/**
 * @brief Sorts neighbors found by Intersection Min-Hashing (imhsearch_query) using a score.
 *
 * @param query Query list
 * @param neighbors List of IDs of the neighbors found by Intersectio Min-Hashing (imhsearch_query)
 * @param listdb Database of lists stored in the hash tables
 * @param func Function to compute score of each neighbor found (e.g. Jaccard similarity)
 *             Given as a function pointer
 */
void imhsearch_sort_custom(List *query, List *neighbors, ListDB *listdb, double (*func)(List *, List *))
{
     Score *scores = malloc(neighbors->size * sizeof(Score));

     uint i;
     for (i = 0; i < neighbors->size; i++) {
          scores[i].index = i;
          scores[i].value = func(query, &listdb->lists[neighbors->data[i].item]);
     }

     qsort(scores, neighbors->size, sizeof(Score), list_score_compare_back);

     List sorted = list_create(neighbors->size);
     for (i = 0; i < neighbors->size; i++) 
          sorted.data[i] = neighbors->data[scores[i].index];

     list_destroy(neighbors);
     free(scores);
     
     *neighbors = sorted;
}

/**
 * @brief Queries the hash tables of an hash index structure with a given list
 *
 * @param query Query list
 * @param hash_index Index structure with hash tables
 *
 * @return List of neighbors found
 */
List imhsearch_query(List *query, HashIndex *hash_index)
{
     List neighbors;
     list_init(&neighbors);

     uint i;
     for (i = 0; i < hash_index->number_of_tables; i++) {
          uint index = imh_get_index(query, &hash_index->hash_tables[i]);
          list_append(&neighbors, &hash_index->hash_tables[i].buckets[index].items);
     }

     list_sort_by_item(&neighbors);
     list_unique(&neighbors);
          
     return neighbors;
}

/**
 * @brief Queries the hash tables of an hash index structure with a given database of lists
 *
 * @param queries Queries given as a database of lists 
 * @param hash_index Index structure with hash tables
 *
 * @return Neighbors found (database of lists) for each query 
 */

ListDB imhsearch_query_multi(ListDB *queries, HashIndex *hash_index)
{
     ListDB neighbors = listdb_create(queries->size, queries->dim);

     uint i;
     for (i = 0; i < queries->size; i++)
          neighbors.lists[i] = imhsearch_query(&queries->lists[i], hash_index);

     return neighbors;
}
