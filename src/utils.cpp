#include <execinfo.h>
#include <hmm.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

#define MAX_PANIC_MESSAGE 100

typedef vector<int> observ;
// typedef vector<vector<double> > matrix;
// typedef vector<vector<vector<double> > > tensor;

extern int T, N;

void panic(string message) {
    cout << message << endl;
    exit(1);
}

void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, 2);
    exit(1);
}

void print_number(int n) { printf("%d\n", n); }

observ seq_to_observ(string input, int observ_num) {
    observ observ;

    for (char c : input) {
        int o = c - 'A';
        if (o < 0 || o >= observ_num) {
            panic("sequence out of range");
        }
        observ.push_back(o);
    }

    return observ;
}

vector<observ> seqs_to_observs(vector<string> seqs, int observ_num) {
    vector<observ> ret;

    for (string s : seqs) {
        ret.push_back(seq_to_observ(s, observ_num));
    }

    return ret;
}

void dump_observ(observ o) {
    for (int i = 0; i < o.size(); i++) {
        printf("%d ", o[i]);
    }
    puts("");
}

void dump_2darray(double a[][MAX_STATE]) {
    for (int i = 0; i < T; i++) {
        for (int j = 0; j < N; j++) {
            printf("%e ", a[i][j]);
        }
        puts("");
    }
}

void dump_3darray(double a[][MAX_STATE][MAX_STATE]) {
    for (int i = 0; i < T; i++) {
        printf("t = %d\n", i);
        dump_2darray(a[i]);
    }
}

void calculate_alpha(HMM model, observ o, double alpha[][MAX_STATE]) {
    for (int i = 0; i < N; i++) {
        int o1 = o[0];
        alpha[0][i] = model.initial[i] * model.observation[o1][i];
    }

    for (int t = 1; t < T; t++) {
        int ot = o[t];
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int i = 0; i < N; i++) {
                sum += alpha[t - 1][i] * model.transition[i][j];
            }
            alpha[t][j] = sum * model.observation[ot][j];
        }
    }
}

void calculate_beta(HMM model, observ o, double beta[][MAX_STATE]) {
    for (int i = 0; i < N; i++) {
        beta[T - 1][i] = 1;
    }

    for (int t = T - 2; t >= 0; t--) {
        int ot1 = o[t + 1];
        for (int i = 0; i < N; i++) {
            double sum = 0;
            for (int j = 0; j < N; j++) {
                double a = model.transition[i][j];
                double b = model.observation[ot1][j];
                sum += a * b * beta[t + 1][j];
            }
            beta[t][i] = sum;
        }
    }
}

void calculate_gamma(HMM model, observ o, double alpha[][MAX_STATE],
                     double beta[][MAX_STATE], double gamma[][MAX_STATE]) {
    for (int t = 0; t < T; t++) {
        for (int i = 0; i < N; i++) {
            double denominator = 0;
            for (int j = 0; j < N; j++) {
                denominator += alpha[t][j] * beta[t][j];
            }
            if (denominator == 0) {
                char message[MAX_PANIC_MESSAGE];
                sprintf(message, "gamma divide by zero t:%d i:%d", t, i);
                panic(message);
            }
            gamma[t][i] = (alpha[t][i] * beta[t][i]) / denominator;
        }
    }
}

double _epsilon(HMM model, double alpha[][MAX_STATE], double beta[][MAX_STATE],
                int ot1, int N, int t, int i_in, int j_in) {
    double denominator = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            denominator += alpha[t][i] * model.transition[i][j] *
                           model.observation[ot1][j] * beta[t + 1][j];
        }
    }

    if (denominator == 0) {
        char message[MAX_PANIC_MESSAGE];
        sprintf(message, "epsilon divide by zero t:%d i:%d j:%d", t, i_in,
                j_in);
        panic(message);
    }

    double numerator = alpha[t][i_in] * model.transition[i_in][j_in] *
                       model.observation[ot1][j_in] * beta[t + 1][j_in];

    return numerator / denominator;
}

void calculate_epsilon(HMM model, observ o, double alpha[][MAX_STATE],
                       double beta[][MAX_STATE],
                       double epsilon[][MAX_STATE][MAX_STATE]) {
    for (int t = 0; t < T - 1; t++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                epsilon[t][i][j] =
                    _epsilon(model, alpha, beta, o[t + 1], N, t, i, j);
            }
        }
    }
}

bool close_to_one(double input) { return input > 0.9 && input < 1.1; }

bool validate_hmm(HMM model) {
    // initial prob. sums to 1
    double sum = 0;
    for (int i = 0; i < N; i++) {
        sum += model.initial[i];
    }
    if (!close_to_one(sum)) {
        printf("initial prob. sums to %lf\n", sum);
        return false;
    }

    // each row of transition sums to 1
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < N; j++) {
            sum += model.transition[i][j];
        }
        if (!close_to_one(sum)) {
            printf("row %d of transition sums to %lf\n", i, sum);
            return false;
        }
    }

    // each column of observation sums to 1
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < N; j++) {
            sum += model.observation[j][i];
        }
        if (!close_to_one(sum)) {
            printf("column %d of observation sums to %lf\n", i, sum);
            return false;
        }
    }

    return true;
}