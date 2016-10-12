#include "defn.h"


/*1st argument -cnt*/

void cnt(char* directory,int* datatypenum){

    DIR *dirp;
    struct dirent *direntp;
    char *newdir=NULL;
    
    /* Open the given directory and search the included files */

    if((dirp=opendir(directory))==NULL)
        fprintf(stderr,"Error opening directory %s \n",directory);
    else{
        while((direntp=readdir(dirp))!=NULL){
        
        /* Increase the matrix variables depending upon the type */

            if(direntp->d_type==DT_REG)
                datatypenum[0]++;
            else if(direntp->d_type==DT_DIR){
            
                /* If the current is a directory call yourself for the new directory */

                if(strcmp(direntp->d_name,".") && strcmp(direntp->d_name,"..")){
                    datatypenum[1]++;
                    int count=strlen(directory);
                    newdir=malloc((count+strlen(direntp->d_name)+2)*sizeof(char));
                    strcpy(newdir,directory);
                    strcpy(newdir+count,"/");
                    strcpy(newdir+count+1,direntp->d_name);
                    cnt(newdir,datatypenum);

                }

            }
            else if(direntp->d_type==DT_FIFO)
                datatypenum[2]++;
            else if(direntp->d_type==DT_SOCK)
                datatypenum[3]++;
            else if(direntp->d_type==DT_CHR)
                datatypenum[4]++;
            else if(direntp->d_type==DT_BLK)
                datatypenum[5]++;
            else if(direntp->d_type==DT_LNK)
                datatypenum[6]++;
            else
                datatypenum[7]++;

        }
        if(newdir!=NULL){
            free(newdir);
            newdir=NULL;

        }
        closedir(dirp);

    }
}

/*2nd argument -lr/lw*/

void lrlw(char **argv, int n) {

    int i = 0, br = 0;
    exec_mode use;
    char *toprint = NULL;
    char *timet;
    char *dirt;
    time_t back, curtime = time(NULL);

    /*Arguments parsing*/

    for (i = 1; i < n; i++)
        if (!strcmp("-type", argv[i])) {
            toprint = malloc(sizeof(char) * (strlen(argv[i+1]) + 1));
            strcpy(toprint, argv[i+1]);
            assert(toprint != NULL);
            break;
        }

    if (i == n) i = 1;
    else i = 3;

    if (!strcmp(argv[i], "-lr")) use = ACC;
    else if (!strcmp(argv[i], "-lw")) use = MOD;
    else {
        printf("Wrong argument\n");
        return;
    }

    timet = malloc(sizeof(char) * (strlen(argv[i+1]) + 1));
    assert(timet != NULL);
    strcpy(timet, argv[i+1]);

    dirt = malloc(sizeof(char) * (strlen(argv[i+2]) + 1));
    assert(dirt != NULL);
    strcpy(dirt, argv[i+2]);

    /*Time parsing*/

    back = curtime - strtime(timet);

    /*Recursive function call*/

    lrlw_rec(back, dirt, use, toprint);
    free(timet);
    free(dirt);
    if (toprint != NULL) free(toprint);
    toprint = NULL;
    timet = NULL;
    dirt = NULL;
}


