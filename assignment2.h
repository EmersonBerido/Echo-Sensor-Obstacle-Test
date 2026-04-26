/***********************************************************************
 * Class:: CSC-615-01 Spring 2026 
 * Name::  Emerson Berido 
 * Student ID:: 923160976 
 * Github-Name:: EmersonBerido 
 * Project:: Assignment 2 - Tapeless Ruler
 * File:: assignment2.c
 * 
 * Description:: This program uses a sensor to measure distance and uses
 *  the time it takes to calculate the distance. This file will contain the
 *  definitions of the constants used in the program: GPIO pin, timeout,
 *  speed of sound, and levels for the GPIO pins.
 *  
 **********************************************************************/

#ifndef ASSIGNMENT2_H
#define ASSIGNMENT2_H

// GPIO pin levels
#define HIGH_LEVEL 1
#define LOW_LEVEL 0

enum Direction {FORWARD, LEFT, RIGHT};

// GPIO pin numbers on Raspberry Pi
#define FORWARD_ECHO_GPIO 6
#define FORWARD_TRIG_GPIO 5
#define LEFT_ECHO_GPIO 4
#define LEFT_TRIG_GPIO 14
#define RIGHT_ECHO_GPIO 15
#define RIGHT_TRIG_GPIO 23

typedef struct {
  int TRIGGER;
  int ECHO;
  enum Direction direction;
} EchoData;

// Timeouts
#define HIGH_LEVEL_TIMEOUT 10
#define TIMEOUT 0.03

#define SPEED_OF_SOUND 340

#endif