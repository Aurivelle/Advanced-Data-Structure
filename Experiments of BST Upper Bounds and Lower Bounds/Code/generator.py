import random
import argparse


def is_power_of_two(n):
    return (n & (n - 1)) == 0 and n != 0


def generate_monotonic(n, increasing=True):
    return list(range(1, n + 1)) if increasing else list(range(n, 0, -1))


def generate_random(n, key_range=None):
    if key_range is None:
        key_range = (1, n + 1)
    return random.sample(range(*key_range), n)


def bit_reverse(i, bits):
    result = 0
    for _ in range(bits):
        result = (result << 1) | (i & 1)
        i >>= 1
    return result


def generate_bit_reversal(n):
    assert is_power_of_two(n), "n must be power of 2"
    bits = n.bit_length() - 1
    return [bit_reverse(i, bits) + 1 for i in range(n)]  # shift to 1-based keys


def generate_hotspot(n, hotspots=5, ratio=0.8):
    key_range = (1, n + 1)
    hotspot_keys = random.sample(range(*key_range), hotspots)
    seq = []
    for _ in range(n):
        if random.random() < ratio:
            seq.append(random.choice(hotspot_keys))
        else:
            seq.append(random.randint(*key_range))
    return seq


def generate_zigzag(n):
    left = 1
    right = n
    seq = []
    while left <= right:
        seq.append(left)
        left += 1
        if left <= right:
            seq.append(right)
            right -= 1
    return seq


def write_sequence_to_file(seq, output_file):
    with open(output_file, "w") as f:
        f.write(" ".join(map(str, seq)))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="BST Test Sequence Generator")
    parser.add_argument(
        "--type",
        choices=[
            "random",
            "monotonic_inc",
            "monotonic_dec",
            "hotspot",
            "zigzag",
            "bit_reversal",
        ],
        required=True,
    )
    parser.add_argument("--size", type=int, default=1024)
    parser.add_argument("--output", type=str, default="sequence.txt")
    parser.add_argument("--hotspots", type=int, default=5)
    parser.add_argument("--ratio", type=float, default=0.8)

    args = parser.parse_args()

    if not is_power_of_two(args.size):
        raise ValueError("For consistency, --size must be a power of 2")

    if args.type == "random":
        seq = generate_random(args.size)
    elif args.type == "monotonic_inc":
        seq = generate_monotonic(args.size, increasing=True)
    elif args.type == "monotonic_dec":
        seq = generate_monotonic(args.size, increasing=False)
    elif args.type == "hotspot":
        seq = generate_hotspot(args.size, hotspots=args.hotspots, ratio=args.ratio)
    elif args.type == "zigzag":
        seq = generate_zigzag(args.size)
    elif args.type == "bit_reversal":
        seq = generate_bit_reversal(args.size)
    else:
        raise ValueError("Unsupported sequence type")

    write_sequence_to_file(seq, args.output)
    print(f"[OK] Sequence ({args.type}) of size {args.size} written to {args.output}")
