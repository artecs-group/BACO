#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_


#include "common/msg.hpp"
#include "common/dynamic_bitset.hpp"
#include "CommunicationManager.hpp"


//Forward declarations:
class Server;
class ClientInfo;
//class CommunicationManager;


class Client
{
public:
    Client() = delete;
    Client(int id, Server *srv);
    ~Client();

    int startCommunication();
    void Run();

    void setClientInfo(ClientInfo *clInfo);

    
    //Process input messages
    int processMsgStart( msg_ack_t *msg);
    int processMsgUnregister( msg_registration_t *msg);
    int processMsgUpdateNTasks( msg_task_stats_t *msg);
    int processMsgSendNewMask( msg_ack_t *msg);
    int processMsgConfirmMask( msg_mask_t *msg);
    int processMsgCoreFree( msg_int_t *msg);
    int processMsgRequestBudget(msg_float2_t *msg);
    

    //Send info to the client
    void sendStopMsg() const;
    void startExecution(dynamic_bitset bt ) const;
    void sendMaskMsg(const dynamic_bitset &bt) const;
    void sendReducedMaskMsg(const dynamic_bitset &bt) const;
    void sendReleaseCoreOrder() const;
    void sendNewBudget(float budget) const;
    
	
private:
    int _id;

    CommunicationManager *_comIn;
    CommunicationManager *_comOut;

    Server *_srv;
    ClientInfo *_info;
};
    

#endif /* _CLIENT_HPP_ */




