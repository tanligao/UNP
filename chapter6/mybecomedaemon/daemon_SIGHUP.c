#include <signal.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include "become_daemon.h"

#define oops(str,err_code) \
		fprintf(stderr,str); \
		exit(err_code);

static const char *LOG_FILE = "/tmp/ds.log";
static const char *CONFIG_FILE = "/tmp/ds.conf";

// log文件
static FILE *logfp;

static void logMessage(const char *format,...)
{
	va_list argList;
	const char *TIMESTAMP_FMT = "%F %X";
#define TS_BUF_SIZE sizeof("YYYY-MM-DD HH:MM:SS")
	char timestamp[TS_BUF_SIZE];
	time_t tt;
	struct tm *localtm;
	tt = time(NULL);
	localtm = localtime(&tt);
	if(localtm == NULL || strftime(timestamp,TS_BUF_SIZE,TIMESTAMP_FMT,localtm)
		== 0)
		fprintf(logfp,"???Unknown time???:");
	else
		fprintf(logfp,"%s: ",timestamp);
	va_start(argList,format);
	vfprintf(logfp,format,argList);
	fprintf(logfp,"\n");
	va_end(argList);
}

static void logOpen(const char *filename)
{
	mode_t m;
	m = umask(077);
	logfp = fopen(filename,"a");
	umask(m);
	if(logfp == NULL)
		exit(EXIT_FAILURE);
	setbuf(logfp,NULL);
	
	logMessage("Opened log file");
}

static void logClose()
{
	logMessage("Close log file");
	fclose(logfp);
}

static void readConfigFile(const char *configFilename)
{
	FILE *configfp;
#define SBUF_SIZE 100
	char str[SBUF_SIZE];
	configfp = fopen(configFilename,"r");
	if(configfp != NULL)
	{
		if(fgets(str,SBUF_SIZE,configfp) == NULL)
			str[0] = '\0';
		else
			str[strlen(str) - 1] = '\0';
		logMessage("Read config file: %s", str);
		fclose(configfp);
	}
}

static volatile sig_atomic_t hupReceived = 0;

static void sighupHandler(int sig)
{
	hupReceived = 1;
}

int main(int argc,char *argv[])
{
	const int SLEEP_TIME = 15;
	int count = 0;
	int unslept;
	/*
	struct sigaction sa;	

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = sighupHandler;
	if(sigaction(SIGHUP,&sa,NULL) == -1)
		oops("sigaction error",2);	
	*/
	becomeDaemon(0);
	logOpen(LOG_FILE);
	readConfigFile(CONFIG_FILE);
	unslept = SLEEP_TIME;
	
	for( ; ;)
	{
		unslept = sleep(unslept);
		if(hupReceived)
		{
			logClose();
			logOpen(LOG_FILE);
			readConfigFile(CONFIG_FILE);
			hupReceived = 0;
		}
	
		if(unslept ==0)
		{
			++count;
			logMessage("Main: %d",count);
			unslept = SLEEP_TIME;
		}
	}
}

