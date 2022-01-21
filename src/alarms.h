// alarms.h

/*
 * |========= A L A R A M - S T A T U S ===========|
 * | | ENABLED| PRES| PRES| HUM | HUM | TEMP| TEMP |  
 * | | | HIGH| LOW | HIGH| LOW | High| LOW  | TYPE 
 * |-----------------------------------------------|
 * |16|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| BIT
 * | 8| 4| 2| 1| 8| 4| 2| 1| 8| 4| 2| 1| 8| 4| 2| 1| Mask
 * |-----------------------------------------------|
 * | A| P| H| T| D| A| D| A| D| A| D| A| D| A| D| A|
 * | A| R| U| E| E| C| E| C| E| C| E| C| E| C| E| C|
 * | C| E| M| M| T| K| T| K| T| K| T| K| T| K| T| K|
 * | K| S|  | P|  |  |  |  |  |  |  |  |  |  |  |  |
 * 
 * 
 */
#define ACK     0x00000001
#define DET     0x00000002
#define ACKDET  0x00000003
#define AABLED  0x0000F000	// All alarms enabled

#define MAXALRM 6

// bits mask
#define ATBITS   0x000000F
#define ATLBITS  0x0000003
#define ATHBITS  0x000000C
#define AHBITS   0x00000F0
#define AHLBITS  0x0000030
#define AHHBITS  0x00000C0
#define APBITS   0x0000F00
#define APLBITS  0x0000300
#define APHBITS  0x0000C00
#define AEBITS   0x000F000
#define AEMASK   0x0001000
#define ACKAMASK 0x0000111
#define DETAMASK 0x0000222

#define ALRM_DET 0
#define ALRM_ACK 1
#define ALRM_CLR 2

#define IsLoTempAct(x)     ((x & ATBITS) & ATLBITS)	// is low Temp
							// Alarm active
#define IsHiTempAct(x)      ((x & ATBITS) & ATHBITS)	// is low Temp
							// Alarm active
#define IsLoHumAct(x)      ((x & AHBITS) & AHLBITS)	// is low Hum
							// Alarm active
#define IsHiHumAct(x)       ((x & AHBITS) & AHHBITS)	// is low HUM
							// Alarm active
#define IsLoPresAct(x)     ((x & APBITS) & APLBITS)	// is low Pres
							// Alarm active
#define IsHiPresAct(x)     ((x & APBITS) & APHBITS)	// is low Pres
							// Alarm active

#define strIsLoTempAct(x)   ((char*)IsLoTempAct(x) ? "Act" : "Ok ")
#define strIsHiTempAct(x)   ((char*)IsHiTempAct(x) ? "Act" : "Ok ")

#define strIsLoHumAct(x)    ((char*)IsLoHumAct(x) ? "Act" : "Ok ")
#define strIsHiHumAct(x)    ((char*)IsHiHumAct(x) ? "Act" : "Ok ")

#define strIsLoPresAct(x)  ((char*)IsLoPresAct(x) ? "Act" : "Ok ")
#define strIsHiPresAct(x)   ((char*)IsHiPresAct(x) ? "Act" : "Ok ")

