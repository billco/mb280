// bill shep modbus server for bme280
// ver 1.0.1 12.29.2021 06:48:46
  
 
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
#include "m.h"
typedef struct {
	int16_t data[LASTREG];		// modbus holding registers
	int16_t sCal[LASTS];		// modbus input registers

	char tag[20];		// string name
	int16_t loop;		// i2c bus 0
	int16_t adr ;		// bme280 address (0x76 or 0x77) | 0x80 = enabled
} bme280_t;

static bme280_t b[MAXLP+1];

int16_t data[LASTREG] = {0};
int16_t sCal[LASTS] = {0};

static modbus_t *ctx = NULL;
static modbus_mapping_t *mb_mapping;


pthread_attr_t attr;
static void gager(void );
pthread_t tGager = (pthread_t) NULL;
volatile int gagerRun = 1;

pthread_attr_t attr2;
static void Slave(void);
pthread_t tSlave = (pthread_t) NULL;
static void retsetAll( void ) ;
int init280( int ix );


char host[20] = "127.0.0.1";
int  port = 1502;
int  debug = FALSE;
int  slaveId = 0x2;


int dolog = 3;
int doreset = 3;
int loging = 5; // in mintunes "0 = disabled

extern int read280( float *temp, float *hum, float *pres);
void WriteStat( void );
void ReadStat( void );
void WriteDefualts( void );

static int server_socket = -1;
time_t tt;
struct tm tm;

int init280( int ix ){
	memset( b[ix].data, 0, (LASTREG << 1));		// modbus holding registers
	memset(b[ix].sCal, 0, (LASTS << 1));				// modbus input registers
	
	sprintf( b[ix].tag, "Tag%d", ix);					// string name
	b[ix].loop = 0;									// i2c bus 0
	b[ix].adr = 0 ;									// bme280 address (0x76 or 0x77) | 0x80 = enabled
	return ix;
}

void readDefualts(){
	char buf[512] = {0};

	
	FILE *fd = fopen("bme280.cfg","r" );
	
	if( fd == NULL ) { printf( "error opening bme280.cfg\n"); return; }
	do {
		fgets(buf, 511, fd); // dummy read of header
	} while(buf[0]=='#');

	//printf( "[%s]\n", buf );
	fgets(buf, 511, fd);
	//printf( "[%s]\n", buf );
	sscanf(buf,"%s %d %d %d\n", host, &port, &slaveId, &debug);
	
	fclose(fd);
}


void WriteDefualts(){
	FILE *fd = fopen("bme280.cfg","w" );

	fprintf( fd,"# host, port, slaveId, debug\n");

	fprintf( fd, "%s %d %d %d\n", host, port, slaveId, debug);
	fclose(fd);

}

void WriteStat(){
	FILE *fd = fopen("bme280stat.cfg","wb" );
     fwrite(&data, sizeof(data), 1, fd); 
     fwrite(&sCal, sizeof(sCal), 1, fd); 
	syslog (LOG_NOTICE, "Saving stat...");

	fclose(fd);
//for(int i = 0; i < LASTREG; ++i ){
	//printf("data[%d] = %d\n", i, data[i] );
//}for(int i = 0; i < LASTS; ++i ){
	//printf("sCal[%d] = %d\n", i, sCal[i] );
//}
}
	
void ReadStat(){
	FILE *fd = fopen("bme280stat.cfg","rb" );
		
	if( fd == NULL ) {
		retsetAll();
			syslog (LOG_NOTICE, "Error Reading stat...");
		return;
	}
    fread(&data, sizeof(data), 1, fd); 
    fread(&sCal, sizeof(sCal), 1, fd); 
	syslog (LOG_NOTICE, "Reading stat...");
//for(int i = 0; i < LASTREG; ++i ){
	//printf("data[%d] = %d\n", i, data[i] );
//}for(int i = 0; i < LASTS; ++i ){
	//printf("sCal[%d] = %d\n", i, sCal[i] );
//}

	fclose(fd);
}
	
void writeLog(){
	FILE *fd = fopen("history.csv","a" );
	if( fd != NULL ) {
		fprintf( fd, "%02d/%02d/%04d %02d:%02d:%02d,%5.1f,%5.1f,%5.1f\n", 
			tm.tm_mon+1,
			tm.tm_mday,
			tm.tm_year +1900,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec,				
			((float)data[TEMP]) * 0.1,
			((float)data[HUM]) * 0.1,
			((float)data[PRES]) * 0.1
		);
		fclose(fd);
		syslog (LOG_NOTICE, "Logging...");
	} else {
		syslog (LOG_NOTICE, "Logging Error %s", strerror(errno));
	}
}

static void close_sigint(int dummy)
{ 	
    int x;
    void *res;
    if (server_socket != -1) {
        close(server_socket);
    }
       
	gagerRun = 0;
	if (tSlave != (pthread_t) NULL)
	x = pthread_join(tSlave, &res);
	if( x ) {
		//- TODO add syslog
	}
	
	if (tGager != (pthread_t) NULL)	
	 x = pthread_join(tGager, &res);
	if( x ) {
		//- TODO add syslog
	}
	WriteStat();
	
	syslog (LOG_NOTICE, "Program stopped by SIGINT\n");
	
 	closelog();    
    exit(0);
}

