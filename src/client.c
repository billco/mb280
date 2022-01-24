/*
 * modbus tcp client for mb280 slave
 * shep rev 1.0.1 12.30.2021 11:41:00
 *   
 */


#include "mbcom.h"
#include "mc.h"
#include "alarms.h"
       
void alrmSum(void);
void  get_Regs();
int getUserIn(char*b);

void     hlep();
void     userHelp();
int initModBus( void );
int closeOpenMb( int ix );

char  wbuf[256] = {0};

int  doUser(void);
void sendIt(int ix, int id, int reg, int val);

int numRead = 0;
time_t      tt;
struct tm   tm;
//int ncmd=0;


void sendIt(int ix, int Id, int reg, int val)
{
  int id = 0;
  int curId = 0;
  
      printf("sendit> ix:%d Id:%d reg:%d val:%d\n", ix, Id, reg,  val );
    if( Id == 0 && ix == 0){
	id = 0;
    } else{
	id =  b[ix].sensor;
    }
    curId = modbus_get_slave(b[ix].ctx);
    modbus_set_slave(b[ix].ctx, id );
    rc = modbus_write_register(b[ix].ctx, reg, val);
    if (rc == -1) {
      fprintf(stderr, "%s\n", modbus_strerror(errno));
    }
	 //   b[ix].ctx = NULL;
    printf("sendit ix:%d id:%d val:%d\n", ix, id, val );

    modbus_set_slave(b[ix].ctx, curId);
    sleep(2);
}

int IsOnScan(int ix ) {
int  tmp = 0;
  switch (b[ix].sensor) {
    case 1:
      tmp = b[ix].data0[ADR_1];
      break;
    case 2:
      tmp = b[ix].data0[ADR_2];
      break;
    case 3:
      tmp = b[ix].data0[ADR_3];
      break;
    case 4:
      tmp = b[ix].data0[ADR_4];
      break;
  }
  return (tmp & 0x80);
}

int getLoop(int ix) {
int  tmp = 0;
  switch (b[ix].sensor) {
    case 1:
      tmp = b[ix].data0[LOP_1];
      break;
    case 2:
      tmp = b[ix].data0[LOP_2];
      break;
    case 3:
      tmp = b[ix].data0[LOP_3];
      break;
    case 4:
      tmp = b[ix].data0[LOP_4];
      break;
  }
  return (tmp);
}

int getAdr(int ix){
int  tmp = 0;
  switch (b[ix].sensor) {
    case 1:
      tmp = b[ix].data0[ADR_1];
      break;
    case 2:
      tmp = b[ix].data0[ADR_2];
      break;
    case 3:
      tmp = b[ix].data0[ADR_3];
      break;
    case 4:
      tmp = b[ix].data0[ADR_4];
      break;
  }
  return (tmp);
}

int   WriteDefualts() {

  FILE  *fd = fopen("client.cfg", "w");
   for (int ix = 0; ix < MAXLP; ++ix) {
    fprintf(fd, "%s %d %d\n", b[ix].host, b[ix].port, b[ix].sensor );
  }

  fclose(fd);
  return 0;
}

int   ReadDefualts() {

  FILE  *fd = fopen("client.cfg", "r");
   for (int ix = 0; ix < MAXLP; ++ix) {
    fscanf(fd, "%s %d %d\n", b[ix].host, &b[ix].port, &b[ix].sensor );
    b[ix].open = FALSE;
  }

  fclose(fd);
  return 0;
}
int closeOpenMb( int ix ){
      printf("ix:%d host={%s} port:%d sensor:%d \n", ix, b[ix].host, b[ix].port, b[ix].sensor);

  if( b[ix].open ) {
    printf("closing\n");
    modbus_close(b[ix].ctx);
    modbus_free(b[ix].ctx);
  }
  sleep(1);
  b[ix].ctx = modbus_new_tcp(b[ix].host, b[ix].port);

  if (modbus_connect(b[ix].ctx) == -1) {
    fprintf(stderr, "closeOpenMb# %s\n", modbus_strerror(errno));
  }
    // - modbus_set_response_timeout(ctx, 0, 0);
  modbus_set_debug(b[ix].ctx, debug);
  
  modbus_set_error_recovery(b[ix].ctx,
	  MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);
		
  b[ix].open = TRUE;
  return 1;
}

