#include "../utils/utils.h"

vector<vector<unsigned int>> get_lookup_table(int number_of_rows){
    vector<vector <unsigned int>> lookup_table;
    for(int i = 0; i < number_of_rows; i++) {
        vector<unsigned int> aux;
        for(int j = 1; ; j++) {
            long aux_res = pow(j, i + 2);
            if(aux_res > UINT_MAX) {
                break;
            }
            aux.push_back(aux_res);
        }
        lookup_table.push_back(aux);
    }
    return lookup_table;
}

 