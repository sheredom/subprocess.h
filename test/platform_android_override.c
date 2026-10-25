/* User-defined configuration must take precedence over platform detection. */
#define __ANDROID__ 1
#define SUBPROCESS_SPAWN_VIA_FORK 0

#if !defined(__ANDROID__)
#error "Android simulation is required"
#endif

#include <subprocess.h>

#if SUBPROCESS_SPAWN_VIA_FORK
#error "SUBPROCESS_SPAWN_VIA_FORK override was ignored"
#endif

int main(void) { return 0; }
