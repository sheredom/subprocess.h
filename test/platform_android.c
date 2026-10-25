/* Verify Android uses fork()+exec(), since its spawn.h lacks the chdir action. */
#define __ANDROID__ 1
#include <subprocess.h>

#if !SUBPROCESS_SPAWN_VIA_FORK
#error "Android must use fork()+exec()"
#endif

#if !SUBPROCESS_HAVE_CWD
#error "fork()+exec() must support process_cwd"
#endif

int main(void) { return 0; }
