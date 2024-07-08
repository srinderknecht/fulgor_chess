#ifndef __FF_H__
#define __FF_H__

#include <stdbool.h>
#include "include/index.hpp"
#include <mutex>

#ifdef __cplusplus
extern "C" {
#endif
	// import fulgor library/functions somehow

	/*
	Takes in a fasta file with the query sequence in it
	Assigns ret_arr[i] = 1 if subset i contains at least 'threshold' of the k-mers in query sequence 
		Parameters:
			ret_arr (int array, len = number of subsets): output parameter
			query_file (char array): file path to the query fasta file
				do the functions of fulgor work better with passing in a file or the full string? 
			threshold (double): the fraction of k-mers that need to be present in a subset in order to return it as a match
				do we also want the option for evaluating just one k-mer present?
			strict_threshold (bool): whether to consider natural cutoffs in subset fractions (allowing flex in the threshold) or stick to the strict threshold
				we might not want to give them this choice?
			return_output (bool): i.e. do we want it to print? 
				this could be handled in the call to this function from python (as you can direct where std out goes...)
		Returns:
			array of bits [num subsets], arr[i] is 1 if subset i contains num k-mers over the given threshold, 0 otherwise
				could also do it as an array of length [num subsets over the threshold], but would then need to store the integer of which subset it is 
	*/
	/// @brief Takes in a index_ptr, return array, query file and number of threds, then runs fulgor query on each sequence in the query file against the fulgor index
	/// @param indexPtr a void pointer to the loaded index that is in memory
	/// @param ret_arr an int pointer that points to an int array which will hold the outputed locations for each sequence
	/// @param query_file a char pointer that points to the sequence file that Fastx uses to load all the sequence into a parsar
	/// @param threshold delete
	/// @param num_threads the number of threads that are to be used when generating the fulgor array
	/// @return array of bits [num subsets], arr[i] is 1 if subset i contains num k-mers over the given threshold, 0 otherwise
	int batch_query(void* indexPtr, int* ret_arr, char* query_file, double threshold, 
					uint64_t num_threads);
	/// @brief Takes in a index_ptr, return array, query sequence and number of threds, then runs fulgor query on the singular sequence that is submitted
	/// @param indexPtr a void pointer to the loaded index that is in memory
	/// @param ret_arr an int pointer that points to an int array which will hold the outputed locations for the query sequence
	/// @param query_file a char pointer that points to the sequence that will be queryed
	/// @param threshold delete
	/// @param num_threads the number of threads that are to be used when generating the fulgor array
	/// @return array of bits [num subsets], arr[i] is 1 if subset i contains num k-mers over the given threshold, 0 otherwise
	int point_query(void* indexPtr, int* ret_arr, char* query_string, double threshold, 
					uint64_t num_threads);

	int chess_map(void* indexPtr, char* query_sequence, 
					const double threshold, std::ofstream& out_file, 
					std::mutex& iomut, std::mutex& ofile_mut, std::vector<uint32_t>& all_col);

	/*	
	Dumps the statistics of the current fulgor index to std::out
		Parameters: 
			sys args: argc - 1, argv missing the initial ./fulgor call
		Returns: 
			Nothing
			Writes to std::out
				fulgor stats
	*/
	void index_stats(void* indexPtr);

	/*
	If we want the client to be able to specify parameters for the building of fulgor index? 
		Parameters:
			k (int): size of k-mer
			m (int): size of minimizer
			t (int): number of threads
			verbose (bool)
		Returns:
			True if fulgor index was successfully written to disk that the program will use later on
			False otherwise
			Might want to return a file path? That we can then pass into query (depends on the specs of fulgor's functions)
				the default could be the original index

	build()
	*/

	/*
	If we want the index to be loaded the whole time, would need to implement a load function, return the index, and then pass it in to each of the functions above. 
	*/

	void* load_index(char* index_filename);

	void unload_index(void* idx_ptr);
	

#ifdef __cplusplus
}
#endif


#endif /* __FF_H__ */