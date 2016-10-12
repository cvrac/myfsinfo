#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h>
#include <time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

#include "defn.h"

int main(int argc,char* argv[]){

    if (argc < 2) {
        fprintf(stderr, "Not enough arguments\n");
        exit(1);
    }


    if(!strcmp(argv[1],"-cnt")){

        int *datatypenum=malloc(8*sizeof(int));
        int i;

        for(i=0;i<8;i++)
            datatypenum[i]=0;

        cnt(argv[2],datatypenum);

        int total=0;
        for(i=0;i<8;i++)
            total+=datatypenum[i];

        printf("Number of regural files:    %7d | %.1f %% \n",datatypenum[0],(float)datatypenum[0]/total*100);
        printf("Number of directories:      %7d | %.1f %%\n",datatypenum[1],(float)datatypenum[1]/total*100);
        printf("Number of named pipes:      %7d | %.1f %%\n",datatypenum[2],(float)datatypenum[2]/total*100);
        printf("Number of sockets:          %7d | %.1f %%\n",datatypenum[3],(float)datatypenum[3]/total*100);
        printf("Number of char devices:     %7d | %.1f %%\n",datatypenum[4],(float)datatypenum[4]/total*100);
        printf("Number of block devices:    %7d | %.1f %%\n",datatypenum[5],(float)datatypenum[5]/total*100);
        printf("Number of symbolic links:   %7d | %.1f %%\n",datatypenum[6],(float)datatypenum[6]/total*100);
        printf("Number of uknown data types:%7d | %.1f %%\n",datatypenum[7],(float)datatypenum[7]/total*100);

        free(datatypenum);

    }else if (!strcmp(argv[1], "-lr") || !strcmp(argv[1], "-lw") || !strcmp(argv[1], "-type"))
        lrlw(argv, argc);
    else if(!strcmp((argv[1]),"-ps"))
        ps(); 
    else if (!strcmp(argv[1], "-ft"))
        ft();
    else if(!strcmp((argv[1]),"-iostat")){
		int numofrecs;
		char fieldname[20];
		if(argc!=6){
			fprintf(stderr,"Invalid number of arguments\n");
			return -1;
		}
		else{
			if(!strcmp(argv[2],"-k"))
				numofrecs=atoi(argv[3]);
			else{
				fprintf(stderr,"Arguments in wrond order\n");
				return -1;
			}
			if(!strcmp(argv[4],"-f"))
				strcpy(fieldname,argv[5]);
			else{
				fprintf(stderr,"Arguments in wrond order\n");
				return -1;
			}
		}
		iostat(numofrecs,fieldname);
	}
    else if (!strcmp(argv[1], "-netstat")) {
        if (argc < 3)
            netstat(NULL);
        else if (argc == 3)
            netstat(argv[2]);
    } else
        printf("Uknown Flag\n");

    return 0;

}

