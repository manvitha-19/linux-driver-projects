#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define DEVICE_PATH "/dev/simple_char_device"
#define BUF_SIZE 1024

void write_to_driver(int fd) {
    char write_buf[BUF_SIZE] = {0};

    printf("Enter the string to write into driver: ");
    fgets(write_buf, BUF_SIZE, stdin);

    // Remove newline if present
    write_buf[strcspn(write_buf, "\n")] = 0;

    ssize_t written = write(fd, write_buf, strlen(write_buf));
    if (written < 0)
        perror("Write failed");
    else
        printf("Data written successfully.\n");
}

void read_from_driver(int fd) {
    char read_buf[BUF_SIZE] = {0};

    ssize_t read_bytes = read(fd, read_buf, BUF_SIZE);
    if (read_bytes < 0)
        perror("Read failed");
    else {
        printf("Data read successfully.\n");
        printf("Data = %s\n", read_buf);
    }
}

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Cannot open device file");
        return EXIT_FAILURE;
    }

    while (1) {
        char option[3];

        printf("\n********** MENU **********\n");
        printf("1. Write\n");
        printf("2. Read\n");
        printf("3. Exit\n");
        printf("Enter your option: ");

        if (!fgets(option, sizeof(option), stdin)) break;

        switch (option[0]) {
            case '1':
                write_to_driver(fd);
                break;
            case '2':
                read_from_driver(fd);
                break;
            case '3':
                close(fd);
                printf("Exiting...\n");
                return EXIT_SUCCESS;
            default:
                printf("Invalid option. Try again.\n");
        }
    }

    close(fd);
    return EXIT_SUCCESS;
}

