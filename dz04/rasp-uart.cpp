#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>

#define UART_PATH "/dev/serial0"

void init_uart(int& fd) {
    fd = open(UART_PATH, O_RDWR);

    if(fd == -1)
        perror("Error: Unable to open UART\n");

    termios options{};
    options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);
}

void send_byte(int& fd, uint8_t b) {
    if(fd == -1)
        perror("Error: Unable to open UART\n");

    uint16_t buffer[1] = {b};

    if (write(fd, buffer, 1) < 0)
        perror("Error: TX\n");
}

void receive_byte(int& fd, uint16_t& b){

    uint8_t buffer[2] = {};

    if(fd != -1){
        read(fd, (void*)buffer, 2);
        b = (buffer[0] << 8) | buffer[1];
    } else {
        perror("Error: RX\n");
    }
}

int main() {
    int fd{};

    init_uart(fd);

    uint16_t r{0};

    while(true){
        receive_byte(fd, r);
        printf("%u\n",r);
        usleep(100000);
    }

    close(fd);

    return 0;
}