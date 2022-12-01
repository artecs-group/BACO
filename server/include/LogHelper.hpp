#ifndef _LOG_HELPER_HPP_
#define _LOG_HELPER_HPP_

#include <unistd.h>
#include <string>
#include <cstdio>

/////////////////////////////////////////////////////////////////////////////
// WARNING! ONLY THE FOLLOWING LEVELS ARE SUPPORTED (they are hard coded): //
//     DEBUG                                                               //
//     INFO                                                                //
//     VERBOSE                                                             //
//     WARNING                                                             //
/////////////////////////////////////////////////////////////////////////////

namespace Log
{
    class LogHelper
    {
    public:
	//--------------------------------------------------
	//Access to the class through:
	static LogHelper *Singleton();
	//--------------------------------------------------


	//Log Levels
	enum logLevel_t{
	    INFO_LVL,         /* 0 */
	    DEBUG_LVL,        /* 1 */
	    VERBOSE_LVL,      /* 2 */
	    WARNING_LVL,      /* 3 */
	};

	const char * logLevel_strings[4] = {"[INFO]",
					    "[DEBUG]",
					    "[VERBOSE]",
					    "[WARNING]" };
  
	inline const char* getLevelName(logLevel_t l) {
	    return logLevel_strings[l];
	}
  

	void INFO_MSG(std::string str);
	void INFO_MSG(const char* str);

	void DEBUG_MSG(std::string str);
	void DEBUG_MSG(const char* str);

	void VERBOSE_MSG(std::string str);
	void VERBOSE_MSG(const char* str);

	void WARNING_MSG(std::string str);
	void WARNING_MSG(const char* str);


	void writeLog(logLevel_t logLvl, std::string str);
	void writeLog(logLevel_t logLvl, const char* str);
  
  
	~LogHelper();
    
	LogHelper( const LogHelper& other ) = delete;
	LogHelper& operator=( const LogHelper& other ) = delete;

  
    private:  
	LogHelper();

	void parseLevels();
	void parseLogfile();
  
  
	static LogHelper *s_instance; //singleton pointer

	//Active levels for logging
	bool _debugActive;
	bool _infoActive;
	bool _verboseActive;
	bool _warningActive;

	//istream for writing the log
	FILE* _fd;

	//reference timestamp
	unsigned long long _refTimestamp;
  
    };



    void INFO_MSG(std::string str);
    void INFO_MSG(const char* str);
    void DEBUG_MSG(std::string str);
    void DEBUG_MSG(const char* str);
    void VERBOSE_MSG(std::string str);
    void VERBOSE_MSG(const char* str);
    void WARNING_MSG(std::string str);
    void WARNING_MSG(const char* str);

}// namespace Log
#endif /* _LOG_HELPER_HPP_ */
