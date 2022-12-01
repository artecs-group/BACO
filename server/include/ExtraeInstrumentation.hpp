#ifndef _EXTRAE_INSTRUMENTATION_HPP_
#define _EXTRAE_INSTRUMENTATION_HPP_



#ifndef INSTR
//There is not instrumentation methods if INSTR is not enabled
#else

#include <map>
#include "extrae.h"


class ExtraeInstrumentation
{
public:
    //--------------------------------------------------
    //Access to the class through:
    static ExtraeInstrumentation *Singleton();
    //--------------------------------------------------

    // Add a new name to the events dictionary.  
    unsigned int addEventName(std::string eventName);


    // Emits an event. It's not necessary to register the name before
    void emitEvent(std::string eventName, extrae_value_t value);

    // Emits an event only if the level is active. It's not necessary to register the name before
    void emitEventInLevel(int level, std::string eventName, extrae_value_t value);

    ~ExtraeInstrumentation();
    ExtraeInstrumentation( const ExtraeInstrumentation& other ) = delete;
    ExtraeInstrumentation& operator=( const ExtraeInstrumentation& other ) = delete;
  
private:
    static ExtraeInstrumentation *s_instance; //singleton pointer
    ExtraeInstrumentation();


  
    unsigned int _levels; //bitmask of active levels

    //Related with event names and ids
    unsigned int _eventOffset=4000;
    std::map<std::string, unsigned int> _eventsNames;
};


#endif //ifdef instr
#endif /* _EXTRAE_INSTRUMENTATION_HPP_ */
