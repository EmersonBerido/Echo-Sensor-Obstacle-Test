/***********************************************************************
 * Class:: CSC-615-01 Spring 2026 
 * Name::  Emerson Berido 
 * Student ID:: 923160976 
 * Github-Name:: EmersonBerido 
 * Project:: Assignment 2 - Tapeless Ruler
 * File:: assignment2.c
 * 
 * Description:: This program uses a sensor to measure distance and uses
 *  the time it takes to calculate the distance. It will measure 
 *  continuously and contain small timeouts to ensure proper data 
 *  reading. Distance will be in centimeters.
 *  
 **********************************************************************/

#include <pigpio.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include "assignment2.h"

volatile double forwardValue = 0;
volatile double leftValue = 0;
volatile double rightValue = 0;
pthread_t threads[3];
EchoData* pthreadArgs[3];

// Helper Functions
int WaitForEchoHigh(int ECHO);
double ReadDataInSeconds(int ECHO);
void *ReadData(void* args);
EchoData* createReadDataArgs(int TRIGGER, int ECHO, Direction direction);
void freeReadDataArgs();
void ExitProgram(int signal);

int main() {
  if (gpioInitialise() < 0) return -1;

  
  EchoData* forwardData = createReadDataArgs(FORWARD_TRIG_GPIO, FORWARD_ECHO_GPIO, FORWARD);
  EchoData* leftData = createReadDataArgs(LEFT_TRIG_GPIO, LEFT_ECHO_GPIO, LEFT);
  EchoData* rightData = createReadDataArgs(RIGHT_TRIG_GPIO, RIGHT_ECHO_GPIO, RIGHT);
  pthreadArgs[0] = forwardData;
  pthreadArgs[1] = leftData;
  pthreadArgs[2] = rightData;

  
  // crete threads
  if (pthread_create(&threads[0], NULL, ReadData, (void*)forwardData) != 0) 
    return -1;
  if (pthread_create(&threads[1], NULL, ReadData, (void*)leftData) != 0) 
    return -1;
  if (pthread_create(&threads[2], NULL, ReadData, (void*)rightData) != 0) 
    return -1;

  signal(SIGINT, ExitProgram);
  while(1) {
    printf("Forward: %.2f cm, Left: %.2f cm, Right: %.2f cm\n", forwardValue, leftValue, rightValue);
    gpioDelay(100000); // 100 ms
  }

  
  ExitProgram(0);
  
  return 0;
}

// Helper Functions

/*
  Wait for echo pin to be HIGH
  Return -1 if failed, 0 if success
*/
int WaitForEchoHigh(int ECHO) {
  double startTime = time_time();

  while (gpioRead(ECHO) == LOW_LEVEL){

      // echo pin didn't reach HIGH_LEVEL, error
      if (time_time() - startTime > TIMEOUT){

        printf("Timeout exceeded: can't reach echo HIGH, retrying...\n");
        return -1;
      }
    }
  
  return 0;
  
}

/*
  Returns time (seconds) for echo pin to go HIGH -> LOW
  Returns -1 if failed, time in seconds if success
*/
double ReadDataInSeconds(int ECHO) {

  double startTime = time_time();
  while (gpioRead(ECHO) == HIGH_LEVEL) {
    
    // echo pin HIGH too long, error
    if (time_time() - startTime > TIMEOUT){

      printf("Timeout exceeded: Echo on HIGH too long, retrying...\n");
      return -1;
    }
  }

  double totalTime = time_time() - startTime;
  return totalTime;
}

EchoData* createReadDataArgs(int TRIGGER, int ECHO, Direction direction) {
  EchoData* data = malloc(sizeof(EchoData));
  data->TRIGGER = TRIGGER;
  data->ECHO = ECHO;
  data->direction = direction;
  return data;
}

void freeReadDataArgs() {
  for (int i = 0; i < 3; i++) {
    free(pthreadArgs[i]);
    pthreadArgs[i] = NULL;
  }
}

void *ReadData(void* args) {
  EchoData* data = (EchoData*) args;

  // Set GPIO pin Input/Output; change this to pthread exit
  if (gpioSetMode(data->TRIGGER, PI_OUTPUT) != 0) pthread_exit(NULL);
  if (gpioSetMode(data->ECHO, PI_INPUT) != 0) pthread_exit(NULL);
  
  while (1) {
    // Trigger sensor to start measuring
    gpioWrite(data->TRIGGER, HIGH_LEVEL); 
    gpioDelay(HIGH_LEVEL_TIMEOUT);    
    gpioWrite(data->TRIGGER, LOW_LEVEL); 

    if (WaitForEchoHigh(data->ECHO) != 0) continue;
    
    /* Module starts transmitting ultrasonic by this point */

    // Get time from echo pin 
    double readTime = ReadDataInSeconds(data->ECHO);
    if (readTime < 0) continue; // bad read data, next loop

    double distance = (SPEED_OF_SOUND * readTime) / 2;
    double distanceCentimeters = distance * 100;

    if (data->direction == FORWARD) forwardValue = distanceCentimeters;
    else if (data->direction == LEFT) leftValue = distanceCentimeters;
    else if (data->direction == RIGHT) rightValue = distanceCentimeters;

    // printf("Distance is %.2f cm\n", distanceCentimeters);
  }
}

void ExitProgram(int signal){
  printf("Exiting program...\n");
  for (int i = 0; i < 3; i++) {
    pthread_cancel(threads[i]);
  }
  freeReadDataArgs();
  gpioTerminate();
  exit(0);
}

