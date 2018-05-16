#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include "argvTable.h"

#define PATH_SIZE 64
#define CMDLINE_SIZE 256
#define MAX_PROC 1024

struct ProcessInfo {
	int pid;
	int uid;
	int gid;
	int ppid;
	int pgid;
	int sid;
	int tty;
	char tty_u[16];
	char status;
	char cmdline[CMDLINE_SIZE];
};

int check_if_number(char *str);

void ps(struct ArgvTable *argvTable) {

	DIR *dp;
	struct dirent *dirp;

	if((dp = opendir("/proc")) == NULL) {
		fprintf(stderr, "cannot open /proc\n");
	}else {
		
		struct ProcessInfo procs[MAX_PROC];
		int proc_count = 0;
		FILE *fp;
		char path[PATH_SIZE];
		int pid, uid, gid, ppid, pgid, sid, tty;
		char status;
		char cmdline[CMDLINE_SIZE];	
		char dump[256];
		char whitespace[50];

		while((dirp = readdir(dp)) != NULL) {
			if(check_if_number(dirp->d_name)) {
				
				pid = 0, uid = 0, gid = 0, ppid = 0, pgid = 0, sid = 0, tty = 0;
				status = ' ';
				strcpy(path, "");
				strcpy(cmdline, "");
				strcpy(dump, "");
				strcpy(whitespace, "");

				// /proc/[pid]/stat
				strcpy(path, "/proc/");
				strcat(path, dirp->d_name);
				strcat(path, "/stat");
				fp = fopen(path, "r");
				if(fp != NULL) {
					fscanf(fp, "%d %s %c %d %d %d %d", &pid, cmdline, &status, &ppid, &pgid, &sid, &tty);
				}
				fclose(fp);
				
				// /proc/[pid]/state
				strcpy(path, "/proc/");
				strcat(path, dirp->d_name);
				strcat(path, "/status");
				fp = fopen(path, "r");
				if(fp != NULL) {
					for(int i=0;i<8;i++) {
						fgets(dump, 256, fp);
					}
					fgets(dump, 256, fp);
					sscanf(dump, "Uid:%s%d ", whitespace, &uid);
					fgets(dump, 256, fp);
					sscanf(dump, "Gid:%s%d ", whitespace, &gid);
				}
				fclose(fp);

				// /proc/[pid]/cmdline
				strcpy(dump, "");
				strcpy(path, "/proc/");
				strcat(path, dirp->d_name);
				strcat(path, "/cmdline");
				fp = fopen(path, "r");
				if(fp != NULL) {
					fgets(dump, 256, fp);
					strcat(cmdline, " ");
					strcat(cmdline, dump);
				}
				fclose(fp);

				if(proc_count < MAX_PROC) {
					struct ProcessInfo proc = {pid, uid, gid, ppid, pgid, sid, tty, "-", status, ""};
					strcpy(proc.cmdline, cmdline);
					procs[proc_count++] = proc;
				}
				

			}
		}
		closedir(dp);

		// find tty
		struct stat buf;
		for(int i=0;i<proc_count;i++) {
			if(procs[i].tty != 0) {
				if((dp = opendir("/dev")) != NULL) {
					while((dirp = readdir(dp)) != NULL) {
						strcpy(path, "/dev/");
						strcat(path, dirp->d_name);
						if(!stat(path, &buf)) {
							if(buf.st_rdev == procs[i].tty) {
								strcpy(procs[i].tty_u, dirp->d_name);
							}
						}
					}
				}
				closedir(dp);
				if((dp = opendir("/dev/pts")) != NULL) {
					while((dirp = readdir(dp)) != NULL) {
						strcpy(path, "/dev/pts/");
						strcat(path, dirp->d_name);
						if(!stat(path, &buf)) {
							if(buf.st_rdev == procs[i].tty) {
								strcpy(procs[i].tty_u, "pts/");
								strcat(procs[i].tty_u, dirp->d_name);
							}
						}
					}
				}
				closedir(dp);
			}
		}

		// sort the processes
		int order[proc_count];
		int proc_check[proc_count];
		int min = 99999, index = 0;

		for(int i=0;i<proc_count;i++) {
			order[i] = 0;
			proc_check[i] = 0;
		}

		if(argvTable->q == 0 && argvTable->r == 0 && argvTable->s == 0) {

			for(int i=0;i<proc_count;i++) {
				min = 99999, index = 0;
				for(int j=0;j<proc_count;j++) {
					if(!proc_check[j] && procs[j].pid < min) {
						index = j;
						min = procs[j].pid;
					}
				}
				proc_check[index] = 1;
				order[i] = index;
			}

		} else if(argvTable->p == 0 && argvTable->q == 1 && argvTable->r == 0 && argvTable->s == 0) {

			for(int i=0;i<proc_count;i++) {
				min = 99999, index = 0;
				for(int j=0;j<proc_count;j++) {
					if(!proc_check[j] && procs[j].ppid < min) {
						index = j;
						min = procs[j].ppid;
					}
				}
				proc_check[index] = 1;
				order[i] = index;
			}

		} else if(argvTable->p == 0 && argvTable->q == 0 && argvTable->r == 1 && argvTable->s == 0) {

			for(int i=0;i<proc_count;i++) {
				min = 99999, index = 0;
				for(int j=0;j<proc_count;j++) {
					if(!proc_check[j] && procs[j].pgid < min) {
						index = j;
						min = procs[j].pgid;
					}
				}
				proc_check[index] = 1;
				order[i] = index;
			}

		} else if(argvTable->p == 0 && argvTable->q == 0 && argvTable->r == 0 && argvTable->s == 1) {

			for(int i=0;i<proc_count;i++) {
				min = 99999, index = 0;
				for(int j=0;j<proc_count;j++) {
					if(!proc_check[j] && procs[j].sid < min) {
						index = j;
						min = procs[j].sid;
					}
				}
				proc_check[index] = 1;
				order[i] = index;
			}

		} 
		

		// print the processes
		printf("%6s %6s %6s %6s %6s %6s %10s %2s %s\n", "pid", "uid", "gid", "ppid", "pgid", "sid", "tty", "St", "(img) cmd");

		int pos = 0;
		if(argvTable->a == 0 && argvTable->x == 0) {
			int current_uid = getuid();
			for(int i=0;i<proc_count;i++) {
				pos = order[i];
				if(procs[pos].uid == current_uid && procs[pos].tty != 0) {
					printf("%6d %6d %6d %6d %6d %6d %10s  %c %s\n", procs[pos].pid, procs[pos].uid, procs[pos].gid, procs[pos].ppid, procs[pos].pgid, procs[pos].sid, procs[pos].tty_u, procs[pos].status, procs[pos].cmdline);
				}
			}

		} else if(argvTable->a == 0 && argvTable->x == 1) {
			
			int current_uid = getuid();
			for(int i=0;i<proc_count;i++) {
				pos = order[i];
				if(procs[pos].uid == current_uid) {
					printf("%6d %6d %6d %6d %6d %6d %10s  %c %s\n", procs[pos].pid, procs[pos].uid, procs[pos].gid, procs[pos].ppid, procs[pos].pgid, procs[pos].sid, procs[pos].tty_u, procs[pos].status, procs[pos].cmdline);
				}
			}

		} else if(argvTable->a == 1 && argvTable->x == 0) {
		
			for(int i=0;i<proc_count;i++) {
				pos = order[i];
				if(procs[pos].tty != 0) {
					printf("%6d %6d %6d %6d %6d %6d %10s  %c %s\n", procs[pos].pid, procs[pos].uid, procs[pos].gid, procs[pos].ppid, procs[pos].pgid, procs[pos].sid, procs[pos].tty_u, procs[pos].status, procs[pos].cmdline);
				}
			}

		} else if(argvTable->a == 1 && argvTable->x == 1) {
			
			for(int i=0;i<proc_count;i++) {
				pos = order[i];
				printf("%6d %6d %6d %6d %6d %6d %10s  %c %s\n", procs[pos].pid, procs[pos].uid, procs[pos].gid, procs[pos].ppid, procs[pos].pgid, procs[pos].sid, procs[pos].tty_u, procs[pos].status, procs[pos].cmdline);
			}

		}

	}

	return;
}

int check_if_number(char *str) {

	for(int i=0; str[i]!='\0'; i++) {
		if(!isdigit(str[i])) {
			return 0;
		}
	}
	return 1;
}
