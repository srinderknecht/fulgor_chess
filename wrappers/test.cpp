#include "/nfshomes/srober22/fulgor_chess/wrappers/ff.h"
#include "include/index_types.hpp"

int main(int argc, char *argv[]) {
    void* ptr = load_index("/nfshomes/srober22/chess_fulgor_test.fur");
    fulgor::index_type* i_ptr = (fulgor::index_type*) ptr;
    delete i_ptr;
    return 0;
}

