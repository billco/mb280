/* 
 * modbus tcp client for mb280 slave
 * shep rev 1.0.1 12.30.2021 11:41:00
 *  
*/


#include "mc.h"

#define BRIGHTWHITE "\e[1;37;40m" //bold white black
#define DIMWHITE "\e[2;37;40m"  
#define DEFCOLOR "\e[0;39;49m"
#define UNDERLINEON "\e[4m"
#define UNDERLINEOFF "\e[24m"

int16_t data[LASTREG] = {0};
int16_t data0[LASTREG] = {0};
int16_t sCal[LASTS] = {0};

char host[20] = "127.0.0.1";
int port    = 1502;
int debug   = 0;
int polRate = 1000000; // in uS ie 1 second
int opt;
modbus_t *ctx;
	uint16_t tab_reg[LASTREG];
	int rc;
	int Id, curId;
	int reset = 0;
	
void disregs( int rc, int start, uint16_t * regs);

void help( void ) {
	printf("modbus tcp client for mb280 slave\n");
	printf("\t-d [modbus debug] 0|1 (%d) \n", debug );
	printf("\t-H [Slave IP] set Host/Slave ipv4 (%s)\n", host );
	printf("\t-P [Slave lisen] ipv4 Port (%d)\n", port );
	printf("\t-p [pooling rate] (%d)\n", polRate );
	printf("\t-u print user help\n");	
	printf("\t-h print cli help\n");	
}
void userHelp( void ){
	printf("\tA # val -  set address of BME# val I2C address\n \
	 \t\t(0,76,77) OR 0x80 = (0=disabled,f6,f7)\n");
	printf("\tL # val -  set Loop of BME# val I2C bus 0|1\n");
	printf("\tct # val -  Softcal temp\n");
	printf("\tch # val -  Softcal temp\n");
	printf("\tcp # val -  Softcal temp\n");
	printf("\tl  val -  Set logging on slave 0-60 every minutes 0=off\n");				
	printf("\trt # - reset temp\n");
	printf("\trh # - reset hum\n");
	printf("\trt # - reset pres\n");
	printf("\tq - quit\n");	
	printf("\t  # - Bme # = (1-4)\n");	
}
void sendIt( int id, int reg, int val ){
	int curId = modbus_get_slave(ctx);
		modbus_set_slave(ctx, id);
	rc = modbus_write_register(ctx,reg,val);
	modbus_set_slave(ctx, curId);

}
int IsOnScan( int ix ) {
 int tmp = 0;
	switch( ix ){
		case 1:
			tmp = data0[ADR_1]; break;
		case 2:
			tmp = data0[ADR_2]; break;
		case 3:
			tmp = data0[ADR_3]; break;
		case 4:
			tmp = data0[ADR_4]; break;			
	}
	
	return ( tmp & 0x80);	
}
int getLoop( int ix ) {
	switch( ix ){
		case 1:
			return (data0[LOP_1]); break;
		case 2:
			return (data0[LOP_2]); break;
		case 3:
			return (data0[LOP_3]); break;
		case 4:
			return (data0[LOP_4]); break;
	}	
	return ( 0);	
}

int getAdr( int ix ) {
	switch( ix ){
		case 1:
			return (data0[ADR_1]); break;
		case 2:
			return (data0[ADR_2]); break;
		case 3:
			return (data0[ADR_3]); break;
		case 4:
			return (data0[ADR_4]); break;
	}	
	return ( 0);	
}

