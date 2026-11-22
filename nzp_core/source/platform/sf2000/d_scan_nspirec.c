#include "../../nzportable_def.h"
#include "d_local.h"

extern int bbextents;
extern int bbextentt;

static inline void draw_span_nspire_fw_8(byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth)
{
    for (int i = 0; i < 8; i++) {
        int s = f16_rps >> 16;
        int t = f16_rpt >> 16;
        *pdest++ = *(pbase + t * i_cachewidth + s);
        f16_rps += f16_sstep;
        f16_rpt += f16_tstep;
    }
}

static inline void draw_span_nspire_bw_8(byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth)
{
    for (int i = 0; i < 8; i++) {
        int s = f16_rps >> 16;
        int t = f16_rpt >> 16;
        *pdest-- = *(pbase + t * i_cachewidth + s);
        f16_rps += f16_sstep;
        f16_rpt += f16_tstep;
    }
}

static inline void draw_span_nspire_fw_s(byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth, int i_count)
{
    for (int i = 0; i < i_count; i++) {
        int s = f16_rps >> 16;
        int t = f16_rpt >> 16;
        *pdest++ = *(pbase + t * i_cachewidth + s);
        f16_rps += f16_sstep;
        f16_rpt += f16_tstep;
    }
}

static inline void draw_span_nspire_bw_s(byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth, int i_count)
{
    for (int i = 0; i < i_count; i++) {
        int s = f16_rps >> 16;
        int t = f16_rpt >> 16;
        *pdest-- = *(pbase + t * i_cachewidth + s);
        f16_rps += f16_sstep;
        f16_rpt += f16_tstep;
    }
}

int rgi_mtab[16] = {
    0x10000, 0x8000, 0x5555, 0x4000, 0x3333, 0x2aaa, 0x2492, 0x2000,
    0x1c71,  0x1999, 0x1745, 0x1555, 0x13b1, 0x1249, 0x1111, 0x1000
};

void draw_span_nspire_fw_c(draw_span8_nspire_t *ps_dset)
{
    byte *pdest = ps_dset->pdest;
    byte *pbase = ps_dset->pbase;
    int i_cachewidth = ps_dset->i_cachewidth;
    int count = ps_dset->count;
    fixed16_t f16_p = ps_dset->f16_p;
    fixed16_t f16_stepp = ps_dset->f16_stepp;
    fixed16_t f16_rps = ps_dset->f16_ps;
    fixed16_t f16_rpt = ps_dset->f16_pt;
    fixed16_t f16_sstep = ps_dset->f16_steps;
    fixed16_t f16_tstep = ps_dset->f16_stept;

    while (count > 0) {
        int spancount = count > 8 ? 8 : count;
        draw_span_nspire_fw_s(pdest, pbase, f16_rps, f16_sstep, f16_rpt, f16_tstep, i_cachewidth, spancount);
        pdest += spancount;
        f16_rps += f16_sstep * spancount;
        f16_rpt += f16_tstep * spancount;
        count -= spancount;
    }
}

void draw_span_nspire_bw_c(draw_span8_nspire_t *ps_dset)
{
    byte *pdest = ps_dset->pdest;
    byte *pbase = ps_dset->pbase;
    int i_cachewidth = ps_dset->i_cachewidth;
    int count = ps_dset->count;
    fixed16_t f16_rps = ps_dset->f16_ps;
    fixed16_t f16_rpt = ps_dset->f16_pt;
    fixed16_t f16_sstep = ps_dset->f16_steps;
    fixed16_t f16_tstep = ps_dset->f16_stept;

    while (count > 0) {
        int spancount = count > 8 ? 8 : count;
        draw_span_nspire_bw_s(pdest, pbase, f16_rps, f16_sstep, f16_rpt, f16_tstep, i_cachewidth, spancount);
        pdest -= spancount;
        f16_rps += f16_sstep * spancount;
        f16_rpt += f16_tstep * spancount;
        count -= spancount;
    }
}
