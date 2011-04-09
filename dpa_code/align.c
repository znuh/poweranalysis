#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mmap.h"

float lsqd(uint8_t *p1, uint8_t *p2, int len) {
	float val=0;
	
	for(;len;len--,p1++,p2++) {
		float diff = *p1;
		diff -= *p2;
		diff *= diff;
		val += diff;
	}
	return val;
}

int main(int argc, char **argv) {
	mf_t mf, mf2;
	char buf[128];
	int i, res, len, min_i = 0;
	float min_diff = 0;

	assert(argc>1);
	res = map_file(&mf, argv[1], 0, 0);	
	assert(!res);

	len = mf.len;

	assert(len >= 400000);

	while(fgets(buf,128,stdin)) {
		int slen = strlen(buf);
		buf[--slen]=0;

		res = map_file(&mf2, buf, 0, 0);
		assert(!res);

		assert(mf2.len == len);

		min_diff = -1.0;
		for(i=-1000;i<=1000;i++) {
			float val = lsqd(mf.ptr + 90000, mf2.ptr + 90000 + i, 1000);
			if(min_diff == -1.0) {
				min_diff = val;
				min_i = i;
			}
			if(val < min_diff) {
				min_diff = val;
				min_i = i;
			}
			//printf("%d %f\n",i,val);
		}
		printf("%s %d %f\n",buf,min_i,min_diff);

		unmap_file(&mf2);
	}

	return 0;
}