int main(void){
	
    signal(SIGINT, close_sigint);
openlog ("mb280", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_NOTICE, "Program started by User %d", getuid ());
	
	for( int ix = 0; ix < MAXLP; ++ix ){
		init280( ix );
printf("%d [%s]\n", ix, b[ix].tag );
	}
//setlogmask (LOG_UPTO (LOG_NOTICE));

	
	ReadStat();
	readDefualts();
	
	pthread_attr_init(&attr);
	
	int x = pthread_create(&tGager, &attr, (void *(*)(void *))&gager,NULL);
	if( x ) {
		syslog (LOG_NOTICE, "Error starting tGager");
	} 
	
	usleep( 1500000  ); // delay start of slave some gager can poll bme280 first
	
	pthread_attr_init(&attr2);
	int s = pthread_create(&tSlave, &attr2, (void *(*)(void *))&Slave,NULL);
	if( s ) {
		syslog (LOG_NOTICE, "Error starting tSlave");	
	}
	
	
	while(1) {
		// update global time stamp
		tt 			= time(NULL);
		tm 			= *localtime(&tt);
		data[HOUR] 	= tm.tm_hour;
		data[MINIT] = tm.tm_min;
		data[SEC] 	= tm.tm_sec;
		
		// reset stats at midnite
		if ( doreset ) --doreset ;

		if(!doreset &&!data[HOUR] && !data[MINIT] && !data[SEC] )	{
			retsetAll();
			syslog (LOG_NOTICE, "Resetting stats\n");
			doreset = 3;
		} 
		
		// log data 
		if ( dolog ) --dolog ;

		if( !dolog && sCal[SLOG] ){
			if(!(data[MINIT] % sCal[SLOG] ) && !data[SEC] )	{
				writeLog();
				dolog = 3;
			}
		} 

		usleep( 500000 );		
	}
}

 void resetHum( ){
	data[MHUM] = 9999; data[XHUM] = data[AHUM] = 0;
}

 void resetTemp( ){
	data[MTEMP] = 9999; data[XTEMP] = -9999; data[ATEMP] = 0;
}

 void resetPres( ){
	data[MPRES]  = 32767;  data[XPRES] = -9999; data[APRES] = 0;
}

 void retsetAll() {
	resetHum();
	resetTemp();
	resetPres();
}

 void stats( ){
	// find max min av
	if( data[TEMP] > data[XTEMP] ) {
		data[XTEMP] = data[TEMP];
	}
	if( data[TEMP] < data[MTEMP]) {
		data[MTEMP] = data[TEMP];
	}
	data[ATEMP] = (data[MTEMP] + data[XTEMP]) >> 1;
	
	if( data[HUM] > data[XHUM] ) {
		data[XHUM] = data[HUM];
	}
	if( data[HUM] < data[MHUM]) {
		data[MHUM] = data[HUM];
	}
	data[AHUM] = (data[MHUM] + data[XHUM]) >> 1;

	if( data[PRES] > data[XPRES] ) {
		data[XPRES] = data[PRES];
	}
	if( data[PRES] < data[MPRES]) {
		data[MPRES] = data[PRES];
	}
	data[APRES] = (data[MPRES] + data[XPRES]) >> 1;
} 

static void gager(void )
{
	int res = 0;
	float temp,hum,pres;
	
	//retsetAll();

	while (gagerRun) {
		res = read280(  &temp,  &hum,  &pres);
		if( res) {
			syslog (LOG_NOTICE,"Error read280 %d\n", res);	
		} else {						
			data[RTEMP] = (int16_t)(int)(temp * 10.0);
			data[TEMP]  = data[RTEMP] + sCal[STEMP];
			
			data[RHUM] = (int16_t)(int)(hum * 10.0);
			data[HUM]  = data[RHUM] + sCal[SHUM];
			
			data[RPRES] = (int16_t)(int)(pres * 10.0);
			data[PRES]  = data[RPRES] + sCal[SPRES];
					
			stats(); 
		}
		
		usleep( 1000000 );
	}	
}
 
