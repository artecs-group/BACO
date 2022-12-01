#ifndef _CPU_USAGE_HPP_
#define _CPU_USAGE_HPP_

#include <cstdio>

/**
 ** Class in charge of calculate the load of each core in the system.
 ** The value returned represents the load between two consecutives calls
 **/

class CpuUsage{
public:
    CpuUsage();
    ~CpuUsage();
    float getGlobalCpuUsage();
    float* getCpuUsagePerCpu();

    void START();
    void STOP();

    
private:
    const int _nCpus;
    FILE *_fd;
    int _sleep;

    // Info per core
    int *_prev_idle;
    int *_prev_total;

    // Info global
    int _prev_idle_global;
    int _prev_total_global;

    float _cpuGlobalUsage;
    float *_cpuUsages;


    //OJO: the cursor of _fd has to be pointing to the
    //     beginning of the line before calling to this functions.
    void reloadStats();

    bool _stop;
    void RUN();
    void LOG();
};

#endif /* _CPU_USAGE_HPP_ */
