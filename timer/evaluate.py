#!/usr/bin/python
import sys
content = "".join(file(sys.argv[1]).readlines())
main = [0] * 512
exec(content)
total = 0
for i in range(256):
    total += main[i * 2 + 1] - main[i * 2]
print total / 256


