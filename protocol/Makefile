run: bin/libtest.so
	pscript run.pss raw > raw.txt && python evaluate.py raw.txt
	pscript run.pss proto > proto.txt && python evaluate.py proto.txt
	pscript run.pss json > json.txt && python evaluate.py json.txt
	pscript run.pss xml > xml.txt && python evaluate.py xml.txt

SERVLETS=test
CFLAGS=-g -Wall -DLOG_LEVEL=0 -O3  `xml2-config --cflags` `pkg-config --cflags json-c` -Wno-deprecated-declarations
LDFLAGS=`xml2-config --libs` `pkg-config --libs json-c`

include shared.mk


