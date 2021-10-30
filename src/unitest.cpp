#include <stdio.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "utils.cpp"
using namespace std;

observ o = {0, 1, 2, 3, 4, 5};
HMM model;

void test_matrix(int T, int N) {
    matrix m = new_matrix(T, N);
    dump_matrix(m);
}

void test_alpha() {
    int observ[] = {0, 1, 2, 3, 4, 5};
    matrix m = calculate_alpha(model, o);
    dump_matrix(m);
}

void test_beta() {
    matrix m = calculate_beta(model, o);
    dump_matrix(m);
}

void test_gamma() {
    matrix alpha = calculate_alpha(model, o);
    matrix beta = calculate_beta(model, o);
    matrix gamma = calculate_gamma(model, alpha, beta, o);
    dump_matrix(gamma);
}

void test_epsilon() {
    matrix alpha = calculate_alpha(model, o);
    matrix beta = calculate_beta(model, o);
    tensor t = calculate_epsilon(model, alpha, beta, o);
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

    vector<observ> observs = seqs_to_observs(train_seqs, 6);
    for (observ o : observs) {
        dump_observ(o);
    }
}

int main(int argc, char const *argv[]) {
    loadHMM(&model, "model_init.txt");
    // test_matrix(6, 6);
    // test_alpha();
    // test_beta();
    // test_gamma();
    test_epsilon();
    // test_observ();
    return 0;
}
