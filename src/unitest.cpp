#include <stdio.h>

#include <fstream>
#include <iostream>
#include <string>
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

    int observ[] = {0, 1, 2, 3, 4, 5};
    matrix m = calculate_alpha(model, observ, 6, 6);
    dump_matrix(m);
}

void test_beta() {
    HMM model;
    loadHMM(&model, "model_init.txt");

    int observ[] = {0, 1, 2, 3, 4, 5};
    matrix m = calculate_beta(model, observ, 6, 6);
    dump_matrix(m);
}

void test_gamma() {
    HMM model;
    loadHMM(&model, "model_init.txt");

    int observ[] = {0, 1, 2, 3, 4, 5};
    matrix alpha = calculate_alpha(model, observ, 6, 6);
    matrix beta = calculate_beta(model, observ, 6, 6);
    matrix gamma = calculate_gamma(model, alpha, beta, observ, 6, 6);
    dump_matrix(gamma);

    for (int t = 0; t < 6; t++) {
        double sum = 0;
        for (int i = 0; i < 6; i++) {
            sum += gamma[t][i];
        }
        printf("%e\n", sum);
    }
}

void test_epsilon() {
    HMM model;
    loadHMM(&model, "model_init.txt");

    int observ[] = {0, 1, 2, 3, 4, 5};
    matrix alpha = calculate_alpha(model, observ, 6, 6);
    matrix beta = calculate_beta(model, observ, 6, 6);
    tensor t = calculate_epsilon(model, alpha, beta, observ, 6, 6);
    dump_tensor(t);
}

void test_observ() {
    cout << "hello" << endl;
    vector<string> train_seqs;
    string line;

    ifstream infile("data/train_seq_01.txt");

    while (getline(infile, line)) {
        train_seqs.push_back(line);
    }

    for (string s : train_seqs) {
        cout << s << endl;
    }
}

int main(int argc, char const *argv[]) {
    // test_matrix(6, 6);
    // test_alpha();
    // test_beta();
    // test_gamma();
    test_observ();
    return 0;
}
