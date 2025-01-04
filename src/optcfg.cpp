///
/// src/utility/optcfg.cpp
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

#include <stdexcept>
#include <fstream>
#include <sstream>

#include "optcfg.h"

void optcfg::read(std::string cfgfname)
{
    /// open input file
    std::ifstream ifs(cfgfname);
    
    if(!ifs.is_open())
    {
        throw std::runtime_error("File not found");
    }
    
    /// read graph data
    std::string algo_str;
    ifs >> algo_str;
    opt_algo = getAlg(algo_str);
    ifs >> alg_mode;
    ifs >> intp0;
    ifs >> intp1;
    ifs >> timelimsec;
    ifs >> dblp0;
    
    return;
}

std::string getAlg(optcfg::Algo opta)
{
    switch(opta)
    {
        case optcfg::Algo::LP: return std::string("lp");
        case optcfg::Algo::TS: return std::string("ts");
        default: return std::string("none");
    }
}

optcfg::Algo getAlg(std::string algstr)
{
    if(algstr == "lp")
    {
        return optcfg::Algo::LP;
    }
    else if(algstr == "ts")
    {
        return optcfg::Algo::TS;
    }
    else
    {
        throw std::invalid_argument("Unknown algorithm '" + algstr + "'");
    }
}
    
/// function implementation
std::string log_opt_config(optcfg ocfg)
{
    std::stringstream ss;
    ss << "Opt. config.:\n";
    ss << "\t\t\tOptimization algorithm: " << getAlg(ocfg.opt_algo) << std::endl;
    ss << "\t\t\tAlgorithm mode: " << ocfg.alg_mode << std::endl;
    ss << "\t\t\tMax mem for solver: " << ocfg.maxmem << std::endl;
    ss << "\t\t\tMax nr of threads: " << ocfg.threads2use << std::endl;
    ss << "\t\t\tTime limit [s]: " << ocfg.timelimsec << std::endl;
    ss << "\t\t\tMax opt gap allowed: " << ocfg.maxgap << std::endl;    
    ss << "\t\t\tInt param 0: " << ocfg.intp0 << std::endl;    
    ss << "\t\t\tInt param 1: " << ocfg.intp1 << std::endl;    
    ss << "\t\t\tDbl param 0: " << ocfg.dblp0;
    return ss.str();
}

