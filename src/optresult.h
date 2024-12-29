///
/// include/utility/optresult.h
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

#ifndef OPTRESULT_H
#define OPTRESULT_H

#include <string>
#include <limits>
#include <vector>


struct optresult
{
    std::string ofname;
    unsigned int tags_cnt = 3;
    std::vector<std::string> tags;

    double root_time = std::numeric_limits<double>::quiet_NaN();
    double run_time = std::numeric_limits<double>::quiet_NaN();
    double opt_gap = std::numeric_limits<double>::quiet_NaN();
    double root_lprlx = std::numeric_limits<double>::quiet_NaN();
    double best_bound = std::numeric_limits<double>::quiet_NaN();
    double obj_value = std::numeric_limits<double>::quiet_NaN();
    
    unsigned int integers_cnt = 2;
    unsigned int doubles_cnt = 0;
    std::vector<long long> integer_values;
    std::vector<double> double_values;
    
    void write(std::string);
};

/// function prototype
std::string log_opt_result(optresult);


void write_optresult(optresult ors, std::string fname);

#endif // OPTRESULT_H