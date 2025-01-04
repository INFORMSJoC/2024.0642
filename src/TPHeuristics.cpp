///
/// src/algs/TPHeuristics.cpp
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


#include <limits>
#include <numeric>
#include <chrono>
#include <algorithm>
#include <utility>
#include <random>
#include <cstdlib> 
#include <ctime> 
#include <stack> 
#include <set> 
#include <map> 
#include <cmath> 
#include <iterator>
#include <iostream>
#include <time.h>

#include "optresult.h"
#include "TpInstance.h"
#include "TSimplexDatastructs.h"

#include "TSimplex.h"
#include "util.h"

#include "MyLog.h"


#define GETOPTT(start) ((double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0)
#define GETOPTTMS(start) double((std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()) / 1000.0)


optresult TSimplex::nwcorner(const std::shared_ptr<tplex_alg_data>& tplxd_sptr)
{
    /// local vars
    std::vector<TpQuantityType> q_at_srcs;
    std::vector<TpQuantityType> q_at_dsts;
    std::vector<bool> nodes;
    NodeArcIdType M;
    NodeArcIdType N;
    NodeArcIdType i;
    NodeArcIdType j;
    unsigned long iter;
    NodeArcIdType srcs_rem;
    NodeArcIdType dsts_rem;
    double min_q;
    double tot_cost;
    double tot_q;
    double opt_sec;

    /// loging
    if(verbose_log)
    {
        FILE_LOG(logINFO)  << "Compute solution with NW corner method ... ";    
    }

    /// start chrono
    auto start = std::chrono::steady_clock::now();
    
    /// init. data
    q_at_srcs = tpdata_sptr->sources;
    q_at_dsts = tpdata_sptr->destinations;
    M = tpdata_sptr->m;
    N = tpdata_sptr->n;
    nodes = std::vector<bool>(M + N, true);
    
    /// main loop
    srcs_rem = dsts_rem = iter = i = j = 0;
    tot_cost = tot_q = 0.0;
    do
    {
        /// get min. q.
        min_q = std::min(q_at_srcs[i], q_at_dsts[j]);

        q_at_srcs[i] -= min_q;
        q_at_dsts[j] -= min_q;
        
        //if(!(min_q > EPSQ)) min_q = 0.0;
        tot_q += min_q;
        tot_cost += tpdata_sptr->costs[N * i + j] * min_q;
        //tplxd_sptr->quantities.set(N * i + j, !(min_q > MYEPS) ? EPSQ : min_q);
        tplxd_sptr->quantities.set(N * i + j, !(min_q > MYEPS) 
                                                ? 
                                                #ifdef EPSQIS0
                                                0.0
                                                #else
                                                EPSQ
                                                #endif
                                                : min_q);
        
        /// update data structs
        if(tplxd_sptr.get() != nullptr)
        {
            tplxd_sptr->rows[i].push_back(CellVar(i, j, tpdata_sptr->costs[N * i + j], 0.0));
            tplxd_sptr->cols[j].push_back(CellVar(i, j, tpdata_sptr->costs[N * i + j], 0.0));
            tplxd_sptr->solution.push_back(CellVar(i, j, tpdata_sptr->costs[N * i + j], 0.0));
        }
        
        if(std::round(q_at_srcs[i]) < MYEPS && std::round(q_at_dsts[j]) < MYEPS)
            nodes[dsts_rem > srcs_rem ? i : M + j] = false;
        else
            nodes[std::round(q_at_srcs[i]) < MYEPS ? i : M + j] = false;
        
        nodes[i] ? ++dsts_rem : ++srcs_rem;
        nodes[i] ? ++j : ++i;
        
        /// update iter
        ++iter;
    }
    while(iter < M + N - 1);
    
    /// get opt time
    opt_sec = GETOPTT(start);

    /// return value
    optresult optres;
    optres.obj_value = tot_cost;
    optres.run_time = opt_sec;
    optres.tags.resize(2);
    optres.tags[1] = std::string("NWC");
    optres.integer_values.resize(1);
    optres.integer_values[0] = iter;
    optres.double_values.resize(3, std::numeric_limits<double>::quiet_NaN());
    optres.double_values[0] = tot_q;
    optres.double_values[2] = GETOPTTMS(start);
    
    if(verbose_log)
    {
        FILE_LOG(logINFO)  << "North-west corner (NWC) method t cost = " << std::setprecision(1) << std::fixed << optres.obj_value << ", shipped q = " << tot_q
                           << ", opt time = " << std::setprecision(3) << std::fixed << optres.run_time << " [s], " << iter << " iterations";
    }
    
    return optres;
}