void lrlw_rec(time_t ttime, char *path, exec_mode use, char *toprint) {

    DIR *dirp;
    struct dirent *direntp;
    struct stat statbuf;
    struct tm lt;
    char *npath;
    int count, i;

    if((dirp=opendir(path))==NULL) {
        fprintf(stderr,"Error opening directory %s \n",path);
        return;
    }
    else {
        while((direntp = readdir(dirp)) != NULL){
            if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, "..")) continue;
            count = strlen(path) + strlen(direntp->d_name) + 2;
            npath = malloc(count * sizeof(char));
            strcpy(npath,path);
            if (npath[strlen(npath)-1] != '/')
                strcat(npath, "/");
            strcat(npath, direntp->d_name);

            if (stat(npath, &statbuf) == -1) {
                perror("Failed to get file status");
                free(npath);
                npath = NULL;
                continue;
            }

            /*Depending on the file type and the argument given (-lr/-lw), print the filesystem entity name using pprint function*/
            if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
                pprint(npath, 'd', toprint, ttime, (use == ACC) ? statbuf.st_atime : statbuf.st_mtime);
                lrlw_rec(ttime, npath, use, toprint);
            } else {
                if ((statbuf.st_mode & S_IFMT) == S_IFREG) {
                    pprint(npath, 'f', toprint, ttime, (use == ACC) ? statbuf.st_atime : statbuf.st_mtime);
                } else if ((statbuf.st_mode & S_IFMT) == S_IFBLK) {
                    pprint(npath, 'b', toprint, ttime, (use == ACC) ? statbuf.st_atime : statbuf.st_mtime);
                } else if ((statbuf.st_mode & S_IFMT) == S_IFCHR) {
                    pprint(npath, 'c', toprint, ttime, (use == ACC) ? statbuf.st_atime : statbuf.st_mtime);
                } else if ((statbuf.st_mode & S_IFMT) == S_IFIFO) {
                    pprint(npath, 'p', toprint, ttime, (use == ACC) ? statbuf.st_atime : statbuf.st_mtime);
                } else if ((statbuf.st_mode & S_IFMT) == S_IFLNK) {
                    pprint(npath, 'l', toprint, ttime, (use == ACC) ? statbuf.st_atime : statbuf.st_mtime);
                } else if ((statbuf.st_mode & S_IFMT) == S_IFSOCK) {
                    pprint(npath, 's', toprint, ttime, (use == ACC) ? statbuf.st_atime : statbuf.st_mtime);
                }
            }
            free(npath);
            npath = NULL;

        }
        closedir(dirp);
    }
}

time_t strtime(char *ttime) {

    /*Parse given time string argument and convert it*/

    int num = 0, i = 1;

    while (isdigit(ttime[i])) {
        num = num * 10 + ttime[i] - '0';
        i++;
    }

    switch (ttime[i]) {

        case 's': num = num; break;
        case 'm': num = num * 60; break;
        case 'h': num = num * 60 * 60; break;
        case 'd': num = num * 60 * 60 * 24; break;

    }
    return (time_t)num;
}

void pprint(char *path, char t, char *toprint, time_t ttime, time_t statime) {

    /*If -type argument ain't given, simply print full path name, else check if the filesystem object type
     *exists on the types to be printed. If yes, print it*/

    int i = 0;
    if (toprint == NULL) {
        if (statime > ttime) printf("%s\n", path);
        return;
    }

    for (i = 0; i < strlen(toprint); i++) {
        if (toprint[i] == t) {
            if (statime > ttime) printf("%s\n", path);
        }
    }

}

/*3rd argument -ps*/

void ps(){

    int ttyid=getppid();
    DIR *dirp;
    struct dirent *direntp;
    int readflag=0;
    char readbuffer[1024];
    char cmdline[512];
    int pid;
    int ppid;
    char state;
    long int totalmem=0;
    float totalcpu=0;
    int* pidarr=NULL;
    int numofpids=0;
    float uptime;
    long unsigned int utime,stime,vsize;
    long int cutime,cstime,priority,rss;
    long long unsigned int starttime;
    long unsigned int cputicks;
    struct tm *ttime;
    long currtime;
    int pgs=getpagesize();

    if((dirp=opendir("/proc"))==NULL)
        fprintf(stderr,"Error opening proc");
    else{
        FILE *fp;
        printf("PID\tPPID\tCPU\tSTATE\tSTART\tVSZ\tRSS\tPRIORITY\tCMDLINE\n");

        while((direntp=readdir(dirp))!=NULL){

            /* Begin searching after init */

            if(strcmp(direntp->d_name,"1")==0)
                readflag=1;
            if(readflag==1){

                /* Open stat file and get the first 3 parameters */

                sprintf(readbuffer,"/proc/%s/stat",direntp->d_name);
                fp = fopen (readbuffer,"r");
                if (fp==NULL) {
                    printf("Cannot open stat file\n");
                    return;
                }
                fscanf(fp,"%d %*s %c %d",&pid,&state,&ppid);

                /* If the current process is of the same tty(pidcheck is for forked,execed processes etc)*/

                if(ppid==ttyid || pidcheck(pidarr,ppid,numofpids) ){

                    /* Read the rest of the parameters and print the results */

                    pidarr=(int*)realloc(pidarr,(numofpids+1)*sizeof(int));
                    pidarr[numofpids]=pid;
                    numofpids++;

                    int i;
                    for(i=0;i<9;i++)
                        fscanf(fp,"%*d");
                    fscanf(fp,"%lu %lu %ld %ld %ld",&utime,&stime,&cutime,&cstime,&priority);
                    cputicks=utime+stime+cutime+cstime;
                    fscanf(fp,"%*d %*d %*d %llu %lu %ld",&starttime,&vsize,&rss);

                    FILE *uptp;
                    uptp=fopen("/proc/uptime","r");
                    if (uptp==NULL) {
                        printf("Cannot open uptime file\n");
                        return;
                    }
                    fscanf(uptp,"%f",&uptime);

                    FILE *cmlp;
                    sprintf(readbuffer,"/proc/%s/cmdline",direntp->d_name);
                    cmlp=fopen(readbuffer,"r");
                    if (cmlp==NULL) {
                        printf("Cannot open cmdline file\n");
                        return;
                    }
                    fscanf(cmlp,"%s",cmdline);
                    
                    currtime=time(NULL);
                    time_t psectime = (float)currtime - uptime + (float)starttime/sysconf(_SC_CLK_TCK);
                    ttime = localtime(&psectime);
                    int starthour = ttime->tm_hour;
                    int startmin = ttime->tm_min;

                    printf("%d\t%d\t%.2f\t%c\t%d:%d\t%lu\t%ld\t%ld\t%s\n",pid,ppid,(float)cputicks/CLOCKS_PER_SEC,state,starthour,startmin,vsize/1024,rss*pgs/1024,priority,cmdline);

                    totalmem+=vsize/1024;
                    totalcpu+=(float)cputicks/CLOCKS_PER_SEC;
                }
            }
        }
        printf("\nTotal memory usage Kb:%ld \n",totalmem);
        printf("Total cpu time sec's:%f \n",totalcpu);

        if(numofpids>0)
            free(pidarr);

        closedir(dirp);
    }

}

