// header for slave

#ifndef __m_H__


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include <pthread.h>
#include <syslog.h>

#include <modbus/modbus.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "mbcom.h"
// //////

#define __m_H__


// gme280 data modubus map
#define NB_CONNECTION    5

/*
 * SRESET bits * 0 0x1 - temp * 1 0x2 - hum * 2 0x4 - pres * 3 0x10 - log
 * file * * 
 */
typedef struct {
    int16_t         data[LASTREG];	// modbus holding registers
    int16_t         sCal[LASTREG0];	// modbus input registers
    // char tag[20]; // string name
    int             loop;	// i2c bus 0
    int             adr;	// bme280 address (0x76 or 0x77) | 0x80 =
    // enabled
} bme280_t;

#ifndef MAIN__
#define ext extern
#else
#define ext
#endif

#define MAXLP   5
ext bme280_t    b[MAXLP];
ext modbus_mapping_t *mb_mapping;

ext char        host[];

ext int         port;
ext int         debug;
ext int         slaveId;
ext time_t      tt;
ext struct tm   tm;
ext int         loog;

ext void        archist(void);
ext void        checkAlarms(int);
ext void ackAllArm( void );


void            retsetAll(void);
void            init280(void);
int             read280(int adr, float *temp, float *hum, float *pres);
void            WriteStat(void);
void            ReadStat(void);
void            WriteDefualts(void);
void            readDefualts(void);
void            writeLog(void);
int             IsOnScan(int ix);

#endif
