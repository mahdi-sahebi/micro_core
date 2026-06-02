#ifndef MC_IO_COMMUNICATION_POSIX_H_
#define MC_IO_COMMUNICATION_POSIX_H_

/* Single declaration point for the POSIX calls used by the communication
 * layer. Under -std=c99 the <unistd.h> declarations are gated behind a
 * feature-test macro and the MISRA checker does not expand them; declaring
 * the prototype here once satisfies Rule 17.3 (no implicit declaration)
 * without violating Rule 8.5 (one declaration in one file). The signature
 * matches POSIX/glibc. */
extern int usleep(unsigned int usec);

#endif /* MC_IO_COMMUNICATION_POSIX_H_ */
