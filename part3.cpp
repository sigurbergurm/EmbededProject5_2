/* Simple send and receive C example for communicating with the
*  Arduino echo program using /dev/ttyAMA0. Written by Derek Molloy
*  (www.derekmolloy.ie) for the book Exploring Raspberry Pi. */

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdint.h>
#include <stdlib.h>
#include<termios.h>
#include<string.h>
#define MSG_LEN 2

#include <inttypes.h>

int main(int argc, char *argv[]){
   int file, count;
   uint8_t msg[MSG_LEN];

   if(argc!=2){
       printf("Invalid number of arguments, exiting!\n");
       return -2;
   }

   if ((file = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY))<0){
      perror("UART: Failed to open the file.\n");
      return -1;
   }

   struct termios options;

   tcgetattr(file, &options);

   //cfmakeraw(&options);

   options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL;
   options.c_lflag &= ~ (ECHO | ICANON);
   tcflush(file, TCIFLUSH);
   tcsetattr(file, TCSANOW, &options);

   // send the string plus the null character
   msg[0] = 0;
   msg[1] = atoi(argv[1]);
   printf("%" PRIu8 "\n", msg[1]);
   if ((count = write(file, msg, MSG_LEN-1))<0){
      perror("Failed to write to the output\n");
      return -1;
   }

   usleep(100000);

   unsigned char receive[100];

   if ((count = read(file, (void*)receive, 100))<0){
      perror("Failed to read from the input\n");
      return -1;
   }

   if (count==0) printf("There was no data available to read!\n");
   else {
      receive[count]=0;  //There is no null character sent by the Arduino
      printf("The following was read in [%d]: %s",count,receive);
   }

   close(file);
   return 0;
}