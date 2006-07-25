/***************************Copyright-DO-NOT-REMOVE-THIS-LINE**
  *
  * Condor Software Copyright Notice
  * Copyright (C) 1990-2006, Condor Team, Computer Sciences Department,
  * University of Wisconsin-Madison, WI.
  *
  * This source code is covered by the Condor Public License, which can
  * be found in the accompanying LICENSE.TXT file, or online at
  * www.condorproject.org.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * AND THE UNIVERSITY OF WISCONSIN-MADISON "AS IS" AND ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY, OF SATISFACTORY QUALITY, AND FITNESS
  * FOR A PARTICULAR PURPOSE OR USE ARE DISCLAIMED. THE COPYRIGHT
  * HOLDERS AND CONTRIBUTORS AND THE UNIVERSITY OF WISCONSIN-MADISON
  * MAKE NO MAKE NO REPRESENTATION THAT THE SOFTWARE, MODIFICATIONS,
  * ENHANCEMENTS OR DERIVATIVE WORKS THEREOF, WILL NOT INFRINGE ANY
  * PATENT, COPYRIGHT, TRADEMARK, TRADE SECRET OR OTHER PROPRIETARY
  * RIGHT.
  *
  ****************************Copyright-DO-NOT-REMOVE-THIS-LINE**/

 

/*
** This program makes sure getdirentries works
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#if defined(HPUX)
#include <ndir.h>
#else
#include <dirent.h>
#endif

#define MATCH (0)

#ifdef LINUX

// This appears to be the structure of a dent on Linux. Note that the
// wtf variable seems to not be documented.
// This works on Linux 2.2.19...
// Erik 9/2001
typedef struct kernel_dirent
  {
    long int d_ino;
    long d_off;
    unsigned short d_reclen;
	char wtf;
    char d_name[255];
  } DENT;

#elif defined(HPUX)
	typedef struct direct DENT;
#else
	typedef struct dirent DENT;
#endif

/* for some reason, g++ needs this extern definition */
#if defined(DUX4) && defined(__cplusplus) && defined(__GNUC__)
extern "C" ssize_t getdirentries(int fd, char *buf, size_t  nbytes ,
				 off_t *basep);
#endif

int main() {
/* HPUX should work, but it doesn't currently with the given code */
#if !defined(Solaris) && !defined( IRIX ) && !defined( HPUX )
	int results;
	off_t basep;
	int fd;
	char buf[8192];
	DENT *window;
	unsigned long voidstar;
	int numfiles, expectednumfiles, totalfiles;

	results = mkdir("testdir", 0777);	
	if(results == -1) {
		perror("mkdir");
		return -1;
	}

    // now let's create some files...
	fd = open("testdir/first", O_CREAT | O_TRUNC, O_RDWR);	
	if(fd < 0) { perror("Open First"); return -1; }
	close(fd);

	fd = open("testdir/second", O_CREAT | O_TRUNC, O_RDWR);	
	if(fd < 0) { perror("Open Second"); return -1; }
	close(fd);

	fd = open("testdir/third", O_CREAT | O_TRUNC, O_RDWR);	
	if(fd < 0) { perror("Open Third"); return -1; }
	close(fd);

	fd = open("testdir", O_RDONLY);	
	if(fd < 0) { perror("Open Directory"); return -1; }

	basep = 0;
#if defined(HPUX)
	results = getdirentries(fd, (DENT*)buf, 8192, &basep);
#else
	results = getdirentries(fd, buf, 8192, &basep);
#endif
	if(results < 0) {perror("getdirentries: "); return -1;}

	numfiles = 0;
	expectednumfiles = 5; /* '.', '..', 'first', 'second', 'third' */
	totalfiles = 0;

	voidstar = (unsigned long int)buf;
	// Walk the list we got back from dirents	
	window = (DENT *)voidstar;
	while(window->d_reclen > 0) {

		/* commented out but left present so you can see how to access the 
			name of the file out of the structure */
/*		printf("ENTRY: %s\n", window->d_name);	*/

		/* check to make sure I found what I wanted */
		if (strcmp(window->d_name, ".") == MATCH) {
			numfiles++;
		}

		if (strcmp(window->d_name, "..") == MATCH) {
			numfiles++;
		}

		if (strcmp(window->d_name, "first") == MATCH) {
			numfiles++;
		}

		if (strcmp(window->d_name, "second") == MATCH) {
			numfiles++;
		}

		if (strcmp(window->d_name, "third") == MATCH) {
			numfiles++;
		}

		totalfiles++;

		voidstar = voidstar + window->d_reclen;
		window = (DENT *)voidstar;
	}
	close(fd);
	
	/* this is not a transitive boolean, because it implies I found
		_certain_ expected files. */
	if (numfiles == expectednumfiles && expectednumfiles == totalfiles) {
		fprintf(stdout, "completed successfully\n");
	} else {
		fprintf(stdout, "expectednumfiles = %d\n", expectednumfiles);
		fprintf(stdout, "totalfiles = %d\n", totalfiles);
		fprintf(stdout, "numfiles = %d\n", numfiles);
		fprintf(stdout, "failed\n");
	}
#endif
	return 0;
}