int pidcheck(int* pidarray,int ppid,int numofpids){
    
    /* Checks if the given ppid is in the pidarray */
    
    int i;
    for(i=0;i<numofpids;i++){
        if(ppid==pidarray[i])
            return 1;
    }
    return 0;

}

/*4th argument -ft*/

void ft() {
    FILE *fp;
    DIR *dirp, *dirpp;
    struct dirent *direntp, *direntpp;
    struct stat st;
    struct passwd *pw;
    struct group *gr;
    uid_t uid = getuid();
    pid_t tty_id = getppid();
    pid_t proc_id, pproc_id;
    long ne;
    char *path, *pidz, *fdpath;
    char line[512], filepath[1024];
    int count = 0, spaces = 0, i, ch, k, j;
    int *inodes, *tempino;
    ssize_t bts;
    char *modes[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"}, permissions[10];

    if ((dirp = opendir("/proc")) == NULL) {
        perror("error openning /proc directory");
        return;
    }

    while ((direntp = readdir(dirp)) != NULL) {
        if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, "..")) continue;

        /*If it's not a process dir skip*/

        for (i = 0; i < strlen(direntp->d_name); i++) 
            if (!isdigit(direntp->d_name[i])) break;

        if (i < strlen(direntp->d_name)) continue;


        /*Check if it's a user process*/

        count = strlen("/proc/") + strlen(direntp->d_name) + strlen("/stat") + 1;
        path = malloc(count * sizeof(char));
        assert(path != NULL);

        strcpy(path, "/proc/");
        strcat(path, direntp->d_name);

        if (stat(path, &st) == -1) {
            perror("Failed to get file status");
            continue;
        }
        if (st.st_uid != uid) {
            free(path);
            continue;
        }

        strcat(path, "/stat");

        /*Check if it's a process of the same tty*/
        if ((fp = fopen(path, "r")) == NULL) {
            perror("error opening stat file of process");
            free(path);
            continue;
        }

        int chars = 0;
        spaces = 0;
        while ((ch = fgetc(fp)) != '\n' && spaces < 3) {
            chars++;
            if (isspace(ch)) {
                spaces++;
            }
        }
        fseek(fp, chars-1, SEEK_SET);
        fscanf(fp, "%ld", &ne);
        pproc_id = (pid_t)ne;

        fclose(fp);

        if (pproc_id != tty_id) { 
            free(path);
            continue;
        }


        free(path);
        path = NULL;

        /*Open fd of the current process*/
        count = count - strlen("/stat") + strlen("/fd/");
        path = malloc(count * sizeof(char));
        assert(path != NULL);

        strcpy(path, "/proc/");
        strcat(path, direntp->d_name);
        strcat(path, "/fd/");

        if ((dirpp = opendir(path)) == NULL) {
            perror("error openning process fd directory");
            continue;
        }


        printf("PPID  %s\n", direntp->d_name);
        inodes = malloc(200 * sizeof(int));
        for (i = 0; i < 200; i++) inodes[i] = -1;
        i = 0;
        k = 0;

        /*For each filesystem entity on /proc/[PID]/fd, print the corresponding information*/
        while ((direntpp = readdir(dirpp)) != NULL) {
            if (!strcmp(direntpp->d_name, ".") || !strcmp(direntpp->d_name, "..")) continue;

            fdpath = malloc(sizeof(char) * (count + strlen(direntpp->d_name)));
            assert(fdpath != NULL);
            strcpy(fdpath, path);            
            strcat(fdpath, direntpp->d_name);

            if (stat(fdpath, &st) == -1) {
                free(fdpath);
                continue;
            }


            if (search_inode(inodes, st.st_ino, 200)) {
                free(fdpath);
                fdpath = NULL;
                continue;
            } else inodes[k++] = st.st_ino;


            bts = readlink(fdpath, filepath, sizeof(filepath)-1);
            filepath[bts] = '\0';

            if (stat(filepath, &st) == -1) {
                free(fdpath);
                continue;
            }

            pw = getpwuid(st.st_uid);
            gr = getgrgid(st.st_gid);

            switch (st.st_mode & S_IFMT) {
                case S_IFREG: printf("-"); break;
                case S_IFDIR: printf("d"); break;
                case S_IFLNK: printf("l"); break;
                case S_IFSOCK: printf("s"); break;
                case S_IFIFO: printf("p"); break;
                case S_IFCHR: printf("c"); break;
                case S_IFBLK: printf("b"); break;
                default:    printf("?"); break;
            }

            *permissions = '\0';

            for (i = 2; i >= 0; i--) {
                j = (st.st_mode >> (i*3)) & 07;
                strcat(permissions, modes[j]);
            }

            printf("%-10s%-5d%-12s%-12s%-10ld%-10s\n", permissions, (int)st.st_nlink, pw->pw_name, gr->gr_name, (long int)st.st_size, filepath);


            free(fdpath);
            fdpath = NULL;
        }
        free(path);
        path = NULL;
        free(inodes);
        inodes = NULL;
        printf("\n");
        closedir(dirpp);
    }

    closedir(dirp);

}

