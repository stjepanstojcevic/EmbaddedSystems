#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_PATH "/dev/spidev0.0"
#define SEND_UINT16 0xff
#define RECEIVE_UINT16 0xfe
#define SEND_UINT32 0xfd
#define RECEIVE_UINT32 0xfc


void initSPI(int& fd, uint8_t* mode) {
    if ((fd = open(SPI_PATH, O_RDWR)) < 0) {
        perror("Unable to open SPI device");
        return;
    }

    if (ioctl(fd, SPI_IOC_WR_MODE, mode) < 0) {
        perror("Unable to set SPI mode");
        return;
    }

    if (ioctl(fd, SPI_IOC_RD_MODE, mode) < 0) {
        perror("Unable to get SPI mode");
        return;
    }

    printf("SPI mode is: %d\n", *mode);
}

int transfer(int& fd, const unsigned char* send, const unsigned char* rec, unsigned int len) {
    spi_ioc_transfer transfer{};

    transfer.tx_buf = (unsigned long)send;
    transfer.rx_buf = (unsigned long)rec;
    transfer.len = len;
    transfer.speed_hz = 1000000;
    transfer.bits_per_word = 8;
    transfer.delay_usecs = 0;

    int status = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);
    if (status < 0) {
        perror("Unable to transmit the data");
        return -1;
    }
    return status;
}

void send_uint16_t(int& fd, int16_t d) {
	spi_ioc_transfer transfer{};
	uint8_t tx[3];

	tx[0] = RECEIVE_UINT16;
	tx[1] = (d >> 8) & 0xff;
	tx[2] = d & 0xff;

    transfer.tx_buf = (unsigned long)rx;
    transfer.len = 3;
    transfer.speed_hz = 1000000;
    transfer.bits_per_word = 8;
    transfer.delay_usecs = 0;

    int status = ioctl(fd, SPI_IOC_MESSAGE(3), &transfer);
    if (status < 0) {
        perror("Unable to transmit the data");
        return -1;
    }
    return status;
}

void receive_uint16_t(int& fd, int16_t d) {
	spi_ioc_transfer transfer{};
	uint8_t tx[3];

	tx[0] = send_uint16_t;
	tx[1] = (d >> 8) & 0xff;
	tx[2] = d & 0xff;

    transfer.tx_buf = (unsigned long)rx;
    transfer.len = 3;
    transfer.speed_hz = 1000000;
    transfer.bits_per_word = 8;
    transfer.delay_usecs = 0;

    int status = ioctl(fd, SPI_IOC_MESSAGE(3), &transfer);
    if (status < 0) {
        perror("Unable to transmit the data");
        return -1;
    }
    return status;
}

int main() {
    int fd;
    uint8_t mode{0};

    initSPI(fd, &mode);

    unsigned char s{0};
    unsigned char r{0};

    for (unsigned int k = 0; k <= 255; ++k) {
        ++s;
        transfer(fd, &s, &r, 1);
        printf("%u\t%u\n", s, r);
        usleep(100000);
    }

    close(fd);
    return 0;
}