run: bin/libcopy.so
	pscript run.pss 1 > out_1.txt && python evaluate.py out_1.txt
	pscript run.pss 64 > out_64.txt && python evaluate.py out_64.txt
	pscript run.pss 4096 > out_4096.txt && python evaluate.py out_4096.txt
	pscript run.pss 65536 > out_65536.txt && python evaluate.py out_65536.txt
	pscript run.pss 131072 > out_131072.txt && python evaluate.py out_131072.txt

SERVLETS=copy
CFLAGS=-g -Wall -DLOG_LEVEL=0 -O3

include shared.mk


