#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "jobs.h"

//JOB OBJECT: FUNCTION DEFINITIONS
job *new_job(void) {
	job *unit = (job *) malloc(sizeof(job));
	unit->jid = -1;
	unit->pid = -1;
	unit->cmd_line = (char *) malloc(sizeof(char));
	unit->status = -1;
	return unit;
}

job* get_job(int jid, joblist* list) {
	for(int i = 0; i < list->numJobs; i++) {
		if(list->jobs[i]->jid == jid) {
			return list->jobs[i];
		}
	}
	return NULL;
}

void report_job(job* j) {
	int fmt = 24;
	printf("[%d] %d\n", j->jid+1, j->pid);
}

job *find_job(pid_t pid, joblist *list) {
	for(int i = 0; i < list->numJobs; i++) {
		if(list->jobs[i]->pid == pid) {
			return list->jobs[i];
		}
	}
	return NULL;
}

job *set_job(pid_t pid, char *cmdline, int status, joblist* list) {
	job *j = new_job();
	j->pid = pid;
	j->status = status;
	j->cmd_line = cmdline;
	j->jid = joblist_empty(list) ? 0 : list->jobs[list->numJobs-1]->jid+1;
	list->jobs[list->numJobs++] = j;
	return j;
}

//JOBLIST OBJECT: FUNCTION DEFINITIONS
joblist *new_joblist(void) {
	joblist *list = (joblist *) malloc(sizeof(joblist));
	list->capacity = 10;
	list->jobs = (job **) calloc(list->capacity, sizeof(job*));
	for(int i = 0; i < 10; i++) {
		list->jobs[i] = new_job();
	}
	list->numJobs = 0;
	return list;
}

int joblist_empty(joblist *list) {
	if(list->numJobs == 0) {
		return 1;
	}
	return 0;
}

void free_joblist(joblist *list) {
	if(joblist_empty(list)) {
		free(list);
	}
}

void report_jobs(joblist * list) {
	if(list->numJobs == 0) {
		printf("No jobs running...\n");
		return;
	}
	for(int i = 0; i < list->numJobs; i++) {
		int fmt = 30;
		printf("[%d]+%*s\n", list->jobs[i]->jid+1, fmt, list->jobs[i]->cmd_line);
	}
}

void remove_job(job *j, joblist* list) {
	int i;
	for(i = 0; i < list->numJobs; i++) {
		if(list->jobs[i]->jid == j->jid) {
			break;
		}
	}
	if(i < list->numJobs-1) {
		for(; i < list->numJobs-1; i++) {
			list->jobs[i] = list->jobs[i+1];
		}
		list->numJobs--;
		return;
	}
	list->numJobs--;
}