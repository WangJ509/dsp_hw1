#include <hmm.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "utils.cpp"
using namespace std;
using namespace std::chrono;

#ifndef MAX_TRAIN_SEQ
#define MAX_TRAIN_SEQ 10000
#endif

// assume these variables are fixed
int T, N, M;
double alphas[MAX_TRAIN_SEQ][MAX_STATE][MAX_STATE];
double betas[MAX_TRAIN_SEQ][MAX_STATE][MAX_STATE];
double gammas[MAX_TRAIN_SEQ][MAX_STATE][MAX_STATE];
double epsilons[MAX_TRAIN_SEQ][MAX_SEQ][MAX_STATE][MAX_STATE];

void _update_transition(HMM *model,
                        double epsilons[][MAX_SEQ][MAX_STATE][MAX_STATE],
                        double gammas[][MAX_STATE][MAX_STATE],
                        vector<observ> os, int i, int j) {
    double numerator = 0;
    double denominator = 0;
    for (int n = 0; n < os.size(); n++) {
        for (int t = 0; t < os[n].size() - 1; t++) {
            numerator += epsilons[n][t][i][j];
            denominator += gammas[n][t][i];
        }
    }

    model->transition[i][j] = numerator / denominator;
}

void _update_observation(HMM *model, double gammas[][MAX_STATE][MAX_STATE],
                         vector<observ> os, int i, int k) {
    double numerator = 0;
    double denominator = 0;
    for (int n = 0; n < os.size(); n++) {
        for (int t = 0; t < os[n].size(); t++) {
            if (os[n][t] == k) {
                numerator += gammas[n][t][i];
            }
            denominator += gammas[n][t][i];
        }
    }

    model->observation[k][i] = numerator / denominator;
}

int train(HMM *model, vector<observ> os) {
    for (int n = 0; n < M; n++) {
        calculate_alpha(model, os[n], alphas[n]);
        calculate_beta(model, os[n], betas[n]);
        calculate_gamma(model, os[n], alphas[n], betas[n], gammas[n]);
        calculate_epsilon(model, os[n], alphas[n], betas[n], epsilons[n]);
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

    auto start = high_resolution_clock::now();
    // update observation prob.
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < model->observ_num; k++) {
            _update_observation(model, gammas, os, i, k);
        }
    }
    auto end = high_resolution_clock::now();cout << duration_cast<microseconds>(end - start).count() << endl;

    return 0;
}

int main(int argc, char **argv) {
    signal(SIGSEGV, handler);

    int iter = atoi(argv[1]);
    char *model_init_path = argv[2];
    char *seq_path = argv[3];
    char *output_model_path = argv[4];
    FILE *fp_out = open_or_die(output_model_path, "w");

    HMM model;
    loadHMM(&model, model_init_path);

    vector<string> train_seqs;
    string line;
    ifstream infile(seq_path);
    while (getline(infile, line)) {
        train_seqs.push_back(line);
    }

    vector<observ> observs = seqs_to_observs(train_seqs, model.observ_num);

    // set global variables
    T = observs[0].size();
    N = model.state_num;
    M = observs.size();

    for (int i = 0; i < iter; i++) {
        printf("iteration: %d\n", i);

        int err = train(&model, observs);

        printf("finish iteration: %d\n", i);
        dumpHMM(stdout, &model);
        if (!validate_hmm(&model)) {
            panic("model is invalid");
        }
    }

    dumpHMM(fp_out, &model);
}
