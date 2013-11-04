#include "motor.h"

Mutex motorMutex;
uint16_t motorDuty;

/* as specified in Talon User Manual */
static PWMConfig motorPWMcfg = {
  1000000,
  20000,
  NULL,
  { PWM_OUTPUT_ACTIVE_HIGH, NULL },
  0  
};

msg_t motorThread(void *arg) {
  // PWMChannelConfig chcfg = { PWM_OUTPUT_ACTIVE_HIGH, NULL };
  // motorPWMcfg.channels[0] = chcfg;
  
  palSetPadMode(GPIOB, 1, PAL_STM32_OTYPE_PUSHPULL);
  
  pwmStart(&PWMD1, &motorPWMcfg);
  
  chMtxInit(&motorMutex);
  
  while (true) {
    chMtxLock(&motorMutex);
    pwmEnableChannel(&PWMD1, 0, (pwmcnt_t)motorDuty);
    chMtxUnlock();
    chThdYield();
  }
}
