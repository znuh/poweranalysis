#ifndef MMAP_H
#define MMAP_H

typedef struct mf_s {
	unsigned char *mem;
	unsigned char *ptr;
	unsigned long len;
	unsigned long real_len;
} mf_t;

int map_file(mf_t * mf, char *file, int ofs, int size);
void unmap_file(mf_t * mf);

#endif
