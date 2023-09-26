#ifndef TIMER_H
#define TIMER_H

#define TIMER_PERIOD 1000

// Executes ISR every 1ms until it returns FINISHED
void start_timer(void (*isr)());

#endif
