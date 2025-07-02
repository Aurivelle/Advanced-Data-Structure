#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>

class HyperLogLog
{
private:
    int b_, m_;
    std::vector<uint8_t> M_; // registers

    // Bias‐correction constants for small m
    static constexpr double alpha16 = 0.673;
    static constexpr double alpha32 = 0.697;
    static constexpr double alpha64 = 0.709;

    // Choose alpha based on m
    static inline double alpha(int m)
    {
        if (m == 16)
            return alpha16;
        if (m == 32)
            return alpha32;
        if (m == 64)
            return alpha64;
        return 0.7213 / (1.0 + 1.079 / m);
    }

    // MurmurHash3 finalizer — same “mix” as in FM++
    static inline uint64_t murmur_mix(uint64_t x)
    {
        x ^= x >> 33;
        x *= 0xff51afd7ed558ccdULL;
        x ^= x >> 33;
        x *= 0xc4ceb9fe1a85ec53ULL;
        x ^= x >> 33;
        return x;
    }

    // Count leading zeros, return 64 if x==0
    static inline int lzcnt(uint64_t x)
    {
        return x ? __builtin_clzll(x) : 64;
    }

public:
    explicit HyperLogLog(int b)
        : b_(b), m_(1 << b), M_(m_, 0) {}

    // Process one element
    void add(uint64_t x)
    {
        // 1) scramble the input exactly as in FM++
        uint64_t h = murmur_mix(x);
        // 2) use the top b_ bits as register index
        int idx = h >> (64 - b_);
        // 3) the remaining bits determine rho
        uint64_t w = h << b_;
        int rho = lzcnt(w) + 1;
        // 4) update if this element exhibits a longer run of zeros
        if (rho > M_[idx])
        {
            M_[idx] = rho;
        }
    }

    // Estimate distinct count
    double estimate() const
    {
        // harmonic sum of 2^{-M_[i]}
        double sum = 0.0;
        for (auto v : M_)
        {
            sum += std::pow(2.0, -static_cast<int>(v));
        }
        double raw = alpha(m_) * m_ * m_ / sum;

        // small‐range correction
        if (raw <= 2.5 * m_)
        {
            int V = std::count(M_.begin(), M_.end(), 0);
            if (V > 0)
            {
                return m_ * std::log(static_cast<double>(m_) / V);
            }
        }
        return raw;
    }
};