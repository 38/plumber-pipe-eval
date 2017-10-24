for size in 1 64 4096 65536 131072
do 
	for ((i=0;i<32;i++)) 
	do 
		echo ".TEXT case_${i}";
		cat  /dev/urandom  | tr -dc '0-9a-zA-Z' | head -c ${size}; 
		echo "\n.END";

	done > data/test_${size}.in; 
	echo ".STOP" >> data/test_${size}.in; 
done
