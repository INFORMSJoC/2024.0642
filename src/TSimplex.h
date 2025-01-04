///
/// src/algs/TSimplex.h
///
/// This library provides an implementation of the iterated inside-out algorithm by R. Bargetto, F. Della Croce, and R. Scatamacchia, for the transportaton problem
/// Iterated inside-out algorithm: https://arxiv.org/pdf/2302.10826.pdf
/// This library includes also several well-known heuristic solution methods (i.e., matrix minimum rule, Vogel's approximation, north west corner, etc.)
/// 
/// Written by Roberto Bargetto
/// 	   DIGEP
/// 	   Politecnico di Torino
/// 	   Corso Duca degli Abruzzi, 10129, Torino
/// 	   Italy
///
/// -------------------
///     Two class methods implementing the shielding algorithm (DOI 10.1007/s10851-016-0653-9), namely
///            NodeArcIdType TSimplex::compute_shields(const std::vector<double>& quantities, const std::vector<double>& costs,
///                                                   NodeArcIdType ori, NodeArcIdType dest, NodeArcIdType grsize, std::vector<CellVar>& varredcsts);
///            NodeArcIdType TSimplex::compute_shields_v2(const std::vector<double>& costs, bool redp, NodeArcIdType M, NodeArcIdType N, NodeArcIdType grsize,
///                                                      tplex_alg_data::THEgrid& board, std::vector<std::vector<NodeArcIdType>>& supp,
///                                                      std::vector<CellVar>& varredcsts, double shldeps = 1.0e-20);
///     are adaptations of a routine written by Rosario Scatamacchia, DIGEP, Politecnico di Torino, 
///     Work licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// -------------------
///
/// Copyright 2023 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
///
/// This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// See the license at http://creativecommons.org/licenses/by-nc-sa/4.0/
///


#ifndef TSIMPLEX_H
#define TSIMPLEX_H

#ifdef _WIN32
// windows code goes here
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <windows.h>
#endif

#include <memory>
#include <vector>
#include <list>
#include <limits>
#include <unordered_map>

#include "TpInstance.h"
#include "TSimplexDatastructs.h"
#include "optresult.h"

//#define EXPNET

using namespace TSimplexData;

#define RCCOMPMETHOD(mname) \
std::pair<bool,bool> mname(const ts_sol&, \
                           const std::vector<double>&, \
                           const std::vector<double>&, \
                           tplex_alg_data::var_data&, \
                           std::vector<CellVar>&, \
                           std::vector<CellVar>&)


class TSimplex
{
public:
    TSimplex(const std::shared_ptr<TpInstance::TProblemData>&,
             unsigned long long mode = 0,
             unsigned long long window_size_factor = 10,
             unsigned long long window_size_2_factor = 10,
             double partition_factor = 0.25);
    ~TSimplex();
    
    void setVerbose();
    #ifdef EXPTRACING_2
    void dump_tracing_data_to_file(std::string, std::string);
    #endif
    
    /// known heuristic methods
    optresult nwcorner(const std::shared_ptr<tplex_alg_data>& tplxd_sptr = nullptr);
    optresult matrix_min_rule(const std::shared_ptr<tplex_alg_data>& tplxd_sptr = nullptr,
                              const std::shared_ptr<std::pair<std::vector<double>, std::vector<double>>>& coverage = nullptr,
                              bool fill_1d_sol = false, NodeArcIdType VCT = 0);
    optresult tree_minimum_rule(const std::shared_ptr<tplex_alg_data>& tplxd_sptr = nullptr);
    
    /// main class methods
    optresult tsimplex(double tlim = std::numeric_limits<double>::quiet_NaN(), bool alginfolog = false, bool reptab = false);
    
private:
    struct algo_config
    {
        bool multipiv = false;
        bool spatvarsel = false;
        unsigned int spatvarsel_greed_lvl = 0;
        unsigned int rccpol = 0;
        unsigned int init_sol_method = 0;
        
        unsigned long long window_size_factor = 10;
        unsigned long long window_size_2_factor = 10;
        unsigned long long max_macroiter = (std::numeric_limits<unsigned long long>::max)();
        
        double partition_factor = 0.25;
        
        /// substitute values
        unsigned long long max_shield_neigh_macroiter = (std::numeric_limits<unsigned long long>::max)();
    };
    
    /// private method declarations
    void initVarData(tplex_alg_data::var_data& vdata);
    
