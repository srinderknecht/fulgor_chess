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


// Wrapper struct to hold the index

template <typename FulgorIndex>
void* loadIndex(char* index_filename){
    FulgorIndex* index = new FulgorIndex();  // Use 'new' instead of malloc
    essentials::load(*index, index_filename);
    cout << "Complete" << endl;
    return index;
}


void* wrappedLoadIndex(char* index_filename) {
    return loadIndex<index_type>(const_cast<char*>(index_filename));
}
int* query(int** ret_arr, char* query_file, char* index_file, double threshold, 
                bool strict_threshold) {
    std::vector<uint32_t> colors;
    pseudoalign<index_type>(std::string(index_file), std::string(query_file),
                std::string("/nfshomes/srinder/output.txt"), 1, threshold, pseudoalignment_algorithm::FULL_INTERSECTION, colors);
    *ret_arr = (int*) (malloc(sizeof(int) * 20));
    for(size_t i = 0; i < colors.size(); i++) {
        *ret_arr[i] = colors[i];
    }
    return *ret_arr;
}

void index_stats(int argc, char** argv) {
	stats(argc, argv);
}

