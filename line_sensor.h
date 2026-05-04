
#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H

extern volatile int centerLineValue;
extern volatile int leftLineValue;
extern volatile int rightLineValue;

#define CENTER_LINE_GPIO 19
#define LEFT_LINE_GPIO 13
#define RIGHT_LINE_GPIO 26

#define LINE_DETECTED 1
#define LINE_NOT_DETECTED 0

int InitializeLineSensor();
int StartLineSensorReading();
int CleanupLineSensor();

#endif