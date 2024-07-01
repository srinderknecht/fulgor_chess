#include "ff.h"

#include <filesystem>
#include "include/index_types.hpp"
#include "src/index.cpp"
#include "external/sshash/external/pthash/external/cmd_line_parser/include/parser.hpp"

#include "tools/util.cpp"
#include "tools/build.cpp"
#include "tools/pseudoalign.cpp"
#include <iostream>
#include <string>

using namespace std;

int* query(int** ret_arr, char* query_file, char* index_file, double threshold, 
                bool strict_threshold) {
    std::vector<uint32_t> colors;
    pseudoalign<index_type>(std::string(index_file), std::string(query_file),
                std::string("/nfshomes/srober22/fulgor/out.txt"), 1, threshold, pseudoalignment_algorithm::FULL_INTERSECTION, colors);
    *ret_arr = (int*) (malloc(sizeof(int) * 20));
    for(size_t i = 0; i < sizeof(*ret_arr) / sizeof(int); i++) {
        *ret_arr[i] = colors[i];
    }
    return *ret_arr;
}

void index_stats(int argc, char** argv) {
	stats(argc, argv);
}

/*
using namespace std;
int building(int xx, char** yy){
    build(xx, yy);
}
void query(int** ret_arr, char* query_file, char* index_file, double threshold, 
                bool strict_threshold) {
    pseudoalign<index_type>(std::string(index_file), std::string(query_file),
                std::string("/fs/cbcb-scratch/srinder/output.txt"), 1, threshold, pseudoalignment_algorithm::FULL_INTERSECTION);
}

void index_stats(int argc, char** argv) {
	stats(argc, argv);
}
*/