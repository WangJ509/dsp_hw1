#include <hmm.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "utils.cpp"
using namespace std;

#ifndef MAX_TRAIN_SEQ
#define MAX_TRAIN_SEQ 10000
#endif

void _update_transition(HMM *model, vector<tensor> es, vector<matrix> gs,
                        vector<observ> os, int i, int j) {
    double numerator = 0;
    double denominator = 0;
    for (int n = 0; n < os.size(); n++) {
        for (int t = 0; t < os[n].size() - 1; t++) {
            numerator += es[n][t][i][j];
            denominator += gs[n][t][i];
        }
    }

    model->transition[i][j] = numerator / denominator;
}

void _update_observation(HMM *model, vector<matrix> gs, vector<observ> os,
                         int i, int k) {
    double numerator = 0;
    double denominator = 0;
    for (int n = 0; n < os.size(); n++) {
        for (int t = 0; t < os[n].size(); t++) {
            if (os[n][t] == k) {
                numerator += gs[n][t][i];
            }
            denominator += gs[n][t][i];
        }
    }

    model->observation[k][i] = numerator / denominator;
}

int train(HMM *model, vector<observ> os) {
    int N = model->state_num;
    int M = os.size();
    vector<matrix> gammas;
    vector<tensor> epsilons;

    for (int n = 0; n < os.size(); n++) {
        int T = os[n].size();
        matrix alpha = calculate_alpha(*model, os[n]);
        matrix beta = calculate_beta(*model, os[n]);
        matrix gamma = calculate_gamma(*model, alpha, beta, os[n]);
        // dump_matrix(gamma);
        // cout << endl;
        tensor epsilon = calculate_epsilon(*model, alpha, beta, os[n]);

        gammas.push_back(gamma);
        epsilons.push_back(epsilon);
    }

    // update initial prob.
    for (int i = 0; i < N; i++) {
        double pi = 0;
        for (int n = 0; n < M; n++) {
            pi += gammas[n][1][i];
        }
        model->initial[i] = pi / M;
    }

    // update transition prob.
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            _update_transition(model, epsilons, gammas, os, i, j);
        }
    }

    // update observation prob.
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < model->observ_num; k++) {
            _update_observation(model, gammas, os, i, k);
        }
    }

    return -1;
}

int main(int argc, char **argv) {
    signal(SIGSEGV, handler);

    int iter = atoi(argv[1]);
    char *model_init_path = argv[2];
    char *seq_path = argv[3];
    char *output_model_path = argv[4];

    HMM model;
    loadHMM(&model, model_init_path);

    vector<string> train_seqs;
    string line;
    ifstream infile(seq_path);
    while (getline(infile, line)) {
        train_seqs.push_back(line);
    }

    vector<observ> observs = seqs_to_observs(train_seqs, model.observ_num);

    for (int i = 0; i < iter; i++) {
        printf("iteration: %d\n", i);

        int err = train(&model, observs);

        printf("finish iteration: %d\n", i);
        dumpHMM(stdout, &model);
    }
}
