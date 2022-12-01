#include "GlobalState.hpp"

#include <unistd.h>

CommunicationManager* GlobalState::comm = nullptr;
int GlobalState::id = -1;
long long GlobalState::last_cores_timestamp = 0;
int GlobalState::numCores = sysconf(_SC_NPROCESSORS_ONLN);
