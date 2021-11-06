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

ofstream result;
int num_model;
vector<HMM> models;
vector<string> model_names;
double alpha[MAX_SEQ][MAX_STATE];
double delta[MAX_SEQ][MAX_STATE];
int T, N;

void test_forward(observ o) {
    double max_likely = -1;
    int best_idx = 0;
    for (int i = 0; i < models.size(); i++) {
        double likely = calculate_alpha(&models[i], o, alpha);
        if (likely > max_likely) {
            max_likely = likely;
            best_idx = i;
        }
    }

    result << model_names[best_idx] << " " << max_likely << endl;
}

void test(observ o) {
    double max_prob = -1;
    int best_idx = 0;
    for (int i = 0; i < models.size(); i++) {
        double p_max = calculate_delta(models[i], o, delta);
        if (p_max > max_prob) {
            max_prob = p_max;
            best_idx = i;
        }
    }

    double likely = calculate_alpha(&models[best_idx], o, alpha);
    result << model_names[best_idx] << " " << likely << endl;
}

int main(int argc, char **argv) {
    signal(SIGSEGV, handler);

    char *model_list_path = argv[1];
    char *test_seq_path = argv[2];
    char *result_path = argv[3];
    result.open(result_path);
    result << scientific;

    string line;
    ifstream model_list(model_list_path);
    while (getline(model_list, line)) {
        HMM model;
        loadHMM(&model, line.c_str());
        models.push_back(model);
        model_names.push_back(line);
        num_model += 1;
        // cout << "load success: " << line << endl;
    }
    model_list.close();

    vector<string> test_seqs;
    ifstream infile(test_seq_path);
    while (getline(infile, line)) {
        test_seqs.push_back(line);
    }
    infile.close();

    vector<observ> observs = seqs_to_observs(test_seqs, models[0].observ_num);

    // set global variables
    T = observs[0].size();
    N = models[0].state_num;

    for (observ o : observs) {
        if (getenv("USE_FORWARD") != NULL)
            test_forward(o);
        else
            test(o);
    }
}
