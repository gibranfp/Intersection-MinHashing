#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "listdb.h"
#include "iminhash.h"

#define red "\033[0;31m"
#define cyan "\033[0;36m"
#define green "\033[0;32m"
#define blue "\033[0;34m"
#define brown "\033[0;33m"
#define magenta "\033[0;35m"
#define none "\033[0m"

#define MAX_LIST_SIZE 10
#define ELEMENT_MAX_VALUE 15


void test_create_sublistdb(uint sublist_size)
{
     ListDB listdb = listdb_random(50,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);

     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = 1;

     listdb_print(&listdb);
     
     uint *sublist_number = (uint *) malloc(listdb.size * sizeof(uint));
     uint sublistdb_size = imh_get_sublist_numbers(&listdb,
                                                   sublist_size,
                                                   sublist_number);

     uint *sublistdb_ids = (uint *) malloc(sublistdb_size * sizeof(uint));
     ListDB sublistdb = imh_create_sublistdb_from_listdb(&listdb,
                                                         sublist_number,
                                                         sublistdb_size,
                                                         sublist_size,
                                                         sublistdb_ids);
     
     printf("Generated sublists\n");
     for (i = 0; i < sublistdb.size; i++) {
          printf("(%d) [  %d  ] ", sublistdb_ids[i], i);
          list_print(&sublistdb.lists[i]);
     }
}

void test_store_listdb(uint sublist_size)
{
     ListDB listdb = listdb_random(50,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);

     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = 1;

     printf("Database of lists\n");
     listdb_print(&listdb);

     uint *sublist_number = (uint *) malloc(listdb.size * sizeof(uint));
     uint sublistdb_size = imh_get_sublist_numbers(&listdb,
                                                   sublist_size,
                                                   sublist_number);

     uint *sublistdb_ids = (uint *) malloc(sublistdb_size * sizeof(uint));
     ListDB sublistdb = imh_create_sublistdb_from_listdb(&listdb,
                                                         sublist_number,
                                                         sublistdb_size,
                                                         sublist_size,
                                                         sublistdb_ids);

     printf("Generated sublists\n");
     for (i = 0; i < sublistdb.size; i++) {
          printf("(%d) [  %d  ] ", sublistdb_ids[i], i);
          list_print(&sublistdb.lists[i]);
     }

     HashTable hash_table = imh_create_table(2048, 3, listdb.dim, sublist_size);
     imh_generate_permutations(hash_table.dim, hash_table.tuple_size, hash_table.permutations); 
     imh_store_sublistdb(&sublistdb, sublistdb_ids, &hash_table);

     printf("Buckets\n");
     for (i = 0; i < hash_table.used_buckets.size; i++) {
          printf("[  %u  ] ", hash_table.used_buckets.data[i].item);
          list_print(&hash_table.buckets[hash_table.used_buckets.data[i].item].items);
     }
}

int main(int argc, char **argv)
{
     imh_init_rng(1123123123);
     
     /* test_create_sublistdb(2); */
     test_store_listdb(2);
 
     return 0;
}
