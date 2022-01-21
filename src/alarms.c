

#include "m.h"
#include "alarms.h"

char *strArlmNames[] = { "??","temp", "Temp", "hum", "Hum", "pres", "Pres","ALR1", "Alr2"};
char *strArlmAct[] = {"Detect", "Ackd", "Cleared"};
char *strArlmType[] = {"??","Low", "Hi","Low", "Hi","Low", "Hi"};


int             IsInAlarm(int ix, int al);
  
void ackAllArm( void ){
uint16_t        dmask = DET;
uint16_t        amask = ACK;
uint16_t        curStat = 0;


  printf("ackAlarm() \n");
 for(int ix = 1; ix < MAXLP; ++ix ){
   dmask = DET;amask = ACK;
   curStat = b[ix].data[ALSTAT];
   for( int al = 0; al < MAXALRM; ++al ){     
     	if ((curStat & dmask) == dmask) 	// in alarm ?
          curStat |= amask;
     
     // printf("ix:%d Al:%d am:%x dm:%x cur:%x\n", ix, al, amask, dmask, curStat);
      amask <<= 2;
      dmask <<= 2;
      //if(al & 0x01)
        //abmask <<= 1;
      //} 
    }
    b[ix].data[ALSTAT] = curStat;   

  }
}


//void logAlrm( int ix, int AlNum, int alrmType, int alrmAct ){
void logAlrm( int ix, int AlNum,  int alrmAct ){
  //              ix, HI, Det
  int16_t val;
  int16_t alarmPoint;
   switch (AlNum) {		// alarm #
    case 1:			// lo temp
	val = b[ix].data[TEMP];
	alarmPoint = b[ix].sCal[ASP_TLO];
	break;
    case 2:			// hi temp
	val = b[ix].data[TEMP];
	alarmPoint = b[ix].sCal[ASP_THI];
	break;
    case 3:			// lo hum
	val = b[ix].data[HUM];
	alarmPoint = b[ix].sCal[ASP_HLO];
	break;
    case 4:			// hi hum
	val = b[ix].data[HUM];
	alarmPoint = b[ix].sCal[ASP_HHI];
	break;
    case 5:			// lo pres
	val = b[ix].data[PRES];
	alarmPoint = b[ix].sCal[ASP_PLO];
	break;
    case 6:			// hi pres
	val = b[ix].data[PRES];
	alarmPoint = b[ix].sCal[ASP_PHI];
	break;
    }
    
    
        syslog(LOG_NOTICE,  "Sensor:%d Alarm: %-4s %-3s %-7s %02d/%02d/%04d %02d:%02d:%02d  cur:%5.1f ap:%5.1f\n",
	 ix,  strArlmNames[AlNum], strArlmType[AlNum],strArlmAct[alrmAct],
	  tm.tm_mon+1, tm.tm_mday,tm.tm_year+1900, 
	  tm.tm_hour, tm.tm_hour, tm.tm_sec,
	  (float)val*0.1, (float)alarmPoint* 0.1
	  );
	

 printf( "Sensor:%d Alarm: %-4s %-3s %-7s %02d/%02d/%04d %02d:%02d:%02d  cur:%5.1f ap:%5.1f\n",
	 ix,  strArlmNames[AlNum], strArlmType[AlNum],strArlmAct[alrmAct],
	  tm.tm_mon+1, tm.tm_mday,tm.tm_year+1900, 
	  tm.tm_hour, tm.tm_hour, tm.tm_sec,
	  (float)val*0.1, (float)alarmPoint* 0.1
	  );
	    
}

void
checkAlarms(int ix)
{
uint16_t        abmask = AEMASK;  // alarm enabled mask
uint16_t        dmask = DET;
uint16_t        amask = ACK;
uint16_t        curStat = 0;

    curStat = b[ix].data[ALSTAT];

    if (!IsOnScan(ix)) {
	b[ix].data[ALSTAT] &= AEBITS;
	return;
    }

    for (int al = 1; al < MAXALRM; ++al) {
  //    	 printf("checkAlarms- al:%d  ix: %d abmask:%x curStat:%X  %x mask:%x \n", al, ix, abmask, curStat, (curStat & (amask | dmask)), (amask | dmask));

	if (!(curStat & abmask))
	  break;		// alarms disabled

	// printf("checkAlarms> al:%d  ix: %d curStat: %X  %x mask:%x \n", al, ix, curStat, (curStat & (amask | dmask)), (amask | dmask));
	if ((curStat & (amask | dmask)) == (amask | dmask)) {	// in alarm ?
	    if (!IsInAlarm(ix, al)) {
		    curStat &= ~(amask | dmask);	// clear alarm
		   // printf("Clear curStat: %x\n",curStat);
		    logAlrm(  ix, al, ALRM_CLR );
	    }

	} else if( (curStat & (amask | dmask)) == 0) {
	  if (IsInAlarm(ix, al)){
      if(curStat & 0x08000) {  // check auto-ack
          curStat |= (amask | dmask);	// set alarm bits
      } else {
        curStat |= dmask;	// set alarm bits
      }
        //printf("set curStat: %x\n",curStat);
		  logAlrm(  ix, al, ALRM_DET );
	  }
	}
	//printf("checkAlarms< al:%d  ix: %d curStat: %X  %x mask:%x \n", al, ix, curStat, (curStat & (amask | dmask)), (amask | dmask));

	amask <<= 2;
	dmask <<= 2;
	if(al & 0x01)
	  abmask <<= 1;
    }
    b[ix].data[ALSTAT] = curStat;
}

int
IsInAlarm(int ix, int al)
{
    int16_t         val = 0;
    int16_t         aval = 0;
    uint16_t        IsIn = 0;

    switch (al) {		// alarm #
    case 1:			// lo temp
	val = b[ix].data[TEMP];
	aval = b[ix].sCal[ASP_TLO];
	IsIn = (val <= aval);
	break;
    case 2:			// hi temp
	val = b[ix].data[TEMP];
	aval = b[ix].sCal[ASP_THI];
	IsIn = (val >= aval);
	break;
    case 3:			// lo hum
	val = b[ix].data[HUM];
	aval = b[ix].sCal[ASP_HLO];
	IsIn = (val <= aval);
	break;
    case 4:			// hi hum
	val = b[ix].data[HUM];
	aval = b[ix].sCal[ASP_HHI];
	IsIn = (val >= aval);
	break;
    case 5:			// lo pres
	val = b[ix].data[PRES];
	aval = b[ix].sCal[ASP_PLO];
	IsIn = (val <= aval);
	break;
    case 6:			// hi pres
	val = b[ix].data[PRES];
	aval = b[ix].sCal[ASP_PHI];
	IsIn = (val >= aval);
	break;
    }
    return IsIn;
}
