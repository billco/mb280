/*
  common modbus register layout
  */
  
// holding registers
#define HOUR	  0
#define MINIT	  1
#define SEC		  2
#define TEMP	  3
#define HUM		  4
#define PRES	  5
#define RTEMP	  6
#define RHUM	  7
#define RPRES	  8
#define MTEMP	  9
#define XTEMP	 10
#define ATEMP	 11
#define MHUM	 12
#define XHUM	 13
#define AHUM	 14
#define MPRES	 15
#define XPRES	 16
#define APRES	 17
#define ALSTAT	 18
#define SPAREH	 19
#define LASTREG	 (SPAREH+1)

 // input registers 

//slave Id 0  regs
#define LOP_1	  0
#define ADR_1	  1
#define LOP_2	  2
#define ADR_2	  3
#define LOP_3	  4
#define ADR_3	  5
#define LOP_4	  6
#define ADR_4	  7
#define SLOG	  8
#define ACKALRM	  9
#define SPARE_3	 10
#define SPARE_4	 11
#define SPARE_5  12
#define SPARE_6  13
#define SPARE_7  14
#define SPARE_8	 15
#define SPARE_9	 16
#define SPARE_10 17
#define SPARE_11 18
#define SPARE_12 19
#define LASTREG0	 (SPARE_12+1)

//slave Id 1 - 4 input regs
#define SRESET	 0
#define STEMP	 1
#define SHUM	 2
#define SPRES	 3
#define SPAREC	 4
#define ASP_TLO		5	// low temp alarm set point
#define ASP_THI		6	// High temp alarm set point
#define ASP_HLO		7	// low hum alarm set point
#define ASP_HHI		8	// high hum alarm set point
#define ASP_PLO		9	// low pres alarm set point
#define ASP_PHI		10	// high pres alarm set point
#define LASTS  (ASP_PHI+1)

#define OFFSET		6

#define SLAVEID 	(0+OFFSET)	// slaveId offset into query
#define COMMAND 	(1+OFFSET)	// command offset into query
#define REGADR		(3+OFFSET)	// command offset into query
#define REGVAL		(5+OFFSET)	// command offset into query

#define FUNC6	6		//  modbus funtion write input register

//#define MAXLP   4

#define NB_CONNECTION    5