int initModBus( void ){
ReadDefualts();
  for (int ix = 0; ix < MAXLP; ++ix) {
    
    printf("ix:%d host={%s} port:%d sensor:%d \n", ix, b[ix].host, b[ix].port, b[ix].sensor);

    b[ix].ctx = modbus_new_tcp(b[ix].host, b[ix].port);
    if (modbus_connect(b[ix].ctx) == -1) {
      fprintf(stderr, "Connection failed: %s:%d %s\n", b[ix].host,b[ix].port,
      modbus_strerror(errno));
      modbus_free(b[ix].ctx);
      b[ix].open = FALSE;
      return 1;
    }
    rc = modbus_connect(b[ix].ctx);
    if (rc == -1) {
      fprintf(stderr, "%s\n", modbus_strerror(errno));
	 //   b[ix].ctx = NULL;
    }
    // - modbus_set_response_timeout(ctx, 0, 0);
    modbus_set_debug(b[ix].ctx, debug);
  
    modbus_set_error_recovery(b[ix].ctx,
	  MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);
		
    b[ix].open = TRUE;
  }
  return 0;
}
  
int main(int argc, char *argv[]) {

    if (argc < 1) {
	help();
	return 1;
    }
    while ((opt = getopt(argc, argv, "H:Phudps?:")) != -1) {
	switch (opt) {
	case 'd':
	    debug = atoi(optarg);
	    break;
	case 'H':
	    strncpy(host, optarg, 19);
	    break;
	case 'P':
	    port = atoi(optarg);
	    break;
	case 's':
	    hideOffScan = atoi(optarg);
	    break;
	case 'p':
	    polRate = atoi(optarg);
	    if (polRate < 1000)
		polRate = 1000000;
	    break;
	case 'u':
	    userHelp();
	    return 0;
	    break;
	case 'h':
	case '?':
	    help();
	    return 0;
	    break;
	} 
    }  
    
    initModBus();
    
      // int alarmStat = 0;
    memset(buf, 0, sizeof(buf));

    printf("Starting\n");
sleep( 3);
    // read sCal regs
    curPg = 1;
    while(1){
      tt = time(NULL);
      tm = *localtime(&tt);
      printf("%s%s", HOME, ERASEDIS);
      
      get_Regs();
	 
     
  
      // save cusor
      printf("%s", SAVE_CUR);
      
      // display
      switch (curPg) {
	case 0:
	  ssDisPlay();
	  break;
	case 1:
	  asDisPlay();
	  break;
   	case 2: 
	  hcDisPlay();
	  break;
   	case 3:
	  RawDisPlay();
	  break;
   	case 4:
	  sdDisPlay(&subPg);
	  break;
      }
    
      //disregs(rc, 0, (uint16_t *)b[1].data );
      
      alrmSum();
      // restore cussor
      //	printf("%s>%s%s", RESTOR_CUR, buf,"\e[6D");
      //	printf("%s >%s", RESTOR_CUR, buf);
      fflush(stdout);
      
      if( doUser() ){
	newCmd = numRead = 0;
	memset(buf, 0, sizeof(buf));
	continue;
      }else {
	 break;
      }
      usleep(polRate);
    }
  for (int ix = 0; ix < MAXLP; ++ix) {

    modbus_close(b[ix].ctx);
    modbus_free(b[ix].ctx);
  }
    printf(RESTOR_CUR);		// restore current cursor

    printf("\n");
}