/*5th argument -iostat*/

void iostat(int numofrecs,char* fieldname){

	uid_t uid = getuid();
	DIR *dirp;
	struct dirent *direntp;
	int count=0;
	int readflag=0;
	struct stat st;
	char *path;
	int numofupr=0;

	ioinfo* ioarr=NULL;

	if((dirp=opendir("/proc"))==NULL)
		fprintf(stderr,"Error opening proc");
	else{
		FILE *fp;
		printf("PID\tRCHCAR\tWCHAR\tSYSR\tSYSW\tRFS\tWFS\n");
		while((direntp=readdir(dirp))!=NULL){
        
            /* Begin searching after the init process */
        
			if(strcmp(direntp->d_name,"1")==0)
				readflag=1;
			if(readflag==1){
                /* Get the user's processes */
                
				count = strlen("/proc/") + strlen(direntp->d_name) + strlen("/io") + 1;
		        path = malloc(count * sizeof(char));
		        assert(path != NULL);

		        strcpy(path, "/proc/");
		        strcat(path, direntp->d_name);

		        if (stat(path, &st) == -1) {
		            perror("Failed to get file status");
		            continue;
		        }
		        if (st.st_uid != uid) {
		            free(path);
		            continue;
		        }

		        strcat(path, "/io");

		        FILE *iop;
	    		iop=fopen(path,"r");
	    		if (iop==NULL) {
        			free(path);
        			continue;
    			}
                
                /* Put the results into a dynamic array */
                
    			ioarr=(ioinfo*)realloc(ioarr,(numofupr+1)*sizeof(ioinfo));
    			ioarr[numofupr].pid=atoi(direntp->d_name);
    			fscanf(iop,"%*s %d %*s %d %*s %d %*s %d %*s %d %*s %d",&ioarr[numofupr].rchar,&ioarr[numofupr].wchar,&ioarr[numofupr].syscr,&ioarr[numofupr].syscw,&ioarr[numofupr].rfs,&ioarr[numofupr].wfs);
    			numofupr++;

    			free(path);
			}
		}
        
         /* Sort the array in relation to the given fieldname and print the k-first results */

		if(!strcmp(fieldname,"PID"))
			qsort(ioarr, numofupr, sizeof(ioinfo), PidSort);
		else if(!strcmp(fieldname,"RCHAR"))
			qsort(ioarr, numofupr, sizeof(ioinfo), RcharSort);
		else if(!strcmp(fieldname,"WCHAR"))
			qsort(ioarr, numofupr, sizeof(ioinfo), WcharSort);
		else if(!strcmp(fieldname,"SYSR"))
			qsort(ioarr, numofupr, sizeof(ioinfo), SyscrSort);
		else if(!strcmp(fieldname,"SYSW"))
			qsort(ioarr, numofupr, sizeof(ioinfo), SyscwSort);
		else if(!strcmp(fieldname,"RFS"))
			qsort(ioarr, numofupr, sizeof(ioinfo), RfsSort);
		else if(!strcmp(fieldname,"WFS"))
			qsort(ioarr, numofupr, sizeof(ioinfo), WfsSort);
		else
			printf("Error,Uknown fieldname\n");

		int i;
		if (numofupr<numofrecs)
			numofrecs=numofupr;
		for(i=0;i<numofrecs;i++)
			printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n",ioarr[i].pid,ioarr[i].rchar,ioarr[i].wchar,ioarr[i].syscr,ioarr[i].syscw,ioarr[i].rfs,ioarr[i].wfs);

		if(ioarr!=NULL)
			free(ioarr);
	}

}

