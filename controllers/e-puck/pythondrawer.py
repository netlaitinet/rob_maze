#!/usr/bin/env python3
import sys

src, dst = sys.argv[1:]
routes = []

with open(src) as f:
    for line in f:
        line = line.strip()
        if not line:
            continue

        if line.startswith("# route"):
            grid = [[" "] * 256 for _ in range(128)]
            routes.append(grid)
        elif not line.startswith("#"):
            x, y = map(float, line.split())
            x, y = int(x), int(y)
            if 0 <= x < 256 and 0 <= y < 256:
                grid[y // 2][x] = "#"

with open(dst, "w") as f:
    for i, grid in enumerate(routes):
        if i:
            f.write("\n")
        for row in reversed(grid):
            f.write("".join(row) + "\n")
