import pandas as pd
import matplotlib.pyplot as plt
import glob


csv_files = sorted(glob.glob("results_*.csv"))


algorithms = []
comparisons = []
rotations = []
times = []


wilber_bound = None

for file in csv_files:
    df = pd.read_csv(file)

    if "wilber" in file.lower() or "LowerBoundType" in df.columns:
        if "Value" in df.columns:
            wilber_bound = int(df["Value"][0])
        continue

    if "Algorithm" not in df.columns:
        print(f"⚠️ Skipping invalid file: {file}")
        continue

    algo = df["Algorithm"][0]
    algorithms.append(algo)
    comparisons.append(df["Comparisons"][0])
    times.append(df["ExecutionTime"][0])
    rotations.append(df.get("Rotations", pd.Series([0]))[0])


if not algorithms:
    raise RuntimeError("❌ 未找到任何合法的上界測試結果（results_*.csv），請檢查輸出。")


zipped = list(zip(algorithms, comparisons, rotations, times))
zipped.sort(key=lambda x: x[1], reverse=True)
algorithms, comparisons, rotations, times = zip(*zipped)


fig, axes = plt.subplots(1, 2, figsize=(14, 6))
bar_width = 0.35
x = range(len(algorithms))


axes[0].bar(
    [i - bar_width / 2 for i in x],
    comparisons,
    width=bar_width,
    label="Comparisons",
    color="steelblue",
)
axes[0].bar(
    [i + bar_width / 2 for i in x],
    rotations,
    width=bar_width,
    label="Rotations",
    color="darkorange",
)
axes[0].set_xticks(x)
axes[0].set_xticklabels(algorithms, rotation=15)
axes[0].set_ylabel("Counts (log scale)")
axes[0].set_title("Comparisons vs. Rotations")
axes[0].set_yscale("log")
if wilber_bound:
    axes[0].axhline(
        y=wilber_bound,
        color="red",
        linestyle="--",
        label=f"Wilber I Lower Bound = {wilber_bound:,}",
    )
axes[0].legend()


for i in x:
    axes[0].text(
        i - bar_width / 2,
        comparisons[i] * 1.05,
        f"{comparisons[i]:,}",
        ha="center",
        fontsize=8,
        rotation=90,
    )
    axes[0].text(
        i + bar_width / 2,
        rotations[i] * 1.05,
        f"{rotations[i]:,}",
        ha="center",
        fontsize=8,
        rotation=90,
    )


axes[1].bar(algorithms, times, color="coral")
axes[1].set_ylabel("Execution Time (ms)")
axes[1].set_title("Execution Time Comparison")
for i in x:
    axes[1].text(i, times[i] * 1.05, f"{times[i]:.2f} ms", ha="center", fontsize=8)


plt.tight_layout()
plt.savefig("comparison_chart.png")
plt.show()


print("📊 Summary Report")
print(
    f"{'Algorithm':<12} | {'Comparisons':>12} | {'Rotations':>10} | {'Time(ms)':>10} | {'C/Wilber':>10} | {'R/C (%)':>10}"
)
print("-" * 74)
for i in range(len(algorithms)):
    ratio_cw = comparisons[i] / wilber_bound if wilber_bound else float("nan")
    ratio_rc = (rotations[i] / comparisons[i] * 100) if comparisons[i] else 0.0
    print(
        f"{algorithms[i]:<12} | {comparisons[i]:>12,} | {rotations[i]:>10,} | {times[i]:>10.2f} | {ratio_cw:>10.2f} | {ratio_rc:>9.2f}%"
    )

if wilber_bound:
    print(f"\n🔻 Wilber I Lower Bound: {wilber_bound:,}")
