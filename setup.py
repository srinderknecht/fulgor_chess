from ctypes import *

# Load the shared library
lib = CDLL('/nfshomes/srober22/fulgor_chess/build/libwrapper.so')  # Adjust the path as needed

# Define the function prototype

#arguments are: ret array (need to figure out how to get this to work) <-- currently printing to a  file, query file, threshold, strict 



lib.load_index.restype = c_void_p
lib.load_index.argtypes = [c_char_p]

index_ptr = lib.load_index(b'/nfshomes/srober22/chess_fulgor_test.fur')
print(index_ptr)

# lib.index_stats.argtype = c_void_p
# lib.index_stats(index_ptr)

# lib.query.argtypes = [c_void_p, POINTER(c_int), c_char_p, c_double, c_uint64, c_bool]

# print("\n\n")
# arrayType = c_int * 20
# x = arrayType(*[1] * 20)
# returned = lib.query(index_ptr, x, b'/nfshomes/srober22/fasta_files/one_query.fa', 
#                          0, 1, True)
# print("back in python")
# # for j in returned:
# #     print(j)
# print(list(x))