    /// shielding stuff
    NodeArcIdType compute_shields_v2(const TpInstance::tcosts& costs,
                                     bool redp,
                                     NodeArcIdType M,
                                     NodeArcIdType N,
                                     NodeArcIdType grsize,
                                     Shielding::THEgrid& board,
                                     std::vector<std::vector<NodeArcIdType>>& supp,
                                     std::vector<CellVar>& varredcsts, double shldeps = 1.0e-20);    
    
    /// compute red.costs
    RCCOMPMETHOD(computeReducedCostsPol_4);
    RCCOMPMETHOD(computeReducedCostsPol_3);
    /// std TP simplex 1st neg. r.c. rule
    RCCOMPMETHOD(computeReducedCostsPol_1);
    /// std TP simplex most neg. r.c. rule
    RCCOMPMETHOD(computeReducedCostsPol_0);
    
    /// pivoting
    typedef std::tuple<unsigned long, unsigned long, unsigned long, unsigned long, double, double> pivs_data;
    pivs_data pivoting(const std::shared_ptr<tplex_alg_data>& tplxd_sptr,
                       const std::shared_ptr<SpanningTree>& bspat_sptr,
                       std::vector<CellVar>& entering_var,
                       double& objf_value, double& tot_q,
                       bool update_rwcl_sol = false);
    
    /// problem data
    std::shared_ptr<TpInstance::TProblemData> tpdata_sptr;
    std::shared_ptr<tplex_alg_data> optdata_sptr;
    std::shared_ptr<std::vector<std::pair<NodeArcIdType, NodeArcIdType>>> ext_sol_arcs_sptr;
    std::shared_ptr<TSimplexData::ts_sol> ext_sol_sptr;
    optresult ext_sol_optres;
    std::shared_ptr<std::vector<double>> solvars_sptr;
    algo_config algcfg;
    
    /// control values
    /// flags
    bool retheur = false;
    bool verbose_log = false;
    /// doubles
    double iter_objfv = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> objf_improvs;
    const double ERTV = std::numeric_limits<double>::quiet_NaN();
    /// integers
    unsigned long alg_super_iter;
    unsigned long alg_iter;
    unsigned int impr_vec_size = 8;
    unsigned short alg_state = 1;
    
    /// tracing data (experiments and development stuff)
    #ifdef EXPTRACING_2
    unsigned long step2impr_ct;
    unsigned long step2imprpls_ct;
    unsigned long step2imprmnd_ct;
    unsigned long comp_rcs;
    unsigned long comp_negrcs;
    unsigned long espqbchng_counter;
    unsigned long deg_bchng_ctr;
    unsigned long total_loop_len;
    unsigned long total_loop_len_f1;
    unsigned long total_loop_len_f2;
    unsigned long total_path_len;
    unsigned long total_path_len_f1;
    unsigned long total_path_len_f2;
    unsigned long cmpd_loops_cnt_f1;
    unsigned long cmpd_loops_cnt_f2;
    unsigned long odk_counter;
    unsigned long node_clrng_counter;
    unsigned long tree_update_counter;
    unsigned long shortlist_size;
    unsigned long rcs_get_pos;
    unsigned long rcs_chg_sgn_base;
    unsigned long rcs_chg_sgn_ctr;
    
    unsigned long rcs_neg;
    unsigned long rcs_0;
    unsigned long rcs_pos;
    
    double step1_improv;
    double step2_improv;
    double step2_improv_plus;
    double step2_improv_minus;
    double avg_loop_len_f1;
    double avg_loop_len_f2;
    double tm2fndloop;
    double tm2clrtree;
    double tm2udtree;
    double tmshielneigh;
    double rcs_chg_sgn_perc_ave;
    double rcs_chg_sgn_perc_min;
    double rcs_chg_sgn_perc_max;
    #endif
    
    /// function pointer to method for reduced cost computing
    std::pair<bool,bool>
    (TSimplex::*reduced_cost_fptr)(const ts_sol&,
                                   const std::vector<double>&,
                                   const std::vector<double>&,
                                   tplex_alg_data::var_data&,
                                   std::vector<CellVar>&,
                                   std::vector<CellVar>&)
        = nullptr;

    std::pair<bool,bool>
    (TSimplex::*reduced_cost_f2_ptr)(const ts_sol&,
                                     const std::vector<double>&,
                                     const std::vector<double>&,
                                     tplex_alg_data::var_data&,
                                     std::vector<CellVar>&,
                                     std::vector<CellVar>&)
        = nullptr;
};


#endif // TSIMPLEX_H