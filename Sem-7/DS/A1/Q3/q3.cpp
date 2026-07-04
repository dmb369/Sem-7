#include <bits/stdc++.h>
#include <omp.h>
#include <sys/time.h>
using namespace std;

#define NUM_SUBSEQ 1000
#define SUBSEQ_SIZE 1000000ULL
#define TOTAL_SIZE (NUM_SUBSEQ * SUBSEQ_SIZE)

double get_time_in_seconds() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

inline unsigned int generate_value(unsigned long long subseq_index, unsigned long long j) 
{
    unsigned long long x = (6364136223846793005ULL * (j + subseq_index * SUBSEQ_SIZE) + 1ULL);
    return (unsigned int)(x % SUBSEQ_SIZE + subseq_index * SUBSEQ_SIZE);
}

int main() 
{
    int thread_counts[] = {1, 2, 4, 6, 8, 10, 12, 14, 16};
    int num_configs = sizeof(thread_counts) / sizeof(thread_counts[0]);

    cout << "# N = " << TOTAL_SIZE << " elements (" << NUM_SUBSEQ
         << " subsequences × " << SUBSEQ_SIZE << " each)\n";
    cout << "threads,avg_time_sec,speedup,check_first10\n";

    double baseline_time = -1.0;

    for (int t = 0; t < num_configs; t++) 
    {
        int num_threads = thread_counts[t];
        omp_set_num_threads(num_threads);

        double total_time = 0.0;
        vector<int> merged;
        merged.reserve(TOTAL_SIZE);

        for (int run = 0; run < 5; run++) 
        {
            vector<vector<int>> subseq(NUM_SUBSEQ, vector<int>(SUBSEQ_SIZE));

            double start = get_time_in_seconds();

            #pragma omp parallel for schedule(static)
            for (int i = 0; i < NUM_SUBSEQ; i++) {
                for (unsigned long long j = 0; j < SUBSEQ_SIZE; j++) {
                    subseq[i][j] = generate_value(i, j);
                }
                sort(subseq[i].begin(), subseq[i].end());
            }
            vector<int> temp(TOTAL_SIZE);
            unsigned long long pos = 0;
            for (int i = 0; i < NUM_SUBSEQ; i++) {
                copy(subseq[i].begin(), subseq[i].end(), temp.begin() + pos);
                pos += SUBSEQ_SIZE;
            }

            double end = get_time_in_seconds();
            total_time += (end - start);

            if (run == 0) 
            {
                merged = move(temp);
            }
        }

        double avg_time = total_time / 5.0;
        if (num_threads == 1) baseline_time = avg_time;
        double speedup = baseline_time / avg_time;

        cout << num_threads << "," << fixed << setprecision(6) << avg_time
             << "," << speedup << ",";
        for (int i = 0; i < 10; i++) {
            cout << merged[i] << (i == 9 ? "" : " ");
        }
        cout << "\n";
        cout.flush();
    }

    return 0;
}
