#ifndef MOTOR_H_
#define MOTOR_H_

#include "DigitalIoPin.h"

class Motor {
public:

    Motor(
        DigitalIoPin& motor,
        DigitalIoPin& lim0,
        DigitalIoPin& lim1,
        DigitalIoPin& direction
    );
    virtual ~Motor();

private:
    DigitalIoPin& motor;
    DigitalIoPin& lim0; // 0 meaning zero point aka origin
    DigitalIoPin& lim1;
    DigitalIoPin& direction;
};
#endif /* MOTOR_H_ */
