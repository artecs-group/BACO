#ifndef _GLOBAL_STATE_HPP_
#define _GLOBAL_STATE_HPP_

/* La idea es mantener aquí todas las cosas globales a la librería, pero no 
   exponerlas al usuario a través de los include
*/

#include "CommunicationManager.hpp"

class GlobalState
{
public:
    static CommunicationManager *comm;
    static int id;
    static long long last_cores_timestamp;
    static int numCores;
};

#endif /* _GLOBAL_STATE_HPP_ */

