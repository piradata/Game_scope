#ifdef __cplusplus
extern "C" {
#endif
#ifndef _DEFINES_H
#define _DEFINES_H

#include "includes.h"

#define CPU_FREQ_HZ 84000000

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define clamp(x, m, M) (min(max((x), (m)), (M)))

#define bs(b, p, v) { if (v) { (b) |=  (1 << (p)); } else { (b) &= ~(1 << (p)); } }
#define bg(b, p) (((b) & (1 << (p))) >> (p))

#define bs1(b, p) bs(b,p,1)
#define bs0(b, p) bs(b,p,0)
#define bsn(b, p) bs(b,p, !bg(b,p))

#define ps(p, v) { if (v) { g_APinDescription[p].pPort -> PIO_SODR = g_APinDescription[p].ulPin; } else { g_APinDescription[p].pPort -> PIO_CODR = g_APinDescription[p].ulPin; } }
#define ps1(p) ps(p, 1)
#define ps0(p) ps(p, 0)

#define swap(i, j) {j ^= i; i ^= j; j ^= i; }

#endif
#ifdef __cplusplus
}
#endif
