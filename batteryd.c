#include <u.h>
#include <libc.h>

const char *Batteryfile = "/mnt/acpi/battery";
const char *Logfile = "haltd";		/* log to /var/log/syslog/haltd */
const int Batterycheckinterval = 5;	/* seconds between battery checks */
const int Batterylowlimit = 5;		/* Battery charge (mA) to chut down */

char Buf[20];	/* Read data from Batteryfile here */

/* read the number of mA left in the battery */
int
readnum(int fd)
{
	int r;
	seek(fd, 0, 0);
	r = read(fd, Buf, sizeof(Buf));
	Buf[sizeof(Buf)-1] = '\0'; /* terminate for mem safety */

	if(r > 0){
		return atoi(Buf);	
	}
	else return -1;
}


void
main(void)
{
	int fd;

	fd = open(Batteryfile, OREAD);
	if(fd < 0){
		syslog(1, Logfile,"Cannot open %s, qutting", Batteryfile);
		exits("file error");
	}
	for(;;){
		int b;

		b = readnum(fd);
		if(b <= Batterylowlimit)
		{
			syslog(1, Logfile, "Battery too low, shutting down!");
			execl("/bin/fshalt", "/bin/fshalt", nil);
		}

		sleep(Batterycheckinterval * 1000);
	}
}
