#include "ff.h"

#include <filesystem>
#include "include/index_types.hpp"
#include "src/index.cpp"
#include "external/sshash/external/pthash/external/cmd_line_parser/include/parser.hpp"

#include "tools/util.cpp"
#include "tools/build.cpp"
#include "tools/pseudoalign.cpp"
#include "include/index.hpp"
#include "include/color_classes/hybrid.hpp"
#include "include/color_classes/color_classes.hpp"
#include "external/sshash/external/pthash/external/essentials/include/essentials.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace fulgor;

// // Wrapper struct to hold the index

// template <typename FulgorIndex>
// void* loadIndex(char* index_filename){
//     FulgorIndex* index = new FulgorIndex();  // Use 'new' instead of malloc
//     essentials::load(*index, index_filename);
//     cout << "Complete" << endl;
//     return index;
// }


// void* wrappedLoadIndex(char* index_filename) {
//     return loadIndex<index_type>(const_cast<char*>(index_filename));
// }

void* load_index(char* index_filename) {
    index_type* index = new index_type();  // Use 'new' instead of malloc
    essentials::load(*index, index_filename);
    cout << "Complete" << endl;
    return index;
}

int* query(int* ret_arr, char* query_file, char* index_file, double threshold, 
                bool strict_threshold) {
    std::vector<uint32_t> colors;
    pseudoalign<index_type>(std::string(index_file), std::string(query_file),
                std::string("/nfshomes/srober22/fulgor/out.txt"), 1, threshold, pseudoalignment_algorithm::FULL_INTERSECTION, colors);
    for(size_t i = 0; i < colors.size(); i++) {
        ret_arr[i] = colors[i];
    }
    return ret_arr;
}

void index_stats(void* indexPtr) {
    index_type* f_ptr = (index_type*) indexPtr;
    index_type f = (*f_ptr);
    f.print_stats();
}

// template <typename FulgorIndex>
// void index_stats(void* indexPtr){
//     FulgorIndex* f_ptr = (FulgorIndex*) indexPtr;
//     FulgorIndex f = (*f_ptr);
//     f.print_stats();
// }


// void wrapped_index_stats(void* indexPtr) {
//     index_stats<index_type>(indexPtr);
// }
