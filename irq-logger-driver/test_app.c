#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int fd;
    char buffer[100];
    ssize_t bytesRead;

    fd = open("/dev/irq_logger_device", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open /dev/irq_logger_device");
        return EXIT_FAILURE;
    }

    bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead < 0) {
        perror("Failed to read from device");
        close(fd);
        return EXIT_FAILURE;
    }

    buffer[bytesRead] = '\0';
    printf("Read from device: %s\n", buffer);

    close(fd);
    return 0;
}

