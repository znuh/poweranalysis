
#ifndef UTILS_H
#define UTILS_H

void skip_char(char sc, char **buf);

void find_char(char fc, char **buf);

char *parse_hex(char *buf, uint8_t * dst, int len);

void dump_hex(uint8_t * src, int len);

#endif
