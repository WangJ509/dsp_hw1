#include <stdio.h>

#include <vector>

#include "utils.cpp"
using namespace std;

void test_matrix(int T, int N) {
    matrix m = new_matrix(T, N);
    dump_matrix(m);
}

void test_alpha() {
    HMM model;
    loadHMM(&model, "model_init.txt");

    int observ[] = {1, 2, 3, 4, 5, 6};
    matrix m = calculate_alpha(model, observ, 6, 6);
    dump_matrix(m);
}

int main(int argc, char const *argv[]) {
    // test_matrix(6, 6);
    test_alpha();
    return 0;
}