int PidSort(const void* a,const void* b) {
    ioinfo *p1 = (ioinfo*)a;
    ioinfo *p2 = (ioinfo*)b;
    return (p2->pid-p1->pid);
}
int RcharSort(const void* a,const void* b) {
    ioinfo *p1 = (ioinfo*)a;
    ioinfo *p2 = (ioinfo*)b;
    return (p2->rchar-p1->rchar);
}
int WcharSort(const void* a,const void* b) {
    ioinfo *p1 = (ioinfo*)a;
    ioinfo *p2 = (ioinfo*)b;
    return (p2->wchar-p1->wchar);
}
int SyscrSort(const void* a,const void* b) {
    ioinfo *p1 = (ioinfo*)a;
    ioinfo *p2 = (ioinfo*)b;
    return (p2->syscr-p1->syscr);
}
int SyscwSort(const void* a,const void* b) {
    ioinfo *p1 = (ioinfo*)a;
    ioinfo *p2 = (ioinfo*)b;
    return (p2->syscw-p1->syscw);
}
int RfsSort(const void* a,const void* b) {
    ioinfo *p1 = (ioinfo*)a;
    ioinfo *p2 = (ioinfo*)b;
    return (p2->rfs-p1->rfs);
}
int WfsSort(const void* a,const void* b) {
    ioinfo *p1 = (ioinfo*)a;
    ioinfo *p2 = (ioinfo*)b;
    return (p2->wfs-p1->wfs);
}


/*BONUS! 6th argument -netstat*/

