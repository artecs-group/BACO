#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>


#include "msg.hpp"


int main(){
  key_t key_msg;
  int fd, fd2, retval;
  
  struct msg_registration_t msg;
  
  //A) Get the key
  if((key_msg=ftok(SYSV_KEY_PATH, SYSV_KEY_NUMBER))==-1){
    perror(__FILE__ ": Error creating the key");
    return EXIT_FAILURE;
  }

  //B) Create the queue
  if((fd=msgget(key_msg, 0600|IPC_CREAT))==-1){
    perror(__FILE__": Error creating the queue");
    return EXIT_FAILURE;
  }

  //C) Read the message
  retval=msgrcv(fd, &msg, sizeof(struct msg_registration_t) - sizeof(long), (enum message_t)registration, 0);
  if(retval==-1)
    perror(__FILE__ ": Error while reading the queue\n");
  else 
    fprintf(stdout, "Registrado cliente con id %d, timestamp: %llu\n", msg.id, msg.timestamp);
  

  //D) Send the ack message to the other queue
  int clientId = msg.id;


  //D.1 -> open the queue
  if((key_msg=ftok(SYSV_KEY_PATH, clientId))==-1){
    perror(__FILE__ ": Error creating the key");
    return EXIT_FAILURE;
  }
  if((fd2=msgget(key_msg, 0600|IPC_CREAT))==-1){
    perror(__FILE__": Error opening the queue");
    return EXIT_FAILURE;
  }


  //D.2 -> send the message
  enum message_t t =  ack;
  struct msg_ack_t ack;
  ack.type = t;
  if(msgsnd(fd2, (void *)&ack, sizeof(struct msg_ack_t) - sizeof(long), IPC_NOWAIT)<0){
    perror(__FILE__ ": Error en viando mensaje de ack\n");
    return EXIT_FAILURE;
  }
  

  // E) Wait for the finishing message
  retval=msgrcv(fd, &msg, sizeof(struct msg_registration_t) - sizeof(long), (enum message_t)unregistration, 0);
  if(retval==-1)
    perror(__FILE__ ": Error while reading the queue\n");
  else 
    fprintf(stdout, "UnRegistrado cliente con id %d, timestamp: %llu\n", msg.id, msg.timestamp);

  
  //F) Close the queue
  if(msgctl(fd, IPC_RMID, (struct msqid_ds *)NULL) == -1){
    perror(__FILE__ ": Error closing the queue");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
