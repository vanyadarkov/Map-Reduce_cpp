#include "../utils/thread_utils.h"

void * map_reduce_function(void * arg) {
    thread_arg_t * thread_arg = (thread_arg_t *)arg;
    if(thread_arg->type == THREAD_TYPE_MAPPER) {
        string line;
        bool running = true;
        while(running) {
            // Lock pe citirea din fisierul comun
            pthread_mutex_lock(thread_arg->mutex); 
            // Daca urmeaza sa citim EOF -> terminam citirea
            if((*(thread_arg->input_file)).peek() == EOF) {
                running = false;
            }
            getline((
                *(thread_arg->input_file)), line);
            pthread_mutex_unlock(thread_arg->mutex);
            // Daca linie goala - treci la urmatoarea
            if(line.empty()) {
                continue;
            }
            // Daca am putut sa citim -> spunem mapperului ce fisier proceseaza si incepem
            // operatia MAP
            if(running) {
                thread_arg->processed_files.push_back(line);
                thread_arg->mapper_function(arg);
            }
        }
    }
    // Asteptam toate threadurile sa termine (in special, MAPPER)
    pthread_barrier_wait(thread_arg->barrier);
    // Incepem operatia reduce
    if(thread_arg->type == THREAD_TYPE_REDUCER) {
        thread_arg->reducer_function(arg);
    }
    pthread_exit(NULL);
    return NULL;
}

thread_arg_t* thread_arg_init(  int id, 
                                int type, 
                                pthread_mutex_t * mutex, 
                                pthread_barrier_t * barrier, 
                                ifstream* input_file, 
                                void * thread_util, 
                                int number_of_mappers, 
                                int number_of_reducers, 
                                void (*mapper_function)(void *), void (*reducer_function)(void *)) {

    thread_arg_t *thread_arg = (thread_arg_t *)calloc(1, sizeof(thread_arg_t));
    DIE(thread_arg == NULL, "Error calloc");

    thread_arg->id = id;
    thread_arg->type = type;
    thread_arg->mutex = mutex;
    thread_arg->barrier = barrier;
    thread_arg->input_file = input_file;
    thread_arg->number_of_mappers = number_of_mappers;
    thread_arg->number_of_reducers = number_of_reducers;
    thread_arg->thread_util = thread_util;
    thread_arg->mapper_function = mapper_function;
    thread_arg->reducer_function = reducer_function;
    return thread_arg;
}

mapper_utils_t * mapper_utils_init(vector<vector<vector<unsigned int>>>* threads_results, vector<vector<unsigned int>> lookup_table) {
    mapper_utils_t * mapper_utils = (mapper_utils_t *)calloc(1, sizeof(mapper_utils_t));
    DIE(mapper_utils == NULL, "Error calloc");

    mapper_utils->lookup_table = lookup_table;
    mapper_utils->threads_results = threads_results;

    return mapper_utils;
}

reducer_utils_t * reducer_utils_init(vector<vector<vector<unsigned int>>>* threads_results) {
    reducer_utils_t * reducer_utils = (reducer_utils_t *)calloc(1, sizeof(reducer_utils_t));
    DIE(reducer_utils == NULL, "Error calloc");

    reducer_utils->threads_results = threads_results;

    return reducer_utils;
}

