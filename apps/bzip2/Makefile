#
# General
#
#

SRC_DIR=../..

CC=$(SRC_DIR)/afl-clang-fast
CXX=$(SRC_DIR)/afl-clang-fast++
# Use local copy of Fuzzer
# LIBFUZZER_SRC=$(SRC_DIR)/Fuzzer
# LIBFUZZER_LIB=$(LIBFUZZER_SRC)/libFuzzer.a

# CFLAGS=-O1 -fno-omit-frame-pointer -gline-tables-only -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION -fsanitize=address -fsanitize-address-use-after-scope 
# CFLAGS=-O2 -Wall -Wno-unused-variable $(DBGFLAGS)
# CXXFLAGS=-O2 -Wall -std=c++11 -Wno-unused-variable $(DBGFLAGS)
DBGFLAGS=-g -ggdb3 -gline-tables-only
COV_FLAGS=-fsanitize=address -fsanitize-address-use-after-scope 
CFLAGS=-O1 -fno-omit-frame-pointer -Wall -Wno-unused-variable $(DBGFLAGS)
CXXFLAGS=-O1 -fno-omit-frame-pointer -Wall -Wno-unused-variable $(DBGFLAGS)

# Defaults for LibFuzzer
CORPUS=corpus/
OUTPUT=out/
RUNS=-1
MAXLEN=250
OBJDIR=objs

BZIP_OBJS=blocksort huffman crctable randtable compress decompress bzlib
LIBBZIP=libbz2.a
BZIP_SRC=bzip2


TARGET=driver
TARGET_SRC=bzip2_decompress_target.c

.PHONY:all
all: prelim $(TARGET)

bzip_prelim:
	mkdir -p $(OBJDIR)

$(BZIP_OBJS): bzip_prelim
	$(CC) $(CFLAGS) $(COV_FLAGS) -c $(BZIP_SRC)/$@.c -o $(OBJDIR)/$@.o -I./$(BZIP_SRC)

$(LIBBZIP): $(BZIP_OBJS)
	ar cq $(LIBBZIP) $(foreach I,$(BZIP_OBJS),$(OBJDIR)/$I.o)
	ranlib $(LIBBZIP)

prelim:
	rm -f $(TARGET)


$(TARGET): $(TARGET_SRC)
	# $(CXX) $(CXXFLAGS) $(COV_FLAGS) $< $(LIBFUZZER_LIB) -o $@
	# $(CC) $(CFLAGS) $(COV_FLAGS) -c -I./$(BZIP_SRC) $< -o $@.o
	# $(CXX) $(CXXFLAGS) $(COV_FLAGS) $@.o $(LIBBZIP) -o $@
	$(CC) $(CFLAGS) $(COV_FLAGS) -I./$(BZIP_SRC) $< $(LIBBZIP) -o $@
	

# ./seed corpus/1 `seq -s' ' 0 999`
.PHONY: test
test: $(TARGET)
	rm -rf $(CORPUS) $(OUTPUT)
	mkdir -p $(CORPUS) $(OUTPUT)
	unzip bzip2_decompress_target_seed_corpus.zip -d $(CORPUS)
	ASAN_OPTIONS=halt_on_error=0:coverage=1:coverage_order_pcs=1:coverage_counters=1 \
	./$(TARGET) $(CORPUS) \
	-artifact_prefix=$(OUTPUT) -print_final_stats=1 \
	-detect_leaks=0 -rss_limit_mb=10000 -shuffle=0 \
	-runs=-1 -max_len=$(MAXLEN) -death_mode=3 -max_total_time=86400 \
	-seed=42

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
