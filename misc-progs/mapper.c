/*
 * mapper.c -- simple file that mmap()s a file region and prints it
 *
 * Copyright (C) 1998,2000,2001 Alessandro Rubini
 * 
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#define TIMEIT 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    char *fname;
    int fd;
    unsigned long offset, len;
    void *address;

    if (argc !=4
       || sscanf(argv[2],"%li", &offset) != 1
       || sscanf(argv[3],"%li", &len) != 1) {
        fprintf(stderr, "%s: Usage \"%s <file> <offset> <len>\"\n", argv[0],
                argv[0]);
        exit(1);
    }
    /* the offset might be big (e.g., PCI devices), but conversion trims it */
    if (offset == INT_MAX) {
	if (argv[2][1]=='x')
            sscanf(argv[2]+2, "%li", &offset);
        else
            sscanf(argv[2], "%li", &offset);
    }

    fname=argv[1];

    if (-1 == (fd=open(fname, 0, O_RDONLY))) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], fname, strerror(errno));
        exit(1);
    }

    address=mmap(0, len, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, offset);

    if (address == (void *)-1) {
        fprintf(stderr,"%s: mmap(): %s\n",argv[0],strerror(errno));
        exit(1);
    }
    close(fd);
    fprintf(stderr, "mapped \"%s\" from %lu (0x%08lx) to %lu (0x%08lx)\n",
            fname, offset, offset, offset+len, offset+len);

    clock_t begin = clock();
    fwrite(address, 1, len, stdout);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
#if TIMEIT
    fprintf(stderr, "writing took %g sec\n", time_spent);
#endif
    return 0;
}
        
