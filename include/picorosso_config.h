#ifndef __PICOROSSO_CONFIG_H
#define __PICOROSSO_CONFIG_H

/*
 Size of the pre-allocated buffer for serialization
*/
//#if !defined(PUBLISHER_BUF_SIZE)
//#define PUBLISHER_BUF_SIZE 1024
//#endif

/*
Use sync_time node to get synchronize
*/
#if !defined(USE_SYNC_TIME)
#define USE_SYNC_TIME true
#endif


#endif // __PICOROSSO_CONFIG_H
