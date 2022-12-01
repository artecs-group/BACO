#include <iostream>
#include <vector>
#include <queue>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include "client.h"
using namespace std;

#define NUM_CORES 8

void finishComm(int sig){
  SS_finishLibrary();
  exit(0);
}



void runClient(int sleepTime);


int main(){
 
  vector<int> startTime = { 0 };//, 2, 2, 2, 2, 2};
  vector<int> stopTime  = {12,12,12,12,12,12};

  
  int idx=0;
  for(auto it: startTime){
    sleep(it);

    runClient(stopTime[idx++]);
  }

  sleep(50);
  
  return 0;
}


void runClient(int sleepTime)
{
  // if(fork()!=0)
  //   return;
    
  /* CHILDREN */

  signal(SIGINT, finishComm);
  signal(SIGUSR1, finishComm);

  //connect with server
  dynamic_bitset bt;

  int ret;
  
  if((ret=SS_initLibrary())<0){
    cout << "ERROR init " << ret << endl;
    exit(1);
  } else {
    cout << "INIT OK" << endl;
  }
  
    
  
    
  if((ret=SS_startExecution(&bt))<0){
    cout << "ERROR start " << ret << endl;
    exit(1);
  } else {
    cout << "Start OK" << endl;
  }
  

  char a;
  int res;
  while(1){
      cin >> a;


      res=SS_needToFreeCore();

      if(res>0){
	  int pos = bt.find_next(-1);
	  bt.flip(pos);
	  SS_informFreeCore(pos);

	  cout << "A: " << pos << endl;
      } else {
	  SS_askForNewCpuMask(&bt);
	  cout << "B: " << bt.to_string() << endl;
      }
  }
  
  
  // int a;
  // while(1){
  //   cin >> a;
  //   cout << "kk" << endl;
    
  //   SS_askForNewCpuMask(&bt);

  //   cout << bt.to_string() << endl;
  // }

  //Disconnect from server
  // int a;
  // cin >> a;
  // SS_finishLibrary();

  
  exit(0);
}
