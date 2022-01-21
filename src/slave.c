// bill shep modbus server for bme280
// ver 1.0.1 12.29.2021 06:48:46
#define MAIN__ TRUE

#include "mbcom.h"
#include "m.h"
#include "alarms.h"


static modbus_t *ctx = NULL;
// modbus_mapping_t *mb_mapping;

pthread_attr_t  attr;
static void     gager(void);
pthread_t       tGager = (pthread_t) NULL;
volatile int    gagerRun = 1;
volatile int    slaveRun = 1;

pthread_attr_t  attr2;
static void     Slave(void);
pthread_t       tSlave = (pthread_t) NULL;

void            initAlarmPoints(void);

char            host[20] = { 0 };

int             port = 1502;
int             debug = 0;
// int slaveId = 0x2;
time_t          tt;
struct tm       tm;

int             dolog = 4;
int             doreset = 4;
// int cur = 0;

static int      server_socket = -1;

void
init280()
{

    for (int ix = 0; ix < MAXLP; ++ix) {

	memset(b[ix].data, 0, (LASTREG << 1));	// modbus holding
	// registers
	memset(b[ix].sCal, 0, (LASTS << 1));	// modbus input registers
	b[ix].loop = 0;		// i2c bus 0
	b[ix].adr = 0;		// bme280 address (0x76 or 0x77) | 0x80 =
	// enabled
    }

}

static void
close_sigint(int dummy)
{
    int             x;
    void           *res;



    gagerRun = 0;
    slaveRun = 0;
    if (tSlave != (pthread_t) NULL)
	x = pthread_join(tSlave, &res);
    if (x) {
	// - TODO add syslog
	syslog(LOG_NOTICE, "Error stop slave thread\n");

    }
    if (server_socket != -1) {
	close(server_socket);
    }

    if (tGager != (pthread_t) NULL)
	x = pthread_join(tGager, &res);
    if (x) {
	// - TODO add syslog
	syslog(LOG_NOTICE, "Error stop gage thread\n");

    }
    WriteStat();

    syslog(LOG_NOTICE, "Program stopped by SIGINT\n");

    closelog();
    exit(0);
}

