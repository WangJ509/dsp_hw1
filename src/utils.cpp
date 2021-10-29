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
typedef vector<vector<double> > matrix;
typedef vector<vector<vector<double> > > tensor;

void panic(string message) {
    cout << message << endl;
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
    vector<observ> ret(seqs.size());

    for (string s : seqs) {
        ret.push_back(seq_to_observ(s, observ_num));
    }

    return ret;
}

matrix new_matrix(int T, int N) {
    matrix m(T);
    for (int i = 0; i < T; i++) {
        m[i] = vector<double>(N);
    }

    return m;
}

tensor new_tensor(int T, int N) {
    tensor t(T);
    for (int i = 0; i < T; i++) {
        t[i] = new_matrix(N, N);
    }

    return t;
}

void dump_observ(observ o) {
    for (int i = 0; i < o.size(); i++) {
        printf("%d ", o[i]);
    }
    puts("");
}

void dump_matrix(matrix m) {
    for (int i = 0; i < m.size(); i++) {
        int n = m[i].size();
        for (int j = 0; j < n; j++) {
            printf("%e ", m[i][j]);
        }
        puts("");
    }
}

void dump_tensor(tensor input) {
    for (int t = 0; t < input.size(); t++) {
        printf("t = %d\n", t);
        dump_matrix(input[t]);
    }
}

matrix calculate_alpha(HMM model, int observ[], int T, int N) {
    matrix alpha = new_matrix(T, N);

    for (int i = 0; i < N; i++) {
        int o1 = observ[0];
        alpha[0][i] = model.initial[i] * model.observation[o1][i];
    }

    for (int t = 1; t < T; t++) {
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int i = 0; i < N; i++) {
                sum += alpha[t - 1][i] * model.transition[i][j];
            }
            alpha[t][j] = sum * model.observation[t][j];
        }
    }

    return alpha;
}

matrix calculate_beta(HMM model, int observ[], int T, int N) {
    matrix beta = new_matrix(T, N);

    for (int i = 0; i < N; i++) {
        beta[T - 1][i] = 1;
    }

    for (int t = T - 2; t >= 0; t--) {
        for (int i = 0; i < N; i++) {
            double sum = 0;
            for (int j = 0; j < N; j++) {
                double a = model.transition[i][j];
                double b = model.observation[t + 1][j];
                sum += a * b * beta[t + 1][j];
            }
            beta[t][i] = sum;
        }
    }

    return beta;
}

matrix calculate_gamma(HMM model, matrix alpha, matrix beta, int observ[],
                       int T, int N) {
    matrix gamma = new_matrix(T, N);

    for (int t = 0; t < T; t++) {
        for (int i = 0; i < N; i++) {
            double denominator = 0;
            for (int j = 0; j < N; j++) {
                denominator += alpha[t][j] * beta[t][j];
            }
            // if (denominator == 0) {
            //     char message[MAX_PANIC_MESSAGE];
            //     sprintf(message, "gamma divide by zero t:%d i:%d", t, i);
            //     panic(message);
            // }
            gamma[t][i] = (alpha[t][i] * beta[t][i]) / denominator;
        }
    }

    return gamma;
}

double _epsilon(HMM model, matrix alpha, matrix beta, int ot1, int N, int t,
                int i_in, int j_in) {
    double denominator = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            denominator += alpha[t][i] * model.transition[i][j] *
                           model.observation[ot1][j] * beta[t + 1][j];
        }
    }

    // if (denominator == 0) {
    //     char message[MAX_PANIC_MESSAGE];
    //     sprintf(message, "epsilon divide by zero t:%d i:%d j:%d", t, i_in,
    //             j_in);
    //     panic(message);
    // }

    double numerator = alpha[t][i_in] * model.transition[i_in][j_in] *
                       model.observation[ot1][j_in] * beta[t + 1][j_in];

    return numerator / denominator;
}

tensor calculate_epsilon(HMM model, matrix alpha, matrix beta, int observ[],
                         int T, int N) {
    tensor epsilon = new_tensor(T, N);

    for (int t = 0; t < T - 1; t++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                epsilon[t][i][j] =
                    _epsilon(model, alpha, beta, observ[t + 1], N, t, i, j);
            }
        }
    }

    return epsilon;
}