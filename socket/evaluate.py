#!/usr/bin/python
import sys
content = "".join(file(sys.argv[1]).readlines())
client = [0] * 64
server = [0] * 64
exec(content)
total = 0
for i in range(32):
    total += server[i*2] - client[i*2] + client[i*2+1] - server[i*2+1]
print total / 32


