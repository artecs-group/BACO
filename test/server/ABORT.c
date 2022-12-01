/** Envía un mensaje de abort al servidor */

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include <stdio.h>
#include <errno.h>
#include "msg.hpp"

/* Variables globales */
int _fd;

/* Prototipos de funcion */
void init();
void sendAbort();
int sendMsg(int size, void *msg);


int main(){
  init();
  sendAbort();

  return 0;
}



void init(){
  _fd = -1;

  key_t key_msg;
  //a) Get the queue key
  if((key_msg=ftok(SYSV_KEY_PATH, SYSV_KEY_NUMBER))==-1){
    perror(__FILE__ ": Error getting the queue key");
  }

  //b) Get the queue fd
  if((_fd=msgget(key_msg, 0600))==-1){
    perror(__FILE__ ": Error getting the queue fd");
  }
}


void sendAbort(){
  int retval;

  msg_info_t msg;
  initStopMsg(&msg);

  if((retval=sendMsg(sizeof(struct msg_registration_t), (void *)&msg)) < 0)
    perror("Error al enviar el mensaje");
}


int sendMsg(int size, void *msg){
  int msgflag = 0;
  
  int retval = msgsnd(_fd, msg, size - sizeof(long), msgflag);

#ifdef DEBUG
  if(retval==-1){
    switch(errno){
    case EACCES: puts("EACCESS"); break;
    case EAGAIN: puts("EAGAIN"); break;
    case EFAULT: puts("EFAULT"); break;
    case EIDRM:  puts("EIDRM"); break;
    case EINTR:  puts("EINTR"); break;
    case EINVAL: puts("EINVAL"); break;
    case ENOMEM: puts("ENOMEM"); break;
    }
  }
#endif

  return retval;
}
