
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/stat.h>

extern int LLVMFuzzerInitialize(int *argc, char ***argv);
extern int LLVMFuzzerTestOneInput(const char *data, size_t size);

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
    LLVMFuzzerInitialize(&argc, &argv);
	uint8_t *data;
	size_t size = read_file(argv[1], &data);

    int ret = LLVMFuzzerTestOneInput(data, size);
    free(data);
    return ret;
}
