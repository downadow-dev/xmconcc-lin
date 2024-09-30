/*
 * ЛИЦЕНЗИЯ: эта программа лицензирована также, как и ``forschar.h``.
 *
 * Автор --- Menshikov Sviatoslav Sergeevich, 2024 год.
 */


#ifndef __xmconc_lib_h
#define __xmconc_lib_h   1

#include <math.h>
#include <sys/syscall.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <wchar.h>

static bool clear_color = false;

void *create_thread1() {
#ifdef PROG_DBG
    fprintf(stderr, "*** thread 1 is started\n");
#endif
	mem_exec(1);
}

void xmconc_call(int t, char *f) {
	if(strcmp(f, "setcolor") == 0 || strcmp(f, "setbg") == 0)
		clear_color = true;
	
	/* puts */
	if(strcmp(f, "puts") == 0) {
		stackptr[t]--;
		for(int i = stack[t][stackptr[t]]; mem[i] != '\0'; i++)
			putchar(mem[i]);
		fflush(stdout);
	}
	/* putn */
	else if(strcmp(f, "putn") == 0) {
		printf("%d", stack[t][--stackptr[t]]);
		fflush(stdout);
	}
	/* putc */
	else if(strcmp(f, "putc") == 0 && stack[t][stackptr[t] - 1] > 127) {
		printf("%lc", stack[t][--stackptr[t]]);
		fflush(stdout);
	}
	else if(strcmp(f, "putc") == 0) {
		putchar(stack[t][--stackptr[t]]);
		fflush(stdout);
	}
	/* exit */
	else if(strcmp(f, "exit") == 0) {
		if(clear_color == true)
			printf("\033[0m");
		exit(stack[t][--stackptr[t]]);
	}
	/* newline */
	else if(strcmp(f, "newline") == 0) {
		putchar('\n');
		fflush(stdout);
	}
	/* backspace */
	else if(strcmp(f, "backspace") == 0) {
		putchar('\b');
		fflush(stdout);
	}
	/* cr */
	else if(strcmp(f, "cr") == 0) {
		putchar('\r');
		fflush(stdout);
	}
	/* memset */
	else if(strcmp(f, "memset") == 0) {
	    stackptr[t] -= 3;
	    
        for(int i = stack[t][stackptr[t]]; i < stack[t][stackptr[t]] + stack[t][stackptr[t] + 2]; i++)
            mem[i] = stack[t][stackptr[t] + 1];
        
        stackptr[t]++;
	}
	/* memcpy */
	else if(strcmp(f, "memcpy") == 0) {
	    stackptr[t] -= 3;
	    
        for(int i = 0; i < stack[t][stackptr[t] + 2]; i++)
            mem[stack[t][stackptr[t]] + i] = mem[stack[t][stackptr[t] + 1] + i];
        
        stackptr[t]++;
	}
	/* goto */
	else if(strcmp(f, "goto") == 0 || strcmp(f, "gnu_code") == 0)
		pc[t] = stack[t][--stackptr[t]] - 1;
	/* getc */
	else if(strcmp(f, "getc") == 0)
		stack[t][stackptr[t]++] = getwchar();
	/* getuid */
	else if(strcmp(f, "getuid") == 0)
		stack[t][stackptr[t]++] = getuid();
	/* ОПЕРАЦИИ НАД ЧИСЛАМИ */
	else if(strcmp(f, "+") == 0) {
		int num1 = stack[t][--stackptr[t]],
			num2 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 + num2;
	}
	else if(strcmp(f, "-") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 - num2;
	}
	else if(strcmp(f, "*") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 * num2;
	}
	else if(strcmp(f, "/") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 / num2;
	}
	else if(strcmp(f, "mod") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 % num2;
	}
	else if(strcmp(f, "**") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = (int)pow((double)num1, (double)num2);
	}
	else if(strcmp(f, "^") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 ^ num2;
	}
	else if(strcmp(f, "|") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 | num2;
	}
	else if(strcmp(f, "and") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 & num2;
	}
	else if(strcmp(f, "lsh") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 << num2;
	}
	else if(strcmp(f, "rsh") == 0) {
		int num2 = stack[t][--stackptr[t]],
			num1 = stack[t][--stackptr[t]];
		stack[t][stackptr[t]++] = num1 >> num2;
	}
	else if(strcmp(f, "++") == 0) {
		stack[t][stackptr[t] - 1]++;
	}
	else if(strcmp(f, "--") == 0) {
		stack[t][stackptr[t] - 1]--;
	}
	else if(strcmp(f, "neg") == 0) {
		stack[t][stackptr[t] - 1] = -stack[t][stackptr[t] - 1];
	}
	/* clear_output */
	else if(strcmp(f, "clear_output") == 0) {
		if(clear_color == true) {
			printf("\033[0m");
			clear_color = false;
		}
		system("clear");
	}
	/* dup */
	else if(strcmp(f, "dup") == 0) {
		stack[t][stackptr[t]] = stack[t][stackptr[t] - 1];
		stackptr[t]++;
	}
	/* drop */
	else if(strcmp(f, "drop") == 0)
		stackptr[t]--;
	/* ЗАГРУЗКА И СОХРАНЕНИЕ В ПАМЯТЬ */
	else if(strcmp(f, ".") == 0) {
		stack[t][stackptr[t] - 1] = mem[stack[t][stackptr[t] - 1]];
	}
	else if(strcmp(f, "=") == 0) {
		mem[stack[t][stackptr[t] - 1]] = stack[t][stackptr[t] - 2];
		stackptr[t] -= 2;
	}
	/* strlen */
	else if(strcmp(f, "strlen") == 0) {
		int length = 0;
		for(int i = stack[t][stackptr[t] - 1]; mem[i] != '\0'; i++)
			length++;
		stack[t][stackptr[t] - 1] = length;
	}
	/* getargs */
	else if(strcmp(f, "getargs") == 0) {
		stackptr[t]--;
		for(int i = stack[t][stackptr[t]]; i - stack[t][stackptr[t]] < sizeof(args); i++)
			mem[i] = args[i - stack[t][stackptr[t]]];
	}
	/* sel */
	else if(strcmp(f, "sel") == 0) {
		int num;
		syscall(SYS_getrandom, &num, sizeof(int), NULL); /* нс */
		if(num < 0)
			num = -num;
		stack[t][stackptr[t] - 1] = num % stack[t][stackptr[t] - 1];
	}
	/* sleep и msleep */
	else if(strcmp(f, "sleep") == 0) {
		stackptr[t]--;
		struct timespec ts = {stack[t][stackptr[t]], 0};
		nanosleep(&ts, NULL);
	}
	else if(strcmp(f, "msleep") == 0) {
		stackptr[t]--;
		struct timespec ts = {stack[t][stackptr[t]] / 1000, (stack[t][stackptr[t]] * 1000000) % 1000000000};
		nanosleep(&ts, NULL);
	}
	/****************************/
	else if(strcmp(f, "=?") == 0) {
		stackptr[t] -= 2;
		if(stack[t][stackptr[t]] == stack[t][stackptr[t] + 1])
			stack[t][stackptr[t]++] = 1;
		else
			stack[t][stackptr[t]++] = 0;
	}
	else if(strcmp(f, "!?") == 0) {
		stackptr[t] -= 2;
		if(stack[t][stackptr[t]] != stack[t][stackptr[t] + 1])
			stack[t][stackptr[t]++] = 1;
		else
			stack[t][stackptr[t]++] = 0;
	}
	else if(strcmp(f, "lt?") == 0) {
		stackptr[t] -= 2;
		if(stack[t][stackptr[t]] < stack[t][stackptr[t] + 1])
			stack[t][stackptr[t]++] = 1;
		else
			stack[t][stackptr[t]++] = 0;
	}
	else if(strcmp(f, "gt?") == 0) {
		stackptr[t] -= 2;
		if(stack[t][stackptr[t]] > stack[t][stackptr[t] + 1])
			stack[t][stackptr[t]++] = 1;
		else
			stack[t][stackptr[t]++] = 0;
	}
	
	else if(strcmp(f, "?") == 0) {
		stackptr[t] -= 2;
		if(stack[t][stackptr[t]] == 1 && stack[t][stackptr[t] + 1] == 1)
			stack[t][stackptr[t]++] = 1;
		else
			stack[t][stackptr[t]++] = 0;
	}
	else if(strcmp(f, "|?") == 0) {
		stackptr[t] -= 2;
		if(stack[t][stackptr[t]] == 1 || stack[t][stackptr[t] + 1] == 1)
			stack[t][stackptr[t]++] = 1;
		else
			stack[t][stackptr[t]++] = 0;
	}
	else if(strcmp(f, "!") == 0) {
		stack[t][stackptr[t] - 1] = (stack[t][stackptr[t] - 1] == 0 ? 1 : 0);
	}
	
	else if(strcmp(f, "then") == 0) {
		if(stack[t][stackptr[t] - 2] == 1)
			pc[t] = stack[t][stackptr[t] - 1] - 1;
		stackptr[t] -= 2;
	}
	else if(strcmp(f, "else") == 0) {
		if(stack[t][stackptr[t] - 2] == 0)
			pc[t] = stack[t][stackptr[t] - 1] - 1;
		stackptr[t] -= 2;
	}
	/* cat */
	else if(strcmp(f, "cat") == 0) {
		stackptr[t] -= 6;
		char buf[10];
		snprintf(buf, sizeof(buf), "%d%d%d%d%d%d", stack[t][stackptr[t]], stack[t][stackptr[t] + 1], stack[t][stackptr[t] + 2], stack[t][stackptr[t] + 3], stack[t][stackptr[t] + 4], stack[t][stackptr[t] + 5]);
#ifdef PROG_DBG
        fprintf(stderr, "*** cat result: ``%s``\n", buf);
#endif
		stack[t][stackptr[t]++] = atoi(buf);
	}
	/* halt */
	else if(strcmp(f, "halt") == 0)
		pc[t] = MEM_LEN;
	/* save_stack_ptr и load_stack_ptr */
	else if(strcmp(f, "save_stack_ptr") == 0)
	    mem[stack[t][stackptr[t] - 1]] = stackptr[t] - 1;
	else if(strcmp(f, "load_stack_ptr") == 0)
	    stackptr[t] = mem[stack[t][stackptr[t] - 1]];
	/* setcolor и setbg */
	else if(strcmp(f, "setcolor") == 0 && stack[t][stackptr[t] - 1] == 1 /* белый */)
		printf("\033[97m ");
	else if(strcmp(f, "setcolor") == 0 && stack[t][stackptr[t] - 1] == 2 /* зелёный */)
		printf("\033[92m ");
	else if(strcmp(f, "setcolor") == 0 && stack[t][stackptr[t] - 1] == 3 /* синий */)
		printf("\033[34m ");
	else if(strcmp(f, "setcolor") == 0 && stack[t][stackptr[t] - 1] == 4 /* тёмно-зелёный */)
		printf("\033[32m ");
	else if(strcmp(f, "setcolor") == 0 && stack[t][stackptr[t] - 1] == 5 /* серый */)
		printf("\033[90m ");
	else if(strcmp(f, "setcolor") == 0 && stack[t][stackptr[t] - 1] == 6 /* красный */)
		printf("\033[31m ");
	else if(strcmp(f, "setcolor") == 0 && stack[t][stackptr[t] - 1] == 7 /* жёлтый */)
		printf("\033[93m ");
		
	else if(strcmp(f, "setbg") == 0 && stack[t][stackptr[t] - 1] == 1 /* белый */)
		printf("\033[7m");
	else if(strcmp(f, "setbg") == 0 && stack[t][stackptr[t] - 1] == 2 /* зелёный */)
		printf("\033[42m");
	else if(strcmp(f, "setbg") == 0 && stack[t][stackptr[t] - 1] == 3 /* синий */)
		printf("\033[44m");
	else if(strcmp(f, "setbg") == 0 && stack[t][stackptr[t] - 1] == 6 /* красный */)
		printf("\033[41m");
	/* create_thrd1 */
	else if(strcmp(f, "create_thrd1") == 0) {
		pc[1] = stack[t][--stackptr[t]] - 1;
		pthread_t t1;
		pthread_create(&t1, NULL, create_thread1, NULL);
	}
	
	/********************************
	 ********************************/
    else {
        fprintf(stderr, "Ошибка библиотеки\n");
#ifdef PROG_DBG
        fprintf(stderr, "*** (unknown ``%s``)\n", f);
#endif
        exit(31);
    }
}

#endif
