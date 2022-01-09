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

//slave Id 0 input regs
#define LOP_1	 0
#define ADR_1	 1
#define LOP_2	 2
#define ADR_2	 3
#define LOP_3	 4
#define ADR_3	 5
#define LOP_4	 6
#define ADR_4	 7
#define SPARE_1	 8
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

 // input registers 
#define SRESET	 0
#define STEMP	 1
#define SHUM	 2
#define SPRES	 3
#define SLOG	 4
#define LASTS  (SLOG+1)

#define OFFSET		6

#define SLAVEID 	(0+OFFSET) 	// slaveId offset into query 
#define COMMAND 	(1+OFFSET) 	// command offset into query 
#define REGADR		(3+OFFSET) 	// command offset into query 
#define REGVAL		(5+OFFSET) 	// command offset into query 

#define FUNC6	6	//  modbus funtion write input register

#define MAXLP   5



#endif
