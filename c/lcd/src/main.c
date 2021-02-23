#define USE_OWN_LCD_LIB 1
#if USE_OWN_LCD_LIB
#include "lcd.h"
#else
#include <lcd.h>
#endif

#include <wiringPi.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#define RS 29
#define EN 28
#define D4 21
#define D5 22
#define D6 23
#define D7 24

void initPins();

int main() {

    wiringPiSetup();
    initPins();

    lcdCreate(RS, EN, D4, D5, D6, D7);

    lcdClear();
    lcdPrint("Getting");
    lcdSetCursor(0, 1);
    lcdPrint("IP address");
    delay(2000);

    int fd;
    struct ifreq ifr;
    while (1) {
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        ifr.ifr_addr.sa_family = AF_INET; // IPv4 IP address
        strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
        ioctl(fd, SIOCGIFADDR, &ifr);
        close(fd);

        char ipAddress[16];
        snprintf(ipAddress, 16, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
        printf("%s\n", ipAddress);
        lcdClear();
        lcdPrint("%s\n", ipAddress);
        delay(2000);
    }
    return 0;
}

void initPins() {
    digitalWrite(RS, 0); pinMode(RS, OUTPUT);
    digitalWrite(EN, 0); pinMode(EN, OUTPUT);
    digitalWrite(D4, 0); pinMode(D4, OUTPUT);
    digitalWrite(D5, 0); pinMode(D5, OUTPUT);
    digitalWrite(D6, 0); pinMode(D6, OUTPUT);
    digitalWrite(D7, 0); pinMode(D7, OUTPUT);
    delay (35) ; // mS

}
