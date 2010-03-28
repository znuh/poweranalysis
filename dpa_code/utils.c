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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void skip_char(char sc, char **buf)
{
	char *ptr = *buf;

	while (*ptr == sc)
		ptr++;

	*buf = ptr;
}

void find_char(char fc, char **buf)
{
	char *ptr = *buf;

	while (*ptr != fc)
		ptr++;

	*buf = ptr;
}

char *parse_hex(char *buf, uint8_t * dst, int len)
{
	char tmp[3] = { 0, 0, 0 };

	while (len--) {
		skip_char(' ', &buf);
		tmp[0] = buf[0];
		tmp[1] = buf[1];
		*dst = strtoul(tmp, NULL, 16);
		buf += 2;
		dst++;
	}
	return buf;
}

void dump_hex(uint8_t * src, int len)
{
	while (len--) {
		printf("%02x ", *src);
		src++;
	}
	printf("\n");
}
