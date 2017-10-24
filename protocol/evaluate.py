#!/usr/bin/python
import sys
content = "".join(file(sys.argv[1]).readlines())
source = [0] * 512
sink = [0] * 512
exec(content)
total = 0
for i in range(512):
    total += sink[i] - source[i]
print total / 512


