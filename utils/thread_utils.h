#ifndef __THREAD_UTILS__
#define __THREAD_UTILS__

#include "utils.h"

thread_arg_t* thread_arg_init(int id, 
                                int type, 
                                pthread_mutex_t * mutex, 
                                pthread_barrier_t * barrier, 
                                ifstream* input_file, 
                                void * thread_util, 
                                int number_of_mappers, 
                                int number_of_reducers, 
                                void (*mapper_function)(void *), void (*reducer_function)(void *));
mapper_utils_t * mapper_utils_init(vector<vector<vector<unsigned int>>>* threads_results, vector<vector<unsigned int>> lookup_table);
reducer_utils_t * reducer_utils_init(vector<vector<vector<unsigned int>>>* threads_results);
void * map_reduce_function(void * arg);

#endif