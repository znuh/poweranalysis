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
	int i, res, len, min_i = 0;
	float min_diff = 0;

	assert(argc>2);
	res = map_file(&mf, argv[1], 0, 0);
	assert(!res);
	res = map_file(&mf2, argv[2], 0, 0);
	assert(!res);

	assert(mf.len == mf2.len);
	len = mf.len;

	assert(len >= 400000);

	for(i=-10000;i<=10000;i++) {
		float val = lsqd(mf.ptr + len/2, mf2.ptr + len/2 + i, 10000);
		if(min_diff == 0.0) {
			min_diff = val;
			min_i = i;
		}
		if(val < min_diff) {
			min_diff = val;
			min_i = i;
		}
		printf("%d %f\n",i,val);
	}
	fprintf(stderr,"min diff %f at %d\n",min_diff,min_i);

	return 0;
}