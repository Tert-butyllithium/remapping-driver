#ifndef u64
#define u64 unsigned long
#endif

u64 get_phyaddr_at(u64 va)
{
    static const u64 low_48_bit_mask = 281474976710655ULL - 4095ULL;
    u64 res;
    asm("at s1e1w, %0" ::"r"(va));
    asm("mrs %0, par_el1"
        : "=r"(res));
    return (res & low_48_bit_mask) | (va & 4095ULL);
}