#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <cstddef>

#include "Morris.cpp"
#include "FM.cpp"
#include "HLL.cpp"

enum class PayloadType
{
    UNIQUE_RANDOM,
    ALL_SAME,
    MANY_DUPLICATES
};

// ---------------------- helper ------------------------------------------------
std::vector<int> generate_payload(int n, PayloadType type)
{
    std::vector<int> v(n);
    std::default_random_engine rng(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()));

    switch (type)
    {
    case PayloadType::UNIQUE_RANDOM:
        std::iota(v.begin(), v.end(), 1);
        std::shuffle(v.begin(), v.end(), rng);
        break;
    case PayloadType::ALL_SAME:
        std::fill(v.begin(), v.end(), 1);
        break;
    case PayloadType::MANY_DUPLICATES:
        for (int &x : v)
            x = rng() % 1000;
        break;
    }
    return v;
}

const char *payload_name(PayloadType t)
{
    switch (t)
    {
    case PayloadType::UNIQUE_RANDOM:
        return "UNIQUE_RANDOM";
    case PayloadType::ALL_SAME:
        return "ALL_SAME";
    case PayloadType::MANY_DUPLICATES:
        return "MANY_DUPLICATES";
    }
    return "";
}

double percentile(std::vector<double> v, double p)
{
    if (v.empty())
        return 0.0;
    std::sort(v.begin(), v.end());
    size_t idx = static_cast<size_t>(std::ceil(p / 100.0 * v.size())) - 1;
    return v[std::min(idx, v.size() - 1)];
}

// 粗略估計 sketch 佔用空間（byte）
size_t estimate_morris_space(int k, int l)
{
    return sizeof(MorrisPP) + static_cast<size_t>(l) * k * sizeof(int);
}
size_t estimate_fm_space(int k, int l)
{
    return sizeof(FMPP) + static_cast<size_t>(l) * k * sizeof(int);
}
size_t estimate_hll_space(int b)
{
    int m = 1 << b;
    return sizeof(HyperLogLog) + static_cast<size_t>(m) * sizeof(uint8_t);
}
// -----------------------------------------------------------------------------
int main()
{
    constexpr int k = 100, l = 6, b = 7; // sketch parameters
    constexpr int n = 100000;            // stream length
    constexpr int trials = 30;           // repetitions

    const double spaceM_kb = estimate_morris_space(k, l) / 1024.0;
    const double spaceF_kb = estimate_fm_space(k, l) / 1024.0;
    const double spaceH_kb = estimate_hll_space(b) / 1024.0;

    std::ofstream csv("output.csv");
    if (!csv)
    {
        std::cerr << "Cannot open output.csv for writing\n";
        return 1;
    }
    csv << std::fixed << std::setprecision(6);

    // header ------------------------------------------------------------
    csv << "ROWTYPE,PAYLOAD,TRIAL,"
           "EST_M,EST_F,EST_H,ERR_M,ERR_F,ERR_H,"
           "TRUE_TOTAL,TRUE_DISTINCT,"
           "SPACE_M_KB,SPACE_F_KB,SPACE_H_KB,"
           "AVG_M,REL_M,AVG_F,REL_F,AVG_H,REL_H,"
           "P99_M,P99_F,P99_H\n";

    // for three payload patterns ---------------------------------------
    for (int pid = 0; pid < 3; ++pid)
    {
        PayloadType type = static_cast<PayloadType>(pid);
        auto input = generate_payload(n, type);

        const int true_total = static_cast<int>(input.size());
        const int true_distinct = static_cast<int>(
            std::unordered_set<int>(input.begin(), input.end()).size());

        std::vector<double> estM(trials), estF(trials), estH(trials);
        std::vector<double> errM(trials), errF(trials), errH(trials);

        for (int t = 0; t < trials; ++t)
        {
            MorrisPP morris(k, l);
            FMPP fm(k, l);
            HyperLogLog hll(b);

            for (int x : input)
            {
                morris.add(x);
                fm.add(x);
                hll.add(x);
            }
            estM[t] = morris.estimate();
            estF[t] = fm.estimate();
            estH[t] = hll.estimate();

            errM[t] = std::abs(estM[t] - true_total) / double(true_total);
            errF[t] = std::abs(estF[t] - true_distinct) / double(true_distinct);
            errH[t] = std::abs(estH[t] - true_distinct) / double(true_distinct);

            // TRIAL row ------------------------------------------------
            csv << "TRIAL," << payload_name(type) << ','
                << (t + 1) << ','
                << estM[t] << ',' << estF[t] << ',' << estH[t] << ','
                << errM[t] << ',' << errF[t] << ',' << errH[t] << ','
                << ",,,,,,,,,,,,\n"; // 13 empty summary columns
        }
        // summary values -----------------------------------------------
        const double avgM = std::accumulate(estM.begin(), estM.end(), 0.0) / trials;
        const double avgF = std::accumulate(estF.begin(), estF.end(), 0.0) / trials;
        const double avgH = std::accumulate(estH.begin(), estH.end(), 0.0) / trials;
        const double relM = std::accumulate(errM.begin(), errM.end(), 0.0) / trials;
        const double relF = std::accumulate(errF.begin(), errF.end(), 0.0) / trials;
        const double relH = std::accumulate(errH.begin(), errH.end(), 0.0) / trials;
        const double p99M = percentile(errM, 99);
        const double p99F = percentile(errF, 99);
        const double p99H = percentile(errH, 99);

        // SUMMARY row -------------------------------------------------
        csv << "SUMMARY," << payload_name(type) << ",,"
            << ",,,,,," // 5 blanks (EST/ERR)
            << true_total << ',' << true_distinct << ','
            << spaceM_kb << ',' << spaceF_kb << ',' << spaceH_kb << ','
            << avgM << ',' << relM << ','
            << avgF << ',' << relF << ','
            << avgH << ',' << relH << ','
            << p99M << ',' << p99F << ',' << p99H << '\n';
    }

    csv.close();
    std::cout << "CSV written to output.csv\n";
    return 0;
}