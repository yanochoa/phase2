/* ------------------------------------------------------------------------
   phase2.c
   Version: 2.0

   Skeleton file for Phase 2. These routines are very incomplete and are
   intended to give you a starting point. Feel free to use this or not.


   ------------------------------------------------------------------------ */

#include <stdlib.h>
#include <usloss.h>
#include <phase1.h>
#include <phase2.h>
#include "libuser.h"

static int ClockDriver(void *arg);
int P2_Spawn(char *name, int(*func)(void *), void *arg, int stackSize, int priority);
int P2_Wait(int *status);
static P1_Semaphore running;

int
P2_Startup(void *arg)
{
    int               status;
    int               result = 1; // default is there was a problem
    int               rc;
    /*
     * Check kernel mode
     */
    // ...

    /*
     * Create clock device drivers.
     */
    rc = P1_SemCreate("running", 0, &running);
    if (rc != 0) {
        USLOSS_Console("P1_SemCreate of running failed: %d\n", rc);
        goto done;
    }
    for (int i = 0; i < USLOSS_CLOCK_UNITS; i++) {
        rc = P1_Fork("Clock driver", ClockDriver, (void *) i, USLOSS_MIN_STACK, 2, 0);
        if (rc < 0) {
            USLOSS_Console("Can't create clock driver\n");
            goto done;
        }
        /*
         * Wait for the clock driver to start.
         */
        rc = P1_P(running);
        if (rc != 0) {
            USLOSS_Console("P1_P(running) failed: %d\n", rc);
            goto done;
        }
    }
    /*
     * Create the other device drivers.
     */
    // ...

    /* 
     * Create initial user-level process. You'll need to check error codes, etc. P2_Spawn
     * and P2_Wait are assumed to be the kernel-level functions that implement the Spawn and 
     * Wait system calls, respectively (you can't invoke a system call from the kernel).
     */
    rc = P2_Spawn("P3_Startup", P3_Startup, NULL,  4 * USLOSS_MIN_STACK, 3);
    if (rc < 0) {
        USLOSS_Console("Spawn of P3_Startup failed: %d\n", rc);
    } else {
        rc = P2_Wait(&status);
        if (rc < 0) {
            USLOSS_Console("Wait for P3_Startup failed: %d\n", rc);
            goto done;
        }
    }


    /*
     * Make the device drivers abort using P1_WakeupDevice.
     */

    for (int i = 0; i < USLOSS_CLOCK_UNITS; i++) {
        rc = P1_WakeupDevice(USLOSS_CLOCK_DEV, i, 1);
        if (rc < 0) {
            USLOSS_Console("Wakeup of clock device driver failed : %d\n", rc);
            goto done;
        }
    }

    // ...

    /*
     * Join with the device drivers.
     */
    // ...

    result = 0;
done:
    return result;
}

static int
ClockDriver(void *arg)
{
    int unit = (int) arg;
    int result = 1; // default is there was a problem
    int dummy;
    int rc; // return codes from functions we call

    /*
     * Let the parent know we are running and enable interrupts.
     */
    rc = P1_V(running);
    if (rc != 0) {
        goto done;
    }
    rc = USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT);
    if (rc != 0) {
        USLOSS_Console("ClockDriver: USLOSS_PsrSet returned %d\n", rc);
        goto done;
    }
    while(1) {
        /*
         * Read new sleep requests from the clock mailbox and update the bookkeeping appropriately.
         */
        rc = P1_WaitDevice(USLOSS_CLOCK_DEV, unit, &dummy);
        if (rc == -3) { // aborted
            break;
        } else if (rc != 0) {
            USLOSS_Console("ClockDriver: P1_WaitDevice returned %d\n", rc);
            goto done;
        }
        /*
         * Compute the current time and wake up any processes
         * that are done sleeping by sending them a response.
         */
    }
    result = 0; // if we get here then everything is ok
done:
    return rc; 
}

int
P2_Sleep(int seconds)
{
		// Check for a valid seconds arg
	if (seconds < 0) {
		return 1;
	}
	
	int i;
	for (i = 0; i < seconds * 10; i++) {
			// Returns after 100ms
		ClockDriver(USLOSS_CLOCK_UNITS);
	}
}

int     
P2_Spawn(char *name, int (*func)(void *arg), void *arg, int stackSize, 
                         int priority) 
{
    return -1;
}

void
P2_TermRead(void *args) 
{
	USLOSS_Sysargs	sa = *(USLOSS_Sysargs *) args;
	
	len = *(int *) sa.arg2;
	int status = USLOSS_DEV_READY;
	char * terminal;
	
	if (len < 0 || len > P2_MAX_LINE) {
		sa.arg4 = (void *) -1;
		return;
	}
	
		// Wait for a character to be transmitted to the terminal
	while (status == USLOSS_DEV_READY) {
		USLOSS_DeviceInput(USLOSS_TERM_DEV, *(int *)sa.arg3, terminal);
		status = 0xF & terminal;
	}
	
	if (status == USLOSS_DEV_ERROR) {
		sa.arg4 = (void *) -1;
		return;
	}
	
	char curr_char = terminal>>8;
	sa.arg1 = (void *) &curr_char;
	
	i = 1;
	while (status == USLOSS_DEV_BUSY && i < len) {
		USLOSS_DeviceInput(USLOSS_TERM_DEV, *(int *)sa.arg3, terminal);
		char curr_char = terminal>>8;
		sa.arg1+i = (void *) &curr_char;
	}
	
	sa.arg2 = (void *) i;
	sa.arg4 = (void *) 0;
}

int     
P2_Wait(int *status) 
{
    return -1;
}

void
P2_Terminate(int status)
{
	
}

int
P2_MboxCreate(int slots, int size)
{
	return -1;
}

int
P2_MboxRelease(int mbox)
{
	return -1;
}

int
P2_MboxSend(int mbox, void *msg, int *size) 
{
	return -1;
}

int
P2_MboxCondSend(int mbox, void *msg, int *size)
{
	return -1;
}

int
P2_MboxReceive(int mbox, void *msg, int *size)
{
	return -1;
}

int
P2_MboxCondReceive(int mbox, void *msg, int *size)
{
	return -1;
}