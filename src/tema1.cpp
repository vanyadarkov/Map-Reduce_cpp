#include "../utils/utils.h"

/**
 * @brief  functia pentru mapper, deci ceea ce va face mapper este descris in functia asta
 * @note   ! se foloseste doar ca parametru pentru structura thread_arg_t
 * @param  arg: argumentul threadului
 * @retval None
 */
void mapper_function(void * arg) {
    thread_arg_t * thread_arg = (thread_arg_t *)arg;
    mapper_utils_t * mapper_utils = (mapper_utils_t *)thread_arg->thread_util;
    ifstream input_data_file(thread_arg->processed_files[thread_arg->processed_files.size() - 1]);
    int number;
    // skip first line
    input_data_file >> number;
    while (input_data_file >> number) {
        if(number <= 0) continue;
        for(int i = 0; i < thread_arg->number_of_reducers; i++) {
            if(binary_search(mapper_utils->lookup_table[i].begin(), mapper_utils->lookup_table[i].end(), number)) {
                (*mapper_utils->threads_results)[thread_arg->id][i].push_back(number);
            }
        }
    }
}

/**
 * @brief functia pentru reducer, deci ceea ce va face reducer este descris in functia asta
 * @note  ! se foloseste doar ca parametru pentru structura thread_arg_t
 * @param  arg: argumentul threadului
 * @retval None
 */
void reducer_function(void *arg) {
    thread_arg_t * thread_arg = (thread_arg_t *)arg;
    reducer_utils_t * reducer_utils = (reducer_utils_t *)thread_arg->thread_util;
    string output = "out" + std::to_string(thread_arg->id + 2) + ".txt";
    ofstream output_reducer_file(output);
    set<unsigned int> results;
    for(vector<vector<unsigned int>> mapper_res : (*reducer_utils->threads_results)) {
        for(unsigned int val : mapper_res[thread_arg->id]) {
            results.insert(val);
        }
    }
    output_reducer_file << results.size();
    output_reducer_file.close();
}

int main(int argc, char *argv[]) {
    DIE(argc != 4, "Wrong number of command line arguments");
    int return_code = 0;

    int number_of_mappers = stoi(argv[1]);
    int number_of_reducers = stoi(argv[2]);
    // Deschidem fisierul
    ifstream input_file(argv[3]);
    DIE(input_file.is_open() == false, "Eroare deschidere fisier intrare (argumentul 3)");
    string line;
    // Trecem peste prima linie
    getline(input_file, line);

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_barrier_t barrier;
    return_code = pthread_barrier_init(&barrier, NULL, number_of_mappers + number_of_reducers);
    DIE(return_code != 0, "Error init barrier");

    // Initializarea structurii de stocare
    vector<vector<vector<unsigned int>>> threads_results;
    for(int i = 0; i < number_of_mappers; i++) {
        vector<vector<unsigned int>> mapper_list;
        for(int j = 0; j < number_of_reducers; j++) {
            vector<unsigned int> perfect_pows;
            mapper_list.push_back(perfect_pows);
        }
        threads_results.push_back(mapper_list);
    }
    vector<vector<unsigned int>> lookup_table = get_lookup_table(number_of_reducers);

    unordered_map<pthread_t, thread_arg_t *> threads;
    int thread_id = 0;
    // Initializarea threadurilor, mai intai se initializeaza MAPPER, dupa reducer
    for(int i = number_of_mappers, j = number_of_reducers; j > 0 || i > 0; ) {
        void *thread_util;
        int type = THREAD_TYPE_MAPPER;
        if(i > 0) {
            i--;
            thread_id = i;
            type = THREAD_TYPE_MAPPER;
            thread_util = mapper_utils_init(&threads_results, lookup_table);
        } else if (j > 0) {
            j--;
            thread_id = j; 
            type = THREAD_TYPE_REDUCER;
            thread_util = reducer_utils_init(&threads_results);
        }
        thread_arg_t * mapper_arg = thread_arg_init(thread_id, type, &mutex, &barrier, &input_file, thread_util, 
                                                    number_of_mappers, number_of_reducers, 
                                                    mapper_function, reducer_function);
        
        pthread_t thread;
        return_code = pthread_create(&thread, NULL, map_reduce_function, mapper_arg);
        DIE(return_code != 0, "Error creating thread");
        threads.insert(make_pair(thread, mapper_arg));
    }

    // Asteptam threadurile si facem join
    // Eliberam fiecare structura auxiliara care e alocata dinamic
    for(auto pair : threads) {
        pthread_join(pair.first, NULL);
        thread_arg_t * thread_arg = pair.second;
        free(thread_arg->thread_util);
        free(thread_arg);
    }

    // Inchidem fisierul si eliberam structurile de sincronizare
    input_file.close();
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);

    return 0;
}