void netstat(char *toprint) {
    FILE *fp;
    DIR *dirp, *dirpp;
    struct dirent *direntp, *direntpp;
    struct stat st;
    struct passwd *pw;
    struct group *gr;
    uid_t uid = getuid();
    pid_t tty_id = getppid();
    pid_t proc_id, pproc_id;
    long ne;
    char *path, *pidz, *fdpath, *strt;
    char line[512], filepath[1024], procprt[14];

    int count = 0, spaces = 0, i, ch, k, j, inode_n;
    int *inodes, *tempino;
    ssize_t bts;


    if ((dirp = opendir("/proc")) == NULL) {
        perror("error openning /proc directory");
        return;
    }


    printf("PROTOCOL    PID    L-ADDRESS    L-PORT  R-ADDRESS   R-PORT\n");
    while ((direntp = readdir(dirp)) != NULL) {
        if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, "..")) continue;

        /*If it's not a process dir skip*/
        for (i = 0; i < strlen(direntp->d_name); i++) 
            if (!isdigit(direntp->d_name[i])) break;

        if (i < strlen(direntp->d_name)) continue;


        /*Check if it's a user process*/

        count = strlen("/proc/") + strlen(direntp->d_name) + strlen("/stat") + 1;
        path = malloc(count * sizeof(char));
        assert(path != NULL);

        strcpy(path, "/proc/");
        strcat(path, direntp->d_name);

        if (stat(path, &st) == -1) {
            free(path);
            path = NULL;
            perror("Failed to get file status");
            continue;
        }
        if (st.st_uid != uid) {
            free(path);
            path = NULL;
            continue;
        }

        free(path);
        path = NULL;

        /*Open fd of the current process*/
        count = count - strlen("/stat") + strlen("/fd/");
        path = malloc(count * sizeof(char));
        assert(path != NULL);

        strcpy(path, "/proc/");
        strcat(path, direntp->d_name);
        strcat(path, "/fd/");

        if ((dirpp = opendir(path)) == NULL) {
            free(path);
            path = NULL;
            continue;
        }

        i = 0;
        k = 0;
        inodes = malloc(200 * sizeof(int));
        for (i = 0; i < 200; i++) inodes[i] = -1;

        /*Check if the process has any open sockets, and add them to an array. Then match the info from /proc/net/[tcp/udp]*/
        while ((direntpp = readdir(dirpp)) != NULL) {
            if (!strcmp(direntpp->d_name, ".") || !strcmp(direntpp->d_name, "..")) continue;

            fdpath = malloc(sizeof(char) * (count + strlen(direntpp->d_name)));
            assert(fdpath != NULL);
            strcpy(fdpath, path);            
            strcat(fdpath, direntpp->d_name);

            if (stat(fdpath, &st) == -1) {
                free(fdpath);
                continue;
            }

            if ((st.st_mode & S_IFMT) != S_IFSOCK) {
                free(fdpath);
                fdpath = NULL;
                continue;
            }


            bts = readlink(fdpath, filepath, sizeof(filepath)-1);
            filepath[bts] = '\0';

            inode_n = 0;
            for (i = 0; i < strlen(filepath); i++) 
                if (isdigit(filepath[i])) inode_n = inode_n * 10 + filepath[i] - '0';

            if (search_inode(inodes, inode_n, 200)) {
                free(fdpath);
                fdpath = NULL;
                continue;
            } else inodes[k++] = inode_n;

            free(fdpath);
            fdpath = NULL;
        }

        /*For each socket of the process, check if it exists on tcp/udp and print the required information*/

        for (i = 0; i < 200 && inodes[i] != -1; i++) {

            strcpy(procprt, "/proc/net/");
            if (toprint == NULL) {
                strcat(procprt, "tcp");
                net_print(procprt, inodes[i], direntp->d_name);
                procprt[10] = '\0';
                strcat(procprt, "udp");
                net_print(procprt, inodes[i], direntp->d_name);
            } else {
                strcat(procprt, toprint);
                net_print(procprt, inodes[i], direntp->d_name);
            }
        }

        free(inodes);
        inodes = NULL;
        free(path);
        path = NULL;
        closedir(dirpp);
    }

    closedir(dirp);

}

int search_inode(int *inarray, int inode, int n) {

    int i = 0;
    for (i = 0; i < n && inarray[i] != -1; i++)
        if (inode == inarray[i]) return 1;
    return 0;

}


void net_print(char *filename, int inode, char *pid) {
    
    /*Prints the required network information for a socket*/
    FILE *fp1;
    char line[512], *strt, laddr[14], lport[5], raddr[14], rport[5];
    char tstr[10];
    char prot[4] = {filename[10] - ('a'-'A'), filename[11] - ('a'-'A'), filename[12] - ('a'-'A')};
    int i;

    if ((fp1 = fopen(filename, "r")) == NULL) return;

    fgets(line, 512, fp1);

    while ((fgets(line, 512, fp1)) != NULL) {
        strt = strtok(line, " ");
        strt = strtok(NULL, " ");
        strcpy(laddr, strt);
        strcpy(lport, laddr + 9);
        laddr[8] = '\0';
        strt = strtok(NULL, " ");
        strcpy(raddr, strt);
        strcpy(rport, raddr + 9);
        raddr[8] = '\0';
        strt = strtok(NULL, " ");

        i = 4;
        while (i++ < 9) strt = strtok(NULL, " ");
        strt = strtok(NULL, " ");
        
        if (inode == atoi(strt)) 
            printf("%-12s%-7s%-13s%-8s%-12s%-8s\n", prot, pid, laddr, lport, raddr, rport);

    }

    fclose(fp1);

}
