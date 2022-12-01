#ifndef INSTR
//If INSTR is not enable, generate an empty obj file
#else

///////////////////////////////////////////////////////////////////////////////////////////////////////
#warning " #####=================== TODO: CHECK EXTRAE FOR MTH ===================#######"
///////////////////////////////////////////////////////////////////////////////////////////////////////


#include "ExtraeInstrumentation.hpp"

#include <string.h>
#include <string>
#include <cassert>

// Allocating and initializing GlobalClass's static data members.
// The pointer is being allocated - not the object inself.
ExtraeInstrumentation *ExtraeInstrumentation::s_instance = 0;
ExtraeInstrumentation::~ExtraeInstrumentation()
{
    Extrae_fini();
    delete s_instance;
}

ExtraeInstrumentation *ExtraeInstrumentation::Singleton()
{
    if (!s_instance)
	s_instance = new ExtraeInstrumentation();
    return s_instance;
}


ExtraeInstrumentation::ExtraeInstrumentation()
{
    // By default, all levels are active
    _levels=0xffff;
    char* levels = getenv("TRACE_LEVEL");

    if(levels!=NULL){
	char *levelid = strtok(levels, ",");
    
	while(levelid != NULL){
	    _levels |= (1 << atoi(levelid));
	    levelid = strtok(NULL, ",");
	}
    }
}


unsigned int ExtraeInstrumentation::addEventName(std::string eventName)
{
    if( _levels == 0 ) return -1;

    int nvalues = 2;
    extrae_value_t values[2]    = {0, 1}; //valores que puede tomar el evento.
    char *description_values[2] = {const_cast<char*>("End"),
				   const_cast<char*>("Begin") };
  
    _eventsNames.insert(
	std::pair<std::string, unsigned int>(eventName, ++_eventOffset));
  
    char *c_event_name = const_cast<char*>(eventName.c_str());

  
    Extrae_define_event_type( &_eventOffset, c_event_name,
			      (unsigned int*)&nvalues, values, description_values);


    return _eventOffset;
}



void ExtraeInstrumentation::emitEvent(std::string eventName,
				      extrae_value_t value)
{
    assert(value==0 || value==1);

    auto it = _eventsNames.find(eventName);

    unsigned int eventId = (it==_eventsNames.end()) ?
	this->addEventName(eventName) :
	it->second;
  
    Extrae_event(eventId, value);
  
}


void ExtraeInstrumentation::emitEventInLevel(int level,
					     std::string eventName,
					     extrae_value_t value)
{
    if((_levels & (1<<level)) != 0)
	this->emitEvent(eventName, value);  
}
   


#endif
