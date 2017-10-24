#!/usr/bin/python
import sys
content = "".join(file(sys.argv[1]).readlines())
source = [0] * 64
sink = [0] * 64
copy = [0] * 64
exec(content)
total = 0
for i in range(32):
    total += copy[i*2] - source[i*2] + sink[i*2] - source[i*2 + 1]
print total / 32


