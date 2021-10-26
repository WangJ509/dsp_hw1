#include <hmm.h>
#include <stdio.h>

#include <vector>
using namespace std;

typedef vector<vector<double> > matrix;

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
        alpha[1][i] = model.initial[i] * model.observation[o1][i];
        printf("%lf ", alpha[1][i]);
    }

    return alpha;
}