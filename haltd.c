/*
 * haltd.c
 * Accects tcp!5000 connections, allowing the
 * usesr to halt or reboot the system
 *
 * the process has to run with priviliges to run /bin/fshalt
 *
 * 2019-01-12 jam
 */

#include <u.h>
#include <libc.h>

int dfd, lcfd;
char buff[256];

const char *Logfile = "haltd";		/* log to /var/log/syslog/haltd */

/* Echo a message to the client */
void
echomsg(char *str)
{
	write(dfd, str, strlen(str));
}


/* terminate a string by first CR or LF  */
void
stripnl(char *s)
{
	while(*s++){
		if( *s == '\r' || *s == '\n'){
			*s = '\0';
			break;
		}
	}
}

int
execcommand(char *cmd)
{
	if(!strcmp(cmd, "halt")){
		syslog(0, Logfile, "Shutdown requested");
		echomsg("Halting the system...\r\n");
		hangup(lcfd);
		close(lcfd);	
		execl("/bin/fshalt", "/bin/fshalt", nil);
	}
	else if(!strcmp(cmd, "reboot")){
		syslog(0, Logfile, "Reboot requested");
		echomsg("Rebooting the system...\r\n");
		hangup(lcfd);
		close(lcfd);	
		execl("/bin/fshalt", "/bin/fshalt", "-r", nil);
	}
	else if(!strcmp(cmd, "help")){
//		fprint(2, "[haltd] Help requested\n");
		echomsg("This is the halt daemon. It can shutdown the "
			"system safely. The commands are:\r\n"
			"halt    Sync disks and halt\r\n"
			"reboot  Sync disks and reboot\r\n"
			"help    Show this help message\r\n"
			"quit    Disconnect from this service\r\n");		
	}
	else if(!strcmp(cmd, "quit"))
		return 2;
	else {
		syslog(0, Logfile, "Invalid command: [%s]", cmd);
		sleep(1);	/* ensure print before the client disconnects */ 
		echomsg("Invalid command. Try [help|halt|quit|reboot]\r\n");
	}	
	return 0;
}


void
main(void)
{
	int acfd;
	char adir[40], ldir[40];
	int n;

	acfd = announce("tcp!*!5000", adir);
	if(acfd < 0)
		exits("");
	for(;;){
		/* listen for a call */

		lcfd = listen(adir, ldir);
		if(lcfd < 0)
			exits("");
	

		/* fork a process for the connection */

		switch(fork()){
		case -1:
			perror("forking");
			close(lcfd);
			break;
		case 0:
			/* accept the call and open the data file */
			dfd = accept(lcfd, ldir);
			if(dfd < 0)
				exits("");
			syslog(0, Logfile, "Incomming connection established");
			echomsg("Welcome to haltd. Commands: [help|halt|reboot|quit]\r\n");

			/* echo until EOF */

			echomsg("haltd> ");		/* initial prompt */
	
			while((n = read(dfd, buff, sizeof(buff)-1)) > 0){
				int res;

				buff[n] = '\0';		/* ensure the string is terminated. */
				stripnl(buff);		/* take a way NL and CR */
				res = execcommand(buff);
				if(res == 2 )		/* user chose 'quit' in the menu */
					break;
				echomsg("haltd> ");	/* show next prompt */
			}
			exits(0);
		default:
			close(lcfd);
			break;
		}
	}
}