int main(int argc, char *argv[]) {

	if( argc < 2 ){		
		help();
		return 1;
	}
	while ((opt = getopt(argc, argv, "H:Phudp?:")) != -1) {
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
			case 'p':
				polRate = atoi(optarg);
				if (polRate < 1000) polRate = 1000000;
				break;
			case 'u' :
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
	
	

	char buf[256]= {0};
	int intVal = 0;
	int reg = 0;
	float val = 0;
		
	printf("Starting\n");
	ctx = modbus_new_tcp( host, port);
	if (modbus_connect(ctx) == -1) {
		fprintf(stderr, "Connection failed: %s:%d %s\n", host, port, modbus_strerror(errno));
		modbus_free(ctx);		
		return -1;
	}		
	rc = modbus_connect(ctx);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno)); 
	}
	// -	modbus_set_response_timeout(ctx, 0, 0);
	modbus_set_debug(ctx, debug);
	
	modbus_set_error_recovery(ctx,
                          MODBUS_ERROR_RECOVERY_LINK |
                          MODBUS_ERROR_RECOVERY_PROTOCOL);
	
	while(1) {		
		memset(buf, 0, sizeof(buf));
		fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
	    int numRead = read(0, buf, 10);
	    if (numRead > 0) {
	        if(buf[0] == 'q' ){
				break;
			} else if(buf[0] == 'r' ){
				if(buf[1] == 't') intVal = 1;
	  			if(buf[1] == 'h') intVal = 2;
	  			if(buf[1] == 'p') intVal = 4;
	  			
	  			sscanf(buf +2, "%d", &Id);	  			
	  			printf("id = %d %d %s\n", Id, intVal, buf);
	  			// write reset
	  			sendIt( Id, SRESET, intVal );				
			} else if(buf[0] == 'l' ){				 
	  			sscanf(buf +2, " %d", &intVal);
	  			printf("  log: %d \n", intVal);
	  			// write log val
	  			sendIt( 0,8, intVal );					
			} else if(buf[0] == 'c' ){
				if(buf[1] == 't') reset = 1;
	  			if(buf[1] == 'h') reset = 2;
	  			if(buf[1] == 'p') reset = 3;
	  			sscanf(buf +2, "%d %f", &Id, &val);	  			
	  			printf("id = %d cal = %3.1f\n", Id, val);
	  			// write sCal
	  			sendIt( Id, reset,(uint16_t)(int)(ceil(val*10.0)));	  			
			} else if(buf[0] == 'A' ){
				sscanf(buf +1, " %d %x", &Id, &reset);	  			
	  			switch( Id ){
					case 1: reg = ADR_1; break;
					case 2: reg = ADR_2; break;
					case 3: reg = ADR_3; break;
					case 4: reg = ADR_4; break;					
				}
				printf("reg %d cal = %d\n", reg,reset);
 	  			// write Adr 	  			
 	  			sendIt( 0,reg, reset );	  						
			} else if(buf[0] == 'L' ){
				sscanf(buf +1, " %d %d", &Id, &reset);	  			
	  			switch( Id ){
					case 1: reg = LOP_1; break;
					case 2: reg = LOP_2; break;
					case 3: reg = LOP_3; break;
					case 4: reg = LOP_4; break;					
				}
				printf("reg %d cal = %d\n", reg,reset);
 	  			// write log
 	  			sendIt( 0, reg, reset );
			} else if(buf[0] == 'h' ){
					userHelp();
					sleep( 10 );
			}	        
	    } 
	     
	    // read sCal regs
	    for( int ix = 0; ix < MAXLP; ++ix ){
			modbus_set_slave(ctx, ix);
	    
		 	rc = modbus_read_input_registers(ctx, 0, LASTS, tab_reg);
			if (rc == -1) {
				fprintf(stderr, "%s\n", modbus_strerror(errno)); 
				continue;
			} 	
			memcpy( sCal, tab_reg, LASTS << 1 );
					 
			// read data regs
			rc = modbus_read_registers(ctx, 0, LASTREG, tab_reg);
			if (rc == -1) {
				fprintf(stderr, "%s\n", modbus_strerror(errno)); 
				continue;
			}
			memcpy( data, tab_reg, LASTREG << 1 );
			// control registers
			if( ix == 0 ) {
			memcpy( data0, data, LASTREG << 1 );

			//printf("%s%s", BRIGHTWHITE,UNDERLINEON ); 				
				  //printf("  bme       LOOP        ADR%s%s\n", DEFCOLOR, UNDERLINEOFF );
				  //printf("    %d        %d         %x \n", 1, data0[LOP_1], data0[ADR_1]);
				  //printf("    %d        %d         %x \n", 2, data0[LOP_2], data0[ADR_2]);
				  //printf("    %d        %d         %x \n", 3, data0[LOP_3], data0[ADR_3]);
				  //printf("    %d        %d         %x \n", 4, data0[LOP_4], data0[ADR_4]);
				  //printf("log: %d\n", data0[8]);
				  continue;
			} 
			printf("%s%s%02d:%02d:%02d", BRIGHTWHITE,UNDERLINEON,
				data[HOUR], data[MINIT], data[SEC] ); 
			printf("  Cur       LOW        HI        AV       Scal        Raw  Bme:%d %s%s\n",
				 ix, DEFCOLOR, UNDERLINEOFF );
			if( !IsOnScan(ix) ) {
				printf("%s", DIMWHITE);
			}
				 
			printf("Temp : %6.1f    %6.1f    %6.1f    %6.1f     %6.1f     %6.1f  lop:%d\n",
				((float)data[TEMP])*0.1,
				((float)data[MTEMP])*0.1,
				((float)data[XTEMP])*0.1,
				((float)data[ATEMP])*0.1,
				((float)sCal[STEMP])*0.1,
				((float)data[RTEMP])*0.1,
				getLoop( ix ));
				
			printf("Hum  : %6.1f    %6.1f    %6.1f    %6.1f     %6.1f     %6.1f  adr:%x\n",
				((float)data[HUM])*0.1,
				((float)data[MHUM])*0.1,
				((float)data[XHUM])*0.1,
				((float)data[AHUM])*0.1,
				((float)sCal[SHUM])*0.1,
				((float)data[RHUM])*0.1,
				getAdr(ix));
			printf("Pres : %6.1f    %6.1f    %6.1f    %6.1f     %6.1f     %6.1f  log:%2d\n",
				((float)data[PRES])*0.1,
				((float)data[MPRES])*0.1,
				((float)data[XPRES])*0.1,
				((float)data[APRES])*0.1,
				((float)sCal[SPRES])*0.1,
				((float)data[RPRES])*0.1,
				( IsOnScan(ix) ? data0[8] : 0 ));
			}
				
			usleep(polRate);
	} 

	modbus_close(ctx);
	modbus_free(ctx);
	printf("\e8"); 	// restore current cursor

	printf("\n");		
}

void disregs( int rc, int start, uint16_t *regs) {
	// display as each reg as %4X 10 colum aray
	int col = 0;
	
	printf("      ");

	for(int i = 0; i < 10; ++i) {
		printf("| %5d", i );
	}	
		
	int ii = 0;
	col = 0;
	printf("|\n| %4d", ((ii+start) / 10) * 10 );
	for( int i = 0; i < start%10; ++i ) {
		printf("|  ... " );	++col;
	}
	float f = 0;
	while( 1 ) {
	
		if( col >= 10 ) {
			col = 0;			
			printf("|\n| %4d", ((ii+start) / 10) * 10 );
		}
		if( ii > 2 ) {
			f = ((float)(int16_t)regs[ii++]) * 0.1;
			printf("|%6.1f", f);		
		}else {
			printf("| %5d", regs[ii++]);
		} 
	
		if( ii >= rc ) break;
		++col;
	} 
	printf("|\n");  
} 

 
