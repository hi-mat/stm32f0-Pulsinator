
#ifndef MOTOR_H
#define MOTOR_H

#include "ch.h"
#include "hal.h"
#include "board.h"
#include "stm32f0xx.h"
#include "pal.h"
#include "pwm.h"

extern Mutex motorMutex;
extern uint16_t motorDuty;

extern msg_t motorThread(void *arg);

#endif

