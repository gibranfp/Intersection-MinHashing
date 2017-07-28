# Intersection-MinHashing
This is a simple extension of Min-Hashing for performing approximate set intersection search.

## Installation
Clone and compile the library:
~~~~
git clone https://github.com/gibranfp/Intersection-MinHashing.git
cd Intersection-MinHashing
mkdir build
cd build
cmake ..
make
~~~~

## Usage
After compilation, the command `imhcmd` will be created inside the `bin` directory. This command takes as input a file (a database of lists) with the database and a file (a database of lists) with the queries, and outputs a file (a database of lists) with the ids of the nearest lists to each query list sorted by proximity (overlap coefficient). The command takes the following form:

~~~~
imhcmd [OPTIONS]... [LISTDB_FILE] [QUERY_FILE] [OUTPUT_FILE]
Valid OPTIONS:
Options:
       --help			        Prints this help
   -r, --tuple_size[=3]		    Number of hash values per tuple
   -l, --number_of_tables[=50]	Number of tables in search index
   -t, --table_size[=20(2^20)]	Number of buckets in hash table (powers of 2)
   -s, --subset_size[=3]	    Size of subsets to create from database of lists
~~~~

The format of a file with a database of lists is as follows:
~~~~
size_of_list_1 item1_1:freq1_1 item2_1:freq2_1 ...
size_of_list_1 item1_2:freq1_22 item2_2:freq2_2 ...
...                        ...
size_of_list_N item1_N:freq1_N item2_N:freq2_N ...
~~~~

For example, the following is the format of a file with a database of 20 lists:
~~~~
5 6:1 12:1 13:1 15:1 17:1
7 4:1 5:1 6:1 9:1 13:1 16:1 17:1
6 2:1 3:1 4:1 5:1 7:1 14:1
6 5:1 6:1 7:1 13:1 16:1 17:1
6 3:1 8:1 9:1 10:1 12:1 19:1
7 1:1 7:1 8:1 14:1 15:1 17:1 18:1
5 4:1 5:1 10:1 12:1 19:1
6 2:1 3:1 7:1 9:1 10:1 15:1
6 4:1 6:1 10:1 11:1 13:1 15:1
6 0:1 1:1 4:1 5:1 9:1 19:1
5 0:1 4:1 8:1 9:1 18:1
5 0:1 3:1 6:1 8:1 19:1
5 1:1 4:1 13:1 17:1 19:1
4 0:1 1:1 12:1 16:1
5 1:1 9:1 11:1 15:1 17:1
5 3:1 4:1 10:1 16:1 17:1
5 4:1 7:1 8:1 11:1 15:1
4 2:1 10:1 13:1 16:1
3 1:1 4:1 5:1
4 0:1 8:1 16:1 19:1
~~~~

Queries are also given as a database of lists. For example:
~~~~
6 0:1 1:1 10:1 11:1 13:1 14:1
4 1:1 5:1 10:1 11:1
6 0:1 2:1 4:1 5:1 6:1 16:1
5 0:1 2:1 4:1 12:1 16:1
3 8:1 13:1 19:1
~~~~

To find the nearest lists from the above database of 20 lists (stored in the file `listdb.txt`) to each of the above 5 query lists (stored in the file `queries.txt`) using `imhcmd` do:
~~~~
./imhcmd -r 3 -l 30 -t 8 -s 2 listdb.txt queries.txt output.txt
~~~~

The command will create the file  `output.txt`, which should look as follows:
~~~~
6 8:1 17:2 6:1 15:1 4:1 7:1
8 18:2 6:1 8:2 9:5 4:1 7:2 15:2 17:1
11 1:2 18:1 2:3 9:1 17:1 6:1 10:1 15:1 8:1 12:1 7:1
13 13:1 17:1 1:3 2:4 6:1 9:2 10:2 15:1 18:1 7:1 8:1 11:1 12:1
7 4:10 12:9 0:1 1:1 3:1 8:1 17:1
~~~~
