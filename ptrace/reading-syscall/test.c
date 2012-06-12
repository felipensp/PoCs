#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <stdlib.h>
#include <string.h>

/* Obtido em /usr/include/asm/ptrace-abi.h */
#define RAX 80
#define RSI 104

const int long_size = sizeof(long);

void getdata(pid_t child, long addr, char *str, int len) {
	char *laddr = str;
	int i = 0, j = len / long_size;
	int is_exact = len % long_size;
	long val;
	
	while (i <= j) {
		if (i == j && is_exact == 0) {
			break;
		}
		
		/* Lendo dados do processo filho */
		val = ptrace(PTRACE_PEEKDATA, child, addr + i * long_size, NULL);
		
		memcpy(laddr, &val, i == j ? (len % long_size) : long_size);		
		++i;
		laddr += long_size;
	}

	str[len] = '\0';
}

int main(int argc, char **argv) {
	pid_t child;
	long orig_rax, eax;
	struct user_regs_struct regs;
	int status, insyscall = 0;

	child = fork();
	
	if (child == 0) {
		/* Diz ao kernel que o processo passará a ser "traceado" */
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("./foo", NULL, NULL);
	} else {
		while(1) {
			/* Aguarda pela execução da syscall sys_execve */
			wait(&status);

			if (WIFEXITED(status)) {
				break;
			}
			/* Obtendo os valores dos registradores usados na syscall */
			ptrace(PTRACE_GETREGS, child, 0, &regs);

			if (regs.orig_rax == __NR_write) {
				if (insyscall == 0) {
					long val;

					/* Endereço da string como argumento da sys_write() */
					val = ptrace(PTRACE_PEEKUSER, child, RSI, NULL);
					
					char *str = malloc(regs.rbx + 1 * regs.rdi);
					
					getdata(child, val, str, regs.rbx);					
					printf("arg: '%s'\n", str);
					
					free(str);
									
					insyscall = 1;
				} else {
					/* Obtem o valor retornado pela syscall */
					eax = ptrace(PTRACE_PEEKUSER, child, RAX, NULL);
					printf("Write returned with %ld\n", eax);
					insyscall = 0;
				}				
			}
			/* Indicamos que o kernel deve parar o processo filho a cada 
			 * syscall ou exit */
			ptrace(PTRACE_SYSCALL, child, NULL, NULL);
		}
	}
	return 0;
}
