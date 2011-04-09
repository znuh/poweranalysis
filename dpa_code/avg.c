#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mmap.h"

int main(int argc, char **argv) {
	FILE *fl;
	mf_t mf;
	uint32_t *di = NULL, cnt=0, len=0;
	uint8_t *db = NULL;
	char buf[128];
	int i, ofs;
	float diff;

	assert(argc>1);
	fl = fopen(argv[1],"w");
	assert(fl);

	while(scanf("%s %i %f",buf,&ofs,&diff)==3) {
		int res;
		
		res = map_file(&mf, buf, 0, 0);
		assert(!res);

		if(!di) {
			len = mf.len;
			di = malloc(len * sizeof(uint32_t));
			assert(di);
			bzero(di, len * sizeof(uint32_t));
		}
		
		assert(mf.len == len);

		assert(ofs<=500);
		
		for(i=500;i<len;i++) {
			di[i-ofs] += *(mf.ptr);
			mf.ptr++;
		}

		unmap_file(&mf);
		
		cnt++;
		
		if(!(cnt%32))
			printf("%d\n",cnt);
	}

	db = malloc(len);
	assert(db);

	for(i=0;i<len;i++) {
		uint32_t val = di[i]/cnt;
		db[i] = val&0xff;
	}

	free(di);
	
	fwrite(db, len, 1, fl);

	free(db);
	
	fclose(fl);

	return 0;
}