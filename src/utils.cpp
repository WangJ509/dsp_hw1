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

double calculate_alpha(HMM *model, observ o, double alpha[][MAX_STATE]) {
    for (int i = 0; i < N; i++) {
        int o1 = o[0];
        alpha[0][i] = model->initial[i] * model->observation[o1][i];
    }

    for (int t = 1; t < T; t++) {
        int ot = o[t];
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int i = 0; i < N; i++) {
                sum += alpha[t - 1][i] * model->transition[i][j];
            }
            alpha[t][j] = sum * model->observation[ot][j];
        }
    }

    double likely = 0;
    for (int i = 0; i < N; i++) {
        likely += alpha[T - 1][i];
    }

    return likely;
}

void calculate_beta(HMM *model, observ o, double beta[][MAX_STATE]) {
    for (int i = 0; i < N; i++) {
        beta[T - 1][i] = 1;
    }

    for (int t = T - 2; t >= 0; t--) {
        int ot1 = o[t + 1];
        for (int i = 0; i < N; i++) {
            double sum = 0;
            for (int j = 0; j < N; j++) {
                double a = model->transition[i][j];
                double b = model->observation[ot1][j];
                sum += a * b * beta[t + 1][j];
            }
            beta[t][i] = sum;
        }
    }
}

void calculate_gamma(HMM *model, observ o, double alpha[][MAX_STATE],
                     double beta[][MAX_STATE], double gamma_sum[][MAX_STATE],
                     double gamma_obs[][MAX_STATE]) {
    for (int t = 0; t < T; t++) {
        double denominator = 0;
        for (int i = 0; i < N; i++) {
            denominator += alpha[t][i] * beta[t][i];
        }
        for (int i = 0; i < N; i++) {
            double tmp = (alpha[t][i] * beta[t][i]) / denominator;
            gamma_sum[t][i] += tmp;
            gamma_obs[o[t]][i] += tmp;
        }
    }
}

void calculate_epsilon(HMM *model, observ o, double alpha[][MAX_STATE],
                       double beta[][MAX_STATE],
                       double epsilon_sum[][MAX_STATE][MAX_STATE]) {
    for (int t = 0; t < T - 1; t++) {
        double denominator = 0;
        int ot1 = o[t + 1];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                denominator += alpha[t][i] * model->transition[i][j] *
                               model->observation[ot1][j] * beta[t + 1][j];
            }
        }
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                double numerator = alpha[t][i] * model->transition[i][j] *
                                   model->observation[ot1][j] * beta[t + 1][j];
                epsilon_sum[t][i][j] += numerator / denominator;
            }
        }
    }
}

double calculate_delta(HMM model, observ o, double delta[][MAX_STATE]) {
    for (int i = 0; i < N; i++) {
        delta[0][i] = model.initial[i] * model.observation[o[0]][i];
    }
    for (int t = 1; t < T; t++) {
        for (int j = 0; j < N; j++) {
            double max = -1;
            for (int i = 0; i < N; i++) {
                double tmp = delta[t - 1][i] * model.transition[i][j];
                if (tmp > max) {
                    max = tmp;
                }
            }
            delta[t][j] = max * model.observation[o[t]][j];
        }
    }

    double p_max = -1;
    for (int i = 0; i < N; i++) {
        if (delta[T-1][i] > p_max) {
            p_max = delta[T-1][i];
        }
    }
    
    return p_max;
}

bool close_to_one(double input) { return input > 0.9 && input < 1.1; }

bool validate_hmm(HMM *model) {
    // initial prob. sums to 1
    double sum = 0;
    for (int i = 0; i < N; i++) {
        sum += model->initial[i];
    }
    if (!close_to_one(sum)) {
        printf("initial prob. sums to %lf\n", sum);
        return false;
    }

    // each row of transition sums to 1
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < N; j++) {
            sum += model->transition[i][j];
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
            sum += model->observation[j][i];
        }
        if (!close_to_one(sum)) {
            printf("column %d of observation sums to %lf\n", i, sum);
            return false;
        }
    }

    return true;
}