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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "hypothesis.h"
#include "aes.h"
#include "utils.h"

uint8_t hamming_weight(uint8_t val) {
	uint8_t ret=0;
	
	while(val) {
		if (val&1)
			ret++;
		val>>=1;
	}
	return ret;
}

void hypo_gen(uint8_t *plain, uint8_t *key, hypo_template_t *templ, uint8_t *dst) {
	
	aes_encrypt(plain, key);
	
	while((templ->ptr_a) || (templ->ptr_b)) {
		uint8_t a=0, b=0;

		if(templ->ptr_a)
			a = *((uint8_t *) (templ->ptr_a));
		
		if(templ->ptr_b)
			b = *((uint8_t *) (templ->ptr_b));
		
		a &= templ->bits_a;
		b &= templ->bits_b;
		
		*(dst++) = hamming_weight(a ^ b);
		
		// TODO: bug at hypothesis 1 !!
		//printf("%d\n",dst[-1]);
		
		templ++;
	}
}

uint8_t *lookup_symbol(char *name) {
	int i;
	
	for(i=0; aes_interim[i].name; i++) {
		
		if(!(strcasecmp(name,aes_interim[i].name)))
			return aes_interim[i].value;
		
	}
	
	printf("error: hypothesis symbol '%s' not found\n",name);
	
	return NULL;
}

int hypo_templ_parse_line(char *line, hypo_template_t *t, hypo_display_t *disp) {
	char sym_a[64], sym_b[64];
	int ofs_a, ofs_b, bits_a = -1, bits_b = -1, disp_id = -1;
	uint32_t color;
	char *hypo_text = NULL;
	int res = sscanf(line,"%s %d&%x %s %d&%x %d %x",sym_a,&ofs_a,&bits_a,sym_b,&ofs_b,&bits_b,&disp_id,&color);
	
	if(disp) {
		disp->window_id = disp_id;
		disp->color = color;
		
		hypo_text = disp->text;
	
		if(hypo_text)
			sprintf(hypo_text,"HD(%s[%d]&%x, %s[%d]&%x)",sym_a,ofs_a,bits_a,sym_b,ofs_b,bits_b);
	}
	
	t->ptr_a = NULL;
	t->ptr_b = NULL;
	
	t->bits_a = bits_a;
	t->bits_b = bits_b;
	
	if(res >= 6) {
		uint8_t *ptr;
		
		if(strcasecmp(sym_a,"null")) {
			ptr = lookup_symbol(sym_a);
			
			if(!ptr)
				return 0;
			
			ptr += ofs_a;
			
			t->ptr_a = ptr;
		}
		
		if(strcasecmp(sym_b,"null")) {
			ptr = lookup_symbol(sym_b);
			
			if(!ptr)
				return 0;
			
			ptr += ofs_b;
			
			t->ptr_b = ptr;
		}
		
		return 1;
	}
	
	return 0;
}

int hypo_templ_gen(char *file, hypo_template_t **dst, hypo_display_t **disp) {
	hypo_template_t *res = NULL, *ptr;
	FILE *fl = fopen(file, "r");
	char buf[256];
	hypo_display_t *disp_ptr = NULL;
	int len = 0;
	int len2 = 0;
	
	*dst = NULL;
	
	if(!fl)
		return -1;
	
	while(fgets(buf, 256, fl))
		len++;
	
	rewind(fl);
	
	assert((res = malloc((len+1) * sizeof(hypo_template_t))));
	ptr = res;
	
	if(disp) {
		assert((disp_ptr = malloc((len + 1) * sizeof(hypo_display_t))));
		*disp = disp_ptr;
	}
	
	while(fgets(buf, 256, fl)) {
		int len = strlen(buf);
		
		assert(len > 1);
		
		buf[--len]=0;
		
		if(hypo_templ_parse_line(buf, ptr, disp_ptr)) {
			ptr++;
			len2++;
			if(disp_ptr)
				disp_ptr++;
		}
	}
	
	fclose(fl);
	
	ptr->ptr_a = ptr->ptr_b = NULL;
	
	*dst = res;
	
	return len2;
}
