
#ifndef HYPOTHESIS_H
#define HYPOTHESIS_H

#include <stdint.h>

typedef struct interim_info_s {
	char *name;
	char *desc;
	int len;
	uint8_t *value;
} interim_info_t;

typedef struct hypo_template_s {
	void *ptr_a;
	void *ptr_b;

	int bits_a;
	int bits_b;
} hypo_template_t;

typedef struct hypo_display_s {
	int window_id;
	uint32_t color;
	char text[64];
} hypo_display_t;

int hypo_templ_gen(char *file, hypo_template_t ** dst,
		   hypo_display_t ** display);
void hypo_gen(uint8_t * plain, uint8_t * key, hypo_template_t * templ,
	      uint8_t * dst);

#endif
