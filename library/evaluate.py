#!/usr/bin/python
import sys
content = "".join(file(sys.argv[1]).readlines())
main = [0] * 64
static_lib = [0] * 64
exec(content)
total = 0
for i in range(32):
    total += static_lib[i*2] - main[i*2] + main[i*2+1] - static_lib[i*2+1]
print total / 32


