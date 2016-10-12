#ifndef _DEFN_H
#define _DEFN_H

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

typedef enum t_args {MOD, ACC} exec_mode;

void cnt(char*,int*);
void lrlw(char **args, int n);
void lrlw_rec(time_t ttime, char *path, exec_mode use, char *toprint);
time_t strtime(char *ttime);
void pprint(char *path, char t, char *toprint, time_t ttime, time_t statime);
void ft();
void ps();
int search_inode(int *inarray, int inode, int n);
void netstat(char *toprint);
void net_print(char *filename, int inode, char *pid);
void iostat(int,char*);
int PidSort(const void* a,const void* b);
int RcharSort(const void* a,const void* b); 
int WcharSort(const void* a,const void* b); 
int SyscrSort(const void* a,const void* b);
int SyscwSort(const void* a,const void* b);
int RfsSort(const void* a,const void* b); 
int WfsSort(const void* a,const void* b); 

typedef struct ioinfo{
	int pid;
	int rchar;
	int wchar;
	int syscr;
	int syscw;
	int rfs;
	int wfs;
} ioinfo;

#endif

