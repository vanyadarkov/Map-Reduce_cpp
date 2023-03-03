#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <pthread.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <limits.h>
#include <math.h>
#include <unordered_map>
#include <fstream>

#include <bits/stdc++.h>
#include <set>

using namespace std;

constexpr int THREAD_TYPE_MAPPER = 0;
constexpr int THREAD_TYPE_REDUCER = 1;

typedef struct thread_argument {
    int id;
    int type;
    pthread_mutex_t * mutex;
    pthread_barrier_t * barrier;
    ifstream* input_file;
    int number_of_mappers;
    int number_of_reducers;
    vector<string> processed_files;
    void (*mapper_function)(void *);
    void (*reducer_function)(void *);
    void * thread_util;
} thread_arg_t;

typedef struct mapper_utils {
    vector<vector<vector<unsigned int>>> *threads_results;
    vector<vector<unsigned int>> lookup_table;
} mapper_utils_t;

typedef struct reducer_utils {
    vector<vector<vector<unsigned int>>> *threads_results;
} reducer_utils_t;

// MACRO folosit la verificarea unei conditii(care ar putea duce la eroare),

/**
 * @brief  Macro folosit la verificarea unei conditii care ar putea duce la eroare si afisarea
 * mesajului de eroare daca este TRUE si inchiderea programului
 * @note   
 * @retval Inchiderea programului daca condition e TRUE
 */
#define DIE(condition, message) \
	do { \
		if ((condition)) { \
			fprintf(stderr, "[(%s:%d)]: %s\n", __FILE__, __LINE__, (message)); \
			perror(""); \
			exit(1); \
		} \
	} while (0)

/**
 * @brief  Functie ce se ocupa de initializarea unui lookup table pentru puteri (nr de puteri = number of rows) si 
 * de popularea ei
 * @note   
 * @param  number_of_rows: cate puteri ne intereseaza, 2 -> puterea 2 si puterea 3
 * @retval lookup table-ul populat
 */
vector<vector<unsigned int>> get_lookup_table(int number_of_rows);

/**
 * @brief  Initializarea unui thred argument
 * @note   
 * @param  id: id-ul threadului
 * @param  type: tipul sau
 * @param  mutex: mutexul
 * @param  barrier: bariera
 * @param  input_file: fisierul initial de input pentru mapper
 * @param  thread_util: thread util, care poate fi pentru mapper sau reducer, in functie de tip
 * @param  number_of_mappers: 
 * @param  number_of_reducers: 
 * @param  (*mapper_function): functia destinata mapperului
 * @param (*reducer_function): functia destinata reducerului 
 * @retval thread_argument initializat si alocat dinamic
 */
thread_arg_t* thread_arg_init(  int id, 
                                int type, 
                                pthread_mutex_t * mutex, 
                                pthread_barrier_t * barrier, 
                                ifstream* input_file, 
                                void * thread_util, 
                                int number_of_mappers, 
                                int number_of_reducers, 
                                void (*mapper_function)(void *), void (*reducer_function)(void *));

/**
 * @brief Functie ce se ocupa de initializarea unei structuri mapper_util, folosita la thread util.
 * Stocheaza informatia necesara pentru ca un mapper sa isi execute sarcina.
 * @note   
 * @param  threads_results: Structura de stocare unde fiecare thread mapper isi va stoca rezultatele prelucrarii
 * @param  lookup_table: Lookup table pentru a alege valorile citite din fisier ce corespund maparii
 * @retval structura mapper util initializata si alocata dinamic
 */
mapper_utils_t * mapper_utils_init(vector<vector<vector<unsigned int>>>* threads_results, vector<vector<unsigned int>> lookup_table);

/**
 * @brief  Functie ce se ocupa de initializarea unei structuri reducer util, folosita la thread util.
 * Stocheaza informatia necesara ca un reducer sa isi execute sarcina
 * @note   
 * @param  threads_results: Structura in care fiecare mapper si-a stocat rezultatele maparii si din care
 * urmeaza reducerul sa citeasca valorile sale pentru prelucrare
 * @retval structura reducer util initializata si alocata dinamic
 */
reducer_utils_t * reducer_utils_init(vector<vector<vector<unsigned int>>>* threads_results);

/**
 * @brief  Functia map reduce, unde are loc distribuirea taskurilor pentru reducer / mapper.
 * @note   Este functia care se da ca argument threadului la creare
 * @param  arg: argumentul pentru thread (particular, thread_arg_t)
 * @retval valoarea de return din thread (by default, NULL) 
 */
void * map_reduce_function(void * arg);

#endif