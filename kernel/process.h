#ifndef __PROCESS_H
#define __PROCESS_H

#include "display.h"
#include "virtualmem.h"

#define PROCESS_STACK_SIZE 16384
#define PROCESS_EXIT_NOW 1
#define PROCESS_POLLING 2


typedef struct process_t {
	uint pid;							// The process ID
	unsigned char buffer;				// A buffer (where the keyboard handler)
	Window *win;						// The window used by the process
	PageDirectory *page_dir;			// The page directory
	struct process_t *next;				// The next process
//	char stack[PROCESS_STACK_SIZE];		// The stack
	unsigned char *stack;
	uint eax;							// Some registers
	uint ebp;
	uint esp;
	char kernel_stack[PROCESS_STACK_SIZE];
	uint eip;
	uint flags;							// Some flags
	void (*function) ();				// The function to call after initialization
	char error[128];					// Buffer for errors
} Process;

void init_processes();
void start_process();
void switch_process();
int fork();
void move_stack(void *new_stack_start, uint size);
void init_tasking();
int getpid();
void error(const char*);
void error_reset();
const char *error_get();

extern volatile Process *current_process;

#endif
