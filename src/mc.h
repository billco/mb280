// header for client

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

//#include <stdio.h>
//#include <string.h>
  
//#include <unistd.h>
//#include <modbus/modbus.h>
//#include <errno.h>

#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <time.h>


#define __m_H__


// gme280 data modubus map
#define NB_CONNECTION    5

#define BRIGHTWHITE "\e[1;37;40m"	// bold white black
#define DIMWHITE "\e[2;37;40m"
#define DEFCOLOR "\e[0;39;49m"
#define UNDERLINEON "\e[4m"
#define UNDERLINEOFF "\e[24m"
#define HOME  "\e[H"
#define ERASEDIS   "\e[0J"
#define SAVE_CUR	"\es"
#define RESTOR_CUR	"\eu"
#ifndef MAIN__
#define ext extern
#else
#define ext
#endif
typedef struct {
  modbus_t *ctx;
 // uint16_t  tab_reg[LASTREG];
 // uint16_t  input_reg[LASTREG0];
  int16_t   data[LASTREG];	// modbus holding registers
  int16_t   data0[LASTREG];	// slave control holding registers
  int16_t   sCal[LASTREG0];	// modbus input registers
  char      host[20];
  int       port;
  int       open;
  int       sensor;
  
} bme280_t;

 char      host[20] = "127.0.0.1";
 int       port = 1502;
 int       sensor;

// modbus_t *ctx;
 uint16_t  tab_reg[LASTREG];
 uint16_t  input_reg[LASTREG0];
 #define MAXLP   4
 
ext void RawDisPlay();
ext  void ssDisPlay();
ext  void asDisPlay( );
ext  void hcDisPlay( );
ext  void help(void);
ext  void userHelp(void);

 bme280_t    b[MAXLP];

 int       debug = 0;
 int       polRate = 500000;	// in uS ie 500ms
 int       opt;

 int       rc;
 int      Id, curId;
 int      reset = 0;
 int       hideOffScan = 1;		// hide sensors off scan
 void      disregs(int rc, int start, uint16_t * regs);
 	char      buf[256] = { 0 };
  int newCmd = 0;
 	int       intVal = 0;
 	int       reg = 0;
 	float     val = 0;
  
 int curPg = 1;

#endif