void  get_Regs() {
  
  for (int ix = 0; ix < MAXLP; ++ix) {
    
      // read slave data0 regs
      modbus_set_slave(b[ix].ctx, 0);
      rc = modbus_read_registers(b[ix].ctx, 0,  LASTREG0, tab_reg);
      if (rc == -1) {
	fprintf(stderr, "get_Regs %s\n", modbus_strerror(errno));
	continue;
      }
      memcpy(b[ix].data0, tab_reg, LASTREG0 << 1);
      
        //printf("%d sensor: %d data0\n",ix, b[ix].sensor);
      //disregs(rc, 0, (uint16_t *)b[ix].data0 );
      
      // get slave control regs
      modbus_set_slave(b[ix].ctx, b[ix].sensor);
      rc = modbus_read_input_registers(b[ix].ctx, 0, LASTS, input_reg);
      if (rc == -1) {
	fprintf(stderr, "get_Regs - %s\n", modbus_strerror(errno));
	continue;
      }
      memcpy(b[ix].sCal, input_reg, LASTS << 1);
  
      // read slave data regs
      modbus_set_slave(b[ix].ctx, b[ix].sensor);
      rc = modbus_read_registers(b[ix].ctx, 0, LASTREG, tab_reg);
      if (rc == -1) {
	fprintf(stderr, "get_Regs * %s\n", modbus_strerror(errno));
	continue;
      }
      memcpy(b[ix].data, tab_reg, LASTREG << 1);
    

  }
}

void hcDisPlay() {
  printf("%s%02d:%02d:%02d\t\t     -Hosts Configuration-      HC - Page \n",
	 BRIGHTWHITE,  tm.tm_hour, tm.tm_min, tm.tm_sec);
  printf("\n\n\n\n\n   %s%sBme             IP       Port Sensor#  Loop   Adrs  Alarm-Abled   Log %s\n",
	      UNDERLINEON, BRIGHTWHITE, UNDERLINEOFF);

  for (int ix = 0; ix < MAXLP; ++ix) {
    printf("   %2d %20s %5d    %1d       %1d     %2x    %3s %3s %3s  %2d \n",
    ix+1, b[ix].host, b[ix].port, b[ix].sensor, getLoop(ix), getAdr(ix),
    (char*)(( b[ix].data[ALSTAT]&0x01000) ? "TE " : "TD "),
    (char*)(( b[ix].data[ALSTAT]&0x02000) ? "HE " : "HD "),
    (char*)(( b[ix].data[ALSTAT]&0x04000) ? "PE " : "PD "),
    b[ix].data0[8] 
    );
  }
  printf("\n\n\n\n\n\n");
  
}

void RawDisPlay() {
 
  printf("%s%02d:%02d:%02d\t\t     -Raw Data-                         RD - Page \n",
	 BRIGHTWHITE,  tm.tm_hour, tm.tm_min, tm.tm_sec);
  for( int ix = 0; ix < MAXLP; ++ix){
    printf("slave Regs control:%d regs\n", b[ix].sensor);
    disregs(SPARE_3, 0, (uint16_t *)b[ix].data0 );
    
    printf("slave Input Sensor:%d regs\n", b[ix].sensor);
    disregs(LASTS, 0, (uint16_t *)b[ix].sCal );
    
    printf("slave Regs Sensor:%d regs\n", b[ix].sensor);
    disregs(LASTREG, 0, (uint16_t *)b[ix].data );
  printf("--------------------------------------------------------------------\n");
  }
  
}


