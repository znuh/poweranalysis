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

int main(int argc, char **argv)
{
	int trace = 0;
	char file[512], cryptdata[128];
	mf_t trace_mf;
	dpa_t *dpa = NULL;
	uint8_t hypotheses[1024], key[16], plain[16];
	int hypos = 0;
	correl_t *results = NULL;
	time_t start = 0, runtime;
	int cnt;
	int last_cnt = 0;
	FILE *fl, *ptxts;
	float signif;
	int best_keybyte = 0;
	float max_correl = 0;
	hypo_template_t *hypo_templates;

	if (argc <= 2) {
		printf("see the README\n");
		return 0;
	}

	sscanf(argv[1], "%2s:%d", file, &cnt);

	assert((fl = fopen("hypo.txt", "w")));

	if (!(strcmp(file, "hd"))) {
		printf("Hamming-Distance keybyte %d\n", cnt);
		fprintf(fl, "sbox_out %d&ff sbox_in %d&ff 0 0\n", cnt, cnt);
	} else if (!(strcmp(file, "hw"))) {
		printf("Hamming-Weight keybyte %d\n", cnt);
		fprintf(fl, "sbox_out %d&ff null 0&ff 0 0\n", cnt);
	} else {
		printf
		    ("usage: hd:<keybyte> for hamming dist or hw:<keybyte> for hamming weight\n");
		return 0;
	}

	fclose(fl);

	// don't ask...
	assert((hypos = hypo_templ_gen("hypo.txt", &hypo_templates, NULL)));
	hypos *= 256;

	sprintf(file, "%s/aes.log", argv[2]);
	assert((ptxts = fopen(file, "r")));

	while (fgets(cryptdata, 512, ptxts)) {

		if (!(trace % 20))
			printf("trace %d\n", trace);

		assert(parse_hex(cryptdata, plain, 16) == (cryptdata + 32));

		sprintf(file, "%s/%06d.dat", argv[2], trace);
		assert(!(open_trace(&trace_mf, file)));

		// generate hypotheses
		for (cnt = 0; cnt < 256; cnt++) {
			memset(key, cnt & 0xff, 16);
			hypo_gen(plain, key, hypo_templates, hypotheses + cnt);
		}

		if (!dpa) {
			dpa = dpa_init(hypos, trace_mf.len);
			assert((results =
				malloc(sizeof(correl_t) * (dpa->tracelen))));
			start = time(NULL);
		}

		dpa_add(dpa, trace_mf.ptr, hypotheses);

		trace++;
		last_cnt = trace;

		signif = 1.3 * (4 / sqrt((float)trace));

		max_correl = 0;
		best_keybyte = 0;

		if (!(trace % 100)) {

			for (cnt = 0; cnt < hypos; cnt++) {
				float max;

				dpa_get_results(dpa, cnt, results, &max);

				if (ABS(max) > ABS(max_correl)) {
					max_correl = max;
					best_keybyte = cnt;
				}
			}
			printf
			    ("key guess 0x%02x correl: %f (signifcant: >=%f)\n",
			     best_keybyte, max_correl, signif);
		}
	}

	runtime = time(NULL);
	runtime -= start;

	dpa_speedinfo(dpa, runtime);

	// get results
	dpa_get_results(dpa, best_keybyte, results, NULL);

	assert((fl = fopen("results.txt", "w")));

	for (cnt = 0; cnt < dpa->tracelen; cnt++)
		fprintf(fl, "%d: %f\n", cnt, results[cnt]);

	fclose(fl);

	free(results);

	dpa_destroy(&dpa);

	return 0;
}
