#include "resource.h"

unsigned short offset = 0;

int random_number_between(int min, int max) {
    int fd = open("/dev/urandom", O_RDWR);
    struct timeval time = {};
    ::gettimeofday(&time, null);
    u_int defaultSeed;
    if (fd == 0) {
        defaultSeed = hash3((u_int) time.tv_sec, (u_int) time.tv_usec, (u_int) getpid());
    } else {
        size_t buffer[1];
        lseek(fd, offset, 0);
        if (read(fd, buffer, sizeof(size_t)) != sizeof(size_t)) {
            defaultSeed = hash3((u_int) time.tv_sec, (u_int) time.tv_usec, (u_int) getpid());
        } else {
            defaultSeed = (u_int) buffer[0];
            offset += 8;

            if (offset > 2048) {
                offset = 0;
            }
        }
        close(fd);
    }

    std::random_device randomDevice;
    std::default_random_engine randomEngine(randomDevice());
    randomEngine.seed(defaultSeed);

    return ((u_int) randomEngine()) % ((max + 1) - min) + min;
}

unsigned int hash3(unsigned int h1, unsigned int h2, unsigned int h3) {
    return (((h1 * 2654435789U) + h2) * 2654435789U) + h3;
}

int error_check_fread(void* __restrict ptr, size_t size, size_t nmemb, FILE* stream) {
    if (fread(ptr, size, nmemb, stream) != nmemb) {
        return 1;
    }

    return 0;
}

int error_check_fwrite(void* __restrict ptr, size_t size, size_t nmemb, FILE* stream) {
    if (fwrite(ptr, size, nmemb, stream) != nmemb) {
        return 1;
    }
    return 0;
}

u_char convert_base10_to_char(u_int integerValue) {
    if (integerValue < 10) {
        return (u_char) ('0' + integerValue);
    } else {
        return (u_char) ('a' + (integerValue - 10));
    }
}

bool strstarts(std::string string, std::string prefix) {
    return string.rfind(prefix, 0) == 0;
}

int get_sign(int value) {
    if (value < 0) {
        return -1;
    } else {
        return 1;
    }
}