void asDisPlay() {
int alarmStat = 0;
  printf("%s%02d:%02d:%02d \t\t-Alarm Set Points-\t\t\t AS - Page \n",
	 BRIGHTWHITE,  tm.tm_hour, tm.tm_min, tm.tm_sec);
  
  for (int ix = 0; ix < MAXLP; ++ix) {
			
    printf("%s%s", BRIGHTWHITE, UNDERLINEON);

      printf
	("Bme:%d     Cur    SP-LOW      SP-HI    Alarm   Scal   Raw             %s%s\n",
	     ix+1, BRIGHTWHITE, UNDERLINEOFF);

      
	if (!IsOnScan(ix)) {
	    printf("%s", DIMWHITE);
	}
	
	alarmStat = b[ix].data[ALSTAT] & 0x0000FFFF;

	printf
	    ("Temp : %6.1f    %6.1f %3s %6.1f %3s %04X %6.1f %6.1f   lop:%d\n",
	     ((float) b[ix].data[TEMP]) * 0.1,
	     ((float) b[ix].sCal[ASP_TLO]) * 0.1, strIsLoTempAct(alarmStat),
	     ((float) b[ix].sCal[ASP_THI]) * 0.1, strIsHiTempAct(alarmStat),
	     alarmStat, // & ATBITS,
	     ((float) b[ix].sCal[STEMP]) * 0.1,
	     ((float) b[ix].data[RTEMP]) * 0.1,
	     getLoop(ix));

	printf
	    ("Hum  : %6.1f    %6.1f %3s %6.1f %3s %04X %6.1f %6.1f   adr:0x%2x\n",
	     ((float) b[ix].data[HUM]) * 0.1,
	     ((float) b[ix].sCal[ASP_HLO]) * 0.1, strIsLoHumAct(alarmStat),
	     ((float) b[ix].sCal[ASP_HHI]) * 0.1, strIsHiHumAct(alarmStat),
	     alarmStat, // & AHBITS,
	     ((float) b[ix].sCal[SHUM]) * 0.1, ((float) b[ix].data[RHUM]) * 0.1,
	     getAdr(ix));
	printf
	    ("Pres : %6.1f    %6.1f %3s %6.1f %3s %04X %6.1f %6.1f   log:%2d\n",
	      ((float) b[ix].data[PRES]) * 0.1,
	      ((float) b[ix].sCal[ASP_PLO]) * 0.1, strIsLoPresAct(alarmStat),
	      ((float) b[ix].sCal[ASP_PHI]) * 0.1, strIsHiPresAct(alarmStat),
	       alarmStat, // & APBITS,
	     ((float) b[ix].sCal[SPRES]) * 0.1, ((float) b[ix].data[RPRES]) * 0.1,
	     (IsOnScan(ix) ? b[ix].data0[8] : 0));
    }
}

void ssDisPlay() {
  int	alarmStat = 0;

  printf("%s%02d:%02d:%02d \t\t-Sensor Summary-\t\t\t SS - Page \n",
	  BRIGHTWHITE, tm.tm_hour, tm.tm_min, tm.tm_sec);
 
  for (int ix = 0; ix < MAXLP; ++ix) {

    printf("%s%s", BRIGHTWHITE, UNDERLINEON);
    printf ("Bme:%d     Cur       LOW         HI         AV     Scal        Raw         %s\n",
       ix+1, UNDERLINEOFF );
      // ix, BRIGHTWHITE , UNDERLINEOFF);
    
    if (!IsOnScan(ix)) {
      printf("%s", DIMWHITE);
    }
    alarmStat = b[ix].data[ALSTAT] & 0x0000FFFF;
    
    printf  ("Temp : %6.1f    %6.1f %3s %6.1f %3s %6.1f   %6.1f     %6.1f  lop:%d\n",
       ((float) b[ix].data[TEMP]) * 0.1,
       ((float) b[ix].data[MTEMP]) * 0.1, strIsLoTempAct(alarmStat),
       ((float) b[ix].data[XTEMP]) * 0.1, strIsHiTempAct(alarmStat),
       ((float) b[ix].data[ATEMP]) * 0.1,
       ((float) b[ix].sCal[STEMP]) * 0.1, ((float) b[ix].data[RTEMP]) * 0.1,
       getLoop(ix));
    
    printf ("Hum  : %6.1f    %6.1f %3s %6.1f %3s %6.1f   %6.1f     %6.1f  adr:0x%2x\n",
       ((float) b[ix].data[HUM]) * 0.1,
       ((float) b[ix].data[MHUM]) * 0.1, strIsLoHumAct(alarmStat),
       ((float) b[ix].data[XHUM]) * 0.1, strIsHiHumAct(alarmStat),
       ((float) b[ix].data[AHUM]) * 0.1, 
       ((float) b[ix].sCal[SHUM]) * 0.1, ((float) b[ix].data[RHUM]) * 0.1,
       getAdr(ix));
    printf ("Pres : %6.1f    %6.1f %3s %6.1f %3s %6.1f   %6.1f     %6.1f  log:%2d\n",
       ((float) b[ix].data[PRES]) * 0.1,
       ((float) b[ix].data[MPRES]) * 0.1, strIsLoPresAct(alarmStat),
       ((float) b[ix].data[XPRES]) * 0.1, strIsHiPresAct(alarmStat),
       ((float) b[ix].data[APRES]) * 0.1, 
       ((float) b[ix].sCal[SPRES]) * 0.1, ((float) b[ix].data[RPRES]) * 0.1,
       (IsOnScan(ix) ? b[0].data0[8] : 0));
  }
}

