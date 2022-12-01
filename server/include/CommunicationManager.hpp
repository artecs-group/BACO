#ifndef _COMMUNICATION_MANAGER_HPP_
#define _COMMUNICATION_MANAGER_HPP_

#include "common/msg.hpp"
#include <cstdio>
class CommunicationManager
{
public:
    CommunicationManager()
    {
	puts("NO DEBERIA ESTAR AQUI");
    }
    
	    
    CommunicationManager(int key_id, const char *key_path);
    ~CommunicationManager();

    int close();
    int getMsg(void *msg, message_t msgType, bool wait=true);
    int sendMsg(int size, void *msg, bool wait=true);

private:
    int _fd;

    ONLY_DBG( int _clientId );
};



#endif /* _COMMUNICATION_MANAGER_HPP_ */
