/**
 * @file imhcmd.c 
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
 * @brief This program performs nearest neighbor search on lists using Intersection Min-Hashing
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <inttypes.h>
#include "iminhash.h"
#include "imhsearch.h"

/**
 * @brief Prints help in screen.
 */
void usage(void)
{
     printf("usage: imhcmd [OPTIONS]... [LISTDB_FILE] [QUERY_FILE] [OUTPUT_FILE]\n"
            "Performs nearest neighbor search on lists using Intersection Min-Hashing\n"
            "Options:\n"
            "       --help\t\t\tPrints this help\n"
            "   -r, --tuple_size[=3]\t\tNumber of hash values per tuple\n"
            "   -l, --number_of_tables[=50]\tNumber of tables in search index\n"
            "   -t, --table_size[=20(2^20)]\tNumber of buckets in hash table (powers of 2)\n"
            "   -s, --subset_size[=3]\tSize of subsets to create from database of lists\n");
}

/**
 * ======================================================
 * @brief Main function
 * ======================================================
 */
int main(int argc, char **argv)
{     
     uint tuple_size = 3; // default tuple size
     uint number_of_tables = 50; // default number of tables
     uint table_size = 1048576; // default table size
     uint sublist_size = 3; // default sublist size
     unsigned long long seed = 123456; // default seed
     char *listdb_file, *query_file, *output; 
     
     int op;
     int option_index = 0;
     
     static struct option long_options[] =
          {
               {"help", no_argument, 0, 'h'},
               {"tuple_size", required_argument, 0, 'r'},
               {"number_of_tables", required_argument, 0, 'l'},
               {"table_size", required_argument, 0, 't'},
               {"sublist_size", required_argument, 0, 's'},
               {"seed", required_argument, 0, 'e'},
               {0, 0, 0, 0}
          };

     //Command-line option parser
     while((op = getopt_long( argc, argv, "hr:l:t:s:e:", long_options, 
                              &option_index)) != -1){
          int this_option_optind = optind ? optind : 1;
          switch (op)
          {
          case 0:
               break;
          case 'h':
               usage();
               exit(EXIT_SUCCESS);
               break;
          case 'r':
               tuple_size = atoi(optarg);
               break;
          case 'l':
               number_of_tables = atoi(optarg);
               break;
          case 't':
               table_size = (uint) pow(2, atoi(optarg));
               break;
          case 's':
               sublist_size = atoi(optarg);
               break;
          case 'e':
               sublist_size = (unsigned long long) atoll(optarg);
               break;
          case '?':
               fprintf(stderr,"Error: Unknown options.\n"
                       "Try `imhcmd --help' for more information.\n");
               exit(EXIT_FAILURE);
          default:
               abort ();
          }
     }
     if (optind + 3 == argc){
          imh_init_rng(seed);

          listdb_file = argv[optind++];
          query_file = argv[optind++];
          output = argv[optind++];

          printf("Reading database of lists from %s . . .\n", listdb_file);
          ListDB listdb = listdb_load_from_file(listdb_file);
          printf("Number of lists: %d\nDimensionality: %d\n", listdb.size, listdb.dim);

          printf("Reading queries from %s . . .\n", query_file);
          ListDB queries = listdb_load_from_file(query_file);

          printf("Creating hash index with %u tables "
                 "(tuple size = %u, table size = %u, sublist size = %u)\n",
                 number_of_tables, tuple_size, table_size, sublist_size);
          HashIndex hash_index = imhsearch_build(&listdb,
                                                 number_of_tables,
                                                 tuple_size,
                                                 table_size,
                                                 sublist_size);

          printf("Searching for neighbors\n");
          ListDB neighbors = imhsearch_query_multi(&queries, &hash_index);

          printf("Sorting neighbors by overlap\n");
          uint i;
          for (i = 0; i < neighbors.size; i++) 
               imhsearch_sort_custom(&queries.lists[i], &neighbors.lists[i], &listdb, list_overlap);

          printf("Saving neighbors in %s\n", output);
          listdb_save_to_file(output, &neighbors);
     } else {
          if (optind + 3 > argc)
               fprintf(stderr, "Error: Missing arguments.\n"
                       "Try `smhcmd --help' for more information.\n");
          else
               fprintf(stderr, "Error: Unknown arguments.\n"
                       "Try `smhcmd --help' for more information.\n");
          exit(EXIT_FAILURE);
     }
     
     return 1;
}
