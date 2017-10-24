all: server client
	@./client 1 > out_1.txt && ./evaluate.py out_1.txt
	@./client 64 > out_64.txt && ./evaluate.py out_64.txt
	@./client 4096 > out_4096.txt && ./evaluate.py out_4096.txt
	@./client 65536 > out_65536.txt && ./evaluate.py out_65536.txt
	@./client 131072 > out_131072.txt && ./evaluate.py out_131072.txt

%:%.c
	gcc -O3 $^ -o $@ -g

clean:
	rm server client

.PHONY: all clean
