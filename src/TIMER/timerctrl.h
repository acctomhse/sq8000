#ifndef _timerctrl_h_included_
#define _timerctrl_h_included_

#ifdef __cplusplus
extern "C" {
#endif

int  timer0ExternalClockTest();
int  timer1ExternalClockTest();
int  timerExternalClockTest();
void timer0ISR(void* pparam);
void timer1ISR(void* pparam);
int  timerInterruptTest();

#ifdef __cplusplus
}
#endif

#endif // _timerctrl_h_included_

