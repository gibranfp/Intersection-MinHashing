#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "listdb.h"
#include "imhsearch.h"

#define red "\033[0;31m"
#define cyan "\033[0;36m"
#define green "\033[0;32m"
#define blue "\033[0;34m"
#define brown "\033[0;33m"
#define magenta "\033[0;35m"
#define none "\033[0m"

#define MAX_LIST_SIZE 10
#define ELEMENT_MAX_VALUE 15



void test_build(uint sublist_size)
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

     HashIndex hash_index = imhsearch_build(&listdb, 10, 3, 2048, sublist_size);
     imhsearch_print_index_head(&hash_index);
     imhsearch_print_index_tables(&hash_index);
}

void test_query(uint sublist_size)
{
     ListDB listdb = listdb_random(50,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);

     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = 1;

     printf("========== Database of lists ==========\n");
     listdb_print(&listdb);

     HashIndex hash_index = imhsearch_build(&listdb, 20, 3, 256, sublist_size);
     imhsearch_print_index_head(&hash_index);
     imhsearch_print_index_tables(&hash_index);

     List query = list_random(8, 20);
     list_sort_by_item(&query);
     list_unique(&query);
     printf("========== Query list ==========\n");
     list_print(&query);

     printf("========== Neighbors ==========\n");
     List neighbors = imhsearch_query(&query, &hash_index);
     imhsearch_sort_custom(&query, &neighbors, &listdb, list_overlap);
     
     for (i = 0; i < neighbors.size; i++) {
          printf("[  %u  ] ", neighbors.data[i].item);
          list_print(&listdb.lists[neighbors.data[i].item]);
     }

}

void test_query_multi(uint sublist_size)
{
     ListDB listdb = listdb_random(50,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);

     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = 1;

     printf("========== Database of lists ==========\n");
     listdb_print(&listdb);

     HashIndex hash_index = imhsearch_build(&listdb, 20, 3, 256, sublist_size);
     imhsearch_print_index_head(&hash_index);
     imhsearch_print_index_tables(&hash_index);

     ListDB queries = listdb_random(10, 8, 20);
     listdb_delete_smallest(&queries, 3);
     listdb_apply_to_all(&queries, list_sort_by_item);
     listdb_apply_to_all(&queries, list_unique);

     for (i = 0; i < queries.size; i++)
          for (j = 0; j < queries.lists[i].size; j++)
               queries.lists[i].data[j].freq = 1;

     printf("========== Queries ==========\n");
     listdb_print(&queries);

     printf("========== Neighbors ==========\n");
     ListDB neighbors = imhsearch_query_multi(&queries, &hash_index);

     for (i = 0; i < neighbors.size; i++) {
          imhsearch_sort_custom(&queries.lists[i], &neighbors.lists[i], &listdb, list_overlap);
          printf("[  %u  ] ", i);
          list_print(&neighbors.lists[i]);
     }
}
int main(int argc, char **argv)
{
     imh_init_rng(1123123123);
     
     /* test_build(2); */
     /* test_query(2); */
     test_query_multi(2);
 
     return 0;
}
