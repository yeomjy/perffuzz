#
# General
#
#
SRC_DIR=$(realpath ../..)

CC=$(SRC_DIR)/afl-clang-fast
CXX=$(SRC_DIR)/afl-clang-fast++

# Use local copy of Fuzzer
LIBFUZZER_SRC=$(SRC_DIR)/Fuzzer
LIBFUZZER_LIB=$(LIBFUZZER_SRC)/libFuzzer.a

# DBGFLAGS=-g -ggdb3
# COV_FLAGS=-fsanitize=address -fsanitize-coverage=edge,trace-pc-guard
# CFLAGS=-O2 -Wall -Wno-unused-variable $(DBGFLAGS) $(COV_FLAGS)
# CXXFLAGS=-O2 -Wall -std=c++11 -Wno-unused-variable $(DBGFLAGS) $(COV_FLAGS)
DBGFLAGS=-g -ggdb3 # -gline-tables-only
COV_FLAGS=-fsanitize=address -fsanitize-address-use-after-scope 
CFLAGS=-O1 -fno-omit-frame-pointer -Wall -Wno-unused-variable $(DBGFLAGS)
CXXFLAGS=-O1 -fno-omit-frame-pointer -Wall -Wno-unused-variable $(DBGFLAGS) -std=c++11

# Defaults for LibFuzzer
CORPUS=corpus/
OUTPUT=out/
RUNS=-1
MAXLEN=500
OBJDIR=objs

TARGET=driver
TARGET_SRC=libpng/contrib/oss-fuzz/libpng_read_fuzzer.cc

.PHONY:all
all: prelim $(TARGET)


prelim:
	rm -f $(TARGET)

.PHONY: libpng
libpng:
	cat libpng/scripts/pnglibconf.dfa | \
  	sed -e "s/option STDIO/option STDIO disabled/" \
      	-e "s/option WARNING /option WARNING disabled/" \
      	-e "s/option WRITE enables WRITE_INT_FUNCTIONS/option WRITE disabled/" \
	> libpng/scripts/pnglibconf.dfa.temp
	mv libpng/scripts/pnglibconf.dfa.temp libpng/scripts/pnglibconf.dfa
	(cd ./libpng && autoreconf -f -i \
		&& ./configure --with-libpng-prefix=SLOWFUZZ_)
	make --directory=./libpng clean
	make --directory=./libpng -j$(nproc) libpng16.la \
		CC="$(CC)" \
		CXX="$(CXX)" \
		CFLAGS="$(CFLAGS) $(COV_FLAGS)" \
		CXXFLAGS="$(CXXFLAGS) $(COV_FLAGS)"

# make our modified version of the Fuzzer
fuzzer:
	$(CXX) -c -g -O2 -std=c++11 $(LIBFUZZER_SRC)/*.cpp -I$(LIBFUZZER_SRC)
	ar rv $(LIBFUZZER_SRC)/libFuzzer.a Fuzzer*.o
	rm Fuzzer*.o

$(TARGET): $(TARGET_SRC)
	# $(CXX) $(CXXFLAGS) $(COV_FLAGS) $< $(LIBFUZZER_LIB) -o $@
	$(CXX) $(CXXFLAGS) $(COV_FLAGS) -I./libpng $< libpng/.libs/libpng16.a -lz -o $@
	

# ./seed corpus/1 `seq -s' ' 0 999`
.PHONY: test
test: $(TARGET)
	rm -rf $(CORPUS) $(OUTPUT)
	mkdir -p $(CORPUS) $(OUTPUT)
	unzip seed.zip -d $(CORPUS)
	ASAN_OPTIONS=halt_on_error=0:coverage=1:coverage_order_pcs=1:coverage_counters=1 \
	./$(TARGET) $(CORPUS) \
	-dict=png.dict \
	-artifact_prefix=$(OUTPUT) -print_final_stats=1 \
	-detect_leaks=0 -rss_limit_mb=10000 -shuffle=0 \
	-runs=-1 -max_len=$(MAXLEN) -death_mode=3 -max_total_time=86400

.PHONY: tools
tools:
	$(CXX) -o print tools/print.cpp
	$(CXX) -o seed tools/seed.cpp

time:
	$(CXX) -o time_test tests/time_test.cpp -I.


ins: tests/ins_test.cpp
	$(CXX) -c -g -O2 -std=c++11 tests/ins_test.cpp -o tests/ins_test.o
	$(CXX) $(CXXFLAGS) $(COV_FLAGS) tests/ins_test.o -o ins_test
	rm tests/ins_test.o

.PHONY:clean
clean:
	rm -rf *.o *.a $(TARGET) qtable.bin print seed time_test ins_test
	rm -rf $(OBJDIR)

.PHONY:cleanf
cleanf:
	rm $(LIBFUZZER_LIB)
