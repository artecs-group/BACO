#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common/dynamic_bitset.hpp"

#ifdef __cplusplus  
extern "C" {  
#endif
    
    int SS_initLibrary(void);
    int SS_finishLibrary(void);

/* Blocks until start message is received */
    int SS_startExecution(dynamic_bitset *bt);

    int SS_needToFreeCore();
    int SS_informFreeCore(int coreId, dynamic_bitset *newMask);

    int SS_updateNtasks(int nCrit, int nNotCrit);

    
    int SS_askForNewCpuMask(dynamic_bitset *bt);
    int SS_askForNewCpuMask_noBlock(dynamic_bitset *bt);
    //int SS_confirmMaskApplied(dynamic_bitset *bt);

    /* int SS_confirmCoreFree(int nCore); */
    /* int SS_confirmCoresFree(dynamic_bitset *bt); */

    /* int SS_confirmCoreOccupied(int nCore); */
    /* int SS_confirmCoresOccupied(dynamic_bitset *bt); */



    /**
     * PowerBudget stuff
     **/
    float SS_requestNewBudget(float currentBudget, float desiredBudget);
    
    

    
    
    
    
    
#ifdef __cplusplus
}
#endif

#endif /* _CLIENT_H_ */

