
#ifndef AES_H
#define AES_H

#include "hypothesis.h"

//interim_info_t *aes_interim;
interim_info_t aes_interim[3];

void aes_encrypt(uint8_t * plain, uint8_t * key);

#endif
