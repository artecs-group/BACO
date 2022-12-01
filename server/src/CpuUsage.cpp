#include "CpuUsage.hpp"

//#include <cstdlib>
//#include <unistd.h>
#include <sys/sysinfo.h>

#include <thread>
#include <unistd.h>

CpuUsage::CpuUsage() : _nCpus(get_nprocs()), _sleep(500000)
{
    _fd = fopen("/proc/stat", "r");

    _prev_idle = new int[_nCpus];
    _prev_total = new int[_nCpus];
    _cpuUsages = new float[_nCpus];
}
    
CpuUsage::~CpuUsage()
{
    fclose(_fd);

    delete[] _prev_idle;
    delete[] _prev_total;
    delete[] _cpuUsages;
}



float CpuUsage::getGlobalCpuUsage()
{
    this->reloadStats();
    return _cpuGlobalUsage;
}


float* CpuUsage::getCpuUsagePerCpu()
{
    this->reloadStats();
    return _cpuUsages;
}


void CpuUsage::START()
{
    _stop=false;
    std::thread t(&CpuUsage::RUN, this);
    t.detach();
}

void CpuUsage::STOP()
{
    _stop=true;
    usleep(2*_sleep);
}
    
//------------------------------------------------------------------------------

void CpuUsage::RUN()
{
    while(!_stop){
	this->reloadStats();
	this->LOG();

	usleep(_sleep);
    }
}


void CpuUsage::LOG()
{
    static int counter=1;

    printf("cpuGlobal(%d) = %f;\n", counter, _cpuGlobalUsage);

    printf("cpuUsage(%d, :) = [ ", counter);
    for(int i=0; i<_nCpus; i++){
	printf("%f ", _cpuUsages[i]);
    }
    printf("];\n");
    counter++;
}


void CpuUsage::reloadStats()
{
    fflush(_fd);
    fseek(_fd, 0, SEEK_SET);
    
    /* A) Global stats */
    unsigned user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    
    if(fscanf(_fd, "%*s %u %u %u %u %u %u %u %u %u %u", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice)<0)
	perror("Error reading /proc/stat file");

    unsigned total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
	
    int diff_total = total - _prev_total_global;
    int diff_idle  = idle  - _prev_idle_global;
	
    _cpuGlobalUsage = ((float)(diff_total - diff_idle)) / ((float)diff_total);
    
    _prev_total_global = total;
    _prev_idle_global = idle;

    /* B) Specific stats */
    for(int i=0; i<_nCpus; i++){
	fscanf(_fd, "%*s %u %u %u %u %u %u %u %u %u %u", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

	unsigned total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
	
	int diff_total = total - _prev_total[i];
	int diff_idle  = idle  - _prev_idle[i];
	
	_cpuUsages[i] = ((float)(diff_total - diff_idle)) / ((float)diff_total);
    
	_prev_total[i] = total;
	_prev_idle[i] = idle;
    }
}
