#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include <pthread.h>

#include "msg.hpp"


/*
 * IDEA: El programa principal va a escuchar continuamente el registro de nuevos clientes.
 * Por cada cliente, va a crear un hilo encargado de comunicarse con el cliente
 * por la nueva cola específica para ello. Cuando el cliente envía la señal de 
 * fin, el hilo finaliza.
 */


void* manageClient(void* args);

int main(){
  key_t key_msg;
  int fd;
  
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


  //C) Read continuously for new clients
  struct msg_registration_t msg;
  while(1){
    if(msgrcv(fd, &msg, sizeof(struct msg_registration_t) - sizeof(long), (enum message_t)registration, 0)==-1){
      perror(__FILE__ ": Error while reading the queue\n");
    }else {
      fprintf(stdout, "Registrado cliente con id %d, timestamp: %llu\n", msg.id, msg.timestamp);
      pthread_t th;
      pthread_create(&th, NULL, manageClient, &msg.id);
    }
  }
  
  
  //F) Close the queue
  if(msgctl(fd, IPC_RMID, (struct msqid_ds *)NULL) == -1){
    perror(__FILE__ ": Error closing the queue");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}



void* manageClient(void* args){
  key_t key_msg;
  int fd2;
  int clientId = *((int *)args);
  struct msg_registration_t msg;
  
  //D.1 -> open the queue
  if((key_msg=ftok(SYSV_KEY_PATH, clientId))==-1){
    perror(__FILE__ ": Error creating the key");
  }
  if((fd2=msgget(key_msg, 0600|IPC_CREAT))==-1){
    perror(__FILE__": Error opening the queue");
  }
  
  //D.2 -> send the message
  enum message_t t =  ack;
  struct msg_ack_t ack;
  ack.type = t;
  if(msgsnd(fd2, (void *)&ack, sizeof(struct msg_ack_t) - sizeof(long), IPC_NOWAIT)<0){
    perror(__FILE__ ": Error en viando mensaje de ack\n");
  }
  
  
  // E) Wait for the finishing message
  if(msgrcv(fd2, &msg, sizeof(struct msg_registration_t) - sizeof(long), (enum message_t)unregistration, 0)==-1)
    perror(__FILE__ ": Error while reading the queue\n");
  else 
    fprintf(stdout, "UnRegistrado cliente con id %d, timestamp: %llu\n", msg.id, msg.timestamp);


  return NULL;
}
