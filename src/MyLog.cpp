///
/// src/optlogger/MyLog.cpp
///
/// Written by Roberto Bargetto
/// 	   DIGEP
/// 	   Politecnico di Torino
/// 	   Corso Duca degli Abruzzi, 10129, Torino
/// 	   Italy
///
/// Copyright 2023 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
///
/// This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// See the license at http://creativecommons.org/licenses/by-nc-sa/4.0/
///

#include "MyLog.h"


bool init_logging (bool close_it, std::string filename, TLogLevel level, std::string md)
{
    if(close_it)
    {
        fflush(Output2FILE::Stream());
        return fclose(Output2FILE::Stream());
    }
    
    std::stringstream logfnss;
    logfnss << "./" << filename;

    FILELog::ReportingLevel() = level;
    Output2FILE::Stream() = fopen(logfnss.str().c_str(), md.c_str());

    //logging_initialized = true;

	return true;
}
