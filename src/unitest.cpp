#include <stdio.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "utils.cpp"
using namespace std;

#define MAX_TRAIN_SEQ 10000

observ o = {0, 1, 2, 3, 4, 5};
HMM model;
int T = 10;
int N = 10;
double alpha[MAX_TRAIN_SEQ][MAX_STATE];
double beta[MAX_TRAIN_SEQ][MAX_STATE];
double gamma[MAX_TRAIN_SEQ][MAX_STATE];
double epsilon[MAX_TRAIN_SEQ][MAX_STATE][MAX_STATE];

void test_matrix(int T, int N) {
    matrix m = new_matrix(T, N);
    dump_matrix(m);
}

void test_alpha() {
    puts("test alpha");
    calculate_alpha(model, o, alpha);
    dump_2darray(alpha);
}

void test_beta() {
    puts("test beta");
    calculate_beta(model, o, beta);
    dump_2darray(beta);
}

void test_gamma() {
    puts("test gamma");
    calculate_gamma(model, o, alpha, beta, gamma);
    dump_2darray(gamma);
}

void test_epsilon() {
    puts("test epsilon");
    calculate_epsilon(model, o, alpha, beta, epsilon);
    // dump_3darray(epsilon);
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

void test_load_train_seq() {
    loadHMM(&model, "model_init.txt");
    ifstream fin("data/train_seq_01.txt");
    string line;
    vector<string> seqs;
    while (getline(fin, line)) {
        seqs.push_back(line);
        T = line.length();
    }
    N = model.state_num;
    vector<observ> os = seqs_to_observs(seqs, model.observ_num);
    o = os[0];
}

int main(int argc, char const *argv[]) {
    signal(SIGSEGV, handler);

    test_load_train_seq();
    // test_matrix(6, 6);
    test_alpha();
    test_beta();
    test_gamma();
    test_epsilon();
    // test_observ();

    return 0;
}
