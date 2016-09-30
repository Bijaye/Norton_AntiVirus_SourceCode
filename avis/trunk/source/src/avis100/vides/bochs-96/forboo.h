#ifndef FORBOO_H
#define FORBOO_H

#include "config.h"

void (*recordFetch)(Bit16u seg, Bit32u ofs, int sz);
void recordFetchOp(Bit16u seg, Bit32u ofs, int sz);
//void printBootCodeMap(void);

#endif
