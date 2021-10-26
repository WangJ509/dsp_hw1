#include <hmm.h>
#include <stdio.h>

#include <vector>
using namespace std;

typedef vector<vector<double> > matrix;

void print_number(int n) { printf("%d\n", n); }

int seq_to_observ(char input, int observ_num) {
    int ret = input - 'A';
    if (ret < 0 || ret >= observ_num) {
        return -1;
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

void dump_matrix(matrix m) {
    for (int i = 0; i < m.size(); i++) {
        int n = m[i].size();
        for (int j = 0; j < n; j++) {
            printf("%lf ", m[i][j]);
        }
        puts("");
    }

    return;
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
            gamma[t][i] = (alpha[t][i] * beta[t][i]) / denominator;
        }
    }

    return gamma;
}