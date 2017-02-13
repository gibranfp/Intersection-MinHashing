/**
 * @file imhsearch.c
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
 * @brief Functions to perform intersection search via Min-Hashing
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <smh/array_lists.h>
#include <smh/listdb.h>
#include <smh/ifindex.h>
#include "imh/imhsearch.h"

/**
 * @brief Builds a hash index
 *
 * @param listdb Database of lists to be hashed
 * @param number_of_tables Number of tables
 * @param tuple_size Number of hash values per tuple
 * @param table_size Number of buckets in the hash table
 */
HashIndex imhsearch_build(ListDB *listdb, uint number_of_tables, uint tuple_size, uint table_size)
{
     HashIndex hash_index;
     hash_index.number_of_tables = number_of_tables;
     hash_index.hash_tables = (HashTable *) malloc(number_of_tables * sizeof(HashTable));
     uint *indices = (uint *) malloc(listdb->size * sizeof(uint));

     uint i;
     for (i = 0; i < number_of_tables; i++) {
          hash_index.hash_tables[i] = imh_create(table_size, tuple_size, listdb->dim);
          imh_generate_permutations(listdb->dim, tuple_size, hash_index.hash_tables[i].permutations);
          imh_store_listdb(listdb, &hash_index.hash_tables[i], indices);
     }

     return hash_index;
}

/**
 * @brief Makes a query from the MinHash structure
 *
 * @param query Query list
 * @param hash_index Index of hash tables
 */
List imhsearch_query(List *query, HashIndex *hash_index)
{
     List neighbors;
     list_init(&neighbors);

     uint i;
     for (i = 0; i < hash_index->number_of_tables; i++) {
          uint index = mh_compute_minhash(query, hash_index->hash_tables[i].permutations);
          list_append(&neighbors, &hash_index->hash_tables[i].buckets[index].items);
     }

     list_sort_by_item(&neighbors);
     list_unique(&neighbors);
     
     return neighbors;
}

/**
 * @brief Sorts results of queries
 *
 * @param query Query list
 * @param neighbors ID's of neighbors found by Min-Hashing
 * @param listdb Database of lists
 * @param func Function to compute score of each neighbor found
 */
void imhsearch_sort_custom(List *query, List *neighbors, ListDB *listdb, double (*func)(List *, List *))
{
     Score *scores = malloc(neighbors->size);

     uint i;
     for (i = 0; i < neighbors->size; i++){
          scores[i].index = i;
          scores[i].value = func(query, &listdb->lists[neighbors->data[i].item]);
     }

     qsort(neighbors->data, neighbors->size, sizeof(Score), listdb_score_compare);
}

/**
 * @brief Makes multiple queries from the MinHash structure
 *
 * @param queries Database of lists to query
 * @param hash_index Index of hash tables for searching
 *
 * @return Database of lists with the results of the queries
 */
ListDB imhsearch_query_multi(ListDB *queries, HashIndex *hash_index)
{
     ListDB neighbors = listdb_create(queries->size, queries->dim);

     uint i;
     for (i = 0; i < queries->size; i++)
          neighbors.lists[i] = imhsearch_query(&queries->lists[i], hash_index);

     return neighbors;
}