static void Slave(void)
{
	uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int master_socket;
    int rc,sc;
    fd_set refset;
    fd_set rdset;
    int fdmax; /* Maximum file descriptor number */
    int val = 0;
    
    syslog (LOG_NOTICE, "listing host:<%s> port:%d slaveId:%d debug:%d\n",
		host, port, slaveId, debug);

    ctx = modbus_new_tcp(host, port);
    //- modbus_set_response_timeout(ctx, 0, 500000);
    modbus_set_debug(ctx, debug);
	modbus_set_slave(ctx,slaveId);
	
	// crate modbus memory map
    mb_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,
                                    LASTREG, LASTS);
    if (mb_mapping == NULL) {
        syslog (LOG_NOTICE, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        close_sigint(1);
    }
    
    for( int ix = 0; ix < LASTREG; ++ix) {
        mb_mapping->tab_registers[ix] = ix; 
    } 
	memcpy( mb_mapping->tab_input_registers, sCal, LASTS << 1 );

    server_socket = modbus_tcp_listen(ctx, NB_CONNECTION); // max 5 connection
    
    if (server_socket == -1) {
	    syslog (LOG_NOTICE, "Unable to listen TCP connection");
        modbus_free(ctx);
        close_sigint(1);
    }

    /* Clear the reference set of socket */
    FD_ZERO(&refset);
    /* Add the server socket */
    FD_SET(server_socket, &refset);
    /* Keep track of the max file descriptor */
    fdmax = server_socket;

    for (;;) {
        rdset = refset;
        if (select(fdmax+1, &rdset, NULL, NULL, NULL) == -1) {
            syslog (LOG_NOTICE, "Server select() failure.");
            close_sigint(1);
        }

        /* Run through the existing connections looking for data to be
         * read */
        for (master_socket = 0; master_socket <= fdmax; master_socket++) {

            if (!FD_ISSET(master_socket, &rdset)) {
                continue;
            }

            if (master_socket == server_socket) {
                /* A client is asking a new connection */
                socklen_t addrlen;
                struct sockaddr_in clientaddr;
                int newfd;

                /* Handle new connections */
                addrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, sizeof(clientaddr));
                newfd = accept(server_socket, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1) {
                    perror("Server accept() error");
                } else {
                    FD_SET(newfd, &refset);

                    if (newfd > fdmax) {
                        /* Keep track of the maximum */
                        fdmax = newfd;
                    } 
					syslog (LOG_NOTICE, "New connection from %s:%d on socket %d\n",
                           inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);			                      
                }
            } else {
                modbus_set_socket(ctx, master_socket); 
                rc = modbus_receive(ctx, query);
                if (rc > 0) {
                    //for( int i = 6; i < 16; ++i){ printf("(%4x)", query[i]) } 
                                       
                   	// update modbus regs
					memcpy( mb_mapping->tab_registers, data, LASTREG << 1 );
					memcpy( mb_mapping->tab_input_registers, sCal, LASTS << 1 );
                   	
  
                   if( query[COMMAND] == FUNC6 && query[SLAVEID] == slaveId){
						//  printf( "-REG %d %d\n", (int)query[REGADR], (int)query[REGVAL]);
						switch( query[REGADR] ) {								
							case SRESET:	// set reset
								if( query[REGVAL] & 0x1) { // temp	
									sCal[SRESET] |= 0x1;					
									resetTemp();
								}
								if( query[REGVAL] & 0x2) { // hum	
									sCal[SRESET] |= 0x2;													
									resetHum();
								}
								if( query[REGVAL] & 0x4) { // pres	
									sCal[SRESET] |= 0x4;												
									resetPres();
								}
								val = query[REGVAL];
								mb_mapping->tab_input_registers[SRESET] = sCal[SRESET];
								break;								
							case STEMP:	// scal temp
								val = (query[REGVAL-1] << 8) | query[REGVAL];
								resetTemp();
								sCal[STEMP] = (uint16_t)val;
								WriteDefualts();
								break;
								
							case SHUM:	// scal hum
								val = (query[REGVAL-1] << 8) | query[REGVAL];
								resetTemp();
								sCal[SHUM] = (uint16_t)val;
								WriteDefualts();
								break;
								
							case SPRES:	// scal pres					
								val = (query[REGVAL-1] << 8) | query[REGVAL];
								resetPres();
								sCal[SPRES] = (uint16_t)val;
								WriteDefualts();
								break;
									
							case SLOG:	// Logging					
								val = (query[REGVAL-1] << 8) | query[REGVAL];
								
								if( val >= 0 ){
									sCal[SLOG] = (uint16_t)val;
									WriteDefualts();
								}
								break;																	
						}	
						syslog (LOG_NOTICE,"Client %d write reg:%d val:%d\n",
							master_socket, query[REGADR], val);

					}
					
                    sc = modbus_reply(ctx, query, rc, mb_mapping);
                    
                    if( query[COMMAND] == FUNC6){
						 //
						switch( query[REGADR] ) {
								// clear reset
							case 0:
								if( query[REGVAL] & 0x1) { // temp	
									sCal[SRESET] &= !0x1;
								}
								if( query[REGVAL] & 0x2) { // hum	
									sCal[SRESET] &= !0x2;	
									resetHum();
								}
								if( query[REGVAL] & 0x4) { // pres	
									sCal[SRESET] &= !0x4;												
									resetPres();
								}
								
								mb_mapping->tab_input_registers[SRESET] = sCal[SRESET];
								break;
							}
						}
					
                    if (sc == -1) {
						/*  any errors. */
						syslog (LOG_NOTICE,"Connection closed on socket %d\n", master_socket);
					} // dedect write to regs 0, 1 2 3
                     
                } else if (rc == -1) {
                    /* This example server in ended on connection closing or
                     * any errors. */
                    syslog (LOG_NOTICE,"Connection ended on socket %d\n", master_socket);
                    close(master_socket);

                    /* Remove from reference set */
                    FD_CLR(master_socket, &refset);

                    if (master_socket == fdmax) {
                        fdmax--;
                    }
                }
            }
        }
    }
}
