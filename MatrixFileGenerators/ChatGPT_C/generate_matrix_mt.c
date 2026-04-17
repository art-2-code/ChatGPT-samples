#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ============================================================
   Mersenne Twister MT19937 Implementation (public domain)
   ============================================================ */

#define MT_N 624
#define MT_M 397
#define MT_MATRIX_A 0x9908b0dfUL
#define MT_UPPER_MASK 0x80000000UL
#define MT_LOWER_MASK 0x7fffffffUL

static unsigned long mt[MT_N];
static int mti = MT_N + 1;

void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<MT_N; mti++) {
        mt[mti] =
            (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        mt[mti] &= 0xffffffffUL;
    }
}

unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MT_MATRIX_A};

    if (mti >= MT_N) {
        int kk;

        if (mti == MT_N + 1)
            init_genrand(5489UL);

        for (kk=0; kk<MT_N-MT_M; kk++) {
            y = (mt[kk]&MT_UPPER_MASK)|(mt[kk+1]&MT_LOWER_MASK);
            mt[kk] = mt[kk+MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (; kk<MT_N-1; kk++) {
            y = (mt[kk]&MT_UPPER_MASK)|(mt[kk+1]&MT_LOWER_MASK);
            mt[kk] = mt[kk+(MT_M-MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[MT_N-1]&MT_UPPER_MASK)|(mt[0]&MT_LOWER_MASK);
        mt[MT_N-1] = mt[MT_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    y ^= (y >> 11);
    y ^= (y << 7)  & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

double genrand_real_open(void)
{
    return (genrand_int32() + 0.5) * (1.0 / 4294967296.0);
}

/* ============================================================
   Main Program
   ============================================================ */

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ArraySize> <MaxRange>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    double maxRange = atof(argv[2]);

    if (n <= 0 || maxRange <= 0.0) {
        fprintf(stderr, "Error: ArraySize and MaxRange must be positive.\n");
        return 1;
    }

    FILE *fp = fopen("matrix.json", "w");
    if (!fp) {
        perror("Error opening output file");
        return 1;
    }

    /* Seed Mersenne Twister with time-based seed */
    init_genrand((unsigned long)time(NULL));

    fprintf(fp, "{\n  \"matrix\": [\n");

    for (int i = 0; i < n; i++) {
        fprintf(fp, "    [");
        for (int j = 0; j < n; j++) {
            double r = genrand_real_open() * maxRange;
            fprintf(fp, "%.4e", r);   // #.####e# format
            if (j < n - 1) fprintf(fp, ", ");
        }
        fprintf(fp, "]");
        if (i < n - 1) fprintf(fp, ",");
        fprintf(fp, "\n");
    }

    fprintf(fp, "  ]\n}\n");

    fclose(fp);

    printf("matrix.json generated using Mersenne Twister.\n");

    return 0;
}