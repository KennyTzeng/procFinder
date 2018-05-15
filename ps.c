#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
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
	char tty_u[32];
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
					struct ProcessInfo proc = {pid, uid, gid, ppid, pgid, sid, tty, "?", status, ""};
					strcpy(proc.cmdline, cmdline);
					procs[proc_count++] = proc;
				}
				

			}
		}
		// print the processes	
		
		if(argvTable->a == 0 && argvTable->x == 0) {
			
			int current_uid = getuid();
			for(int i=0;i<proc_count;i++) {
				if(procs[i].uid == current_uid && procs[i].tty != 0) {
					printf("%6d %6d %6d %6d %6d %6d %6d %c %s\n", procs[i].pid, procs[i].uid, procs[i].gid, procs[i].ppid, procs[i].pgid, procs[i].sid, procs[i].tty, procs[i].status, procs[i].cmdline);
				}
			}

		} else if(argvTable->a == 0 && argvTable->x == 1) {
			
			int current_uid = getuid();
			for(int i=0;i<proc_count;i++) {
				if(procs[i].uid == current_uid) {
					printf("%6d %6d %6d %6d %6d %6d %6d %c %s\n", procs[i].pid, procs[i].uid, procs[i].gid, procs[i].ppid, procs[i].pgid, procs[i].sid, procs[i].tty, procs[i].status, procs[i].cmdline);
				}
			}

		} else if(argvTable->a == 1 && argvTable->x == 0) {
		
			for(int i=0;i<proc_count;i++) {
				if(procs[i].tty != 0) {
					printf("%6d %6d %6d %6d %6d %6d %6d %c %s\n", procs[i].pid, procs[i].uid, procs[i].gid, procs[i].ppid, procs[i].pgid, procs[i].sid, procs[i].tty, procs[i].status, procs[i].cmdline);
				}
			}

		} else if(argvTable->a == 1 && argvTable->x == 1) {
			
			for(int i=0;i<proc_count;i++) {
				printf("%6d %6d %6d %6d %6d %6d %6d %c %s\n", procs[i].pid, procs[i].uid, procs[i].gid, procs[i].ppid, procs[i].pgid, procs[i].sid, procs[i].tty, procs[i].status, procs[i].cmdline);
			}

		}

	}

	return 0;
}

int check_if_number(char *str) {

	for(int i=0; str[i]!='\0'; i++) {
		if(!isdigit(str[i])) {
			return 0;
		}
	}
	return 1;
}
