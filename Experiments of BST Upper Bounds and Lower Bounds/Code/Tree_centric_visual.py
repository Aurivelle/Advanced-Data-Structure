import matplotlib.pyplot as plt
import numpy as np


data = {
    "BasicBST": {
        "Random": {
            "Comparisons": 25634972,
            "Rotations": 0,
            "Time": 289,
            "Wilber": 11521705,
        },
        "Monotonic": {
            "Comparisons": 25634972,
            "Rotations": 0,
            "Time": 105,
            "Wilber": 524287,
        },
        "Hotspot": {
            "Comparisons": 26660220,
            "Rotations": 0,
            "Time": 109,
            "Wilber": 10901120,
        },
        "Zigzag": {
            "Comparisons": 25634972,
            "Rotations": 0,
            "Time": 111,
            "Wilber": 11804428,
        },
        "BitReversal": {
            "Comparisons": 25634972,
            "Rotations": 0,
            "Time": 269,
            "Wilber": 12137338,
        },
    },
    "SplayTree": {
        "Random": {
            "Comparisons": 30482255,
            "Rotations": 29696390,
            "Time": 603,
            "Wilber": 11521705,
        },
        "Monotonic": {
            "Comparisons": 18612271,
            "Rotations": 17870628,
            "Time": 53,
            "Wilber": 524287,
        },
        "Hotspot": {
            "Comparisons": 20373269,
            "Rotations": 19605235,
            "Time": 102,
            "Wilber": 10901120,
        },
        "Zigzag": {
            "Comparisons": 19658353,
            "Rotations": 18916803,
            "Time": 63,
            "Wilber": 11804428,
        },
        "BitReversal": {
            "Comparisons": 34720945,
            "Rotations": 33910530,
            "Time": 512,
            "Wilber": 12137338,
        },
    },
    "TangoTree": {
        "Random": {
            "Comparisons": 13079630,
            "Rotations": 262712,
            "Time": 868,
            "Wilber": 11521705,
        },
        "Monotonic": {
            "Comparisons": 13079630,
            "Rotations": 262314,
            "Time": 596,
            "Wilber": 524287,
        },
        "Hotspot": {
            "Comparisons": 14104878,
            "Rotations": 71969,
            "Time": 690,
            "Wilber": 10901120,
        },
        "Zigzag": {
            "Comparisons": 13079630,
            "Rotations": 261682,
            "Time": 600,
            "Wilber": 11804428,
        },
        "BitReversal": {
            "Comparisons": 13079630,
            "Rotations": 262008,
            "Time": 807,
            "Wilber": 12137338,
        },
    },
}


def plot_tree_centric(tree_name, seq_data):
    """
    給定一棵樹 (tree_name) 與其在五種序列上的資料 (seq_data)，
    產生一張圖(2 subplot):
      (上) Comparisons 與 Wilber (log scale)
      (下) Execution Time (線性刻度)
    X 軸為 5 種序列: [Random, Monotonic, Hotspot, Zigzag, BitReversal]
    """

    sequences = ["Random", "Monotonic", "Hotspot", "Zigzag", "BitReversal"]

    comps = [seq_data[seq]["Comparisons"] for seq in sequences]
    wilbs = [seq_data[seq]["Wilber"] for seq in sequences]
    times = [seq_data[seq]["Time"] for seq in sequences]

    x = np.arange(len(sequences))

    fig, axes = plt.subplots(2, 1, figsize=(8, 10))
    fig.suptitle(f"{tree_name} (Tree-Centric)", fontsize=15, fontweight="bold")

    ax0 = axes[0]
    bar_width = 0.35

    ax0.bar(
        x - bar_width / 2,
        comps,
        width=bar_width,
        color="steelblue",
        label="Comparisons",
    )
    ax0.bar(
        x + bar_width / 2,
        wilbs,
        width=bar_width,
        color="tomato",
        label="Wilber I Bound",
    )

    ax0.set_yscale("log")
    ax0.set_xticks(x)
    ax0.set_xticklabels(sequences, rotation=15)
    ax0.set_ylabel("Counts (log scale)")
    ax0.set_title("Comparisons vs. Wilber Bound")
    ax0.legend()

    for i, val in enumerate(comps):
        ax0.text(
            x[i] - bar_width / 2,
            val * 1.05,
            f"{val:,}",
            ha="center",
            va="bottom",
            fontsize=8,
            rotation=90,
        )
    for i, val in enumerate(wilbs):
        ax0.text(
            x[i] + bar_width / 2,
            val * 1.05,
            f"{val:,}",
            ha="center",
            va="bottom",
            fontsize=8,
            rotation=90,
        )

    ax1 = axes[1]
    ax1.bar(x, times, color="darkorange")
    ax1.set_xticks(x)
    ax1.set_xticklabels(sequences, rotation=15)
    ax1.set_ylabel("Execution Time (ms)")
    ax1.set_title("Execution Time Comparison")

    for i, t in enumerate(times):
        ax1.text(x[i], t * 1.02, f"{t:.2f} ms", ha="center", va="bottom", fontsize=8)

    fig.tight_layout()
    return fig


def main():

    for tree_name in data:
        seq_data = data[tree_name]
        fig = plot_tree_centric(tree_name, seq_data)
        outname = f"{tree_name}_treecentric.png"
        plt.savefig(outname)
        print(f"[OK] Saved figure for {tree_name} => {outname}")
        plt.close(fig)


if __name__ == "__main__":
    main()
