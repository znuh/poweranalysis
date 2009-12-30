#ifndef DPA_H
#define DPA_H

#include <time.h>
#include <stdint.h>

#define ALIGN	16

#define ALIGN_PTR(a)	(a)=(void*)( (((unsigned long) (a))+(ALIGN-1))& (~(ALIGN-1)))

#ifdef SSE
typedef float v4sf __attribute__ ((vector_size(16)));
union f4vec {
	v4sf v;
	float f[4];
};
#endif

typedef float correl_t;

typedef struct dpa_s {
	uint32_t hypo_cnt;
	uint32_t hypo_cnt_fake;
	uint32_t tracelen;
	uint32_t traces;
			
	/* correl. values from traces */
	correl_t *sum_ysq;
	correl_t *mean_y;
	
	/* correl. values from hypotheses */
	correl_t *sum_xsq;
	correl_t *mean_x;
	correl_t *delta_x;	
	
	/* shared correl. values */
	correl_t *sum_cross;
	
} dpa_t;

dpa_t *dpa_init(uint32_t hypotheses, uint32_t tracelen);
void dpa_destroy(dpa_t **dpa);

//void dpa_add_hypotheses(dpa_t *dpa, uint8_t *hypos_data);
//void dpa_first_trace(dpa_t *dpa, uint8_t *trace_data, uint8_t *hypos_data);

void dpa_add(dpa_t *dpa, uint8_t *trace_data, uint8_t *hypos_data);
void dpa_get_results(dpa_t *dpa, uint32_t hypo, correl_t *results, float *max);
void dpa_speedinfo(dpa_t *dpa, time_t runtime);

#endif
