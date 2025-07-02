// FM.cpp — 正宗 FM++ (FM+ + Median‑of‑ℓ)
//
// 用法：
//   FMPP fm(k /*inner*/, ℓ /*outer*/);
//   fm.add(x);        // x 可以是 int / uint64_t
//   double est = fm.estimate();

#include <vector>
#include <random>
#include <algorithm>
#include <cstdint>
#include <cmath>

class FMPP
{
public:
    FMPP(int k, int l) : k_(k), l_(l),
                         lzc_(l, std::vector<int>(k, 0)),
                         seeds_(static_cast<size_t>(l) * k * 2)
    {
        // 為每個 counter 產生獨立 (a,b)，a 需為奇數才保證單射
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dist(1, P - 1);
        for (size_t i = 0; i < seeds_.size(); ++i)
            seeds_[i] = dist(gen);
        // 確保 a 是奇數
        for (size_t idx = 0; idx < seeds_.size(); idx += 2)
            seeds_[idx] |= 1ULL;
    }

    // 對一筆資料更新所有 counter
    template <typename T>
    inline void add(T x)
    {
        const uint64_t xu = static_cast<uint64_t>(x);
        size_t idx = 0;
        for (int i = 0; i < l_; ++i)
        {
            for (int j = 0; j < k_; ++j, idx += 2)
            {
                uint64_t a = seeds_[idx], b = seeds_[idx + 1];
                uint64_t h = (a * xu + b) % P; // 2‑wise universal
                h = murmur_mix(h);             // 再做均勻擾動
                int z = rho(h);
                if (z > lzc_[i][j])
                    lzc_[i][j] = z;
            }
        }
    }

    // 取得 FM++ 估計
    double estimate() const
    {
        std::vector<double> grp(l_);
        for (int i = 0; i < l_; ++i)
            grp[i] = fm_plus(i);
        // 取中位數
        size_t mid = l_ / 2;
        std::nth_element(grp.begin(), grp.begin() + mid, grp.end());
        if (l_ & 1)
            return grp[mid];
        double v1 = grp[mid];
        std::nth_element(grp.begin(), grp.begin() + mid - 1, grp.end());
        return 0.5 * (v1 + grp[mid - 1]);
    }

private:
    // 常數
    static constexpr uint64_t P = 2305843009213693951ULL; // 2^61−1
    static constexpr double φ = 0.77351;                  // Flajolet 校正

    // 內部狀態
    int k_, l_;
    std::vector<std::vector<int>> lzc_; // ρ 值
    std::vector<uint64_t> seeds_;       // (a,b) 對

    /* MurmurHash3 finalizer — 將 64‑bit 打散 */
    static inline uint64_t murmur_mix(uint64_t x)
    {
        x ^= x >> 33;
        x *= 0xff51afd7ed558ccdULL;
        x ^= x >> 33;
        x *= 0xc4ceb9fe1a85ec53ULL;
        x ^= x >> 33;
        return x;
    }

    /* ρ(x) = 前導零個數 + 1，x==0 回傳 65 */
    static inline int rho(uint64_t x)
    {
        return x ? (__builtin_clzll(x) + 1) : 65;
    }

    /* 單組 (k counters) 的 FM+ 估計：φ·k / Σ 2^{−ρ}  */
    double fm_plus(int i) const
    {
        double invsum = 0.0;
        for (int z : lzc_[i])
            invsum += std::ldexp(1.0, -z); // 2^{−z}
        return φ * static_cast<double>(k_) / invsum;
    }
};