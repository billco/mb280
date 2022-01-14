

#include "m.h"

void resetall(void);

int file_exists(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    int is_exist = 0;
    if (fp != NULL) {
	is_exist = 1;
	fclose(fp);		// close the file
    }
    return is_exist;
}

void archist(void)
{
    /* ren cur history file to doy hist x .csv
     */
    char buf[512] = { 0 };
    char buf1[512] = { 0 };
    for (int x = 1; x < MAXLP; ++x) {
	sprintf(buf, "./data/history%d.csv", x);
	if (file_exists(buf)) {
	    sprintf(buf1, "./data/arc/%02d%02d%02dhist%d.csv", tm.tm_mon,
		    tm.tm_mday, tm.tm_year, x);
	    rename(buf, buf1);
	}
    }
}

void readDefualts()
{
    char buf[512] = { 0 };


    FILE *fd = fopen("bme280new.cfg", "r");

    if (fd == NULL) {
	printf("error opening bme280.cfg\n");
	return;
    }
    do {
	fgets(buf, 511, fd);	// dummy read of header
	//printf( "-- [%s]\n", buf );

    } while (buf[0] == '#');


    //printf( "[%s]\n", buf );
    sscanf(buf, "%s %d %d %d\n", host, &port, &slaveId, &debug);
    printf("host %s %d %d %d\n", host, port, slaveId, debug);
    for (int ix = 1; ix < MAXLP; ++ix) {
	fgets(buf, 511, fd);
	//printf( "[%s]\n", buf );
	//  0 0 0 0 0 f7 bedrom
	sscanf(buf, "%d %x ", &b[ix].loop, &b[ix].adr);
	printf("[%d]: lp:%d adr:%x \n", ix, b[ix].loop, b[ix].adr);
    }
    fclose(fd);

}


void WriteDefualts()
{
    FILE *fd = fopen("bme280new.cfg", "w");

    fprintf(fd, "# host, port, debug\n");

    fprintf(fd, "%s %d %d \n", host, port, debug);
    for (int ix = 1; ix < MAXLP; ++ix) {
	fprintf(fd, "%d %x\n", b[ix].loop, b[ix].adr);
	printf("[%d]: %d %x \n", ix, b[ix].loop, b[ix].adr);
    }

    fclose(fd);

}

void WriteStat()
{
    FILE *fd = fopen("bme280stat.cfg", "wb");
    for (int ix = 0; ix < MAXLP; ++ix) {
	fwrite(&b[ix].data, sizeof(b[ix].data), 1, fd);
	fwrite(&b[ix].sCal, sizeof(b[ix].sCal), 1, fd);
	//for(int i = 0; i < LASTREG; ++i ){
	//printf("data[%d][%d] = %d\n", ix, i, b[ix].data[i] );
	//}
    }
    syslog(LOG_NOTICE, "Saving stat...");

    fclose(fd);

//for(int i = 0; i < LASTS; ++i ){
    //printf("sCal[%d] = %d\n", i, sCal[i] );
//}
}

void ReadStat()
{
    FILE *fd = fopen("bme280stat.cfg", "rb");

    if (fd == NULL) {
	retsetAll();
	syslog(LOG_NOTICE, "Error Reading stat...");
	return;
    }
    for (int ix = 0; ix < MAXLP; ++ix) {
	fread(&b[ix].data, sizeof(b[ix].data), 1, fd);
	fread(&b[ix].sCal, sizeof(b[ix].sCal), 1, fd);
	//for(int i = 0; i < LASTREG; ++i ){
	//printf("data[%d][%d] = %d\n", ix, i, b[ix].data[i] );
	//}
    }

    syslog(LOG_NOTICE, "Reading stat...");
//for(int i = 0; i < LASTREG; ++i ){
    //printf("data[%d] = %d\n", i, data[i] );
//}for(int i = 0; i < LASTS; ++i ){
    //printf("sCal[%d] = %d\n", i, sCal[i] );
//}

    fclose(fd);
}

void writeLog()
{
    char fn[20];

    for (int ix = 0; ix < MAXLP; ++ix) {
	//printf("ix:%d, adr: %x\n", ix, b[ix].adr);

	if (!IsOnScan(ix))
	    continue;


	sprintf(fn, "./data/history%d.csv", ix);

	FILE *fd = fopen(fn, "a");
	if (fd != NULL) {
	    fprintf(fd,
		    "%02d/%02d/%04d %02d:%02d:%02d,%5.1f,%5.1f,%5.1f\n",
		    tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900,
		    tm.tm_hour, tm.tm_min, tm.tm_sec,
		    ((float) b[ix].data[TEMP]) * 0.1,
		    ((float) b[ix].data[HUM]) * 0.1,
		    ((float) b[ix].data[PRES]) * 0.1);
	    fclose(fd);
	    syslog(LOG_NOTICE, "Logging...%s", fn);
	} else {
	    syslog(LOG_NOTICE, "Logging Error %s %s", fn, strerror(errno));
	}
    }
}
