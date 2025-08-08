#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 8192

/* Helper: write exactly count bytes handling EINTR and short writes */
static int full_write(int fd, const void *buf, size_t count) {
    const char *p = buf;
    while (count > 0) {
        ssize_t n = write(fd, p, count);
        if (n <= 0)
            return -1;          /* Fehler oder nichts geschrieben */
        p    += n;
        count -= (size_t)n;
    }
    return 0;
}


/**
 * Copies the content from standard input (stdin) to both standard output (stdout)
 * and a specified output file.
 *
 * Functionality:
 * - Reads data from stdin.
 * - Writes the same data to stdout.
 * - Writes the same data to the file specified by `output_file`.
 *
 * Attention!
 * - Print ONLY the data from stdin to stdout! If you want to also print
 *     debugging information, please put it on stderr! (see `man 3 fprintf`)
 *
 * @param output_file Path to the file where the stdin content should be written.
 * @return int Status code indicating success (0) or failure (-1).
 */
int tee(const char *output_file) {
    if (!output_file)
        return -1;

    int out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1)
        return -1;

    char buf[BUF_SIZE];
    ssize_t r;
    while ((r = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if (full_write(STDOUT_FILENO, buf, (size_t)r) == -1 ||
            full_write(out_fd,        buf, (size_t)r) == -1) {
            close(out_fd);
            return -1;
        }
    }

    if (r < 0) { /* read()‑Fehler */
        close(out_fd);
        return -1;
    }

    return close(out_fd);
}

/**
 * Reverses the content of `input_file` byte by byte and writes the result to `output_file`.
 *
 * Functionality:
 * - Reads the content of `input_file`.
 * - Reverses the order of bytes.
 * - Writes the reversed content to `output_file`.
 *
 * Preconditions:
 * - The function should create `output_file` if it does not exist.
 *
 * @param input_file Path to the input file whose content needs to be reversed.
 * @param output_file Path to the file where the reversed content should be written.
 * @return int Status code indicating success (0) or failure (-1).
 */
int reverse_file(const char *input_file, const char *output_file) {
    if (!input_file || !output_file)
        return -1;

    int in_fd = open(input_file, O_RDONLY);
    if (in_fd == -1)
        return -1;

    int out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        close(in_fd);
        return -1;
    }

    off_t size = lseek(in_fd, 0, SEEK_END);
    if (size == -1) {
        close(in_fd);
        close(out_fd);
        return -1;
    }

    char byte;
    for (off_t i = 1; i <= size; ++i) {
        if (lseek(in_fd, -i, SEEK_END) == -1 ||
            read(in_fd, &byte, 1) != 1   ||
            full_write(out_fd, &byte, 1) == -1) {
            close(in_fd);
            close(out_fd);
            return -1;
        }
    }

    close(in_fd);
    return close(out_fd);
}

/**
 * Do the same as reverse_file, but more optmized (you have constraints on
 * execution time and on memory resources
 */
int reverse_file_optimized(const char *input_file, const char *output_file) {
    if (!input_file || !output_file)
        return -1;

    int in_fd = open(input_file, O_RDONLY);
    if (in_fd == -1)
        return -1;

    int out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        close(in_fd);
        return -1;
    }

    const size_t CHUNK = 65536;  /* 64 KiB */
    char buf[65536];

    off_t remaining = lseek(in_fd, 0, SEEK_END);
    if (remaining == -1) {
        close(in_fd);
        close(out_fd);
        return -1;
    }

    while (remaining > 0) {
        size_t to_read = remaining >= (off_t)CHUNK ? CHUNK : (size_t)remaining;
        remaining -= to_read;

        if (lseek(in_fd, remaining, SEEK_SET) == -1 ||
            read(in_fd, buf, to_read) != (ssize_t)to_read) {
            close(in_fd);
            close(out_fd);
            return -1;
        }

        /* Block umdrehen */
        for (size_t i = 0, j = to_read - 1; i < j; ++i, --j) {
            char tmp = buf[i];
            buf[i] = buf[j];
            buf[j] = tmp;
        }

        if (full_write(out_fd, buf, to_read) == -1) {
            close(in_fd);
            close(out_fd);
            return -1;
        }
    }

    close(in_fd);
    return close(out_fd);
}