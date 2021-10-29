#include <hmm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmath>
#include <vector>

#include "utils.cpp"
using namespace std;

#ifndef MAX_TRAIN_SEQ
#define MAX_TRAIN_SEQ 10000
#endif

void update_transition(HMM *model, tensor epsilon, matrix gamma, int T, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double numerator = 0;
            double denominator = 0;
            for (int t = 0; t < T; t++) {
                double e = epsilon[t][i][j];
                double g = gamma[t][i];
                if (isnan(e) || isnan(g)) continue;
                numerator += e;
                denominator += g;
            }
            if (numerator == 0 || denominator == 0) continue;
            model->transition[i][j] = numerator / denominator;
        }
    }
}

int train(HMM *model, char *seq, int M) {
    int T = strlen(seq);
    int N = model->state_num;
    int observ[T];

    for (int i = 0; i < T; i++) {
        observ[i] = seq_to_observ(seq[i], model->observ_num);
        if (observ[i] == -1) {
            puts("invalid training sequence!");
            return -1;
        }
    }

    matrix alpha = calculate_alpha(*model, observ, T, N);
    matrix beta = calculate_beta(*model, observ, T, N);
    matrix gamma = calculate_gamma(*model, alpha, beta, observ, T, N);
    tensor epsilon = calculate_epsilon(*model, alpha, beta, observ, T, N);
    update_transition(model, epsilon, gamma, T, N);
    dumpHMM(stdout, model);

    return -1;
}

int main(int argc, char **argv) {
    int iter = atoi(argv[1]);
    char *model_init_path = argv[2];
    char *seq_path = argv[3];
    char *output_model_path = argv[4];

    HMM model;
    loadHMM(&model, model_init_path);

    char train_seq[MAX_TRAIN_SEQ][MAX_LINE];
    int num_train_seq = 0;
    FILE *fp = open_or_die(seq_path, "r");

    for (int i = 0; fgets(train_seq[i], MAX_LINE, fp) != NULL; i++) {
        // trim the trailing \n
        train_seq[i][strlen(train_seq[i]) - 1] = '\0';
        num_train_seq++;
    }

    for (int i = 0; i < iter; i++) {
        int err = train(&model, train_seq, num_train_seq);
        if (err != 0) {
            printf("error at iter: %d\n", i);
            return 0;
        }
    }
}
