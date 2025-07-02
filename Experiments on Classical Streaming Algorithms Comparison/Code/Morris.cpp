// Morris.cpp — 正確的 Morris++ 實作
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>

class MorrisPP
{
private:
    int k_;                                      // 每組 counter 數量
    int l_;                                      // 組數（實驗次數）
    std::vector<std::vector<int>> counters;      // counters[l_][k_]
    std::mt19937_64 rng;                         // 隨機數引擎
    std::uniform_real_distribution<double> dist; // [0,1) 均勻分布

public:
    // k = 每組 counter 數量, l = 組數
    MorrisPP(int k, int l)
        : k_(k), l_(l),
          counters(l, std::vector<int>(k, 0)),
          rng(std::random_device{}()),
          dist(0.0, 1.0)
    {
    }

    // 處理一筆 stream 元素（x 值本身不影響計數）
    template <typename T>
    void add(const T & /*x*/)
    {
        for (auto &group : counters)
        {
            for (int &c : group)
            {
                double p = std::exp2(-c); // 2^{-c}
                if (dist(rng) < p)
                {
                    ++c; // 以機率 2^{-c} 隨機增量
                }
            }
        }
    }

    // 最終估計：先算出各組平均，再取 median
    double estimate() const
    {
        std::vector<double> grp_est(l_);
        for (int i = 0; i < l_; ++i)
        {
            double sum = 0.0;
            for (int c : counters[i])
            {
                sum += (std::exp2(c) - 1.0); // Morris 原始估計：2^c - 1
            }
            grp_est[i] = sum / k_; // 每組平均
        }
        return median(grp_est);
    }

private:
    // 計算中位數（支援偶數/奇數長度）
    static double median(std::vector<double> v)
    {
        size_t n = v.size();
        size_t mid = n / 2;
        std::nth_element(v.begin(), v.begin() + mid, v.end());
        if (n % 2 == 1)
        {
            return v[mid];
        }
        else
        {
            double hi = v[mid];
            std::nth_element(v.begin(), v.begin() + mid - 1, v.end());
            double lo = v[mid - 1];
            return 0.5 * (hi + lo);
        }
    }
};