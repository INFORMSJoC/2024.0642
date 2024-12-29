///
/// include/optlogger/MyLog.h
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

#ifndef MY_LOG_H_
#define MY_LOG_H_

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <tuple>
#include "log.h"


typedef std::tuple<unsigned int, unsigned int, std::string> OptLogMeta;

template<class... Ts> 
class OptLogger
{
public:
    OptLogger();
    OptLogger(const std::vector<std::tuple<unsigned int, unsigned int, std::string>>& cdim_hdr);
    
    void set(const std::vector<std::tuple<unsigned int, unsigned int, std::string>>& cdim_hdr);
    
    std::string getFormatedHeader();
    std::string getFormatedLine(const std::tuple<Ts...>&);
    
private:
    std::vector<std::tuple<unsigned int, unsigned int, std::string>> coldim_header;
};

template<class... Ts> 
OptLogger<Ts...>::OptLogger()
{
}

template<class... Ts> 
OptLogger<Ts...>::OptLogger(const std::vector<std::tuple<unsigned int, unsigned int, std::string>>& cdim_hdr)
{
    coldim_header = (cdim_hdr);
}

template<class... Ts> 
void OptLogger<Ts...>::set(const std::vector<std::tuple<unsigned int, unsigned int, std::string>>& cdim_hdr)
{
    coldim_header = (cdim_hdr);
}

template<class... Ts> 
std::string OptLogger<Ts...>::getFormatedHeader()
{
    std::stringstream ss;
    for(unsigned int i = 0; i < coldim_header.size(); i++)
    {
        ss << std::setw(std::get<0>(coldim_header[i])) << std::get<2>(coldim_header[i]);
    }

    return ss.str();
}

// Function to iterate through all values
// I equals number of values in tuple
template <size_t I = 0, typename... Ts>
typename std::enable_if<I == sizeof...(Ts),
                   void>::type
printTuple(std::stringstream& ss,const std::vector<std::tuple<unsigned int, unsigned int, std::string>>& cdim_hdr, std::tuple<Ts...> tup)
{
    // If iterated through all values
    // of tuple, then simply return.
    return;
}
 
template <size_t I = 0, typename... Ts>
typename std::enable_if<(I < sizeof...(Ts)),
                   void>::type
printTuple(std::stringstream& ss, const std::vector<std::tuple<unsigned int, unsigned int, std::string>>& cdim_hdr, std::tuple<Ts...> tup)
{
    // Print element of tuple
    ss << std::setw(std::get<0>(cdim_hdr[I])) << std::setprecision(std::get<1>(cdim_hdr[I])) << std::fixed << std::get<I>(tup);
 
    // Go to next element
    printTuple<I + 1>(ss, cdim_hdr, tup);
}
 
template<class... Ts> 
std::string OptLogger<Ts...>::getFormatedLine(const std::tuple<Ts...>& tupline)
{
    std::stringstream ss;
    printTuple(ss, coldim_header, tupline);

    return ss.str();
}

/*
#define LOG_INFO(msg) \
{ \
	(FILE_LOG(logINFO) << "<" << _FILE_ << ":" << _LINE_ << "> " << msg),\
	(void)0\
}
*/

//#define FILE_AND_LINE "<" << _FILE_ << ":" << _LINE_ << "> "

//bool logging_initialized = false;

bool init_logging(bool close_it, std::string filename, TLogLevel level = logINFO, std::string md = "w");

#endif /* MY_LOG_H_ */
