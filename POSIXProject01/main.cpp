#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>

int main() {
    const char* portname = "/dev/ttyS11"; // WSL’de doğru port /dev/ttySx veya COMx olabilir
    int fd = open(portname, O_RDWR); // O_NOCTTY

    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Seri port ayarları
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return 1;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;       // 8 bit
    tty.c_cflag &= ~PARENB;   // parity yok
    tty.c_cflag &= ~CSTOPB;   // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;  // flow control yok

    tcsetattr(fd, TCSANOW, &tty);

    // Veri gönder
    const char* msg = "Merhaba STM32!";
    write(fd, msg, strlen(msg));

    // Veri oku
    char buf[100];
    int n = read(fd, buf, sizeof(buf));
    buf[n] = '\0';
    std::cout << "STM32’den gelen: " << buf << std::endl;

    close(fd);
    return 0;
}
