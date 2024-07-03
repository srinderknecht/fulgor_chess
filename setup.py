from ctypes import *
import ctypes
from Bio import SeqIO

class SubsetCounts(Structure):
    _fields_ = [("s1", c_int),("s2", c_int),("s3", c_int),("s4", c_int),("s5", c_int),("s6", c_int),("s7", c_int),("s8", c_int),("s9", c_int),("s10", c_int)]

# Load the shared library
lib = CDLL('/nfshomes/srinder/fulgor_chess/build/libwrapper.so')  # Adjust the path as needed

# Define the function prototype

#arguments are: ret array (need to figure out how to get this to work) <-- currently printing to a  file, query file, threshold, strict 

lib.load_index.restype = c_void_p
lib.load_index.argtypes = [c_char_p]

index_ptr = lib.load_index(b'/nfshomes/srinder/chess_fulgor_test.fur')
print(index_ptr)

#lib.index_stats.argtype = c_void_p
#lib.index_stats(index_ptr)

lib.batch_query.argtypes = [c_void_p, POINTER(c_int), c_char_p, c_double, c_uint64, c_bool]

print("\n\n")
arrayType = c_int * 10
x = arrayType(*[-1] * 10)
# x = SubsetCounts(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1)
returned = lib.batch_query(index_ptr, x, b'/nfshomes/srinder/random_sequence.fa', 
                         0, 1, True)
print("back in python")

print(list(x))
print("\n\n")

arrayType2 = c_int * 10
y = arrayType(*[-1]*10)
lib.point_query.argtypes = [c_void_p, POINTER(c_int), c_char_p, c_double, c_uint64, c_bool]

records = next(SeqIO.parse('/nfshomes/srinder/random_sequence.fa', 'fasta'))
seq = str(records.seq)
seq = ctypes.create_string_buffer(seq.encode('utf-8'))

returned_point = lib.point_query(index_ptr, y, seq,
                                 0, 1, True)
print(list(y))

lib.unload_index.arg = c_void_p
lin.unload_index(index_ptr)
