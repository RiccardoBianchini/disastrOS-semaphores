#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "linked_list.h"
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int sem_fd = running->syscall_args[0];

  SemDescriptor* sem_desc = SemDescriptorList_byFd(&(running->sem_descriptors), sem_fd);
  if(!sem_desc){
    running->syscall_retvalue = DSOS_ESEMAPHOREWAIT;
    return;
  }
  printf("Pid %d requesting waitSemaphore on semaphore %d\n", running->pid, sem_desc->semaphore->id);
  Semaphore* sem = sem_desc->semaphore;

  sem->count-=1;
  if(sem->count < 0){
    SemDescriptorPtr* sem_desc_ptr = SemDescriptorPtr_alloc(sem_desc);
    List_insert(&(sem->waiting_descriptors), sem->waiting_descriptors.last, (ListItem*)sem_desc_ptr);
    running->status = Waiting;
    List_insert(&waiting_list, waiting_list.last, (ListItem*)running);
    //In theory it should do this
    //while(!ready_list.size);
  }
  running->syscall_retvalue=0;
}
