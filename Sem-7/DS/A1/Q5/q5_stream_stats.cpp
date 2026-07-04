#include <bits/stdc++.h>
#include <omp.h>
#include <sys/time.h>
using namespace std;

// --- wall clock (gettimeofday) ---
static inline double wall_seconds()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// --- RNG: xorshift64* ---
struct XorShift64Star
{
    uint64_t s;
    explicit XorShift64Star(uint64_t seed) : s(seed ? seed : 0x9e3779b97f4a7c15ULL) {}
    inline uint64_t next()
    {
        uint64_t x = s;
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        s = x;
        return x * 2685821657736338717ULL;
    }
    inline uint64_t uniform_u64(uint64_t hi_inclusive)
    {
        __uint128_t span = (__uint128_t)hi_inclusive + 1;
        uint64_t threshold = (uint64_t)(-span % span);
        for (;;)
        {
            uint64_t r = next();
            if (r >= threshold)
                return (uint64_t)((__uint128_t)r % span);
        }
    }
};

int main(int argc, char **argv)
{
    // Usage: ./q5_run <a|b> [seed=42] [sampleK=2000000] [threads=auto]
    char mode = (argc > 1) ? argv[1][0] : 'a';
    uint64_t seed = (argc > 2) ? strtoull(argv[2], nullptr, 10) : 42ULL;
    size_t SAMPLE_K = (argc > 3) ? strtoull(argv[3], nullptr, 10) : 2000000ULL;
    int threads = (argc > 4) ? atoi(argv[4]) : 0;
    if (threads > 0)
        omp_set_num_threads(threads);

    const uint64_t N = (mode == 'a') ? 360000000ULL : 3600000000ULL; // 3.6e8 / 3.6e9
    const uint64_t DOMAIN_MAX = 1000000000000000000ULL;              // 1e18

    int T = omp_get_max_threads();
    size_t per_thr_k = (SAMPLE_K + T - 1) / T;

    vector<__int128> sum_local(T, 0);
    vector<uint64_t> min_local(T, UINT64_MAX), max_local(T, 0);
    vector<vector<uint64_t>> samp(T);
    for (auto &v : samp)
        v.reserve(per_thr_k);

    double t0 = wall_seconds();

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        XorShift64Star rng(seed + 0x9e3779b97f4a7c15ULL * (tid + 1));
        uint64_t loc_min = UINT64_MAX, loc_max = 0;
        __int128 loc_sum = 0;
        size_t kept = 0, cap = per_thr_k;
        uint64_t seen = 0;

#pragma omp for schedule(static)
        for (uint64_t i = 0; i < N; i++)
        {
            uint64_t x = rng.uniform_u64(DOMAIN_MAX);
            if (x < loc_min)
                loc_min = x;
            if (x > loc_max)
                loc_max = x;
            loc_sum += (__int128)x;

            // reservoir sampling
            seen++;
            if (kept < cap)
            {
                samp[tid].push_back(x);
                kept++;
            }
            else
            {
                uint64_t r = rng.uniform_u64(seen - 1);
                if (r < cap)
                    samp[tid][(size_t)r] = x;
            }
        }
        min_local[tid] = loc_min;
        max_local[tid] = loc_max;
        sum_local[tid] = loc_sum;
    }

    // Reduce global stats
    uint64_t gmin = UINT64_MAX, gmax = 0;
    __int128 gsum = 0;
    for (int i = 0; i < T; i++)
    {
        gsum += sum_local[i];
        gmin = min(gmin, min_local[i]);
        gmax = max(gmax, max_local[i]);
    }

    // Merge samples
    vector<uint64_t> sample;
    for (int i = 0; i < T; i++)
    {
        sample.insert(sample.end(), samp[i].begin(), samp[i].end());
    }
    if (sample.size() > SAMPLE_K)
    {
        XorShift64Star r(seed + 777);
        for (size_t i = 0; i < SAMPLE_K; i++)
        {
            size_t j = i + (r.uniform_u64((uint64_t)(sample.size() - i - 1)));
            swap(sample[i], sample[j]);
        }
        sample.resize(SAMPLE_K);
    }
    sort(sample.begin(), sample.end());

    auto pct = [&](double p)
    {
        if (sample.empty())
            return 0ULL;
        double idx = p * (sample.size() - 1);
        size_t lo = floor(idx), hi = ceil(idx);
        if (lo == hi)
            return sample[lo];
        long double w = idx - lo;
        long double v = (1 - w) * sample[lo] + w * sample[hi];
        return (uint64_t)v;
    };

    uint64_t p25 = pct(0.25), med = pct(0.50), p75 = pct(0.75);

    // Mode from sorted sample
    uint64_t mode_val = 0;
    uint64_t best_freq = 0;
    if (!sample.empty())
    {
        uint64_t curr_val = sample[0];
        uint64_t curr_freq = 1;
        for (size_t i = 1; i < sample.size(); ++i)
        {
            if (sample[i] == curr_val)
            {
                ++curr_freq;
            }
            else
            {
                if (curr_freq > best_freq || (curr_freq == best_freq && curr_val < mode_val))
                {
                    best_freq = curr_freq;
                    mode_val = curr_val;
                }
                curr_val = sample[i];
                curr_freq = 1;
            }
        }
        if (curr_freq > best_freq || (curr_freq == best_freq && curr_val < mode_val))
        {
            best_freq = curr_freq;
            mode_val = curr_val;
        }
    }

    long double mean = (long double)gsum / (long double)N;
    double t1 = wall_seconds();
    double seconds = t1 - t0;

    // Console output
    cout.setf(std::ios::fixed);
    cout << setprecision(6);
    cout << "mean=" << mean
         << " median=" << med
         << " mode=" << mode_val
         << " min=" << gmin
         << " p25=" << p25
         << " p75=" << p75
         << " max=" << gmax
         << " seconds=" << seconds << "\n";

    // Save CSV and sample
    string tag = (mode == 'a') ? "a" : "b";
    {
        ofstream f("results/q5_" + tag + "_stats.csv");
        f << "mean,median,mode,min,p25,p75,max,seconds\n";
        f << mean << "," << med << "," << mode_val << "," << gmin << ","
          << p25 << "," << p75 << "," << gmax << "," << seconds << "\n";
    }
    {
        ofstream fb("results/sample_" + tag + ".bin", ios::binary);
        fb.write((char *)sample.data(), (std::streamsize)(sample.size() * sizeof(uint64_t)));
    }
    return 0;
}