void
 sdDisPlay( int* subPg ) {
  if( *subPg < 1 || *subPg > MAXLP) { *subPg = 0; }
  int  ix = *subPg - 1;
  int	alarmStat = b[ix].data[ALSTAT] & 0x0000FFFF;

  printf("%s%02d:%02d:%02d\t\t -----Sensor Detail----- \t\t SD - %d Page \n",
	 BRIGHTWHITE,  tm.tm_hour, tm.tm_min, tm.tm_sec, ix+1);
  printf("                             -----Temp-----\n");
  printf("                  Cur       Min     MAX     Ave     sCal     Raw     \n");
  printf("    Temp       %6.1f    %6.1f  %6.1f  %6.1f   %6.1f  %6.1f   \n",
       ((float) b[ix].data[TEMP]) * 0.1,
       ((float) b[ix].data[MTEMP]) * 0.1,
       ((float) b[ix].data[XTEMP]) * 0.1, 
       ((float) b[ix].data[ATEMP]) * 0.1,
       ((float) b[ix].sCal[STEMP]) * 0.1,
	   ((float) b[ix].data[RTEMP]) * 0.1
	   ); 
  printf("                  Low       High    |  Mask  | Auto-Ack | Enabled | \n"); 	   
  printf("    Alarm/Stat %6.1f %3s %6.1f %3s   %04X       %s       %s \n",
	     ((float) b[ix].sCal[ASP_TLO]) * 0.1, strIsLoTempAct(alarmStat),
	     ((float) b[ix].sCal[ASP_THI]) * 0.1, strIsHiTempAct(alarmStat),
	     alarmStat & ( ATBITS | AETEMBIT | AAUEBIT),
	     (alarmStat & ( AAUEBIT) ?  "Yes" : "No "), 
		(alarmStat & ( AETEMBIT) ?  "Yes" : "No ") );
  printf("                             -----Hum-----\n");
  printf("                  Cur       Min     MAX     Ave     sCal     Raw  \n");
  printf("    Hum        %6.1f    %6.1f  %6.1f  %6.1f   %6.1f  %6.1f\n",
        ((float) b[ix].data[HUM]) * 0.1,
       ((float) b[ix].data[MHUM]) * 0.1,
       ((float) b[ix].data[XHUM]) * 0.1, 
       ((float) b[ix].data[AHUM]) * 0.1,
       ((float) b[ix].sCal[SHUM]) * 0.1,
	   ((float) b[ix].data[RHUM]) * 0.1
	   ); 
  printf("                 Low       High       Mask  | Auto-Ack | Enabled | \n"); 	   
  printf("    Alarm/Stat %6.1f %3s %6.1f %3s   %04X       %s       %s \n",
	     ((float) b[ix].sCal[ASP_HLO]) * 0.1, strIsLoHumAct(alarmStat),
	     ((float) b[ix].sCal[ASP_HHI]) * 0.1, strIsHiHumAct(alarmStat),
	     alarmStat & ( AHBITS | AEHUMBIT | AAUEBIT),
	     (alarmStat & ( AAUEBIT) ?  "Yes" : "No "), 
		(alarmStat & ( AEHUMBIT) ?  "Yes" : "No ") );

  printf("                             -----Pres-----\n");
  printf("                  Cur       Min     MAX     Ave     sCal     Raw\n");
  printf("    Pres       %6.1f    %6.1f  %6.1f  %6.1f   %6.1f  %6.1f   \n",
         ((float) b[ix].data[PRES]) * 0.1,
       ((float) b[ix].data[MPRES])* 0.1,
       ((float) b[ix].data[XPRES])* 0.1, 
       ((float) b[ix].data[APRES])* 0.1,
       ((float) b[ix].sCal[SPRES])* 0.1,
	   ((float) b[ix].data[RPRES])* 0.1
	   ); 
  printf("                  Low       High    |  Mask  | Auto-Ack | Enabled | \n"); 	   
  printf("    Alarm/Stat %6.1f %3s %6.1f %3s   %04X       %s       %s \n",
	     ((float) b[ix].sCal[ASP_PLO]) * 0.1, strIsLoPresAct(alarmStat),
	     ((float) b[ix].sCal[ASP_PHI]) * 0.1, strIsHiPresAct(alarmStat),
	     alarmStat & ( APBITS | AEPRSBIT | AAUEBIT),
	     (alarmStat & ( AAUEBIT) ?  "Yes" : "No "), 
		(alarmStat & ( AEPRSBIT) ?  "Yes" : "No ") );
  printf( "                      -----Host Configuration-----\n");
  printf("  %s%sThis#         IP         Port Sensor#  Loop  Adrs Log Alarm Sam-Time%s\n",
	      UNDERLINEON, BRIGHTWHITE, UNDERLINEOFF); 
    printf("   %2d     %15s %5d    %1d       %1d    %2x   %2d  %4x %02d:%02d:%02d\n",
    ix+1, b[ix].host, b[ix].port, b[ix].sensor, getLoop(ix), getAdr(ix),
     b[ix].data0[8],
	alarmStat,
	b[ix].data[HOUR], b[ix].data[MINIT], b[ix].data[SEC]);
  return ;

  printf( "                             ----Host Data-----\n");
  printf("slave Regs control:%d regs\n", b[ix].sensor);
    disregs(SPARE_3, 0, (uint16_t *)b[ix].data0 );
    
    printf("slave Input Sensor:%d regs\n", b[ix].sensor);
    disregs(LASTS, 0, (uint16_t *)b[ix].sCal );
    
    printf("slave Regs Sensor:%d regs\n", b[ix].sensor);
    disregs(LASTREG, 0, (uint16_t *)b[ix].data );
}

