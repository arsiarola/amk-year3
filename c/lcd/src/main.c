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


int main() {
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to "wlan0" */
	strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);

    wiringPiSetup();
 digitalWrite(RS, 0);
 digitalWrite(EN, 0);
 digitalWrite(D4, 0);
 digitalWrite(D5, 0);
 digitalWrite(D6, 0);
 digitalWrite(D7, 0);

 pinMode(RS, OUTPUT);
 pinMode(EN, OUTPUT);
 pinMode(D4, OUTPUT);
 pinMode(D5, OUTPUT);
 pinMode(D6, OUTPUT);
 pinMode(D7, OUTPUT);
   delay (35) ; // mS

	lcdCreate(RS, EN, D4, D5, D6, D7);
    lcdClear();
	lcdSetCursor(0, 0);
    lcdHome();
	lcdPrint("Getting");
	lcdSetCursor(0, 1);
	lcdPrint("IP address");
    /* int lcd; */
    /* lcd = lcdInit (2, 16, 4, RS, EN, D4, D5, D6, D7, 0, 0, 0, 0); */
    /* delay(2); */
    /* lcdClear(lcd); */
    /* lcdPosition(lcd, 3, 1); */
    /* lcdPuts(lcd, "Hello, world!"); */
    delay(2000);
	/* display result */
    char ipAddress[16];
	snprintf(ipAddress, 16, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    printf("%s\n", ipAddress);
    /* lcdPrintf(lcd, "%s\n", ipAddress); */
	lcdClear();
	lcdPrint("%s\n", ipAddress);
	return 0;
}
