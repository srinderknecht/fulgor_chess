import ctypes as cty
import mappy as mp
from Bio import SeqIO
import kneed as kn
import gzip
import random
import numpy as np

# Load the shared library
lib = cty.CDLL('/nfshomes/srinder/fulgor_chess/build/libwrapper.so')  # Adjust the path as needed

# Define the function prototype
lib.load_index.restype = cty.c_void_p
lib.load_index.argtypes = [cty.c_char_p]
lib.index_stats.argtype = cty.c_void_p
lib.point_query.argtypes = [cty.c_void_p, cty.POINTER(cty.c_int), cty.c_char_p, cty.c_double, cty.c_uint64, cty.c_bool]

index_ptr = lib.load_index(b'/nfshomes/srinder/chess_fulgor_test.fur') 

def get_random_sequences(file_path, num_sequences=1):
    with gzip.open(file_path, "rt") as handle:
        sequences = list(SeqIO.parse(handle, "fasta"))
    random.shuffle(sequences)
    return str(sequences[0])
#arguments are: ret array (need to figure out how to get this to work) <-- currently printing to a  file, query file, threshold, strict 

def kneeFinder(y):
    x_axis = np.arange(1,11)
    np_y = np.ctypeslib.as_array(y)
    np_y[np_y == -1] = 0
    sorted = (np.sort(np_y)[::-1])
    if sorted[0] == 0:
        return 0, 0
    if np.argwhere(np_y).shape[0] == 0:
        return y, 10
    kneedle = kn.KneeLocator(x_axis, sorted, curve = 'convex', direction = 'decreasing')
    knee_point = kneedle.knee
    index= [list(y).index(i) for i in sorted[:knee_point]]
    return index, knee_point

def run_query(q_string, x):
    lib.point_query(index_ptr, x, q_string.encode('utf-8'), 0, 1, True)


arrayType = cty.c_int * 10
x = arrayType(*[-1] * 10)
#sequences = get_random_sequences("/nfshomes/srinder/gencode.vM35.transcripts.fa.gz")

