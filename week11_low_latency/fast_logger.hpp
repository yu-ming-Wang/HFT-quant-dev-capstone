#pragma once
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <iostream>

class FastLogger {
public:
    FastLogger(const char* filename, size_t size = 2 * 1024 * 1024) {
        fd = open(filename, O_RDWR | O_CREAT, 0666);
        if (fd < 0) perror("open failed");

        if (ftruncate(fd, size) != 0) perror("ftruncate failed");

        log_size = size;
        mapped = (char*) mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (mapped == MAP_FAILED) perror("mmap failed");

        cursor = 0;
    }

    void log(const std::string& msg) {
        size_t len = msg.size();
        if (cursor + len + 1 >= log_size) return;
        memcpy(mapped + cursor, msg.c_str(), len);
        cursor += len;
        mapped[cursor++] = '\n';
    }

    ~FastLogger() {
        msync(mapped, log_size, MS_SYNC);
        munmap(mapped, log_size);
        close(fd);
    }

private:
    int fd;
    char* mapped;
    size_t cursor;
    size_t log_size;
};
