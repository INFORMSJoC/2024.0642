///
/// src/main.cpp
///
/// Written by Roberto Bargetto
///        DIGEP
///        Politecnico di Torino
///        Corso Duca degli Abruzzi, 10129, Torino
///        Italy
///
/// Copyright 2024 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
/// 
/// This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// See the license at http://creativecommons.org/licenses/by-nc-sa/4.0/
///

#include <stdio.h>
#include "TpInstance.h"
#include "optcfg.h"

#include "TSimplex.h"
#include "MyLog.h"
#include "util.h"


int main(int argc, char **argv)
{
    std::string ifname(argv[1]);
    std::string cfgfname(argv[2]);
    bool stdout  = argc > 3;
    
    try
    {
        /// read configuration file
        optcfg optc(cfgfname);
        
        /// prepare output file names
        std::string basefn = ifname;
        basefn = basefn.substr(basefn.find_last_of("/") + 1, basefn.size());
        basefn = basefn.substr(0, basefn.find_last_of("."));
        std::string resfn = std::string(basefn + "_" + getAlg(optc.opt_algo) + "_" + std::to_string(optc.alg_mode) + ".optres");
        std::string logfn = std::string(basefn + "_" + getAlg(optc.opt_algo) + "_" + std::to_string(optc.alg_mode) + ".log");
        
        /// prepare logging
        if(!stdout)
            init_logging(false, logfn, LOGLEVEL, "w+");

        /// read the problem instance
        std::shared_ptr<TpInstance> inst_sptr(new TpInstance(ifname));

        /// log configuration info
        FILE_LOG(logINFO) << log_opt_config(optc);
        
        /// reduce instances if 0 r/c
        #ifdef REDINST
        if(inst_sptr->getInstanceData()->reduce())
        {
            FILE_LOG(logINFO)  << "Problem reduced (0 value rows and columns removed)";
        }
        #endif

        /// solve
        TSimplex tspx(inst_sptr->getInstanceData(), 
                      optc.alg_mode, 
                      optc.intp0, 
                      optc.intp1, 
                      optc.dblp0);
        
        /// get opt. data
        optresult optres = tspx.tsimplex(optc.timelimsec, true, true);

        /// update tags
        optres.tags.resize(optres.tags_cnt);
        optres.tags[0] = basefn;
        optres.tags[1] = getAlg(optc.opt_algo) + "_" + std::to_string(optc.alg_mode);
        
        /// print opt. result to file
        optres.write(resfn);
        #ifdef EXPTRACING_2
        tspx.dump_tracing_data_to_file(std::string(getAlg(optc.opt_algo) + "_" + std::to_string(optc.alg_mode)), basefn);
        #endif
        
        if(!stdout)
            init_logging(true, logfn);
    }
    catch(std::exception& e)
    {
        std::cout << "Program thrown an exception, msg: " << e.what() << std::endl << std::endl;
        return -1;
    }
    

    return 0;
}
