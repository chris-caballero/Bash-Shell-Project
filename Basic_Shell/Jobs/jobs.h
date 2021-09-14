#ifndef JOBS_H
#define JOBS_H

typedef struct {
	int jid;
	pid_t pid;
	char *cmd_line;
	int status;
} job;

typedef struct {
	int numJobs;
	int capacity;
	job **jobs;
} joblist;


//Constructor
job *new_job(void);
//Accessor and Utility Functions
job* get_job(int jid, joblist* list);
void report_job(job* j);
//Mutator/Setter Function
job *set_job(pid_t pid, char *cmdline, int status, joblist* list);

//Constructor
joblist *new_joblist(void);
//Accessor and Utility Functions
int joblist_empty(joblist *list);
void free_joblist(joblist *list);
void report_jobs(joblist * list);
job *find_job(pid_t pid, joblist *list);
//Mutator Function
void remove_job(job *j, joblist *list);

#endif