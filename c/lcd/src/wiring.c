#include <wiringPi.h>
int main (void)
{
    wiringPiSetup () ;
    printf("test1\n");
    delayMicroseconds(1000000);
    printf("test2\n");
    delayMicroseconds(1000000);
    printf("test3\n");
    return 0 ;
}
