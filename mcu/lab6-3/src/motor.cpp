#include "motor.h"

Motor::Motor(
        DigitalIoPin& motor,
        DigitalIoPin& lim0,
        DigitalIoPin& lim1,
        DigitalIoPin& direction
    ) :
        motor(motor),
        lim0 (lim0),
        lim1(lim1),
        direction(direction)
{ }
