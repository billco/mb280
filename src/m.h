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
////////

#define __m_H__


// gme280 data modubus map
#define NB_CONNECTION    5

// holding registers
#define HOUR	 0
#define MINIT	 1
#define SEC		 2
#define TEMP	 3
#define HUM		 4
#define PRES	 5
#define RTEMP	 6
#define RHUM	 7
#define RPRES	 8
#define MTEMP	 9
#define XTEMP	 10
#define ATEMP	 11
#define MHUM	 12
#define XHUM	 13
#define AHUM	 14
#define MPRES	 15
#define XPRES	 16
#define APRES	 17
#define LASTREG	 (APRES+1)

 // input registers 
 
//slave Id 0  regs
#define LOP_1	 0
#define ADR_1	 1
#define LOP_2	 2
#define ADR_2	 3
#define LOP_3	 4
#define ADR_3	 5
#define LOP_4	 6
#define ADR_4	 7
#define SLOG	 8
#define SPARE_2	 9
#define SPARE_3	 10
#define SPARE_4	 11
#define SPARE_5  12
#define SPARE_6  13
#define SPARE_7  14
#define SPARE_8	 15
#define SPARE_9	 16
#define SPARE_10 17
#define LASTREG0	 (SPARE_10+1)

//slave Id 1 - 4 input regs
#define SRESET	 0
#define STEMP	 1
#define SHUM	 2
#define SPRES	 3
#define SPAREC	 4
#define LASTS  (SPAREC+1)

#define OFFSET		6

#define SLAVEID 	(0+OFFSET) 	// slaveId offset into query 
#define COMMAND 	(1+OFFSET) 	// command offset into query 
#define REGADR		(3+OFFSET) 	// command offset into query 
#define REGVAL		(5+OFFSET) 	// command offset into query 

#define FUNC6	6	//  modbus funtion write input register

#define MAXLP   5

/* SRESET bits
*	0 0x1 -  temp
* 	1 0x2 - hum
* 	2 0x4 - pres
*   3 0x10 - log file  
* 
* 
*/
typedef struct {
	int16_t data[LASTREG];		// modbus holding registers
	int16_t sCal[LASTS];		// modbus input registers
	//char tag[20];		// string name
	int loop;		// i2c bus 0
	int adr ;		// bme280 address (0x76 or 0x77) | 0x80 = enabled
} bme280_t;

#ifndef MAIN__
#define ext extern
#else
#define ext 
#endif


ext   bme280_t b[MAXLP];
ext  modbus_mapping_t *mb_mapping;

ext char host[];

ext int  port ;
ext int  debug ;
ext int  slaveId;
ext time_t tt;
ext struct tm tm;
ext int loog;

ext void archist( void );


void retsetAll( void ) ;
void  init280(  void );
int read280( int adr,float *temp, float *hum, float *pres);
void WriteStat( void );
void ReadStat( void );
void WriteDefualts( void );
void readDefualts( void );
void writeLog( void );
int IsOnScan( int ix );

#endif
