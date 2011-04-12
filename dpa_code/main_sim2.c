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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <math.h>
#include <assert.h>
#include "mmap.h"
#include "dpa.h"
#include "hypothesis.h"
#include "utils.h"

int open_trace(mf_t * mf, char *file)
{

	if (mf->mem)
		unmap_file(mf);

	assert(!(map_file(mf, file, 0, 0)));

	madvise(mf->ptr, mf->len, MADV_WILLNEED);

	return 0;
}

uint8_t hamming_weight(uint8_t val);

static const uint8_t table_0[512] = {
        102,177,186,162,  2,156,112, 75, 55, 25,  8, 12,251,193,246,188,
        109,213,151, 53, 42, 79,191,115,233,242,164,223,209,148,108,161,
        252, 37,244, 47, 64,211,  6,237,185,160,139,113, 76,138, 59, 70,
         67, 26, 13,157, 63,179,221, 30,214, 36,166, 69,152,124,207,116,
        247,194, 41, 84, 71,  1, 49, 14, 95, 35,169, 21, 96, 78,215,225,
        182,243, 28, 92,201,118,  4, 74,248,128, 17, 11,146,132,245, 48,
        149, 90,120, 39, 87,230,106,232,175, 19,126,190,202,141,137,176,
        250, 27,101, 40,219,227, 58, 20, 51,178, 98,216,140, 22, 32,121,
         61,103,203, 72, 29,110, 85,212,180,204,150,183, 15, 66,172,196,
         56,197,158,  0,100, 45,153,  7,144,222,163,167, 60,135,210,231,
        174,165, 38,249,224, 34,220,229,217,208,241, 68,206,189,125,255,
        239, 54,168, 89,123,122, 73,145,117,234,143, 99,129,200,192, 82,
        104,170,136,235, 93, 81,205,173,236, 94,105, 52, 46,228,198,  5,
         57,254, 97,155,142,133,199,171,187, 50, 65,181,127,107,147,226,
        184,218,131, 33, 77, 86, 31, 44, 88, 62,238, 18, 24, 43,154, 23,
         80,159,134,111,  9,114,  3, 91, 16,130, 83, 10,195,240,253,119,
        177,102,162,186,156,  2, 75,112, 25, 55, 12,  8,193,251,188,246,
        213,109, 53,151, 79, 42,115,191,242,233,223,164,148,209,161,108,
         37,252, 47,244,211, 64,237,  6,160,185,113,139,138, 76, 70, 59,
         26, 67,157, 13,179, 63, 30,221, 36,214, 69,166,124,152,116,207,
        194,247, 84, 41,  1, 71, 14, 49, 35, 95, 21,169, 78, 96,225,215,
        243,182, 92, 28,118,201, 74,  4,128,248, 11, 17,132,146, 48,245,
         90,149, 39,120,230, 87,232,106, 19,175,190,126,141,202,176,137,
         27,250, 40,101,227,219, 20, 58,178, 51,216, 98, 22,140,121, 32,
        103, 61, 72,203,110, 29,212, 85,204,180,183,150, 66, 15,196,172,
        197, 56,  0,158, 45,100,  7,153,222,144,167,163,135, 60,231,210,
        165,174,249, 38, 34,224,229,220,208,217, 68,241,189,206,255,125,
         54,239, 89,168,122,123,145, 73,234,117, 99,143,200,129, 82,192,
        170,104,235,136, 81, 93,173,205, 94,236, 52,105,228, 46,  5,198,
        254, 57,155, 97,133,142,171,199, 50,187,181, 65,107,127,226,147,
        218,184, 33,131, 86, 77, 44, 31, 62, 88, 18,238, 43, 24, 23,154,
        159, 80,111,134,114,  9, 91,  3,130, 16, 10, 83,240,195,119,253
    }, table_1[256] = {
         19, 11, 80,114, 43,  1, 69, 94, 39, 18,127,117, 97,  3, 85, 43,
         27,124, 70, 83, 47, 71, 63, 10, 47, 89, 79,  4, 14, 59, 11,  5,
         35,107,103, 68, 21, 86, 36, 91, 85,126, 32, 50,109, 94,120,  6,
         53, 79, 28, 45, 99, 95, 41, 34, 88, 68, 93, 55,110,125,105, 20,
         90, 80, 76, 96, 23, 60, 89, 64,121, 56, 14, 74,101,  8, 19, 78,
         76, 66,104, 46,111, 50, 32,  3, 39,  0, 58, 25, 92, 22, 18, 51,
         57, 65,119,116, 22,109,  7, 86, 59, 93, 62,110, 78, 99, 77, 67,
         12,113, 87, 98,102,  5, 88, 33, 38, 56, 23,  8, 75, 45, 13, 75,
         95, 63, 28, 49,123,120, 20,112, 44, 30, 15, 98,106,  2,103, 29,
         82,107, 42,124, 24, 30, 41, 16,108,100,117, 40, 73, 40,  7,114,
         82,115, 36,112, 12,102,100, 84, 92, 48, 72, 97,  9, 54, 55, 74,
        113,123, 17, 26, 53, 58,  4,  9, 69,122, 21,118, 42, 60, 27, 73,
        118,125, 34, 15, 65,115, 84, 64, 62, 81, 70,  1, 24,111,121, 83,
        104, 81, 49,127, 48,105, 31, 10,  6, 91, 87, 37, 16, 54,116,126,
         31, 38, 13,  0, 72,106, 77, 61, 26, 67, 46, 29, 96, 37, 61, 52,
        101, 17, 44,108, 71, 52, 66, 57, 33, 51, 25, 90,  2,119,122, 35
    }, table_2[128] = {
         52, 50, 44,  6, 21, 49, 41, 59, 39, 51, 25, 32, 51, 47, 52, 43,
         37,  4, 40, 34, 61, 12, 28,  4, 58, 23,  8, 15, 12, 22,  9, 18,
         55, 10, 33, 35, 50,  1, 43,  3, 57, 13, 62, 14,  7, 42, 44, 59,
         62, 57, 27,  6,  8, 31, 26, 54, 41, 22, 45, 20, 39,  3, 16, 56,
         48,  2, 21, 28, 36, 42, 60, 33, 34, 18,  0, 11, 24, 10, 17, 61,
         29, 14, 45, 26, 55, 46, 11, 17, 54, 46,  9, 24, 30, 60, 32,  0,
         20, 38,  2, 30, 58, 35,  1, 16, 56, 40, 23, 48, 13, 19, 19, 27,
         31, 53, 47, 38, 63, 15, 49,  5, 37, 53, 25, 36, 63, 29,  5,  7
    }, table_3[64] = {
          1,  5, 29,  6, 25,  1, 18, 23, 17, 19,  0,  9, 24, 25,  6, 31,
         28, 20, 24, 30,  4, 27,  3, 13, 15, 16, 14, 18,  4,  3,  8,  9,
         20,  0, 12, 26, 21,  8, 28,  2, 29,  2, 15,  7, 11, 22, 14, 10,
         17, 21, 12, 30, 26, 27, 16, 31, 11,  7, 13, 23, 10,  5, 22, 19
    }, table_4[32] = {
         15, 12, 10,  4,  1, 14, 11,  7,  5,  0, 14,  7,  1,  2, 13,  8,
         10,  3,  4,  9,  6,  0,  3,  2,  5,  6,  8,  9, 11, 13, 15, 12
    }, *table[5] = { table_0, table_1, table_2, table_3, table_4 };