int
main(void)
{
    // sprintf( host,"192.168.68.125" );
    signal(SIGINT, close_sigint);

    openlog("mb280", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog(LOG_NOTICE, "Program started by User %d", getuid());
    init280();

    // setlogmask (LOG_UPTO (LOG_NOTICE));


    ReadStat();
    loog = b[0].data[SLOG];
    readDefualts();
    printf("= host = %s\n", host);
    for(int ix =0; ix < MAXLP; ++ix){ // clear last alarms
      b[ix].data[ALSTAT] &= AEBITS;
    }    
   //  initAlarmPoints();
   //  retsetAll();
   //  WriteStat();
    tt = time(NULL);
    tm = *localtime(&tt);
    pthread_attr_init(&attr);

    int             x =
	pthread_create(&tGager, &attr, (void *(*)(void *)) &gager, NULL);
    if (x) {
	syslog(LOG_NOTICE, "Error starting tGager");
    }

    usleep(1500000);		// delay start of slave some gager can
    // poll bme280 first
 

    pthread_attr_init(&attr2);
    int             s =
	pthread_create(&tSlave, &attr2, (void *(*)(void *)) &Slave, NULL);
    if (s) {
	syslog(LOG_NOTICE, "Error starting tSlave");
    }


    while (1) {
	// update global time stamp
	tt = time(NULL);
	tm = *localtime(&tt);


	// reset stats at midnite
	if (doreset)
	    --doreset;
	// if (!doreset && !tm.tm_hour && !tm.tm_min && !tm.tm_sec) {
	if (!doreset && tm.tm_hour == 23 && tm.tm_min == 59 && !tm.tm_sec) {
	    retsetAll();
	    archist();
	    syslog(LOG_NOTICE, "Resetting stats\n");
	    doreset = 5;
	}
	// log data 
	if (dolog)
	    --dolog;
	if (!dolog && loog) {
	    if (!(tm.tm_min % loog) && !tm.tm_sec) {
		writeLog();
		dolog = 4;
	    }
	}
	usleep(500000);
    }
}

void
resetHum(int ix)
{
    b[ix].data[MHUM] = 1000;
    b[ix].data[XHUM] = 0;
    b[ix].data[AHUM] = (b[ix].data[MHUM] + b[ix].data[XHUM]) >> 1;

    printf("reset %d hum\n", ix);

}

void
resetTemp(int ix)
{

    b[ix].data[MTEMP] = 9990;
    b[ix].data[XTEMP] = -990;   
    b[ix].data[ATEMP] = (b[ix].data[MTEMP] + b[ix].data[XTEMP]) >> 1;

    printf("reset %d temp\n", ix);
}

void
resetPres(int ix)
{
    b[ix].data[MPRES] = 9000;
    b[ix].data[XPRES] = -10;
    b[ix].data[APRES] = (b[ix].data[MPRES] + b[ix].data[XPRES]) >> 1;

    printf("reset %d pres\n", ix);

}

void
retsetAll(void)
{
    for (int ix = 1; ix < MAXLP; ++ix) {
	// force off-scan sensors
	if (!IsOnScan(ix))
	    b[ix].data[HOUR] = b[ix].data[MINIT] = b[ix].data[SEC] = 0;
	resetHum(ix);
	resetTemp(ix);
	resetPres(ix);
    }
}

static void
stats(int ix)
{
    // find max min av
    //if( ix == 4 )
     //printf( "stats() - temp:%d, min:%d  max:%d av:%d\n",
	//b[ix].data[TEMP], b[ix].data[MTEMP] ,  b[ix].data[XTEMP], b[ix].data[ATEMP]);
     
    if (b[ix].data[TEMP] > b[ix].data[XTEMP])	b[ix].data[XTEMP] = b[ix].data[TEMP];
    if (b[ix].data[MTEMP] >= b[ix].data[TEMP])	b[ix].data[MTEMP] = b[ix].data[TEMP];
    b[ix].data[ATEMP] = (b[ix].data[MTEMP] + b[ix].data[XTEMP]) >> 1;

    if (b[ix].data[HUM] > b[ix].data[XHUM])	b[ix].data[XHUM] = b[ix].data[HUM];
    if (b[ix].data[MHUM] >= b[ix].data[HUM])	b[ix].data[MHUM] = b[ix].data[HUM];
    b[ix].data[AHUM] = (b[ix].data[MHUM] + b[ix].data[XHUM]) >> 1;

    if (b[ix].data[PRES] > b[ix].data[XPRES]) b[ix].data[XPRES] = b[ix].data[PRES];
    if (b[ix].data[MPRES] < b[ix].data[PRES]) b[ix].data[MPRES] = b[ix].data[PRES];
    b[ix].data[APRES] = (b[ix].data[MPRES] + b[ix].data[XPRES]) >> 1;
    // printf("stats %d \n", ix);

}

int
IsOnScan(int ix)
{
    return (b[ix].adr & 0x80);
}

void
initAlarmPoints(void)
{
    for (int ix = 1; ix < MAXLP; ++ix) {
	b[ix].sCal[ASP_TLO] = 650;
	b[ix].sCal[ASP_THI] = 700;
	b[ix].sCal[ASP_HLO] = 400;
	b[ix].sCal[ASP_HHI] = 500;
	b[ix].sCal[ASP_PLO] = 9000;
	b[ix].sCal[ASP_PHI] = 10200;
	b[ix].data[ALSTAT] = AEBITS;
    }
}

static void gager(void)
{
    int             res = 0;
    float           temp,
                    hum,
                    pres;

    usleep( 1000000);
    
    while (gagerRun) {
	// map local config of lop & adr to modbus regists
	b[0].data[LOP_1] = b[1].loop;
	b[0].data[ADR_1] = b[1].adr;
	b[0].data[LOP_2] = b[2].loop;
	b[0].data[ADR_2] = b[2].adr;
	b[0].data[LOP_3] = b[3].loop;
	b[0].data[ADR_3] = b[3].adr;
	b[0].data[LOP_4] = b[4].loop;
	b[0].data[ADR_4] = b[4].adr;
	b[0].data[SLOG] =  loog;

	// printf("[%d] LOP:%x\n", 1, b[0].data[LOP_1] );

	for (int cur = 1; cur < MAXLP; ++cur) {

	//    if (!IsOnScan(cur))
	//	continue;

	    b[cur].data[HOUR] = tm.tm_hour;
	    b[cur].data[MINIT] = tm.tm_min;
	    b[cur].data[SEC] = tm.tm_sec;


	    //printf("[%d} adr:%x\n", cur, b[cur].adr);
 
	    res = read280(b[cur].adr, &temp, &hum, &pres);
	  //  printf("gage %d, t:%5.1f h:%5.1f p:%5.1f\n",cur,temp,hum,pres);
	    if (res) {
		syslog(LOG_NOTICE, "Error read280 %d\n", res);
		continue;
	    } else {
		// printf("update gage %d\n", cur);

		b[cur].data[RTEMP] = (int16_t) (int) (temp * 10.0);
		b[cur].data[TEMP] = b[cur].data[RTEMP] + b[cur].sCal[STEMP];

		b[cur].data[RHUM] = (int16_t) (int) (hum * 10.0);
		b[cur].data[HUM] = b[cur].data[RHUM] + b[cur].sCal[SHUM];

		b[cur].data[RPRES] = (int16_t) (int) (pres * 10.0);
		b[cur].data[PRES] =
		    b[cur].data[RPRES] + b[cur].sCal[SPRES];

		checkAlarms(cur);

		stats(cur);
	    }
	    usleep(500000);
	}
    }
}

static void
Slave(void)
{
    uint8_t         query[MODBUS_TCP_MAX_ADU_LENGTH];
    int             master_socket;
    int             rc,
                    sc;
    fd_set          refset;
    fd_set          rdset;
    int             fdmax;	/* Maximum file descriptor number */
    int             val = 0;
    int             reg = 0;
    int             activeId = 0;

    syslog(LOG_NOTICE, "listing host:<%s> port:%d debug:%d\n", host, port,
	   debug);

    ctx = modbus_new_tcp(host, port);
    // - modbus_set_response_timeout(ctx, 0, 500000);
    modbus_set_debug(ctx, debug);
    // modbus_set_slave(ctx, slaveId);

    // crate modbus memory map
    mb_mapping =
	modbus_mapping_new(MODBUS_MAX_READ_BITS, 0, LASTREG, LASTREG0);
    if (mb_mapping == NULL) {
	syslog(LOG_NOTICE, "Failed to allocate the mapping: %s\n",
	       modbus_strerror(errno));
	modbus_free(ctx);
	close_sigint(1);
    }

    server_socket = modbus_tcp_listen(ctx, NB_CONNECTION);	// max 5
    // connection

    if (server_socket == -1) {
	syslog(LOG_NOTICE, "Unable to listen TCP connection");
	modbus_free(ctx);
	close_sigint(1);
    }

    /*
     * Clear the reference set of socket 
     */
    FD_ZERO(&refset);
    /*
     * Add the server socket 
     */
    FD_SET(server_socket, &refset);
    /*
     * Keep track of the max file descriptor 
     */
    fdmax = server_socket;

    while (slaveRun) {
	rdset = refset;
	if (select(fdmax + 1, &rdset, NULL, NULL, NULL) == -1) {
	    syslog(LOG_NOTICE, "Server select() failure.");
	    close_sigint(1);
	}

	/*
	 * Run through the existing connections looking for data to be
	 * read 
	 */
	for (master_socket = 0; master_socket <= fdmax; master_socket++) {

	    if (!FD_ISSET(master_socket, &rdset)) {
		continue;
	    }

	    if (master_socket == server_socket) {
		/*
		 * A client is asking a new connection 
		 */
		socklen_t       addrlen;
		struct sockaddr_in clientaddr;
		int             newfd;

		/*
		 * Handle new connections 
		 */
		addrlen = sizeof(clientaddr);
		memset(&clientaddr, 0, sizeof(clientaddr));
		newfd =
		    accept(server_socket, (struct sockaddr *) &clientaddr,
			   &addrlen);
		if (newfd == -1) {
		    perror("Server accept() error");
		} else {
		    FD_SET(newfd, &refset);

		    if (newfd > fdmax) {
			/*
			 * Keep track of the maximum 
			 */
			fdmax = newfd;
		    }
		    syslog(LOG_NOTICE,
			   "New connection from %s:%d on socket %d\n",
			   inet_ntoa(clientaddr.sin_addr),
			   clientaddr.sin_port, newfd);
		}
	    } else {
		modbus_set_socket(ctx, master_socket);
		rc = modbus_receive(ctx, query);
		if (rc > 0) {
		    // for( int i = 6; i < 16; ++i){ printf("(%4x)",
		    // query[i]) } 

		    // update modbus regs
		    activeId = query[SLAVEID];
		    // printf("activeId = %d\n", activeId );
		    // activeId = 0;
//printf("cmd:%d Id=%d\n", query[COMMAND], activeId);

		    if (activeId >= 0 && activeId < MAXLP) {
			memcpy(mb_mapping->tab_registers, b[activeId].data,LASTREG << 1);
			memcpy(mb_mapping->tab_input_registers, b[activeId].sCal, LASTREG0 << 1);
		    }

		    if (query[COMMAND] == FUNC6) {
			// printf( "-REG %d %d\n", (int)query[REGADR],
			// (int)query[REGVAL]);
			reg = query[REGADR];
			if (activeId != 0) {   //// bme commands handled here

			    switch (reg) {
			    case SRESET:	// set reset
				if (query[REGVAL] & 0x1) {	// temp 
				    b[activeId].sCal[SRESET] |= 0x1;
				    resetTemp(activeId);
				}
				if (query[REGVAL] & 0x2) {	// hum 
				    b[activeId].sCal[SRESET] |= 0x2;
				    resetHum(activeId);
				}
				if (query[REGVAL] & 0x4) {	// pres 
				    b[activeId].sCal[SRESET] |= 0x4;
				    resetPres(activeId);
				}
				val = query[REGVAL];
				mb_mapping->tab_input_registers[SRESET] =
				b[activeId].sCal[SRESET];
				break;
			    case ALSTAT:	// set alarm mask 
				val = (query[REGVAL - 1] << 8) | query[REGVAL];
				b[activeId].data[ALSTAT] = (uint16_t) val;
				break;
			    case STEMP:	// scal temp
				val = (query[REGVAL - 1] << 8) | query[REGVAL];
				b[activeId].sCal[STEMP] = (uint16_t) val;
				break;
			    case SHUM:	// scal hum
				val = (query[REGVAL - 1] << 8) | query[REGVAL];
				b[activeId].sCal[SHUM] = (uint16_t) val;
				break;
			    case SPRES:	// scal pres 
				val = (query[REGVAL - 1] << 8) |  query[REGVAL];
				b[activeId].sCal[SPRES] = (uint16_t) val;
				break;
			    case ASP_TLO :	// set alarm points
			    case ASP_THI :
			    case ASP_HLO :
			    case ASP_HHI :
			    case ASP_PLO :
			    case ASP_PHI :
				val = (query[REGVAL - 1] << 8) | query[REGVAL];
				b[activeId].sCal[reg] = (uint16_t) val;	
				printf("al set id: %d reg:%d val: %d\n", activeId, reg, val);

				break;	    
			    }
			} else {	// slaveId 0
			    switch (reg) {
			    case LOP_1:
			    case LOP_2:
			    case LOP_3:
			    case LOP_4:
				val =
				    (query[REGVAL - 1] << 8) |
				    query[REGVAL];
				reg = (reg >> 1) + 1;
				printf("LOp:%d, %d\n", reg, val);
				if (val >= 0) {
				    b[reg].loop = val;
				    printf(" :%d, %d\n", reg,
					   b[reg].loop);
				}
				reg = query[REGADR];
				WriteDefualts();
				break;

			    case ADR_1:
			    case ADR_2:
			    case ADR_3:
			    case ADR_4:
				val =
				    (query[REGVAL - 1] << 8) |
				    query[REGVAL];
				printf("Adr:%d, %d\n", reg, val);
				reg = (reg >> 1) + 1;

				// if( val >= 0 ){
				b[reg].adr = val;
				printf("xADR:%d, %d\n", val, b[reg].adr);

				// }
				reg = query[REGADR];
				break;

			    case SLOG:	// Logging 
				val =
				    (query[REGVAL - 1] << 8) |
				    query[REGVAL];
				if (val >= 0) {
				    printf("log:%d\n", val);
				    loog = val;
				}
				break;
			    case ACKALRM:	// Ack all 
				val = (query[REGVAL - 1] << 8) | query[REGVAL];
				if (val >= 0) {
				    printf("ack:%d\n", val);
				   ackAllArm();
				   
				}
				break;
			    }
			}	// not slave id 0
			syslog(LOG_NOTICE,
			       "Client:%d Id:%d Write reg:%d val:%d\n",
			       master_socket, activeId, query[REGADR],
			       val);

		    } 
  //if (query[COMMAND] == 4)
    //printf(".\n");
		    sc = modbus_reply(ctx, query, rc, mb_mapping);


		    if (query[COMMAND] == FUNC6) {
			// 
			switch (query[REGADR]) {
			    // clear reset
			case 0:
			    if (query[REGVAL] & 0x1) {	// temp 
				b[activeId].sCal[SRESET] &= !0x1;
			    }
			    if (query[REGVAL] & 0x2) {	// hum 
				b[activeId].sCal[SRESET] &= !0x2;
			    }
			    if (query[REGVAL] & 0x4) {	// pres 
				b[activeId].sCal[SRESET] &= !0x4;
			    }
			    // mb_mapping->tab_input_registers[SRESET] =
			    // b[activeId].sCal[SRESET];
			    break;
			}
			///printf("slave temp low alrm %d \n", mb_mapping->tab_input_registers[ASP_TLO]);
			WriteDefualts();
			WriteStat();
		    }

		    if (sc == -1) {
			/*
			 * any errors. 
			 */
			syslog(LOG_NOTICE,
			       "Connection closed on socket %d\n",
			       master_socket);
		    }		// dedect write to regs 0, 1 2 3

		} else if (rc == -1) {
		    /*
		     * This example server in ended on connection closing
		     * or any errors. 
		     */
		    syslog(LOG_NOTICE, "Connection ended on socket %d\n",
			   master_socket);
		    close(master_socket);

		    /*
		     * Remove from reference set 
		     */
		    FD_CLR(master_socket, &refset);

		    if (master_socket == fdmax) {
			fdmax--;
		    }
		}
	    }
	}
    }
    close(master_socket);
    modbus_free(ctx);
}
