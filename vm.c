/*
 * ЛИЦЕНЗИЯ: эта программа лицензирована также, как и ``forschar.h``.
 *
 * Автор --- Menshikov Sviatoslav Sergeevich, 2024 год.
 */


#define _GNU_SOURCE

#include "forschar.h"
#include <string.h>

static char args[127];

static int stack[2][25000];
static int stackptr[2];
#define MEM_LEN    8700000
static int *mem;

/* опкоды */
#define PUSH_NUM         -10000000
#define RESET_STACK_PTR  -10000001
#define CALL             -10000002
/**********/

#include "program_code.h"

static int current_thread = 0;
static int pc[2];

void xmconc_call(int t /* (current thread) */, char *f);

/* выполнение программы в памяти */
void mem_exec(int t) {
	for(; pc[t] < MEM_LEN; pc[t]++) {
		if(mem[pc[t]] == PUSH_NUM)
			stack[t][stackptr[t]++] = mem[pc[t] + 1];
		else if(mem[pc[t]] == RESET_STACK_PTR)
			stackptr[t] = 0;
		else if(mem[pc[t]] == CALL) {
			char buf[64];
			int ii;
			for(ii = pc[t] + 1; mem[ii] != '\0'; ii++)
				buf[ii - (pc[t] + 1)] = mem[ii];
			buf[ii - (pc[t] + 1)] = '\0';
			
			xmconc_call(t, buf);
		}
	}
}


#include "xmconc_lib.h"


int main(int argc, char **argv) {
	for(int i = 0; i < sizeof(args); i++)
		args[i] = '\0';
	
	int iii = 0;
	for(int i = 1; i < argc; i++) {
		for(int ii = 0; argv[i][ii] != '\0' && iii < sizeof(args); ii++, iii++) {
			args[iii] = argv[i][ii];
		}
		if(i < argc - 1)
			args[iii++] = ' ';
	}
	
	/***************************************/
	
	int program[] = PROGRAM_CODE;
	
	mem = (int *) malloc(MEM_LEN * sizeof(*mem));
	
	memset(mem, '\0', MEM_LEN * sizeof(*mem));
	
	for(int i = 0; i < 1024; i++) {
		stack[0][i] = 0;
		stack[1][i] = 0;
	}
	
	stackptr[0] = 0;
	stackptr[1] = 0;
	
	pc[0] = 0;
	pc[1] = 0;
	
	/* помещение программы в память */
	for(int i = 0; i < (sizeof(program) / sizeof(*program)); i++)
		mem[i] = program[i];
	
#ifdef PROG_DBG
	fprintf(stderr, "*** size: %d\n", (sizeof(program) / sizeof(*program)));
#endif
    
	disable_input_buffering();
	atexit(&restore_input_buffering);
	
	mem_exec(0);
}

