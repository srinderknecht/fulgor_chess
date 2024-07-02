#include "/nfshomes/srober22/fulgor_chess/wrappers/ff.h"

int main(int argc, char *argv[]) {
    void* ptr = load_index("/nfshomes/srober22/chess_fulgor_test.fur");
    delete ptr;
    return 0;
}