optresult TSimplex::matrix_min_rule(const std::shared_ptr<tplex_alg_data>& tplxd_sptr,
                                    const std::shared_ptr<std::pair<std::vector<double>, std::vector<double>>>& coverage_sptr,
                                    bool fill_1d_sol, NodeArcIdType UPTO)
{
    /// local vars
    std::vector<TpQuantityType> q_at_srcs;
    std::vector<TpQuantityType> q_at_dsts;
    std::vector<bool> nodes;
    NodeArcIdType VCT;
    NodeArcIdType rows_rem;
    NodeArcIdType cols_rem;
    NodeArcIdType M;
    NodeArcIdType N;
    NodeArcIdType c;
    NodeArcIdType i;
    NodeArcIdType j;
    unsigned long iter;
    double min_q;
    double tot_cost;
    double tot_q;
    double opt_sec;
    double initsrt_ms = std::numeric_limits<double>::quiet_NaN();
    
    /// loging
    if(verbose_log) { FILE_LOG(logINFO)  << "Compute solution with MMR method ... "; }

    /// start chrono
    auto start = std::chrono::steady_clock::now();
    /// init. data
    q_at_srcs = tpdata_sptr->sources;
    q_at_dsts = tpdata_sptr->destinations;
    rows_rem = cols_rem = 0;
    M = tpdata_sptr->m;
    N = tpdata_sptr->n;
    nodes = std::vector<bool>(M + N, true);
    
    if(tplxd_sptr.get() != nullptr)
        tplxd_sptr->solution.reserve(M + N - 1);
    
    if(tplxd_sptr->vdata.cs.empty())
    {
        if(verbose_log) { FILE_LOG(logINFO) << "Sort problem vars ..."; }
        auto ivdst = std::chrono::steady_clock::now();
        this->initVarData(tplxd_sptr->vdata);
        FILE_LOG(logINFO)  << "Data structure initialization time >> " << GETOPTTMS(ivdst) << " [ms] ::: problem vars " << (algcfg.partition_factor < MYEPS ? "*sorted*" : "*partitioned*");

    }
    
    initsrt_ms = GETOPTTMS(start);
    if(verbose_log) { FILE_LOG(logINFO) << "Data initialization time >> " << std::fixed << std::setprecision(3) << initsrt_ms << " [ms]"; }
    
    /// main loop
    VCT = M * N;
    c = iter = i = j = 0;
    tot_cost = tot_q = 0.0;
    tplxd_sptr->partial_basis_size = 0;
    do
    {
        i = tplxd_sptr->vdata.is[c];
        j = tplxd_sptr->vdata.js[c];
        
        if(nodes[i] && nodes[M + j])
        {
            min_q = std::min(q_at_srcs[i], q_at_dsts[j]);
            q_at_srcs[i] -= min_q;
            q_at_dsts[j] -= min_q;

            tot_q += min_q;
            tot_cost += tpdata_sptr->costs[N * i + j] * min_q;
            tplxd_sptr->quantities.set(N * i + j, !(min_q > MYEPS) 
                                                    ? 
                                                    #ifdef EPSQIS0
                                                    0.0
                                                    #else
                                                    EPSQ
                                                    #endif
                                                    : min_q);

            if(tplxd_sptr.get() != nullptr)
            {
                tplxd_sptr->rows[i].push_back(CellVar(i, j, tpdata_sptr->costs[N * i + j], 0.0));
                tplxd_sptr->cols[j].push_back(CellVar(i, j, tpdata_sptr->costs[N * i + j], 0.0));
                tplxd_sptr->solution.push_back(CellVar(i, j, tpdata_sptr->costs[N * i + j], 0.0));
            }
            
            if(std::round(q_at_srcs[i]) < MYEPS && std::round(q_at_dsts[j]) < MYEPS)
                nodes[cols_rem > rows_rem ? i : M + j] = false;
            else
                nodes[std::round(q_at_srcs[i]) < MYEPS ? i : M + j] = false;
            
            nodes[i] ? ++cols_rem : ++rows_rem;
            iter++;
        }
        
        ++c;
    }
    while(c < VCT && iter < M + N - 1);
        
    /// get opt time
    opt_sec = GETOPTT(start);

    /// return value
    optresult optres;
    optres.obj_value = tot_cost;
    optres.run_time = opt_sec;
    optres.tags.resize(2);
    optres.tags[1] = std::string("MMR");
    optres.integer_values.resize(1);
    optres.integer_values[0] = iter;
    optres.double_values.resize(3, std::numeric_limits<double>::quiet_NaN());
    optres.double_values[0] = tot_q;
    optres.double_values[1] = initsrt_ms;
    optres.double_values[2] = GETOPTTMS(start);
    
    if(verbose_log)
    {
        FILE_LOG(logINFO)  << "Matrix minimum rule (MMR) method t cost = " << std::setprecision(1) << std::fixed << optres.obj_value << ", shipped q = " << tot_q
                           << ", opt time = " << std::setprecision(3) << std::fixed << optres.run_time << " [s], " << iter << " iterations";
    }
    
    return optres;
}