void alrmSum( ){
int aCnt = 0;
int alarmStat = 0;
printf("%s%s                                                                           \n%s",
  BRIGHTWHITE,UNDERLINEON,UNDERLINEOFF);
  for( int ix = 0; ix < MAXLP; ++ix){
    if( ! IsOnScan(ix) ) 
	    continue;
    alarmStat = b[ix].data[ALSTAT] & 0x0000FFFF;
    if( IsLoTempAct(alarmStat)) printf( "[%d-Tlo] ",ix+1);
    if( IsHiTempAct(alarmStat)) printf( "[%d-Thi] ",ix+1);
    if( IsLoHumAct(alarmStat) ) printf( "[%d-Hlo] ",ix+1);
    if( IsHiHumAct(alarmStat) ) printf( "[%d-Hhi] ",ix+1);
    if( IsLoPresAct(alarmStat)) printf( "[%d-Plo] ",ix+1);
    if( IsHiPresAct(alarmStat)) printf( "[%d-Phi] ",ix+1);
    if(++aCnt > 10 ) { aCnt = 0; puts("\n");}
  }
  printf("\n");
}

void disregs(int rc, int start, uint16_t * regs) {
    // display as each reg as %4X 10 colum aray
    int             col = 0;

    printf("      ");

    for (int i = 0; i < 10; ++i) {
	printf("| %5d", i);
    }

    int             ii = 0;
    col = 0;
    printf("|\n| %4d", ((ii + start) / 10) * 10);
    for (int i = 0; i < start % 10; ++i) {
	printf("|  ... ");
	++col;
    }
    // float           f = 0;
    while (1) {

	if (col >= 10) {
	    col = 0;
	    printf("|\n| %4d", ((ii + start) / 10) * 10);
	}
	//if (ii > 2) {
	 //   f = ((float) (int16_t) regs[ii++]) * 0.1;
	 //   printf("|%6.1f", f);
	//} else {
	    printf("| %5d", regs[ii++]);
	//}

	if (ii >= rc)
	    break;
	++col;
    }
    printf("|\n");
}


void help(void){
  printf("modbus tcp client for mb280 slave\n");
  printf("\t-d [modbus debug] 0|1 (%d) \n", debug);
  printf("\t-H [Slave IP] set Host/Slave ipv4 (%s)\n", host);
  printf("\t-P [Slave lisen] ipv4 Port (%d)\n", port);
  printf("\t-p [pooling rate] (%d)\n", polRate);
  printf("\t-s [0|1] hide sensors off scan (%d)\n", hideOffScan);
  printf("\t-u print user help\n");
  printf("\t-h print cli help\n");
}

