#!/usr/bin/env python3
"""
visual.py
讀取 main.cpp 產生的 output.csv 並繪製三張評估圖表：
  • mean_rel_error.png      ─ 平均相對誤差
  • space_comparison.png    ─ Sketch 佔用空間 (KB)
  • 99th_tail_error.png     ─ 99% 尾部相對誤差
"""
import csv
from collections import defaultdict
import matplotlib.pyplot as plt

CSV_FILE = "output.csv"

# --------------------------------------------------------------------
payloads: list[str] = []
metrics: dict[str, dict[str, float]] = defaultdict(dict)

with open(CSV_FILE, newline="", encoding="utf-8-sig") as f:
    reader = csv.reader(f)
    header = next(reader, None)  # 略過表頭
    for row in reader:
        if not row or row[0] != "SUMMARY":
            continue
        p = row[1]  # Payload 名稱
        payloads.append(p)
        metrics[p] = {
            "spaceM": float(row[11]),
            "spaceF": float(row[12]),
            "spaceH": float(row[13]),
            "relErrM": float(row[15]),
            "relErrF": float(row[17]),
            "relErrH": float(row[19]),
            "p99M": float(row[20]),
            "p99F": float(row[21]),
            "p99H": float(row[22]),
        }

if not payloads:
    raise RuntimeError("No SUMMARY rows found in output.csv")


# --------------------------------------------------------------------
def bar_chart(filename: str, title: str, ylabel: str, series: list[tuple[str, str]]):
    """series = [(metric_key, legend_label), ...]"""
    x = range(len(payloads))
    width = 0.25
    plt.figure(figsize=(6, 4))
    for i, (metric, label) in enumerate(series):
        vals = [metrics[p][metric] for p in payloads]
        plt.bar([xi + i * width for xi in x], vals, width, label=label)
    plt.xticks([xi + width for xi in x], payloads)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend()
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()


# three figures ------------------------------------------------------
bar_chart(
    "mean_rel_error.png",
    "Mean Relative Error by Payload",
    "Mean Relative Error",
    [("relErrM", "Morris++"), ("relErrF", "FM++"), ("relErrH", "HLL")],
)

bar_chart(
    "space_comparison.png",
    "Sketch Space (KB) by Payload",
    "Sketch Space (KB)",
    [("spaceM", "Morris++"), ("spaceF", "FM++"), ("spaceH", "HLL")],
)

bar_chart(
    "99th_tail_error.png",
    "99th Percentile Tail Error by Payload",
    "Relative Error",
    [("p99M", "Morris++ 99th%"), ("p99F", "FM++ 99th%"), ("p99H", "HLL 99th%")],
)

print("Charts generated: mean_rel_error.png, space_comparison.png, 99th_tail_error.png")
