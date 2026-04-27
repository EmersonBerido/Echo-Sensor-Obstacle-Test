// #include <pigpio.h>

// /*
//   Reads GPIO pin twice for consisted read

//   Returns 0 - 1 on success
//   Returns -1 if failed
// */

// int main() {
//   if (gpioInitialise() < 0) return -1;

//   int result = GPIOReadCheck(4, 10000);
//   printf("GPIO Read Result: %d\n", result);

//   gpioTerminate();
//   return 0;
// }

// volatile int readValue = 0;

// int readCheck = GPIOReadCheck(12, 300);
// if (readCheck != -1)
//   readValue = readCheck;


// int GPIOReadCheck(int GPIO, int delay) { // 

//   int initialValue = gpioRead(GPIO); //1

//   // checks if consistent
//   for (int i = 0; i < 3; i++) {
//     gpioDelay(delay); // delay for any change 300us

//     if (gpioRead(GPIO) != initialValue) {
//       return -1;
//     }
//   } 

//   return initialValue;
// }