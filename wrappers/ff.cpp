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
#include <mutex>
#include <sstream>

using namespace std;
using namespace fulgor;

void* load_index(char* index_filename) {
    index_type* index = new index_type();  // Use 'new' instead of malloc
    essentials::load(*index, index_filename);
    return index;
}

void unload_index(void* indexPtr) {
    fulgor::index_type* i_ptr = (fulgor::index_type*) indexPtr;
    delete i_ptr;
}

void index_stats(void* indexPtr) {
    index_type* f_ptr = (index_type*) indexPtr;
    index_type f = (*f_ptr);
    f.print_stats();
}

int batch_query(void* indexPtr, int* ret_arr, char* query_file, double threshold, 
                uint64_t num_threads) {
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
    std::string output_filename = "/dev/null";

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

    for(size_t i = 0; i < all_col.size(); i = i + 2) {
        int var = all_col[i];
        ret_arr[var] = all_col[i + 1];
    }
    return 0;
}

int point_query(void* indexPtr, int* ret_arr, char* query_string, double threshold, 
					uint64_t num_threads) {
    index_type* f_ptr = (index_type*) indexPtr;
    index_type index = (*f_ptr);
    std::vector<uint32_t> all_col;
    std::atomic<uint64_t> num_mapped_reads{0};
    std::atomic<uint64_t> num_reads{0};
    //auto query_filenames = std::vector<std::string>({query_file});
    if (num_threads == 1) {
        num_threads += 1;
        essentials::logger(
            "1 thread was specified, but an additional thread will be allocated for parsing");
    }
    std::vector<std::thread> workers;
    std::mutex iomut;
    std::mutex ofile_mut;

    std::ofstream out_file;
    std::string output_filename = "/dev/null"; 
    out_file.open(output_filename, std::ios::out | std::ios::trunc);
    if (!out_file) {
        essentials::logger("could not open output file " + output_filename);
        return 1;
    }

    chess_map(indexPtr, query_string, threshold, out_file, iomut, ofile_mut, all_col);
    for(size_t i = 0; i < all_col.size(); i = i + 2) {
        int var = all_col[i];
        ret_arr[var] = all_col[i + 1];
    }

    return 0;
}

int chess_map(void* indexPtr, char* query_sequence,
           const double threshold, std::ofstream& out_file, std::mutex& iomut,
           std::mutex& ofile_mut, std::vector<uint32_t>& all_col) {
    //Potentially replace indexPtr with FulgorIndex const& index
    index_type* f_ptr = (index_type*) indexPtr;
    index_type index = (*f_ptr);
    std::vector<uint32_t> colors;
    std::stringstream ss;
    uint64_t buff_size = 0;
    constexpr uint64_t buff_thresh = 50;
    uint64_t num_reads = 0;
    uint64_t num_mapped_reads = 0;

    std::string query(query_sequence);
    index.pseudoalign_threshold_union(query, colors, all_col, threshold);
    buff_size += 1;
    if (!colors.empty()) {
        num_mapped_reads += 1;
        ss << "query" << "\t" << colors.size();
        for (auto c : colors) {
            ss << "\t" << c;
        }
        ss << "\n";
    } else {
        ss << "query" << "\t0\n";
    }

    num_reads += 1;
    colors.clear();

    if (buff_size > buff_thresh) {
        std::string outs = ss.str();
        ss.str("");
        ofile_mut.lock();
        out_file.write(outs.data(), outs.size());
        ofile_mut.unlock();
        buff_size = 0;
    }

    // dump anything left in the buffer
    if (buff_size > 0) {
        std::string outs = ss.str();
        ss.str("");
        ofile_mut.lock();
        out_file.write(outs.data(), outs.size());
        ofile_mut.unlock();
        buff_size = 0;
    }

    iomut.lock();
    std::cout << "mapped " << num_reads << " reads" << std::endl;
    iomut.unlock();

    return 0;
}