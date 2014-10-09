all: make-libs cds-ubb sdsl-ubb

make-libs:
	(cd libcds; ./configure --prefix=`readlink -f ./../` CXX='g++ --std=c++11' && make && make install)
	(cd randbitmap; make)
	./sdsl_install.sh
	./randbitmap/bin/randbitmap 1234 0.1 134217728 data/testfile.dat

sdsl-ubb:
	g++ -O2 -g -o bin/sdsl-ubb -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64

sdsl-run:
	./bin/sdsl-ubb
	
sdsl-bench:
	valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-ubb
	callgrind_annotate
	kcachegrind
	@echo "Don't forget a make clean!"

cds-ubb:
	g++ -O2 -g -o bin/cds-ubb -Llib -lcds -std=c++11 -Iinclude src/cds-unrank_bottleneck_bench.cpp	

cds-run:
	LD_PRELOAD=./lib/libcds.so ./bin/cds-ubb
	
cds-bench:
	LD_PRELOAD=./lib/libcds.so valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes --instr-atstart=no --dump-instr=yes ./bin/cds-ubb
	callgrind_annotate
	kcachegrind
	@echo "Don't forget a make clean!"
	
clean:
	rm -f callgrind.out.*
	
