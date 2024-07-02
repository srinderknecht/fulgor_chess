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

int query(void* indexPtr, int* ret_arr, char* query_file, double threshold, 
                uint64_t num_threads, bool strict_threshold) {
    index_type* f_ptr = (index_type*) indexPtr;
    index_type index = (*f_ptr);

    std::vector<uint32_t> all_col;

    
    pseudoalignment_algorithm algo = pseudoalignment_algorithm::THRESHOLD_UNION;
    std::atomic<uint64_t> num_mapped_reads{0};
    std::atomic<uint64_t> num_reads{0};
    auto query_filenames = std::vector<std::string>({query_file});
    if (num_threads == 1) {
        num_threads += 1;
        essentials::logger(
            "1 thread was specified, but an additional thread will be allocated for parsing");
    }
    fastx_parser::FastxParser<fastx_parser::ReadSeq> rparser(query_filenames, num_threads,
                                                             num_threads - 1);
    rparser.start();
    std::vector<std::thread> workers;
    std::mutex iomut;
    std::mutex ofile_mut;

    std::ofstream out_file;
    //HARD CODED HERE
    std::string output_filename = "/nfshomes/srinder/output.txt";
    //HARD CODED 
    out_file.open(output_filename, std::ios::out | std::ios::trunc);
    if (!out_file) {
        essentials::logger("could not open output file " + output_filename);
        return 1;
    }

    for (uint64_t i = 1; i != num_threads; ++i) {
        workers.push_back(std::thread([&index, &rparser, &num_reads, &num_mapped_reads, algo,
                                       threshold, &out_file, &iomut, &ofile_mut, &all_col]() {
            do_map(index, rparser, num_reads, num_mapped_reads, algo, threshold, out_file, iomut,
                   ofile_mut, all_col);
        }));
    }

    for (auto& w : workers) { w.join(); }
    rparser.stop();

    for(size_t i = 0; i < all_col.size(); i++) {
        ret_arr[i] = all_col[i];
    }
    return 0;
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