sequence_1 = 'cgggcgcgccgcgTGAGTCCGAGCCGGCCGCTCAGAGCGCTCCAGGCTGGGTTCCCGGGCCCCGCCGCGCCGCCTCCTTCCCAGCTCGCCCGCCCAGGCCTGGCCTCCTGCTTTTCCATTTGATTCCCTGCCTCTTTCTATTCGGACTGGAATGCCGGGCCAGGCTCCGGGGCGCGCCGCTGCGGCAGCCGCACCTCGCAGGTCCCCCGGCCGACCCCGACGCGGAAGCGGCGGCCCTCCTCGCCGTCGGGGAGCCAGGGAGCCGGGGACGatcagtcacataaggcttagaggatcaaggatcctgcccaGATGACTTACCGAAATGTTACAGATTAAGTTGGTGTGGTAACCTGGGCTGAGCACTCTGGGAGAGGAAGAGAAGAGAGAAGACAGGAAACAACTGAACTATGACCAATCCCAGCACGGAGGCCCAGAAAACTTTAAGATTTGAGTATTAATGTCTCAAGGTCAGGAGCAACCTCAAGGCTAAAACTCAGATCTCAGGACTCAATTTCACAGAAGTTCCACTATAAAGGCAATAATCTAAAGCTTTAAATGATATGAAAATTTTGTAATAAGAGTTCAGTATTTCTGCCAACATTGGCGCATGGATTGCAAAGTTCACAGGATTGAAAACACCATCGACATAATGGAAATTGAACAGCATCTGATTACTGAGTGCTATATCAGCAAGTTAAAAGGATCTTTTGCATACCTTTTAATGGTATATATCCTAAAACTGAAGTGTTCAATATAGACATCCAGATTGAAACTCAGGCAGTGAATTACATACACAACAAATCAGTTGAACATGGCAGAGCTTGTCAGACTTATGAAAGATTAAATACATTTTACATTTCCACAAGTGTGGTATTCACAGGTCTAAATTGGAAAATTCTTGGGTTGAAGAAGAATAATCATAATCTTAGCTTCACTCATTGAGGAAACATTATTCAGGGATGATAAGAGGTAAAATCTTAAAAATATTGACAAGCTAATGAGAGAACTTTGAGGTAAGCTGATATGCAGGTATAGTTAGGACTGG'
sequence_2 = 'ctgggattacagGTATCACGGTTACATTTATACATACCGAGTGTCCCAGACAGAAACAGGTTCTTGGAGTGCTGAGACAGCACCTGGGGTACATAAAAGATATTTCCGGAAAATAAAAAATCTCATTTCAGCATTTCAGAAGCCAGATCAAGGCATTGTAATACCTCTGCAGTATCCAGTTGAGAAGAAGTCCTCAGCTAGAAGTACACAAGGTACTACAGGGATAAGAGAAGATCCTGATGTCTGCCTGAAAGCCCCATGAAGAAAAATAAAACACCTTGTACTTTATTTTCTATAATTTAAATATATGCTAAGTCTTATATATTGTAGATAATACAGTTCGGTGAGCTACAAATGCATTTCTAAAGCCATTGTAGTCCTGTAATGGAAGCATCTAGCATGTCGTCAAAGCTGAAATGGACTTTTGTACATAGTGAGGAGCTTTGAAACGAGGATTGGGAAAAAGTAATTCCGTAGGTTATTTTCAGTTATTATATTTACAAATGGGAAACAAAAGGATAATGAATACTTTATAAAGGATTAATGTCAATTCTTGCCAAATATAAATAAAAATAATCCTCAGTTTTTGTGAAAAGCTCCATTTTTAGTGAAATATTATTTTATAGCTACTAATTTTAAAATGTCTTGCTTGATTGTATGGTGGGAAGTTGGCTGGTGTCCCTTGTCTTTGCCAAGTTCTCCACTAGCTATGGTGTCATAGGCTCTTTTGGGATTTTTGAAGCTGTATACTGTGTGCTAAAACAAGCACTAAACAAAGAGTGAAGGATTTATGTTTAATTCTGAAAGCAACCTTCTTGCCTAGTGTTCTGATATTGGACAGTAAAATCCACAGACCAACCTGGAGTTGAAAATCTTATAATTTAAAATATGCTCTAAACATGTTTATCGTATTTGATGCTACAGGATTTGAAATTGTATTACAAATCCAATGAAATGAGTTTTTCTTTTCATTTACCTCTGCCCCAGTTGTTTCTACTACATGGAAGACCTCATTTTGAAGGGAAATTTCAGCAGCTGCAGCTCATGAGTAACTGATTTGTAACAAGCCTCCTTTTAAAGTAACCCTACAAAACCACTGGAAAGTTTATGGTTGTATTATTTTTTAAAAAAATTCCAAGTGATTGAAACCTACACGAGATACAGAATTTTATGCGGCATTTTCTTCTCACATTTATATTTTTGTGATTTTGTGATTGATTATATGTCACTTTGCTACAGGGCTCACAGAattcattcactcaacaaacataatagggcgctgagggcatagaagtaaaaacacctggtccctgctctcagttcactgtcttgttggacgagaaaacaataacgataaaagacagtgaaagaaaataacgataaAAGACAGTGAAAGAAAATAACAATAAAAGACAAGGAAAAAATAACAATGAAAGTTGATAAGTACATGATAAGCGAGGTTCCCCGTGTGTAGGTAGATCTGGTCTTTAGAGGCAGATAGATAGGTCAGTGCAAATACTCTGGTCCATGGGCCATATGAAAAGGCTAAGCTtcactgtaaaataataactgggaattctggattgtgtatgggtgttggtgaacttggttttaattagtgaactgctgagagacagagctattctccatgtactggcaagacctgatttctgagcATTTAATATGGATGCCGTGGGAGTACAAAAGTGGAGTGTGGCCTGAGTAATGCATTATGGGTGGTTTACCATTTCTTGAGGTAAAAGCATCACATGAACTTGTAAAGGAATTTAAAAATCCTACTTTCATAATAAGTTGCATAGGTTTAATAATTTTTAATTATATGGCTTGAGTTTAAATTGTAATAGGCGTAACTAATTTTAACTCTATAATGTGTTCATTCTGGAATAATCCTAAACATATGAATTATGTTTGCATGTTCACTTCCAAGAGCCTTTTTTTGAAAAAAAGCTTTTTTTGAATCATC'
sequences = sequence_2
run_query(sequences, x)
y, kp = kneeFinder(x)
print(y, kp)
if y == 0:
    print("no matches from fulgor")
if x == y:
    idx = x
else:
    idx = y
#x = list(x)
#idx = [x.index(i) for i in temp]
for i in range(len(idx)):
    if x[i] != -1:
        subset = idx[i] + 1
        index = f"/nfshomes/srinder/mm_indexes/s{subset}.mmi"
        print("Aligning to index: " + index)
        a = mp.Aligner(index)  # load or build index
        print("\nMinimap alignment:")
        for hit in a.map(sequences): # traverse alignments
            print("\t{}\t{}\t{}\t{}".format(hit.ctg, hit.r_st, hit.r_en, hit.cigar_str))
        print("\n")

'''
lib.load_index.restype = c_void_p
lib.load_index.argtypes = [c_char_p]

index_ptr = lib.load_index(b'/nfshomes/srinder/chess_fulgor_test.fur')
print(index_ptr)

lib.index_stats.argtype = c_void_p
lib.index_stats(index_ptr)

lib.batch_query.argtypes = [c_void_p, POINTER(c_int), c_char_p, c_double, c_uint64]

print("\n\n")
arrayType = c_int * 10
x = arrayType(*[-1] * 10)
# x = SubsetCounts(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1)
returned = lib.batch_query(index_ptr, x, b'/nfshomes/srinder/random_sequence.fa', 
                         0, 1)
print("back in python")

print(list(x))
print("\n\n")

arrayType2 = c_int * 10
y = arrayType2(*[-1]*10)
lib.point_query.argtypes = [c_void_p, POINTER(c_int), c_char_p, c_double, c_uint64]

records = next(SeqIO.parse('/nfshomes/srinder/random_sequence.fa', 'fasta'))
seq = str(records.seq)
seq = ctypes.create_string_buffer(seq.encode('utf-8'))

returned_point = lib.point_query(index_ptr, y, seq, 0, 1)
print(list(y))

lib.unload_index.arg = c_void_p
lib.unload_index(index_ptr)
'''