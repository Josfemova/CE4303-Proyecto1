#include "timer.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct timespec pause_time = {.tv_sec = 0, .tv_nsec = 1000*TIMER_PERIOD};

void *timer_thread(void *isr) {
  while (1) {
		nanosleep(&pause_time, NULL);
    ((void (*)())isr)();
  }

  return 0;
}

void start_timer(void (*isr)()) {
  pthread_t thread;

  // Create the thread
  if (pthread_create(&thread, NULL, timer_thread, isr) != 0) {
    fprintf(stderr, "Error creando timer\n");
    exit(1);
  }
}
