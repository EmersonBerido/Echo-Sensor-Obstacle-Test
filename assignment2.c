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

// thread vars
volatile double forwardValue = 0;
volatile double leftValue = 0;
volatile double rightValue = 0;
pthread_t echoThread;

// Helper Functions
int WaitForEchoHigh(int ECHO);
double ReadDataInSeconds(int ECHO);
void UpdateEchoDate(int TRIGGER, int ECHO, enum Direction direction);
void ExitProgram(int signal);
void* ReadEchoData();

int main() {
  if (gpioInitialise() < 0) return -1;

  // crete thread
  if (pthread_create(&echoThread, NULL, ReadEchoData, NULL) != 0) 
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

        // printf("Timeout exceeded: can't reach echo HIGH, retrying...\n");
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

/*
  Thread function
  Continuously reads echo data for forward/left/right
  Small pauses so echoes dont clash
*/
void* ReadEchoData() {
  // read forward, then left, then right
  // Set GPIO pin Input/Output; change this to pthread exit
  if (gpioSetMode(FORWARD_TRIG_GPIO, PI_OUTPUT) != 0) pthread_exit(NULL);
  if (gpioSetMode(FORWARD_ECHO_GPIO, PI_INPUT) != 0) pthread_exit(NULL);
  if (gpioSetMode(LEFT_TRIG_GPIO, PI_OUTPUT) != 0) pthread_exit(NULL);
  if (gpioSetMode(LEFT_ECHO_GPIO, PI_INPUT) != 0) pthread_exit(NULL);
  if (gpioSetMode(RIGHT_TRIG_GPIO, PI_OUTPUT) != 0) pthread_exit(NULL);
  if (gpioSetMode(RIGHT_ECHO_GPIO, PI_INPUT) != 0) pthread_exit(NULL);
  
  while (1) {
    UpdateEchoDate(FORWARD_TRIG_GPIO, FORWARD_ECHO_GPIO, FORWARD);
    gpioDelay(ECHO_TIMEOUT);
    UpdateEchoDate(LEFT_TRIG_GPIO, LEFT_ECHO_GPIO, LEFT);
    gpioDelay(ECHO_TIMEOUT);
    UpdateEchoDate(RIGHT_TRIG_GPIO, RIGHT_ECHO_GPIO, RIGHT);
    gpioDelay(ECHO_TIMEOUT);
  }

  return NULL;
}

/*
  Helper function for ReadEchoData
  Updates global echo sensor values
*/
void UpdateEchoDate(int TRIGGER, int ECHO, enum Direction direction) {

  
  // Trigger sensor to start measuring
  gpioWrite(TRIGGER, HIGH_LEVEL); 
  gpioDelay(HIGH_LEVEL_TIMEOUT);    
  gpioWrite(TRIGGER, LOW_LEVEL); 

  if (WaitForEchoHigh(ECHO) != 0) return;
  
  /* Module starts transmitting ultrasonic by this point */

  // Get time from echo pin 
  double readTime = ReadDataInSeconds(ECHO);
  if (readTime < 0) return; // bad read data, next loop

  double distance = (SPEED_OF_SOUND * readTime) / 2;
  double distanceCentimeters = distance * 100;

  if (direction == FORWARD) forwardValue = distanceCentimeters;
  else if (direction == LEFT) leftValue = distanceCentimeters;
  else if (direction == RIGHT) rightValue = distanceCentimeters;

  // printf("Distance is %.2f cm\n", distanceCentimeters);
  
}

void ExitProgram(int signal){
  printf("Exiting program...\n");
  pthread_cancel(echoThread);
  gpioTerminate();
  exit(0);
}

