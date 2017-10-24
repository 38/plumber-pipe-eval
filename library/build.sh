for size in `grep '^C' libtest/test.h | awk -F'[()]' '{print $2}'`
do
	gcc -O3 main.c -DTYPE=_c_${size} -DFUNC=f${size} -Llibtest -ltest -o test_${size}
	LD_LIBRARY_PATH=libtest ./test_${size} > out_${size}.txt
	rm test_${size}
	./evaluate.py out_${size}.txt
done

