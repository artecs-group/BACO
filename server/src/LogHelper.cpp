#include "LogHelper.hpp"
#include <cstring>
#include <sstream>

#include "common/utils.hpp"


// Allocating and initializing GlobalClass's static data members.
// The pointer is being allocated - not the object inself.
namespace Log{
LogHelper *LogHelper::s_instance = 0;

LogHelper *LogHelper::Singleton()
{
    if (!s_instance)
	s_instance = new LogHelper();
    return s_instance;
}

//--------- Constructors/destructors -----------------
LogHelper::~LogHelper()
{
    this->VERBOSE_MSG("Closing logFile");
    fclose(_fd);
}


LogHelper::LogHelper()
{
    this->parseLevels();
    this->parseLogfile();
  
    _refTimestamp = getTimeStamp();

    this->VERBOSE_MSG("Opening logFile");
}


void LogHelper::parseLogfile(){
    char* logfile = getenv("LOG_FILE");
    if(logfile == NULL)
	_fd = stdout;
    else{
	_fd = fopen(logfile, "a");  
	if(_fd == NULL){
	    DIE("Error al abrir el fichero de log");
	}
    }
}


void LogHelper::parseLevels()
{
    // By default, all levels are off, unless DEBUG is active

#ifdef DEBUG
    _debugActive = _infoActive = _verboseActive = _warningActive = true;
#else
    _debugActive = _infoActive = _verboseActive = _warningActive = false;
#endif


    char* levels = getenv("LOG_LEVEL");
  
    if(levels!=NULL){
	char *levelid = strtok(levels, ",");
    
	while(levelid != NULL){
	    if(strcmp(levelid, "INFO")==0)
		_infoActive = true;
	    else if(strcmp(levelid, "DEBUG")==0)
		_debugActive = true;
	    else if(strcmp(levelid, "VERBOSE")==0)
		_verboseActive = true;
	    else if(strcmp(levelid, "WARNING")==0)
		_warningActive = true;
      
	    levelid = strtok(NULL, ",");
	}
    }
}

//-------------- Logging methods -----------------
void LogHelper::writeLog(logLevel_t logLvl, const char* str)
{
    switch(logLvl){
    case INFO_LVL:
	if(!_infoActive) return;
	break;
    case DEBUG_LVL:
	if(!_debugActive) return;
	break;
    case VERBOSE_LVL:
	if(!_verboseActive) return;
	break;
    case WARNING_LVL:
	if(!_warningActive) return;
	break;
    }
    
    unsigned long long time = getTimeStamp() - _refTimestamp;  
    fprintf(_fd, "%s,%llu,%s\n", getLevelName(logLvl), time, str);
    fflush(_fd);
}

void LogHelper::writeLog(logLevel_t logLvl, std::string str)
{
    this->writeLog(logLvl, str.c_str());
}



void LogHelper::INFO_MSG(const char* str)
{
    this->writeLog(LogHelper::logLevel_t::INFO_LVL, str);
}

    
void LogHelper::DEBUG_MSG(const char* str)
{
    this->writeLog(LogHelper::logLevel_t::DEBUG_LVL, str);
}
    
    
void LogHelper::VERBOSE_MSG(const char* str)
{
    this->writeLog(LogHelper::logLevel_t::VERBOSE_LVL, str);
}
    
void LogHelper::WARNING_MSG(const char* str)
{
    this->writeLog(LogHelper::logLevel_t::WARNING_LVL, str);
}
    

void LogHelper::INFO_MSG(std::string str){
    this->INFO_MSG(str.c_str());
}
void LogHelper::DEBUG_MSG(std::string str){
    this->DEBUG_MSG(str.c_str());
}
void LogHelper::VERBOSE_MSG(std::string str){
    this->VERBOSE_MSG(str.c_str());
}
void LogHelper::WARNING_MSG(std::string str){
    this->WARNING_MSG(str.c_str());
}


    



//--------- Non member functions -----------------
void INFO_MSG(std::string str){
    LogHelper::Singleton()->INFO_MSG(str);
}

void INFO_MSG(const char* str){
    LogHelper::Singleton()->INFO_MSG(str);
}

void DEBUG_MSG(std::string str){
    LogHelper::Singleton()->DEBUG_MSG(str);
}

void DEBUG_MSG(const char* str){
    LogHelper::Singleton()->DEBUG_MSG(str);
}

void VERBOSE_MSG(std::string str){
    LogHelper::Singleton()->VERBOSE_MSG(str);
}

void VERBOSE_MSG(const char* str){
    LogHelper::Singleton()->VERBOSE_MSG(str);
}

void WARNING_MSG(std::string str){
    LogHelper::Singleton()->WARNING_MSG(str);
}

void WARNING_MSG(const char* str){
    LogHelper::Singleton()->WARNING_MSG(str);
}
  
}
