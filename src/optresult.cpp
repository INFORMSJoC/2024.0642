///
/// src/utility/optresult.cpp
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

#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

#include "optresult.h"


void optresult::write(std::string fname)
{
    std::ofstream ofs(fname);
    ofname = fname;

    /// throw exception if not able to open the file
    if(!ofs) 
        throw std::runtime_error("File creation fails");
    
    if(!ofs.is_open())
        throw std::runtime_error("File not found");
    
    /// strings
    for(auto it = tags.begin(); it != tags.end(); it++)
        ofs << (*it) << " ";
    
    /// doubles
    ofs << std::setprecision(4) << std::fixed << root_lprlx << " ";
    ofs << std::setprecision(4) << std::fixed << root_time << " ";
    ofs << std::setprecision(4) << std::fixed << best_bound << " ";
    ofs << std::setprecision(4) << std::fixed << obj_value << " ";
    ofs << std::setprecision(4) << std::fixed << opt_gap << " ";
    ofs << std::setprecision(4) << std::fixed << run_time << " ";
    /// integers
    for(auto it = integer_values.begin(); it != integer_values.end(); it++)
        ofs << (*it) << " ";
    
    /// doubles
    for(auto it = double_values.begin(); it != double_values.end(); it++)
        ofs << (*it) << (std::next(it) != double_values.end() ? " " : "");
    
    ofs << std::endl;
    
    ofs.close();
}

/// function implementation
std::string log_opt_result(optresult ores)
{
    std::stringstream ss;
    ss << "Opt. results:\n";
    unsigned int idx = 0;
    
    /// strings
    idx = 0;
    ss << "\t\t\tTags: ";
    for(auto it = ores.tags.begin(); it != ores.tags.end(); it++, idx++)
    {
        ss << " [" << idx << "] = " << (*it);
    }
    ss << std::endl;
    /// doubles
    ss << "\t\t\troot_lprlx = " << std::setprecision(4) << std::fixed << ores.root_lprlx << std::endl;
    ss << "\t\t\troot_time = " << std::setprecision(4) << std::fixed << ores.root_time << std::endl;
    ss << "\t\t\tbest_bound = " << std::setprecision(4) << std::fixed << ores.best_bound << std::endl;
    ss << "\t\t\tobj_value = " << std::setprecision(4) << std::fixed << ores.obj_value << std::endl;
    ss << "\t\t\topt_gap = " << std::setprecision(4) << std::fixed << ores.opt_gap << std::endl;
    ss << "\t\t\trun_time = " << std::setprecision(4) << std::fixed << ores.run_time << std::endl;
    /// integers
    idx = 0;
    ss << "\t\t\tintegers:";
    for(auto it = ores.integer_values.begin(); it != ores.integer_values.end(); it++, idx++)
    {
        ss << " [" << idx << "] = " << (*it);
    }
    ss << std::endl;
    /// doubles
    idx = 0;
    ss << "\t\t\tdoubles:";
    for(auto it = ores.double_values.begin(); it != ores.double_values.end(); it++, idx++)
    {
        ss << " [" << idx << "] = " << (*it);
    }
    
    return ss.str();
}


void write_optresult(optresult ors, std::string fname)
{
    std::ofstream ofs(fname);
    ors.ofname = fname;
    
    /// throw exception if not able to open the file
    if(!ofs) 
        throw std::runtime_error("File creation fails");
    
    if(!ofs.is_open())
        throw std::runtime_error("File not found");
    
    /// strings
    for(auto it = ors.tags.begin(); it != ors.tags.end(); it++)
        ofs << (*it) << " ";
    
    /// doubles
    ofs << std::setprecision(4) << std::fixed << ors.root_lprlx << " ";
    ofs << std::setprecision(4) << std::fixed << ors.root_time << " ";
    ofs << std::setprecision(4) << std::fixed << ors.best_bound << " ";
    ofs << std::setprecision(4) << std::fixed << ors.obj_value << " ";
    ofs << std::setprecision(4) << std::fixed << ors.opt_gap << " ";
    ofs << std::setprecision(4) << std::fixed << ors.run_time << " ";
    /// integers
    for(auto it = ors.integer_values.begin(); it != ors.integer_values.end(); it++)
        ofs << (*it) << " ";
    
    /// doubles
    for(auto it = ors.double_values.begin(); it != ors.double_values.end(); it++)
        ofs << (*it) << (std::next(it) != ors.double_values.end() ? " " : "");
    
    ofs << std::endl;
    
    ofs.close();
}

