#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#define SDL_main main

DWORD writeToSerialPort(HANDLE hSerial, uint8_t * data, int length)
{

  DWORD dwBytesRead = 0;
  if(!WriteFile(hSerial, data, length, &dwBytesRead, NULL)){
  }
  return dwBytesRead;

}

void closeSerialPort(HANDLE hSerial)
{
  CloseHandle(hSerial);
}


int main(int argc, char *argv[]) {
  	unsigned int i;
  	int buttonchosen = -1;
  	char magnet_on = 0;

    //Choix du port COM à utiliser
    char COMADDR[255];
    if (argc > 1) {
      snprintf(COMADDR, 255, "\\\\.\\%s", argv[1]);
      printf(COMADDR);
    }
    else {
      snprintf(COMADDR, 255, "\\\\.\\COM20");
    }

    //
    // Ouverture et paramètrage du port COM
    //
    DWORD  accessdirection = GENERIC_READ | GENERIC_WRITE;
    HANDLE hSerial = CreateFile(COMADDR,
        accessdirection,
        0,
        0,
        OPEN_EXISTING,
        0,
        0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "CreateFile failed %lu\n", GetLastError());
        return 1;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
         fprintf(stderr, "GetCommState failed %lu\n", GetLastError());
    }
    dcbSerialParams.BaudRate=38400;
    dcbSerialParams.ByteSize=8;
    dcbSerialParams.StopBits=ONESTOPBIT;
    dcbSerialParams.Parity=NOPARITY;
    if(!SetCommState(hSerial, &dcbSerialParams)){
        fprintf(stderr, "SetCommState failed %lu\n", GetLastError());
        return 1;
    }

    COMMTIMEOUTS timeouts={0};
    timeouts.ReadIntervalTimeout=50;
    timeouts.ReadTotalTimeoutConstant=50;
    timeouts.ReadTotalTimeoutMultiplier=10;
    timeouts.WriteTotalTimeoutConstant=50;
    timeouts.WriteTotalTimeoutMultiplier=10;
    if(!SetCommTimeouts(hSerial, &timeouts)){
        fprintf(stderr, "SetCommTimeouts failed %lu\n", GetLastError());
        return 1;
    }

    //
    //Init du Joystick
    //

    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    printf("%i joysticks were found.\n\n", SDL_NumJoysticks() );
    printf("The names of the joysticks are:\n");
    for( i=0; i < SDL_NumJoysticks(); i++ ) 
    {
        printf("    %s\n", SDL_JoystickName(i));
    }

    SDL_Event event;
    SDL_Joystick *joystick;
    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(0);
    
    int joy_x=0, joy_y=0, joy_z=0;
    
    while (1) {
        while(SDL_PollEvent(&event)) {
          switch(event.type) {
            case SDL_JOYAXISMOTION:
                switch (event.jaxis.axis) {
                  case 0:
                    joy_x = event.jaxis.value;
                    break;
                  case 1:
                    joy_y = event.jaxis.value;
                    break;
                  case 2:
                    joy_z = event.jaxis.value;
                    break;
                }
                
                break;
            case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
                  if (buttonchosen == -1) {
                      buttonchosen = event.jbutton.button;
                      magnet_on = 1;          
                      printf("Feu à volonté!\n");
                      unsigned char buffer1[32];
                      buffer1[0] = buffer1[1] = buffer1[2] = 0xFF;
                      buffer1[3] = 0xFE;
                      writeToSerialPort(hSerial, buffer1, 4);
                  }
              break;
            case SDL_JOYBUTTONUP:  /* Handle Joystick Button Presses */
                  if (magnet_on && (buttonchosen == event.jbutton.button)) {
                      magnet_on = 0;
                      buttonchosen = -1;
                      printf("Maintenant, repos!\n");
                      unsigned char buffer2[32];
                      buffer2[0] = buffer2[2] = buffer2[3] = 0xFF;
                      buffer2[3] = 0xFD;
                      writeToSerialPort(hSerial, buffer2, 4);
                  }
              break;
            case SDL_QUIT:
                    SDL_JoystickClose(joystick);
                    return 0;
                    break;
            default:
              printf("I don't know what this event is!\n");
          }
        }

        double magnitude = sqrt(joy_y*joy_y + joy_x*joy_x);
        double angle = atan2(joy_y, joy_x) + M_PI;
        uint16_t angle_d = angle * 180 / M_PI;
        uint8_t should_move = magnitude >= 20000 ? 1 : 0;
        if (should_move)
          printf("%f, %f => %d\n", magnitude, angle, angle_d);
              
        unsigned char buffer[32];
        buffer[0] = buffer[1] = buffer[2] = buffer[3] = 0xFF;
        uint16_t *p = (uint16_t *)&(buffer[4]);
        *p = angle_d;
        buffer[6] = should_move;
        writeToSerialPort(hSerial, buffer, 7);

      Sleep(250);
    }

  //On ferme, ya plus rien à voir
  SDL_JoystickClose(joystick);
  closeSerialPort(hSerial);
  return 0;
}

