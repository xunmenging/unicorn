#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "util.h"
#include "cgroup.h"
#include "mount.h"

#define STACK_SIZE (1024 * 1024)
static char child_stack[STACK_SIZE];

char* const child_args[] = {
	"/bin/bash",
	NULL
};


char * const rootfs_base="/tmp/rootfs";
char * const cgroup_base="/tmp/cgroup";
char * const mount_base="/tmp/unicorn";
char * const pivot_old=".pivot_old";


int pipes[2];
int child_main(void* arg)
{
	char * unicorn_id = (char *)arg;
    printf("in child main\n");
	char c;

	close(pipes[1]);   
	
	printf(" -[%5d] World ! %s \n", getpid(), unicorn_id);

	pivot_move(mount_base, unicorn_id);

	char hostname[20];
    sprintf(hostname,"unicorn-%s",unicorn_id);
	sethostname(hostname, 20);

    read(pipes[0],&c,1);
	check_result(execv(child_args[0], child_args), "execv");
	printf("Ooops\n");
	return 1;
}

int main()
{
	cgroup_init(cgroup_base);
	mount_init(mount_base);
        
	pipe(pipes);

	printf(" -[%5d] Hello ?\n",getpid());

	char * unicorn_id = malloc(11);
    random_string(unicorn_id, 10);
	prepare_mount(mount_base, unicorn_id, rootfs_base);

	int child_pid = clone(child_main, child_stack+STACK_SIZE, CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, unicorn_id);
	cgroup_add(child_pid);

	sleep(1);
	close(pipes[1]);
	waitpid(child_pid, NULL, 0);
	return 0;
}