int main(int argc, char **argv)
{
	int trace = 0;
	char buf[512];
	mf_t trace_mf;
	dpa_t *dpa = NULL;
	int hypos = (256*6)+1;
	uint8_t hypotheses[hypos];
	correl_t *results = NULL;
	time_t start = 0, runtime;
	int res, cnt;
	int last_cnt = 0;
	FILE *fl_align, *fl_iod;
	float signif;
	int trace_ofs = (372*12*280)/16; // 78k
	int trace_len = 100000;
	int kbyte=0;
	float maxvals[hypos];
	int max_idx;
	
	assert(argc>3);

	fl_iod = fopen(argv[1], "r");
	assert(fl_iod);
	
	fl_align = fopen(argv[2],"r");
	assert(fl_align);

	kbyte = atoi(argv[3]);

	while (fgets(buf, 512, fl_iod)) {
		uint8_t rand[16];
		uint8_t sres_kc[12];
		char *p = parse_hex(buf+9, rand, 16);
		assert(p == buf+9+32);
		p = parse_hex(buf+9+32+1, sres_kc, 12);
		assert(p == buf+9+32+1+24);
		int ofs, i;
		float diff;

		res = fscanf(fl_align, "%s %i %f",buf,&ofs,&diff);
		assert(res == 3);

		res = open_trace(&trace_mf, buf);
		assert(!res);

		for(cnt=0, i=0; cnt<256; cnt++) {
			int y = (cnt+2*rand[kbyte]) % 512;
			int z = (2*cnt+rand[kbyte]) % 512;
			hypotheses[i++] = hamming_weight(table[0][y]);
			hypotheses[i++] = hamming_weight(table[0][z]);
			hypotheses[i++] = hamming_weight(cnt^table[0][y]);
			hypotheses[i++] = hamming_weight(rand[kbyte]^table[0][z]);
			hypotheses[i++] = hamming_weight(y);
			hypotheses[i++] = hamming_weight(z);
		}
		
		hypotheses[i] = rand[kbyte];

		if (!dpa) {
			dpa = dpa_init(hypos, trace_len);
			assert((results =
				malloc(sizeof(correl_t) * (dpa->tracelen))));
			start = time(NULL);
		}

		assert(ofs <= trace_ofs);
		dpa_add(dpa, trace_mf.ptr + trace_ofs - ofs, hypotheses);

		trace++;
		last_cnt = trace;

		if(!(trace%32))
			printf("%d\n",trace);

	}

	fclose(fl_iod);
	fclose(fl_align);

	runtime = time(NULL);
	runtime -= start;

	dpa_speedinfo(dpa, runtime);

	signif = 1.3 * (4 / sqrt((float)trace));
	printf("signif: %f\n", signif);

	max_idx=0;
	for (cnt = 0; cnt < hypos; cnt++) {
		float max;
//		FILE *res_fl;
//		int i;

		dpa_get_results(dpa, cnt, results, &max);
/*
		sprintf(buf,"dpa-%d.txt",cnt);
		res_fl = fopen(buf,"w");
		assert(res_fl);

		for(i=0;i<dpa->tracelen;i++)
			fprintf(res_fl,"%d %f\n",i+trace_ofs,results[i]);
*/		
//		fclose(res_fl);
		maxvals[cnt] = max;
		if(ABS(max) > ABS(maxvals[max_idx])) {
			max_idx = cnt;
		}
	}

	printf("max val %f idx %d\n",maxvals[max_idx],max_idx);

	{
		FILE *hist_fl = fopen("dpa_hist.txt","w");
		FILE *res_fl = fopen("dpa_max.txt","w");
		float max;
		
		for(cnt=0;cnt<hypos;cnt++) {
			fprintf(hist_fl,"%d %f\n",cnt,maxvals[cnt]);
		}
		fclose(hist_fl);
		
		dpa_get_results(dpa, max_idx, results, &max);
		
		for(cnt=0;cnt<dpa->tracelen;cnt++) {
			fprintf(res_fl,"%d %f\n",cnt+trace_ofs,results[cnt]);
		}
		fclose(res_fl);
	}

	free(results);

	dpa_destroy(&dpa);

	return 0;
}