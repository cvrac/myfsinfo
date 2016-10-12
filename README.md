[![License MIT][badge-license]](LICENSE)

#System program, providing information for the processes, files of the user as well as resources of the system, from /proc aka proc filesystem.

1. -cnt argument
execution: ./myfsinfo -cnt <directory> \\
where <directory> = the starting directory \\

2. -lr/-lw arguments
#lr for access time, lw for modification time
execution: ./myfsinfo -type [f|d|l|p|c|b|s] -[lr|lw] -<time>[d|h|m|s] <directory> \\
where [f|d|l|p|c|b|s] refers to the [file|directory|link|pipe|char|block|socket] - multiple types can be given as parameters \\
	-[lr|lw] refers to the selected execution mode \\
	-<time>[d|h|m|s] refers to the time after which we're checking for the information \\
	<directory> = the starting directory \\
Example: ./myfsinfo -type fd -lr -2d ~/. \\

3. -ps argument
#provides information for user's processes on a tty
execution: ./myfsinfo -ps \\

4. -ft argument
#provides information for the files used by user's processes on a tty
execution: ./myfsinfo -ft \\

5. -iostat
#provides information about the I/O of user's processes
execution: ./myfsinfo -iostat -k <#records> -f <fieldname> \\
where <#records> is the number of records to be printed \\
	<fieldname> is one of the : PID, RCHAR, WCHAR, SYSR, SYSW, RFS, WFS. \\

6. -netstat
#provides information for the network usage by user's processes.
execution: ./myfsinfo -netstat [tcp|udp] \\
Examples: ./myfsinfo -netstat \\
		  ./myfsinfo -netstat tcp \\
		  ./myfsinfo -netstat udp \\

[badge-license]: https://img.shields.io/badge/license-MIT-green.svg