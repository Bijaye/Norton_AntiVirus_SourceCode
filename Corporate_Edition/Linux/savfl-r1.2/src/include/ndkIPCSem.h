
#ifndef NDK_IPC_SEM_H_INCLUDED
#define NDK_IPC_SEM_H_INCLUDED

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
//#include <linux/sem.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEM_RESOURCE_MAX        1       /* Initial value of all semaphores */

/* arg for semctl system calls. */
union semun {
	int val;			/* value for SETVAL */
	struct semid_ds *buf;		/* buffer for IPC_STAT & IPC_SET */
	unsigned short *array;		/* array for GETALL & SETALL */
	struct seminfo *__buf;		/* buffer for IPC_INFO */
	void *__pad;
};

#define SEMMNI  128             /* <= IPCMNI  max # of semaphore identifiers */
#define SEMMSL  250             /* <= 8 000 max num of semaphores per id */
#define SEMMNS  (SEMMNI*SEMMSL) /* <= INT_MAX max # of semaphores in system */
#define SEMOPM  32	        /* <= 1 000 max num of ops per semop call */
#define SEMVMX  32767           /* <= 32767 semaphore maximum value */

/* unused */
#define SEMUME  SEMOPM          /* max num of undo entries per process */
#define SEMMNU  SEMMNS          /* num of undo structures system wide */
#define SEMAEM  (SEMVMX >> 1)   /* adjust on exit max value */
#define SEMMAP  SEMMNS          /* # of entries in semaphore map */
#define SEMUSZ  20		/* sizeof struct sem_undo */

void opensem(int *sid, key_t key);
void createsem(int *sid, key_t key, int members);
void locksem(int sid, int member);
void unlocksem(int sid, int member);
void removesem(int sid);
unsigned short get_member_count(int sid);
int getval(int sid, int member);
void dispval(int sid, int member);
void changemode(int sid, char *mode);
void usage(void);

#ifdef __cplusplus
}
#endif
	
#endif // NDK_IPC_SEM_H_INCLUDED
