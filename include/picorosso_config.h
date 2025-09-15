#ifndef __PICOROSSO_CONFIG_H
#define __PICOROSSO_CONFIG_H

/*
 Serial console used for debug output. If commented, no output is prdouced.
 To send to USB serial, set DEBUG_CONSOLE to Serial and PINs to -1.
*/

#define DEBUG_CONSOLE Serial
#define DEBUG_CONSOLE_PIN_RX -1
#define DEBUG_CONSOLE_PIN_TX -1
#define DEBUG_CONSOLE_BAUD 115200

/*
#define DEBUG_CONSOLE Serial1
#define DEBUG_CONSOLE_PIN_RX 9
#define DEBUG_CONSOLE_PIN_TX 10
#define DEBUG_CONSOLE_BAUD 115200
*/

/*
 Size of the pre-allocated buffer for serialization
*/
#define PUBLISHER_BUF_SIZE 1024

/*
Use sync_time node to get synchronize
*/
#define USE_SYNC_TIME true


#endif // __PICOROSSO_CONFIG_H
