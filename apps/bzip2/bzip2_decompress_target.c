/*
# Copyright 2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
###############################################################################
*/

#include "bzlib.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>

extern int BZ2_bzBuffToBuffDecompress(char* dest,
                                      unsigned int* destLen,
                                      char*         source,
                                      unsigned int  sourceLen,
                                      int           small,
                                      int           verbosity);

int
LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int r, small;
    unsigned int nZ, nOut;

    // See: https://github.com/google/bzip2-rpc/blob/master/unzcrash.c#L39
    nOut = size*2;
    char *outbuf = malloc(nOut);
    small = size % 2;
    r = BZ2_bzBuffToBuffDecompress(outbuf, &nOut, (char *)data, size,
            small, /*verbosity=*/0);

    if (r != BZ_OK) {
#ifdef __DEBUG__
        fprintf(stdout, "Decompression error: %d\n", r);
#endif
    }
    free(outbuf);
    return 0;
}

static int read_file(const char *fn, unsigned char **buf)
{
    struct stat file_status;
    FILE *fp;
    int ret = -1;

    if ((stat(fn, &file_status) != 0) ||
        ((fp = fopen(fn, "r")) == NULL) ||
        ((*buf = (unsigned char *)malloc(file_status.st_size)) == NULL)) {
        perror("read_file"); \
        return -1;
    }

    if (!fread(*buf, file_status.st_size, 1, fp)) {
        perror("read_file");
        free(*buf);
    } else {
        ret = file_status.st_size;
    }

    fclose(fp);
    return ret;
}

int main(int argc, char *argv[])
{
	uint8_t *data;
	size_t size = read_file(argv[1], &data);

    int ret = LLVMFuzzerTestOneInput(data, size);
    free(data);
    return ret;
}
