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

void test_beta() {
    HMM model;
    loadHMM(&model, "model_init.txt");

    int observ[] = {1, 2, 3, 4, 5, 6};
    matrix m = calculate_beta(model, observ, 6, 6);
    dump_matrix(m);
}

void test_gamma() {
    HMM model;
    loadHMM(&model, "model_init.txt");

    int observ[] = {1, 2, 3, 4, 5, 6};
    matrix alpha = calculate_alpha(model, observ, 6, 6);
    matrix beta = calculate_beta(model, observ, 6, 6);
    matrix m = calculate_gamma(model, alpha, beta, observ, 6, 6);
    dump_matrix(m);
}

void test_epsilon() {
    HMM model;
    loadHMM(&model, "model_init.txt");

    int observ[] = {1, 2, 3, 4, 5, 6};
    matrix alpha = calculate_alpha(model, observ, 6, 6);
    matrix beta = calculate_beta(model, observ, 6, 6);
    tensor t = calculate_epsilon(model, alpha, beta, observ, 6, 6);
    dump_tensor(t);
}

int main(int argc, char const *argv[]) {
    // test_matrix(6, 6);
    // test_gamma();
    test_epsilon();
    return 0;
}
