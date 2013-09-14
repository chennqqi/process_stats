#include<stdio.h>
#include<unistd.h>
#include"./codelearnmonitor.h"

void add_process(struct cur_process *cp){
  HASH_ADD_INT(processes, pid, cp);
}

struct cur_process *find_process(int pid ){
  struct cur_process *cp;
  HASH_FIND_INT(processes, &pid, cp);
  return cp;
}

void delete_process(struct cur_process *process){
  HASH_DEL(processes, processes);
}

time_t getCurrentTime(){
  time_t now;
  time(&now);
  return now;
}

void process_processes(){
  PROCTAB* proc  = openproc(PROC_FILLMEM | PROC_FILLUSR | PROC_FILLSTAT | PROC_FILLSTATUS);
  proc_t proc_info;
  memset(&proc_info, 0, sizeof(proc_info));
  while(readproc(proc, &proc_info) != NULL) {
   struct cur_process *cpp = find_process(proc_info.tid);
   if(cpp != NULL && proc_info.tid == 3124){
     time_t prev_time = cpp->timestamp;
     time_t cur_time = getCurrentTime();
     double diff_in_seconds = difftime(cur_time,prev_time);
    //printf("Difference is seconds %f \n", diff_in_seconds );
     long unsigned int pid_diff = (proc_info.utime + proc_info.stime) - (cpp->utime + cpp->stime);
     //printf("%fs - %lu %% \n", diff_in_seconds, pid_diff);
     double cpu_percentage = pid_diff / diff_in_seconds;
     unsigned memory = proc_info.vm_size;
     printf("%5d\t%20s\t%20s\t%.f\t%5u\n",proc_info.tid, proc_info.suser, proc_info.cmd, cpu_percentage,memory);
/*     if(memory >= 1028*10)
     printf("Memory has increased by %5d,%20s,%20s,%5u\n",proc_info.tid, proc_info.suser, proc_info.cmd,memory);
*/
   }

   struct cur_process *cp = malloc(sizeof(struct cur_process));
   cp->pid = proc_info.tid;
   cp->cutime = proc_info.cutime;
   cp->cstime = proc_info.cstime;
   cp->utime = proc_info.utime;
   cp->stime = proc_info.stime;
   cp->user = proc_info.suser;
   cp->command = proc_info.cmd;
   cp->vm_size = proc_info.vm_size;
   cp->timestamp = getCurrentTime();
   add_process(cp);
   // printf("%5d\t%20s:\t%5lld\t%5lu\t%s\n",proc_info.tid, proc_info.cmd, proc_info.utime + proc_info.stime, proc_info.vm_size, proc_info.suser);
 }
  closeproc(proc);
}

int main(int argc, char** argv){
  int counter = 10;
  while(1){
    process_processes();
    sleep(1);
  }
}
