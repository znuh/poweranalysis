/*  Copyright (C) 2009 znuh <Zn000h AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "mmap.h"

int map_file(mf_t *mf, char *file, int ofs, int size)
{
	struct stat attr;
	int fd;

	if ((fd = open(file, O_RDONLY)) < 0)
		return fd;

	if (fstat(fd, &attr)) {
		close(fd);
		return -1;
	}

	if (ofs < 1)
		ofs = 0;

	if ((size < 1) || (size > (attr.st_size - ofs)))
		size = attr.st_size - ofs;

	if ((mf->mem =
	     mmap(NULL, size, PROT_READ, MAP_SHARED, fd, ofs)) == MAP_FAILED) {
		close(fd);
		return -1;
	}
	
	//readahead(fd, ofs, size);

	close(fd);

	mf->real_len = mf->len = attr.st_size;
	mf->ptr = mf->mem;

	return 0;
}

void unmap_file(mf_t *mf)
{
	munmap(mf->mem, mf->real_len);
	mf->mem = mf->ptr = NULL;
	mf->real_len = mf->len = 0;
}
