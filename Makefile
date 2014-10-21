all: make-libs sdsl

make-libs:
	(cd randbitmap; make)
	./sdsl_binomsize_install.sh
	./sdsl_linear_install.sh
	./sdsl_install.sh
	./randbitmap/bin/randbitmap 1234 0.1 134217728 data/testfile01.dat
	./randbitmap/bin/randbitmap 1234 0.2 134217728 data/testfile02.dat
	./randbitmap/bin/randbitmap 1234 0.3 134217728 data/testfile03.dat
	./randbitmap/bin/randbitmap 1234 0.01 134217728 data/testfile001.dat
	./randbitmap/bin/randbitmap 1234 0.001 134217728 data/testfile0001.dat

sdsl-binom: sdsl-binom-comp sdsl-binom-run

sdsl-binom-comp:
	g++ -O2 -g -o bin/sdsl-binomial -Isdsl_binomsize_build/include -Lsdsl_binomsize_build/lib src/sdsl-binomial.cpp -lsdsl -ldivsufsort -ldivsufsort64

sdsl: sdsl-normal-comp sdsl-linear-comp

sdsl-normal: sdsl-normal-comp

sdsl-linear: sdsl-linear-comp

sdsl-linear-comp:
	g++ -O2 -g -o bin/sdsl-linear-01-9 -DBLOCKSIZE=9 -DDENSITY=\"01\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-01-17 -DBLOCKSIZE=17 -DDENSITY=\"01\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-01-33 -DBLOCKSIZE=33 -DDENSITY=\"01\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-01-65 -DBLOCKSIZE=65 -DDENSITY=\"01\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-01-129 -DBLOCKSIZE=129 -DDENSITY=\"01\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-02-9 -DBLOCKSIZE=9 -DDENSITY=\"02\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-02-17 -DBLOCKSIZE=17 -DDENSITY=\"02\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-02-33 -DBLOCKSIZE=33 -DDENSITY=\"02\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-02-65 -DBLOCKSIZE=65 -DDENSITY=\"02\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-02-129 -DBLOCKSIZE=129 -DDENSITY=\"02\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-03-9 -DBLOCKSIZE=9 -DDENSITY=\"03\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-03-17 -DBLOCKSIZE=17 -DDENSITY=\"03\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-03-33 -DBLOCKSIZE=33 -DDENSITY=\"03\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-03-65 -DBLOCKSIZE=65 -DDENSITY=\"03\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-03-129 -DBLOCKSIZE=129 -DDENSITY=\"03\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-001-9 -DBLOCKSIZE=9 -DDENSITY=\"001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-001-17 -DBLOCKSIZE=17 -DDENSITY=\"001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-001-33 -DBLOCKSIZE=33 -DDENSITY=\"001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-001-65 -DBLOCKSIZE=65 -DDENSITY=\"001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-001-129 -DBLOCKSIZE=129 -DDENSITY=\"001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-0001-9 -DBLOCKSIZE=9 -DDENSITY=\"0001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-0001-17 -DBLOCKSIZE=17 -DDENSITY=\"0001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-0001-33 -DBLOCKSIZE=33 -DDENSITY=\"0001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-0001-65 -DBLOCKSIZE=65 -DDENSITY=\"0001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-linear-0001-129 -DBLOCKSIZE=129 -DDENSITY=\"0001\" -Isdsl_linear_build/include -Lsdsl_linear_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64

sdsl-normal-comp:
	g++ -O2 -g -o bin/sdsl-01-9 -DBLOCKSIZE=9 -DDENSITY=\"01\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-01-17 -DBLOCKSIZE=17 -DDENSITY=\"01\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-01-33 -DBLOCKSIZE=33 -DDENSITY=\"01\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-01-65 -DBLOCKSIZE=65 -DDENSITY=\"01\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-01-129 -DBLOCKSIZE=129 -DDENSITY=\"01\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-02-9 -DBLOCKSIZE=9 -DDENSITY=\"02\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-02-17 -DBLOCKSIZE=17 -DDENSITY=\"02\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-02-33 -DBLOCKSIZE=33 -DDENSITY=\"02\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-02-65 -DBLOCKSIZE=65 -DDENSITY=\"02\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-02-129 -DBLOCKSIZE=129 -DDENSITY=\"02\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-03-9 -DBLOCKSIZE=9 -DDENSITY=\"03\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-03-17 -DBLOCKSIZE=17 -DDENSITY=\"03\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-03-33 -DBLOCKSIZE=33 -DDENSITY=\"03\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-03-65 -DBLOCKSIZE=65 -DDENSITY=\"03\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-03-129 -DBLOCKSIZE=129 -DDENSITY=\"03\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-001-9 -DBLOCKSIZE=9 -DDENSITY=\"001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-001-17 -DBLOCKSIZE=17 -DDENSITY=\"001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-001-33 -DBLOCKSIZE=33 -DDENSITY=\"001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-001-65 -DBLOCKSIZE=65 -DDENSITY=\"001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-001-129 -DBLOCKSIZE=129 -DDENSITY=\"001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-0001-9 -DBLOCKSIZE=9 -DDENSITY=\"0001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-0001-17 -DBLOCKSIZE=17 -DDENSITY=\"0001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-0001-33 -DBLOCKSIZE=33 -DDENSITY=\"0001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-0001-65 -DBLOCKSIZE=65 -DDENSITY=\"0001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
	g++ -O2 -g -o bin/sdsl-0001-129 -DBLOCKSIZE=129 -DDENSITY=\"0001\" -Isdsl_build/include -Lsdsl_build/lib src/sdsl-unrank_bottleneck_bench.cpp -lsdsl -ldivsufsort -ldivsufsort64
		
sdsl-normal-wobp-bench:
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*

sdsl-normal-bench:
	valgrind --tool=callgrind --branch-sim=yes  --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes  --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes  --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes  --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-01-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-02-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-03-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-001-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-0001-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*

sdsl-linear-bench:
	valgrind --tool=callgrind --branch-sim=yes  --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-01-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes  --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-01-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes  --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-01-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes  --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-01-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-01-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-02-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-02-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-02-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-02-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-02-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-03-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-03-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-03-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-03-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-03-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-001-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-001-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-001-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-001-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-001-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-0001-9
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-0001-17
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-0001-33
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-0001-65
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*
	valgrind --tool=callgrind --branch-sim=yes --cache-sim=yes --instr-atstart=no --dump-instr=yes ./bin/sdsl-linear-0001-129
	callgrind_annotate
	kcachegrind
	rm -f callgrind.out.*

clean:
	rm -f callgrind.out.*
	
