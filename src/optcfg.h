///
/// include/utility/optcfg.h
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

#ifndef OPTCFG_H
#define OPTCFG_H

#include <string>

struct optcfg
{
    /// Optimization algorithm
    enum Algo
    {
        LP,
        TS
    };
    
    /// Configuration parameters
    Algo                    opt_algo = Algo::TS;
    unsigned long long      alg_mode = 0;
    
    unsigned int            maxmem = 512;
    unsigned int            threads2use = 1;
    
    double                  timelimsec = 600;
    double                  maxgap = 0.0;
    
    unsigned int            intp0;
    unsigned int            intp1;
    
    double                  dblp0;    
    
    void read(std::string);
    
    optcfg()
    {
    }
    optcfg(std::string fn)
    {
        this->read(fn);
    }
    ~optcfg()
    {
    }
};


std::string getAlg(optcfg::Algo);
    
optcfg::Algo getAlg(std::string);

/// function prototype
std::string log_opt_config(optcfg);


#endif // OPTCFG_H