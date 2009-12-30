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
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <strings.h>
#include <assert.h>
#include "dpa.h"

dpa_t *dpa_init(uint32_t hypotheses, uint32_t tracelen) {
	size_t memsize;
	correl_t *ptr;
	dpa_t *dpa=NULL;
	uint32_t hypo_cnt_fake=hypotheses;
	
#ifdef SSE
	// must be x*4 hypotheses
	hypo_cnt_fake += 3;
	hypo_cnt_fake &= ~3;
	printf("DPA init - using SSE (%u (+%u fake) hypotheses, tracelen %u)\n",hypotheses,hypo_cnt_fake-hypotheses,tracelen);
#else	
	printf("DPA init (%u hypotheses, tracelen %u)\n",hypotheses,tracelen);
#endif
	
	memsize = (2 * tracelen); // sum_ysq & mean_y
	memsize += (2 * hypotheses); // sum_xsq & mean_x
	memsize += hypo_cnt_fake; // delta_x
	memsize += (1 * hypo_cnt_fake * tracelen); // sum_cross
	memsize *= sizeof(correl_t);
	
	memsize += sizeof(dpa_t);
	
	memsize += ((ALIGN-1)*2); // I don't care if we won't use SSE at all!
	
	printf("memory usage: %zd MByte\n",memsize/(1024*1024));
	
	assert((dpa = malloc(memsize)));
	
	dpa->hypo_cnt = hypotheses;
	dpa->hypo_cnt_fake = hypo_cnt_fake;
	dpa->tracelen = tracelen;
	dpa->traces = 0;
	
	ptr = (correl_t *)(dpa + 1);
	
	bzero(ptr, hypotheses * sizeof(correl_t));
	dpa->sum_xsq = ptr;
	ptr += hypotheses;
	
	// initialized with 1st trace
	dpa->mean_x = ptr;
	ptr += hypotheses;
	
	// align for vector unit
	ALIGN_PTR(ptr);
	// no need to bzero this. 
	dpa->delta_x = ptr;
	ptr += hypo_cnt_fake;
	
	bzero(ptr, tracelen * sizeof(correl_t));
	dpa->sum_ysq = ptr;
	ptr += tracelen;
	
	// initialized with 1st trace
	dpa->mean_y = ptr;
	ptr += tracelen;
	
	// heavy vector stuff ahead!
	ALIGN_PTR(ptr);
	bzero(ptr, hypo_cnt_fake * tracelen * sizeof(correl_t));
	dpa->sum_cross = ptr;
	
	return dpa;
}

void dpa_destroy(dpa_t **dpa) {
	
	assert(dpa);
	assert(*dpa);
	
	free(*dpa);
	*dpa = NULL;
}

void dpa_add_hypotheses(dpa_t *dpa, uint8_t *hypos_data) {
	correl_t *sum_xsq = dpa->sum_xsq;
	correl_t *mean_x = dpa->mean_x;
	correl_t *delta_x = dpa->delta_x;
	correl_t num = dpa->traces + 1.0; // vec
	correl_t ratio = dpa->traces / num; //vec
	uint32_t cnt;
	
	// foreach hypothesis
	for(cnt=0; cnt < dpa->hypo_cnt; cnt++) { // vec
		correl_t delta_x_ = *(hypos_data++) - (*mean_x);
		*(delta_x++) = delta_x_;
		*(sum_xsq++) += delta_x_ * delta_x_ * ratio;
		*(mean_x++) += delta_x_ / num;
	}
}

void dpa_first_trace(dpa_t *dpa, uint8_t *trace_data, uint8_t *hypos_data) {
	uint32_t cnt;
	
	// init mean_x (hypotheses)
	for(cnt=0; cnt < dpa->hypo_cnt; cnt++)
		dpa->mean_x[cnt] = (correl_t) *(hypos_data++);
	
	// init mean_y (traces)
	for(cnt=0; cnt < dpa->tracelen; cnt++)
		dpa->mean_y[cnt] = (correl_t) *(trace_data++);
	
	dpa->traces = 1;
}

void dpa_add(dpa_t *dpa, uint8_t *trace_data, uint8_t *hypos_data) {
#ifdef SSE
	v4sf *sum_cross = (v4sf*)dpa->sum_cross;
	v4sf *delta_x;
	union f4vec delta_y_vec __attribute__((aligned(16)));;
#else
	correl_t *sum_cross = dpa->sum_cross;
	correl_t *delta_x;
#endif
	correl_t *sum_ysq = dpa->sum_ysq;
	correl_t *mean_y = dpa->mean_y;
	correl_t delta_y;
	correl_t num = dpa->traces + 1.0;
	correl_t ratio = dpa->traces / num;
	uint32_t tracepos, hypo;
	
	if(!(dpa->traces))
		return dpa_first_trace(dpa, trace_data, hypos_data);
	
	dpa_add_hypotheses(dpa, hypos_data);
	
	for(tracepos = 0; tracepos < dpa->tracelen; tracepos++) {
		
		delta_y = (correl_t) *(trace_data++) - (*mean_y);
		*(mean_y++) += delta_y / num;
		*(sum_ysq++) += delta_y * delta_y * ratio;
		
		delta_y *= ratio;
		
#ifdef SSE
		// reset delta_x ptr (1 delta_x per hypothesis)
		delta_x = (v4sf*) dpa->delta_x;
		delta_y_vec.f[0] = delta_y_vec.f[1] = delta_y_vec.f[2] = delta_y_vec.f[3] = delta_y;
		
		for(hypo = 0; hypo < dpa->hypo_cnt; hypo+=4) {
			*sum_cross = __builtin_ia32_addps(*sum_cross, __builtin_ia32_mulps(*(delta_x++), delta_y_vec.v));
			sum_cross++;		
#else
		// reset delta_x ptr (1 delta_x per hypothesis)
		delta_x = dpa->delta_x;
		
		for(hypo = 0; hypo < dpa->hypo_cnt; hypo++) {			
			*(sum_cross++) += *(delta_x++) * delta_y;
#endif
		} // calc sum_cross for all hypotheses
	} // foreach tracepo
	
	dpa->traces++;
}

#define ABS(a)	(((a)>0)?(a):(-a))

void dpa_get_results(dpa_t *dpa, uint32_t hypo, correl_t *results, float *max) {
	correl_t *sum_cross = dpa->sum_cross + hypo;
	correl_t *sum_ysq = dpa->sum_ysq;
	correl_t sqrt_sumx = sqrt(dpa->sum_xsq[hypo]);
	uint32_t hypo_cnt = dpa->hypo_cnt_fake;
	uint32_t tracepos;
	float max_val=0;
	
	for(tracepos = 0; tracepos < dpa->tracelen; tracepos++) {
		float res = (*sum_cross) / (sqrt_sumx * sqrt(*(sum_ysq++)));
		*(results++) = res;
		if(ABS(res) > max_val)
			max_val = res;
		sum_cross += hypo_cnt;
	}
	
	if(max)
		*max = max_val;
	
}

void dpa_speedinfo(dpa_t *dpa, time_t runtime) {
	unsigned long long fpu, io;
	
	io = dpa->traces;
	io *= dpa->tracelen;
		
	fpu = io * dpa->hypo_cnt;
	
	io /= (runtime * 1024 * 1024);
	fpu /= (runtime * 1024 * 1024);
	
	printf("FPU: %lld MPoints/s, IO: %lld MBytes/s\n",fpu, io);
	printf("%ld traces/min\n",(dpa->traces*60)/runtime);
}
