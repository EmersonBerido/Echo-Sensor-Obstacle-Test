#include "line_sensor.h"
#include <pigpio.h>
#include <pthread.h>
#include <stdio.h>

volatile int centerLineValue = LINE_NOT_DETECTED;
volatile int leftLineValue = LINE_NOT_DETECTED;
volatile int rightLineValue = LINE_NOT_DETECTED;
pthread_t lineSensorThreads [3];

int centerGpio = CENTER_LINE_GPIO;
int leftGpio = LEFT_LINE_GPIO;
int rightGpio = RIGHT_LINE_GPIO;

int StartLineSensorReading();
void* ReadLineData(void* arg);
int GPIOReadCheck(int GPIO, int delay);
int CleanupLineSensor();

/**
 * Initializes line sensor GPIO pins
 * Returns 0 if success, -1 if failed
 */
int InitializeLineSensor() {
  if (gpioSetMode(CENTER_LINE_GPIO, PI_INPUT) != 0) return -1;
  if (gpioSetMode(LEFT_LINE_GPIO, PI_INPUT) != 0) return -1;
  if (gpioSetMode(RIGHT_LINE_GPIO, PI_INPUT) != 0) return -1;

  if (gpioSetPullUpDown(CENTER_LINE_GPIO, PI_PUD_OFF) != 0) return -1;
  if (gpioSetPullUpDown(LEFT_LINE_GPIO, PI_PUD_OFF) != 0) return -1;
  if (gpioSetPullUpDown(RIGHT_LINE_GPIO, PI_PUD_OFF) != 0) return -1;

  return 0;
}

/**
 * Starts line sensor reading threads
 * Returns 0 if success, -1 if failed
 */
int StartLineSensorReading() {
  if (pthread_create(&lineSensorThreads[0], NULL, ReadLineData, &centerGpio) != 0) {
    printf("Failed to start center line sensor thread\n");
    return -1;
  }
  if (pthread_create(&lineSensorThreads[1], NULL, ReadLineData, &leftGpio) != 0) {
    printf("Failed to start left line sensor thread\n");
    return -1;
  }
  if (pthread_create(&lineSensorThreads[2], NULL, ReadLineData, &rightGpio) != 0) {
    printf("Failed to start right line sensor thread\n");
    return -1;
  }

  return 0;
}

/**
 * Thread Function
 * Reads specified GPIO pin for line sensor data
 * Updates global line sensor values
 */

void* ReadLineData(void* arg) {
  int GPIO = *((int*) arg);
  while (1) {
    pthread_testcancel(); // check for thread cancel
    int value = GPIOReadCheck(GPIO, 300);
    if (value == -1) continue;

    if (GPIO == CENTER_LINE_GPIO) {
      centerLineValue = value;
    } else if (GPIO == LEFT_LINE_GPIO) {
      leftLineValue = value;
    } else if (GPIO == RIGHT_LINE_GPIO) {
      rightLineValue = value;
    }
  }

  return NULL;
}

/*
  Helper function
  Reads GPIO pin value, checks if consistent for 3 reads with delay in between
  Returns -1 if not consistent, returns value if consistent
*/
int GPIOReadCheck(int GPIO, int delay) { // 

  int initialValue = gpioRead(GPIO); //1

  // checks if consistent
  for (int i = 0; i < 3; i++) {
    gpioDelay(delay); // delay for any change 300us

    if (gpioRead(GPIO) != initialValue) {
      return -1;
    }
  } 

  return initialValue;
}

/*
  Cancels line sensor threads
  Returns 0 if success, -1 if failed
*/
int CleanupLineSensor() {
  for (int i = 0; i < 3; i++) {
     if (pthread_cancel(lineSensorThreads[i]) != 0) {
      printf("Failed to cancel line sensor thread %d\n", i);
      return -1;
    }
    if (pthread_join(lineSensorThreads[i], NULL) != 0) {
      printf("Failed to join line sensor thread %d\n", i);
      return -1;
    }
   }
  return 0;
}