void userHelp(void) {
 
FILE  *fd = fopen("../doc/ClientCommands.txt", "r");
  if (fd == NULL){
	printf(" User help ../doc/ClientCommands.txt ");
	return;
  }
  while( fgets(buf, 254, fd) != NULL) {
	printf( "%s", buf);
  }

  fclose(fd);
}

int  parUsrFloat(char* ipStr, int* id, float* Val ){
int c = 0;
  if (curPg == SDPAGE){ 
	c = sscanf( ipStr, " %f", Val);
	*id = subPg;
	return ++c;
  }
  c = sscanf( ipStr, " %d %f", id, Val);
  return c;

} 
int  parUsrInt(char* ipStr, int* id, int* Val ){
int c = 0;
  if (curPg == SDPAGE){ 
	c = sscanf( ipStr, " %d", Val);
	*id = subPg;
	return ++c;
  }
  c = sscanf( ipStr, " %d %d", id, Val);
  return c;

}
int  parUsrHex(char* ipStr, int* id, int* Val ){
int c = 0;
  if (curPg == SDPAGE){ 
	c = sscanf( ipStr, " %x", Val);
	*id = subPg;
	return ++c;
  }
  c = sscanf( ipStr, " %d %x", id, Val);
  return c;

}
int doUser(){
  int c = 0;
      newCmd = getUserIn(buf);
		//printf("=[%s] %d 0:%c 1:%c 2:%c 3:%c 4:%c \n",  buf,newCmd,
		//buf[0], buf[1], buf[2], buf[3], buf[4]
		//);
		
  if ( newCmd && buf[0] == 0 && buf[0] != 0) return 1;
  
  if ( buf[0] == 'U' && buf[1] == '\n') { // UP arrow 
	  subPg = (--subPg < 1 ) ? 1 : subPg; return 1;}
  if ( buf[0] == 'D' && buf[1] == '\n' ) { // DOWN arrow 
	  subPg = (++subPg >= MAXLP ) ? MAXLP : subPg; return 1;}
  // check for Page s changes
  if (buf[0] == 'H' && buf[1] == 'C'){
	  curPg = HCPAGE; return 1;
  } else if (buf[0] == 'S' && buf[1] == 'S'){ 
	  curPg = SSPAGE; return 1;
  } else if (buf[0] == 'A' && buf[1] == 'S'){
	  curPg = ASPAGE; return 1;
  } else if (buf[0] == 'R' && buf[1] == 'D'){
	  curPg = RDPAGE; return 1;
  } else if (buf[0] == 'S' && buf[1] == 'D'){
	  c = sscanf(buf+3," %d", &subPg);
	  if( c != 1 || subPg < 0 || subPg > MAXLP) subPg = 1;
      curPg = SDPAGE; return 1;
  // ack alarms
  } if (buf[0] == 'a' && buf[1] == 'c' && buf[2] == 'k'){ // ack alarm
      sscanf(buf+3,"%d", &Id);
       sendIt(Id-1, 0, ACKALRM, 1); return 1;
  // set IP host port sensor
  } if (buf[0] == 'i' && buf[1] == 'p'){ 
    sscanf(buf + 2, "%d %s %d %d", &Id, host, &port, &sensor); 
    Id=1; 
    printf("id = %d host:%s port:%d sensor:%d buf:%s\n", Id, host, port, sensor, buf);
    if( Id > 0 && Id <= MAXLP ){
      --Id;
      strcpy(b[Id].host, host);
      b[Id].port = port;
      b[Id].sensor = sensor;
      printf("id = %d host:%s port:%d sensor:%d \n", Id, b[Id].host, b[Id].port, b[Id].sensor);
      closeOpenMb(Id);
      sleep(5);
    }      
  }
  // quit
  if (buf[0] == 'q') { WriteDefualts(); return 0;} 
  if (buf[0] == 's') {	// hide off scan sensors
    sscanf(buf + 2, " %d", &hideOffScan);
    printf("%s%s", HOME, ERASEDIS);
  // reset
  } else if (buf[0] == 'r') {	 
   	switch( buf[1] ) {
	  case 't': intVal = 1;break; 
	  case 'h': intVal = 2;break;
	  case 'p': intVal = 4; break;
	}	  
	c = parUsrInt(buf+3,&Id,&reset);
	printf("id = %d %d %s c:%d\n", Id, intVal, buf, c);
    sendIt(Id-1,Id, SRESET, intVal);
  // set alarm mask
  } else if (buf[0] == 'a' && buf[1] == 'm') { 
    //  if (curPg == SDPAGE) Id = subPg; else sscanf(buf + 2, "%d %x", &Id, &intVal);
	  c = parUsrHex(buf+2,&Id,&intVal);
	  printf("am id = %d intval:%x c:%d\n", Id, intVal, c);
	  sendIt(Id-1,Id, ALSTAT, intVal);
  // set logging
  } else if (buf[0] == 'L') {	
	c = parUsrHex( buf+1,  &Id,  &intVal );
    printf("  log: %d \n", intVal);
    sendIt(0, 0, 8, intVal);
	// soft cal scal
  } else if (buf[0] == 'c') {	
    switch( buf[1] ) {
	  case 't': reset = STEMP; break;
	  case 'h': reset = SHUM; break;
	  case 'p': reset = SPRES; break;
	}
	c = parUsrFloat( buf+2,  &Id,  &val );
    printf("id = %d cal = %3.1f c:%d\n", Id, val, c);
    sendIt(Id-1,Id, reset, (uint16_t) (int) (ceil(val * 10.0)));
  } else if (buf[0] == 'a' && buf[1] == 'l') {	// set alarm points low
	switch( buf[2] ) {
	  case 't': reset = ASP_TLO;break;
	  case 'h': reset = ASP_HLO;break;
	  case 'p': reset = ASP_PLO; break;
	}		
	c = parUsrFloat( buf+4,  &Id,  &val );

    printf("id = %d alo = %3.1f c:%d\n", Id, val, c);
    // write alrm lo
    sendIt(Id-1,Id, reset, (uint16_t) (int) (ceil(val * 10.0)));
        return 1;
// set alarm points high
  } else if (buf[0] == 'a' && buf[1] == 'h') {	
	switch( buf[2] ) {
	  case 't': reset = ASP_THI; break;
	  case 'h': reset = ASP_HHI; break;
	  case 'p': reset = ASP_PHI; break;
	}
    c = parUsrFloat( buf+4,  &Id,  &val );
	printf("id = %d ahi = %3.1f\n", Id, val);
    sendIt(Id-1,Id, reset, (uint16_t) (int) (ceil(val * 10.0)));
	// set Address
  } else if (buf[0] == 'a' && buf[1] !='\n') {	
	
	c = parUsrHex( buf+1,  &Id,  &reset); 
    switch (Id) {
      case 1: reg = ADR_1;break;
      case 2: reg = ADR_2;break;
      case 3: reg = ADR_3;break;
      case 4: reg = ADR_4;break;
    }
    printf("reg %d adr = %d c:%d\n", reg, reset, c);
    sendIt(0,0, reg, reset);
  } else if (buf[0] == 'l' && buf[1] != '\n') {	// set loop
	c = parUsrInt( buf+1,  &Id,  &reset );
  //  sscanf(buf + 1, " %d %d", &Id, &reset);
    switch (Id) {
      case 1: reg = LOP_1; break;
      case 2: reg = LOP_2; break;
      case 3: reg = LOP_3; break;
      case 4: reg = LOP_4; break;
    }
    printf("reg %d lop = %d\n", reg, reset);
    // write log
    sendIt(0,0, reg, reset);
    } else if (buf[0] == 'h') {
      userHelp();
      sleep(5);
  }
 ;
  return 1;
}

int getUserIn(char*b) {
fd_set readfds;
struct timeval timeout;
  
  timeout.tv_sec = 0;
  timeout.tv_usec = 500000; // 500ms
  FD_ZERO(&readfds);
   
  while(1){
    FD_SET(STDIN_FILENO, &readfds);
    if (select(1, &readfds, NULL, NULL, &timeout)) {
      numRead = read(0, buf, 30);
      printf("buf: %s %d \n", b, numRead);
      return 1;
    }     
    return numRead;
  }
return(0);
}

