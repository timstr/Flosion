#include <iostream>

#if defined(_WIN32) || defined(_WIN64)

#include <mmintrin.h>
#define DISABLE_DENORMALS   \
        int oldMXCSR = _mm_getcsr();        /*read the old MXCSR setting  */    \
        int newMXCSR = oldMXCSR |= 0x8040;  /* set DAZ and FZ bits        */    \
        _mm_setcsr( newMXCSR );             /* write the new MXCSR setting to the MXCSR */

#define RESTORE_DENORMALS   \
        _mm_setcsr( oldMXCSR );

#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

// #include <fenv.h>
// #define DISABLE_DENORMALS   \
//     fenv_t fenv;                \
//         fegetenv(&fenv);        \
//         fesetenv(FE_DFL_DISABLE_SSE_DENORMS_ENV);

// #define RESTORE_DENORMALS   \
//         fesetenv(&fenv);

// TODO: ??
#define DISABLE_DENORMALS
#define RESTORE_DENORMALS

#endif