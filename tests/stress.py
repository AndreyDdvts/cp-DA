import random
import subprocess
import sys


def gen(rng: random.Random) -> str:
    n = rng.randint(1, 30)
    m = rng.randint(1, 30)
    c = rng.choice([3, 10, 10**9])
    lines = [f"{n} {m}"]
    for _ in range(n):
        l = rng.randint(-c, c - 1)
        r = rng.randint(l + 1, c)
        lines.append(f"{l} {r} {rng.randint(-c, c)}")
    for _ in range(m):
        lines.append(f"{rng.randint(-c, c)} {rng.randint(-c, c)}")
    return "\n".join(lines) + "\n"


def run(exe: str, data: str) -> str:
    return subprocess.run([exe], input=data, capture_output=True,
                          text=True, check=True).stdout


def main() -> None:
    sol, naive = sys.argv[1], sys.argv[2]
    iters = int(sys.argv[3]) if len(sys.argv) > 3 else 2000
    rng = random.Random(1)
    for i in range(iters):
        data = gen(rng)
        if run(sol, data) != run(naive, data):
            print("FAIL on test:\n" + data)
            sys.exit(1)
    print(f"OK: {iters} random tests passed")


if __name__ == "__main__":
    main()
