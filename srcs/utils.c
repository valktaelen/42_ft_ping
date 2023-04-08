#include "../inc/includes.h"

unsigned short checksum(unsigned short *buf, int len) {
  unsigned long sum = 0;

  while (len > 1) {
    sum += *buf++;
    len -= 2;
  }

  if (len == 1) {
    sum += *(unsigned char *)buf;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);

  return ~sum;
}
