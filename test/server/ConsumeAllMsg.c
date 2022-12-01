/**^Consumes all the messages remaining in the queue, ans hows some related info */


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "msg.hpp"


int main(int argc, char** argv){
  int key_id;
  char key_path[80];

  if(argc == 3){
    key_id = atoi(argv[1]);
    strcpy(key_path, argv[2]);
  }else{
    key_id = SYSV_KEY_NUMBER;
    strcpy(key_path, SYSV_KEY_PATH);
  }


  int _fd;
  key_t key_msg;
  //A Get the queue key
  if((key_msg=ftok(key_path, key_id))==-1){
//    DIE(__FILE__ ": Error getting the queue key");
  }
  //B) Get the queue fd
  if((_fd=msgget(key_msg, 0600|IPC_CREAT))==-1){
//    DIE(__FILE__ ": Error getting the queue fd");
  }

  msg_raw_t msg;
  int retval;
  while((retval=msgrcv(_fd, &msg, sizeof(struct msg_raw_t) - sizeof(long), 0, IPC_NOWAIT)) > 0){
    printf("Recibido msg con id: %ld y tam %i\n", msg.type, retval);
  }

  
  return 0;
}
