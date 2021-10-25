#include <hmm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_TRAIN_SEQ
#define MAX_TRAIN_SEQ 10000
#endif

int seq_to_observ(char input, int observ_num) {
    int ret = input - 'A';
    if (ret < 0 || ret >= observ_num) {
        return -1;
    }

    return ret;
}

int train(HMM *model, char seq[]) {
    int n = strlen(seq);
    int observ[n];

    for (int i = 0; i < n; i++) {
        observ[i] = seq_to_observ(seq[i], model->observ_num);
    }

    return 0;
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
        for (int j = 0; j < num_train_seq; j++) {
            int err = train(&model, train_seq[j]);
            if (err != 0) {
                printf("error at iter: %d, seq: %d\n", i, j);
            }
        }
    }
}
