///
/// src/algs/TSimplex.cpp
///
/// This library provides an implementation of the iterated inside-out algorithm by R. Bargetto, F. Della Croce, and R. Scatamacchia, for the transportaton problem
/// Iterated inside-out algorithm: https://arxiv.org/pdf/2302.10826.pdf
/// This library includes also several well-known heuristic solution methods (i.e., matrix minimum rule, Vogel's approximation, north west corner, etc.)
/// 
/// Written by Roberto Bargetto
///        DIGEP
///        Politecnico di Torino
///        Corso Duca degli Abruzzi, 10129, Torino
///        Italy
///
/// -------------------
///     Two class methods implementing the shielding algorithm (DOI 10.1007/s10851-016-0653-9), namely
///            NodeArcIdType TSimplex::compute_shields_v2(const std::vector<double>& costs, bool redp, NodeArcIdType M, NodeArcIdType N, NodeArcIdType grsize,
///                                                      tplex_alg_data::THEgrid& board, std::vector<std::vector<NodeArcIdType>>& supp,
///                                                      std::vector<CellVar>& varredcsts, double shldeps = 1.0e-20);
///     are adaptations of a routine written by Rosario Scatamacchia, DIGEP, Politecnico di Torino
///     Work licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// -------------------
///
/// Copyright 2023 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
///
/// This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// See the license at http://creativecommons.org/licenses/by-nc-sa/4.0/
///


#ifdef _WIN32
// windows code goes here
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <windows.h>
#endif

#include <limits>
#include <numeric>
#include <chrono>
#include <algorithm>
#include <functional>
#include <utility>
#include <random>
#include <cstdlib> 
#include <ctime> 
#include <stack> 
#include <list> 
#include <set> 
#include <cmath> 
#include <fstream>
#include <iterator>
#include <iostream>
#include <time.h>

#include "TSimplex.h"
#include "util.h"

#include "MyLog.h"


//#define DEBUGML
//#define DEBUGMLITERCT 9000000

#define TESTSPATNMLTPSCMP false
#define TESTIFSPATISDIFF false
#define TESTLOOPWSPAT false


//#define VARSORTING_RULE 2
//#define RCSTPOL 2

//#define MULTIPIVOT false
//#define MPVTPSIZE (tpdata_sptr->m + tpdata_sptr->n)

//#define PRINTRCS2F

//#define DMSOLA
//#define DOSQRS

//#define SKIPWSTREE true
//#define GREEDLVL 2
#define ASN_BASIS_DEG_IMPR false

//#define USESPAT4ALL true 
#define HEURSOLIMPR false
#define MAXITERHEURSRCH 100000

#define OBJFRNDD true
#define ELOOPSIZE 40

#define ERTVMAX 1e-1

#define DTMRK_SIGNIFIMPR 1000000.0

#define GETOPTT(start) ((double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0)
#define GETOPTTMS(start) ((double)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0)
#define GETOPTTMS6(start) ((double)std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start).count() / 1.0e6)

/// typedef and macros for logging
typedef OptLogger<unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, double, double> TplexOptLogger;

#ifndef SILENT
#define INITLOG(logstuff) \
    logstuff.push_back(std::make_tuple(10, 0, "sup iter")); \
    logstuff.push_back(std::make_tuple(10, 0, "sls size")); \
    logstuff.push_back(std::make_tuple(10, 0, "iter")); \
    logstuff.push_back(std::make_tuple(10, 0, "bases")); \
    logstuff.push_back(std::make_tuple(10, 0, "impr bc")); \
    logstuff.push_back(std::make_tuple(10, 0, "tot bases")); \
    logstuff.push_back(std::make_tuple(10, 0, "rc-")); \
    logstuff.push_back(std::make_tuple(10, 0, "rc+")); \
    logstuff.push_back(std::make_tuple(10, 0, "tot rc")); \
    logstuff.push_back(std::make_tuple(32, 1, "ObjF")); \
    logstuff.push_back(std::make_tuple(10, 3, "EPT [s]"))
#endif
//max_diff = !(max_diff > std::numeric_limits<double>::lowest() + MYEPS) ? -1.0 : max_diff;

#if defined (SILENT)

#define INITLOG(logstuff)
#define DEBUGLOGHDR(lglevl, optlogger)
#define DEBUGLOG(lglevl, optlogger, super_iter, iter, bases, tot_bases, pvct, cmprc, objf, qs, opt_sec)
#define LOGCLEAR()

#elif defined (COUTRLOG)

#define LOGHDR(lglevl, optlogger) \
    std::cout << optlogger.getFormatedHeader() << std::endl;
#define LOGITER(lglevl, optlogger, super_iter, slsize, iter, bases, tot_bases, pvct, rcm, rc0, rcp, objf, qs, opt_sec) \
    std::cout << "\r" << optlogger.getFormatedLine(std::make_tuple(super_iter, slsize, iter, bases, tot_bases, pvct, rcm, rc0, rcp, objf, opt_sec))
#define LOGCLEAR() \
    std::cout << "\r" << std::endl

#else

#define LOGHDR(lglevl, optlogger) \
    FILE_LOG(lglevl) << optlogger.getFormatedHeader()
#define LOGITER(lglevl, optlogger, super_iter, slsize, iter, bases, tot_bases, pvct, rcm, rc0, rcp, objf, qs, opt_sec) \
    FILE_LOG(lglevl) << optlogger.getFormatedLine(std::make_tuple(super_iter, slsize, iter, bases, tot_bases, pvct, rcm, rc0, rcp, objf, opt_sec))
#define LOGCLEAR()

#endif

typedef OptLogger<std::string, std::string, double, std::string> OptReportTab;
#define INITOPTRTAB(optrtab) \
    optrtab.push_back(std::make_tuple(40, 0, "SCOPE")); \
    optrtab.push_back(std::make_tuple(24, 0, "STAT")); \
    optrtab.push_back(std::make_tuple(24, 1, "VALUE")); \
    optrtab.push_back(std::make_tuple(8, 0, "UNIT"))
#define OPTRTABHDR(lglevl, optrtab) \
    FILE_LOG(lglevl) << optrtab.getFormatedHeader()
#define ORTABLINE(lglevl, optrtab, scope_str, time_str, val, unit_str) \
    FILE_LOG(lglevl) << optrtab.getFormatedLine(std::make_tuple(scope_str, time_str, val, unit_str))


///
/// Class TSimplex implementation
/// 
TSimplex::TSimplex(const std::shared_ptr<TpInstance::TProblemData>& idatsptr, unsigned long long mode, unsigned long long wsf, unsigned long long ws2f, double pf)
    : tpdata_sptr(idatsptr)
    , optdata_sptr(nullptr)
    , solvars_sptr(nullptr)
    , ERTV(EPSQ * (idatsptr->m + idatsptr->n) * std::sqrt(tpdata_sptr->n))
{
    /// check ert value
    if(!(ERTV + MYEPS < ERTVMAX))
        throw std::runtime_error("ERT value too large, " + std::to_string(ERTV) + ": set a smaller Eps. value");
    
    /// process alg. mode
    if(mode)
    {
        algcfg.multipiv = bool(mode % 10);
        algcfg.spatvarsel = bool(mode / 10 % 10);
        algcfg.spatvarsel_greed_lvl = (mode / 100 % 10);
        algcfg.rccpol = (mode / 1000 % 10);
        algcfg.init_sol_method = (mode / 10000 % 10);
    }
    /// Parameter 'alpha' of the initial clustering
    algcfg.window_size_factor = wsf == 0 || wsf * (tpdata_sptr->m + tpdata_sptr->n) > tpdata_sptr->m * tpdata_sptr->n ? std::numeric_limits<unsigned long long>::max() : wsf;
    
    /// Parameter 'alpha' of the triangle heuristic
    algcfg.window_size_2_factor = ws2f == 0 || ws2f * (tpdata_sptr->m + tpdata_sptr->n) > tpdata_sptr->m * tpdata_sptr->n ? std::numeric_limits<unsigned long long>::max() : ws2f;
    algcfg.max_macroiter = tpdata_sptr->max_macroiter < std::numeric_limits<unsigned long long>::max() ? tpdata_sptr->max_macroiter : std::numeric_limits<unsigned long long>::max();
    algcfg.partition_factor = pf * (tpdata_sptr->m + tpdata_sptr->n) > tpdata_sptr->m * tpdata_sptr->n ? double(tpdata_sptr->m * tpdata_sptr->n) / (tpdata_sptr->m + tpdata_sptr->n) : pf;
    
    /// shielding stuff
    algcfg.max_shield_neigh_macroiter = wsf > 0 ? wsf : std::numeric_limits<unsigned long long>::max();
    
    //reduced_cost_fptr = &TSimplex::computeReducedCosts;
    retheur = false;
    switch(algcfg.rccpol)
    {
        case 0:
            reduced_cost_fptr = &TSimplex::computeReducedCostsPol_0;
            break;
        case 1:
            reduced_cost_fptr = &TSimplex::computeReducedCostsPol_1;
            break;
        case 3:
            reduced_cost_fptr = &TSimplex::computeReducedCostsPol_3;
            break;
        case 4:
            reduced_cost_fptr = &TSimplex::computeReducedCostsPol_4;
            break;
        default:
            throw std::runtime_error("Unknown r.c. computation method");
    }
}

TSimplex::~TSimplex()
{
}

void TSimplex::setVerbose()
{
    verbose_log = true;
}

#ifdef EXPTRACING_2
void TSimplex::dump_tracing_data_to_file(std::string tag, std::string iname)
{
    std::string ofname = iname + "_" + tag + ".optres.trace2";    
    std::ofstream ofs(ofname);
    
    /// open input file
    if(!ofs.is_open())
        throw std::runtime_error("File not found");
    
    FILE_LOG(logINFO) << "Write extra opt data to file ...";
    ofs << iname << " ";
    ofs << tag << " ";

    ofs << step2impr_ct << " ";
    ofs << step2imprpls_ct << " ";
    ofs << step2imprmnd_ct << " ";
    ofs << comp_rcs << " ";
    ofs << comp_negrcs << " ";
    ofs << espqbchng_counter << " ";
    ofs << total_loop_len << " ";
    ofs << total_loop_len_f1 << " ";
    ofs << total_loop_len_f2 << " ";
    ofs << total_path_len << " ";
    ofs << total_path_len_f1 << " ";
    ofs << total_path_len_f2 << " ";
    ofs << cmpd_loops_cnt_f1 << " ";
    ofs << cmpd_loops_cnt_f2 << " ";
    ofs << odk_counter << " ";
    ofs << tree_update_counter << " ";
    ofs << node_clrng_counter << " ";
    ofs << (step1_improv) << " ";
    ofs << step2_improv << " ";
    ofs << step2_improv_plus << " ";
    ofs << step2_improv_minus << " ";
    ofs << avg_loop_len_f1 << " ";
    ofs << avg_loop_len_f2 << " ";
    ofs << tm2fndloop << " ";
    ofs << tm2clrtree << " ";
    ofs << tm2udtree << std::endl;
    FILE_LOG(logINFO) << "Done";
    
    ofs.close();
}
#endif

optresult TSimplex::tsimplex(double tlim, bool alginfolog, bool reptab)
{
    /// Simplex for the transportation problem
    /// 
    /// local vars
    /// 
    std::vector<CellVar> varredcsts;
    std::vector<CellVar> entering_vars;
    pivs_data pivotrv;
    std::shared_ptr<SpanningTree> basisspat_sptr(nullptr);
    std::shared_ptr<tplex_alg_data> tplexd_sptr(nullptr);
    unsigned int ml_exit_code;
    unsigned long tot_bchng;
    unsigned long bchng;
    unsigned long tempt_var_count;
    unsigned long saved_lpsrch_count;
    unsigned long failed_lpsrch_count;
    unsigned long succes_lpsrch_count;
    unsigned long allrcscmp_count;
    unsigned long computed_neighs;
    SpanningTree::NodeType nd;

    double initsol_objf_value = std::numeric_limits<double>::quiet_NaN();
    double objf_value = std::numeric_limits<double>::quiet_NaN();
    double opt_sec = std::numeric_limits<double>::quiet_NaN();
    double tot_q = std::numeric_limits<double>::quiet_NaN();
    double elapsed_usrt_ms;
    std::pair<bool,bool> rccmprv;
    
    /// log alg mode info if verbose
    if(alginfolog)
    {
        FILE_LOG(logINFO) << "T. Simplex algorithm mode:"
                          << "\n\t\t\tMultipivoting = " << (algcfg.multipiv ? "Yes" : "No")
                          << "\n\t\t\tUse SPAT for selecting pivot variable  = " << (algcfg.spatvarsel ? "Yes" : "No")
                          << "\n\t\t\tUse SPAT pivot variable selection greedy level = " << algcfg.spatvarsel_greed_lvl
                          << "\n\t\t\tComputation rule for reduced costs = " << algcfg.rccpol
                          << "\n\t\t\tInitial solution method = " << algcfg.init_sol_method
                          << "\n\t\t\tW = " << algcfg.window_size_factor
                          << " (" << (algcfg.window_size_factor * (tpdata_sptr->m + tpdata_sptr->n)) << " variables)"
                          << "\n\t\t\tW2 = " << algcfg.window_size_2_factor 
                          << " (" << (algcfg.window_size_2_factor * (tpdata_sptr->m + tpdata_sptr->n)) << " variables)"
                          << "\n\t\t\tpartition size factor = " << algcfg.partition_factor;
        FILE_LOG(logINFO) << "T. Simplex algorithm"
                          << " ::: program eps >> " << std::setprecision(10) << MYEPS 
                          << " ::: problem eps quantity >> " << std::setprecision(10) << EPSQ;
        FILE_LOG(logINFO) << "T. Simplex algorithm"
                          << " ::: ERT eps >> " << std::setprecision(10) << ERTV;
        
        #if defined TSSOLSPRS
        FILE_LOG(logINFO) << "Sparse matrix solution representation";
        #endif
    }
    
    /// log opt start
    FILE_LOG(logINFO)  << "Start transportation cost optimization ...";
    
    /// chrono stuff
    /// step times
    /// 0. Vogel's time (init. basis time)
    /// 1. T. spx data init. time
    /// 2. T. spx opt. time
    /// 3. Multipliers time
    /// 4. Reduced costs' time
    /// 5. basis change time
    /// 6.     step 1
    /// 7.     step 2
    /// 8. Tot. time from 0 to 5
    std::vector<double> cmp_times(10, 0.0);
        
    FILE_LOG(logINFO) << "Init data structure ...";
    auto st_0 = std::chrono::steady_clock::now();
    clock_t startt = clock();
    tplexd_sptr.reset(new tplex_alg_data(tpdata_sptr, true));
    /// initialize opt data
    if(algcfg.rccpol == 3 || (algcfg.init_sol_method > 1 && algcfg.init_sol_method < 9))
    {
        this->initVarData(tplexd_sptr->vdata);
        cmp_times[1] += GETOPTTMS(st_0);
        FILE_LOG(logINFO)  << "Data structure initialization time >> " << cmp_times[1] << " [ms] ::: problem vars " << (algcfg.partition_factor < MYEPS ? "*sorted*" : "*partitioned*");
    }
    
    /// compute initial solution
    auto st_0_1 = std::chrono::steady_clock::now();
    optresult optres;
    if(algcfg.init_sol_method == 1)
    {   /// north-west corner
        optres = this->nwcorner(tplexd_sptr);
    }
    else if(algcfg.init_sol_method == 2)
    {   /// MMR
        optres = this->matrix_min_rule(tplexd_sptr);
    }
    else
    {   /// throw exception if no method is recognized
        throw std::invalid_argument("Unknow method for the initial basis computation");
    }
    initsol_objf_value = optres.obj_value;
    tot_q = optres.double_values[0];
    
    cmp_times[0] += GETOPTTMS(st_0_1);
    FILE_LOG(logINFO) << optres.tags[1] << " initial solution" 
                      << " ::: time >> " << cmp_times[0] << " [ms]" 
                      << " ::: Initialization time >> " << optres.double_values[1] << " [ms]"
                      << " ::: iters >> " << optres.integer_values[0]
                      << " ::: Obj F >> " << std::setprecision(0) << std::fixed << optres.obj_value;
    
    /// throw except if not a basis
    if((NodeArcIdType)(optres.integer_values[0] + 1) < tpdata_sptr->m + tpdata_sptr->n)
        throw std::runtime_error("Inital heurustic is not a problem basis, |basis vars| = " + std::to_string(optres.integer_values[0]) + " < m + n - 1");
    
    /// init. t. spx data
    if((basisspat_sptr == nullptr || ASN_BASIS_DEG_IMPR))
    {
        double spattm = 0.0;
        auto st_1 = std::chrono::steady_clock::now();
        basisspat_sptr.reset(new SpanningTree(tpdata_sptr->m + tpdata_sptr->n - 1, tpdata_sptr->m + tpdata_sptr->n, tpdata_sptr->m));
        basisspat_sptr->compute(tplexd_sptr->rows, tplexd_sptr->cols);
        cmp_times[1] += (spattm = GETOPTTMS(st_1));
        FILE_LOG(logINFO) << "Spanning tree time >> " << std::fixed << std::setprecision(3) << spattm << " [ms]";
    }
    /// set spanning tree as p. data
    tplexd_sptr->vdata.spat_sptr = basisspat_sptr;
    
    /// tracing data (experiments and development stuff)
    #ifdef EXPTRACING_2
    comp_rcs = comp_negrcs = step2impr_ct = step2imprmnd_ct = step2imprpls_ct = espqbchng_counter = deg_bchng_ctr = total_loop_len
             = total_loop_len_f1 = total_loop_len_f2 = total_path_len = total_path_len_f1 = total_path_len_f2
             = cmpd_loops_cnt_f1 = cmpd_loops_cnt_f2 = odk_counter = node_clrng_counter = computed_neighs = tree_update_counter
             = shortlist_size = rcs_get_pos = rcs_chg_sgn_base = rcs_chg_sgn_ctr
             = rcs_neg = rcs_0 = rcs_pos = 0;
    step1_improv = step2_improv = step2_improv_plus = step2_improv_minus = avg_loop_len_f1 = avg_loop_len_f2
                 = tm2fndloop = tm2clrtree = tm2udtree = tmshielneigh
                 = rcs_chg_sgn_perc_ave = rcs_chg_sgn_perc_min = rcs_chg_sgn_perc_max = 0.0;
    #endif
    
    /// init. logger if verbose
    TplexOptLogger optlogger;
    if(verbose_log)
    {
        std::vector<OptLogMeta> logstuff;
        INITLOG(logstuff);
        optlogger.set(logstuff);
        /// log header
        FILE_LOG(logINFO)  << "Start simplex iteration ...";
        LOGHDR(logDEBUG, optlogger);
    }
    else
    {
        FILE_LOG(logINFO)  << "Simplex iterations ...";
    }
    
    /// init. vars for main loop
    auto st_2 = std::chrono::steady_clock::now();
    varredcsts.reserve(tpdata_sptr->m + tpdata_sptr->n);
    objf_improvs.resize(impr_vec_size, std::numeric_limits<double>::quiet_NaN());
    ml_exit_code = allrcscmp_count = succes_lpsrch_count = failed_lpsrch_count = saved_lpsrch_count = tempt_var_count = computed_neighs = 0;
    tot_bchng = bchng = alg_super_iter = alg_iter = 0;
    iter_objfv = objf_value = optres.obj_value;
    tot_q = optres.double_values[0];
    
    /// main loop
    if(!retheur)
    do
    {
        /// Computer multipliers us and vs
        auto st_3 = std::chrono::steady_clock::now();
        nd = -1;
        if(entering_vars.size() == 1)
            nd = basisspat_sptr->getPredecessor(entering_vars[0].i) == entering_vars[0].j + tpdata_sptr->m
                 ? entering_vars[0].j + tpdata_sptr->m
                 : entering_vars[0].i;
        basisspat_sptr->computeTSpxMultips(tpdata_sptr->costs, tplexd_sptr->us, tplexd_sptr->vs, nd);
        
        /// get time
        cmp_times[3] += GETOPTTMS(st_3);

        /// start chrono
        auto st_4 = std::chrono::steady_clock::now();
        
        /// Compute reduced costs    
        entering_vars.clear();
        rccmprv = (this->*reduced_cost_fptr)(tplexd_sptr->quantities, tplexd_sptr->us, tplexd_sptr->vs, tplexd_sptr->vdata, varredcsts, entering_vars);
        
        /// update some counters
        if(rccmprv.first || algcfg.rccpol == 4)
            alg_super_iter++;
        if(rccmprv.first && algcfg.rccpol == 4)
            computed_neighs++;
        if(rccmprv.second)
        {
            if(!(cmp_times[9] > 0.0))
                cmp_times[9] = cmp_times[2];
            allrcscmp_count++;
        }
        
        /// get time
        cmp_times[4] += GETOPTTMS(st_4);
        
        /// Pivoting phase
        if(!entering_vars.empty())
        {
            /// TRACING CODE
            #ifdef EXPTRACING_2
            tempt_var_count += entering_vars.size();
            #endif
            
            /// start chrono
            auto st_5 = std::chrono::steady_clock::now();
            /// pivoting
            pivotrv = this->pivoting(tplexd_sptr, basisspat_sptr, entering_vars, objf_value, tot_q /*, !USESPAT4ALL */ );
            
            /// get time
            cmp_times[5] += GETOPTTMS(st_5);
            cmp_times[6] += std::get<4>(pivotrv);
            cmp_times[7] += std::get<5>(pivotrv);
            /// get other stat. values
            bchng += std::get<0>(pivotrv);
            tot_bchng += std::get<0>(pivotrv);
            
            /// TRACING CODE
            #ifdef EXPTRACING_2
            saved_lpsrch_count += std::get<1>(pivotrv);
            failed_lpsrch_count += std::get<2>(pivotrv);
            succes_lpsrch_count += std::get<3>(pivotrv);
            #endif
        }
        
        /// update EPT
        opt_sec = GETOPTT(st_0);
        cmp_times[2] = GETOPTTMS(st_2);
        /// update iters
        ++alg_iter;
        
        /// logging
        if(verbose_log && rccmprv.first) //&& (bchng ? bchng - deg_bchng_ctr : 0) > 0)
        {
            LOGITER (logDEBUG, optlogger, 
                     alg_super_iter,
                     #ifdef EXPTRACING_2
                     shortlist_size,
                     #else
                     0,
                     #endif
                     alg_iter,
                     bchng, 
                     #ifdef EXPTRACING_2
                     bchng ? bchng - deg_bchng_ctr : 0, 
                     #else
                     0,
                     #endif
                     tot_bchng, 
                     #ifdef EXPTRACING_2
                     rcs_neg,
                     rcs_pos,
                     rcs_neg + rcs_0 + rcs_pos,
                     objf_value, 
                     #else
                     0,
                     0,
                     0,
                     std::numeric_limits<double>::quiet_NaN(),
                     #endif
                     tot_q, 
                     opt_sec);
        }
        
        /// Optimal solution
        if(entering_vars.empty())
            ml_exit_code = 1;
        /// Time limit
        if(!std::isnan(tlim) && (tlim + MYEPS < opt_sec))
            ml_exit_code = 2;
        /// max iter
        if(alg_iter >= algcfg.max_macroiter)
            ml_exit_code = 3;
        
        if(!algcfg.multipiv || rccmprv.first)
            bchng = 0;
        /// DEBUG
        #ifdef DEBUGML
        std::getchar();
        #endif
        #ifdef DEBUGMLITERCT
        if(alg_iter > DEBUGMLITERCT)
            break;
        #endif
    }
    while(!ml_exit_code);
    /// log
    if(verbose_log) { LOGCLEAR(); }
    FILE_LOG(logINFO) << (ml_exit_code == 1 ? "Optimal solution found" : "Limit reached");
    
    /// Compute obj f
    auto objftm = std::chrono::steady_clock::now();
    auto objf_and_tsq = basisspat_sptr->computeTSpxObjF(tpdata_sptr->costs, tplexd_sptr->quantities);
    objf_value = objf_and_tsq.first;
    tot_q = objf_and_tsq.second;
    double objf_tm = GETOPTTMS(objftm);
    
    /// get g. time in microsec.
    cmp_times[8] += GETOPTTMS(st_0);
    clock_t endt = clock();
    elapsed_usrt_ms = 1000.0 * double(endt - startt) / CLOCKS_PER_SEC;
    
    /// store opt data
    optdata_sptr = tplexd_sptr;
    
    /// Fill-in opt. results
    optres.obj_value = objf_value;
    optres.run_time = opt_sec;
    optres.tags.resize(3);
    optres.tags[2] = ml_exit_code == 1 ? "Optimal" : "NotOptimal";
    optres.integer_values.resize(2, 0);
    optres.integer_values.push_back(alg_super_iter);
    optres.integer_values.push_back(alg_iter);
    optres.integer_values.push_back(tot_bchng);
    optres.integer_values.push_back(allrcscmp_count);
    optres.integer_values.push_back(saved_lpsrch_count);
    optres.integer_values.push_back(failed_lpsrch_count);
    optres.integer_values.push_back(succes_lpsrch_count);
    optres.integer_values.push_back(tempt_var_count);
    optres.double_values.assign(cmp_times.begin(), cmp_times.end());
    optres.double_values.push_back((initsol_objf_value - optres.obj_value)/optres.obj_value);
    #ifdef EXPTRACING_2
    avg_loop_len_f1 = double(total_loop_len_f1) / cmpd_loops_cnt_f1;
    avg_loop_len_f2 = double(total_loop_len_f2) / cmpd_loops_cnt_f2;
    step1_improv = step1_improv / double(optres.obj_value);
    step2_improv = step2_improv / double(optres.obj_value);
    step2_improv_plus = step2_improv_plus / double(optres.obj_value);
    step2_improv_minus = step2_improv_minus / double(optres.obj_value);
    #ifdef RCS_TRACING
    this->write2file_rcs_data();
    #endif
    #endif

    /// Log opt. info.
    FILE_LOG(logINFO) << "----------------------------------------";
    FILE_LOG(logINFO) << std::setw(16) << "objf" << std::setw(24) << std::setprecision(0) << std::fixed  << optres.obj_value;
    FILE_LOG(logINFO) << std::setw(16) << "totq" << std::setw(24) << std::setprecision(0) << std::fixed  << tot_q;
    FILE_LOG(logINFO) << std::setw(16) << "time" << std::setw(24) << std::setprecision(0) << std::fixed  << cmp_times[8] << " [ms]";
    FILE_LOG(logINFO) << std::setw(16) << "user time" << std::setw(24) << std::setprecision(0) << std::fixed << elapsed_usrt_ms << " [ms]";
    FILE_LOG(logINFO) << std::setw(16) << "objf time" << std::setw(24) << std::setprecision(0) << std::fixed << objf_tm << " [ms]";
    FILE_LOG(logINFO) << std::setw(16) << "iterations" << std::setw(24) << std::setprecision(0) << std::fixed << alg_iter;
    FILE_LOG(logINFO) << std::setw(16) << "basis changes" << std::setw(24) << std::setprecision(0) << std::fixed << tot_bchng;
    FILE_LOG(logINFO) << "----------------------------------------";
    if(reptab)
    {
        FILE_LOG(logINFO) << "Optimization report";
        
        std::vector<OptLogMeta> tabstuff;
        INITOPTRTAB(tabstuff);
        OptReportTab optreptab(tabstuff);
        /// log header
        OPTRTABHDR(logINFO, optreptab);
        /// log ta lines
        /// step times
        /// 0. Vogel's time (init. basis time)
        /// 1. T. spx data init. time
        /// 2. T. spx opt. time
        /// 3. Multipliers time
        /// 4. Reduced costs' time
        /// 5. basis change time
        /// 6.     step 1
        /// 7.     step 2
        /// 8. Tot. time from 0 to 5
        ORTABLINE(logINFO, optreptab, "global", "objf value", optres.obj_value, "");
        ORTABLINE(logINFO, optreptab, "global", "transhipped quantity", tot_q, "");
        ORTABLINE(logINFO, optreptab, "global", "initial sol improv", optres.double_values.back()*100.0, "[%]");
        ORTABLINE(logINFO, optreptab, "global", "tot time", optres.run_time, "[s]");
        ORTABLINE(logINFO, optreptab, "global", "tot time", cmp_times[8], " [ms]");
        ORTABLINE(logINFO, optreptab, "global", "data init time", cmp_times[1], " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "initial sol time", cmp_times[0], " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "init sol + opt time", ( cmp_times[0] + cmp_times[2]), " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "shielding neighs", computed_neighs, "");
        ORTABLINE(logINFO, optreptab, "simplex", "macro iterations", alg_super_iter,  "");
        ORTABLINE(logINFO, optreptab, "simplex", "iterations", alg_iter, "");
        ORTABLINE(logINFO, optreptab, "simplex", "basis changes", tot_bchng, "");
        ORTABLINE(logINFO, optreptab, "simplex", "full rc comp", allrcscmp_count, "");
        ORTABLINE(logINFO, optreptab, "simplex", "opt time", cmp_times[2], " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "opt time (1)", cmp_times[9], " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "mulipliers time", cmp_times[3], " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "reduced cost time", cmp_times[4], " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "basis change time", cmp_times[5], " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex multipiv", "step 1 time", cmp_times[6], " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex multipiv", "step 2 time", cmp_times[7], " [ms]");
        #ifdef EXPTRACING_2
        ORTABLINE(logINFO, optreptab, "--------------------------------------", "----------------------", std::numeric_limits<double>::quiet_NaN(), "------");
        ORTABLINE(logINFO, optreptab, "simplex", "deg basis changes", espqbchng_counter, "");
        ORTABLINE(logINFO, optreptab, "simplex", "computed r costs", comp_rcs, "");
        ORTABLINE(logINFO, optreptab, "simplex", "computed neg r costs", comp_negrcs, "");
        ORTABLINE(logINFO, optreptab, "simplex", "ave neg rcs chg sgn", 100.0 * rcs_chg_sgn_perc_ave / rcs_chg_sgn_ctr, "");
        ORTABLINE(logINFO, optreptab, "simplex", "min neg rcs chg sgn", 100.0 * rcs_chg_sgn_perc_min, "");
        ORTABLINE(logINFO, optreptab, "simplex", "max neg rcs chg sgn", 100.0 * rcs_chg_sgn_perc_max, "");
        
        ORTABLINE(logINFO, optreptab, "simplex", "loops' time", tm2fndloop, " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "tree coloring time", tm2clrtree, " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "tree update time", tm2udtree, " [ms]");
        ORTABLINE(logINFO, optreptab, "simplex", "shielding time", tmshielneigh, " [ms]");
        
        ORTABLINE(logINFO, optreptab, "simplex", "tree updated nodes", tree_update_counter, "");
        ORTABLINE(logINFO, optreptab, "simplex", "colored nodes", node_clrng_counter, "");
        ORTABLINE(logINFO, optreptab, "simplex", "computed loops f1", cmpd_loops_cnt_f1, "");
        ORTABLINE(logINFO, optreptab, "simplex", "computed loops f2", cmpd_loops_cnt_f2, "");
        ORTABLINE(logINFO, optreptab, "simplex", "total path len", total_path_len_f1 + total_path_len_f2, "");
        ORTABLINE(logINFO, optreptab, "simplex", "total path len f1", total_path_len_f1, "");
        ORTABLINE(logINFO, optreptab, "simplex", "total path len f2", total_path_len_f2, "");
        ORTABLINE(logINFO, optreptab, "simplex", "total loop len", total_loop_len, "");
        ORTABLINE(logINFO, optreptab, "simplex", "total loop len f1", total_loop_len_f1, "");
        ORTABLINE(logINFO, optreptab, "simplex", "total loop len f2", total_loop_len_f2, "");
        ORTABLINE(logINFO, optreptab, "simplex", "average loop len f1", avg_loop_len_f1, "");
        ORTABLINE(logINFO, optreptab, "simplex", "average loop len f2", avg_loop_len_f2, "");
        
        ORTABLINE(logINFO, optreptab, "(step 1) simplex multipiv", "tested vars", tempt_var_count, "");
        ORTABLINE(logINFO, optreptab, "(step 1) simplex multipiv", "avoided loop search", saved_lpsrch_count, "");
        ORTABLINE(logINFO, optreptab, "(step 1) simplex multipiv", "failed search", failed_lpsrch_count, "");
        ORTABLINE(logINFO, optreptab, "(step 1) simplex multipiv", "success search", succes_lpsrch_count, "");
        ORTABLINE(logINFO, optreptab, "(step 1) simplex multipiv", "oracle don't know", odk_counter, "");
        ORTABLINE(logINFO, optreptab, "(step 1) simplex multipiv", "objf improv", 100.0 * step1_improv, " [%]");
        
        ORTABLINE(logINFO, optreptab, "(step 2) simplex multipiv", "objf improv", 100.0 * step2_improv, " [%]");
        ORTABLINE(logINFO, optreptab, "(step 2) simplex multipiv", "objf improv (+)", 100.0 * step2_improv_plus, " [%]");
        ORTABLINE(logINFO, optreptab, "(step 2) simplex multipiv", "objf improv (-)", 100.0 * step2_improv_minus, " [%]");
        ORTABLINE(logINFO, optreptab, "(step 2) simplex multipiv", "improv count", step2impr_ct, "");
        ORTABLINE(logINFO, optreptab, "(step 2) simplex multipiv", "dir (+) improv count", step2imprpls_ct, "");
        ORTABLINE(logINFO, optreptab, "(step 2) simplex multipiv", "dir (-) improv count", step2imprmnd_ct, "");
        #endif
    }
    
    
    return optres;
}

std::string tplex_alg_data::toStringSol()
{
    NodeArcIdType M = us.size();
    NodeArcIdType N = vs.size();
    std::stringstream ss;
    
    FILE_LOG(logINFO) << "String sol with M = " << M << " and N = " << N;
    for(NodeArcIdType i = 0; i <  M; i++)
    {
        for(NodeArcIdType j = 0; j < N; j++)
        {
            if(this->quantities.contains(N * i + j))
                ss << " (" << i << "," << j << ") = " << this->quantities.get(N * i + j);
        }
    }
    
    return ss.str();
}

void TSimplex::initVarData(tplex_alg_data::var_data& vdata)
{
    NodeArcIdType M = tpdata_sptr->m;
    NodeArcIdType N = tpdata_sptr->n;
    NodeArcIdType VCT = M * N;
    
    /// if 1st call init. data
    if(vdata.cs.empty())
    {
        /// sort data
        if(algcfg.partition_factor < MYEPS)
        {
            auto st_5 = std::chrono::steady_clock::now();
            std::sort(tpdata_sptr->pvars.begin(), tpdata_sptr->pvars.end(), 
                      [](const std::pair<NodeArcIdType, double>& a, const std::pair<NodeArcIdType, double>& b)
                      {
                          return a.second < b.second;
                      } );
            
            FILE_LOG(logINFO) << "Sorting time >> " << GETOPTTMS(st_5) << " [ms]"; 
            st_5 = std::chrono::steady_clock::now();
            
            /// init data struct
            vdata.cs.resize(VCT);
            vdata.is.resize(VCT);
            vdata.js.resize(VCT);
            
            for(NodeArcIdType c = 0; c < tpdata_sptr->pvars.size(); c++)
            {
                vdata.cs[c] = tpdata_sptr->pvars[c].second;
                vdata.is[c] = tpdata_sptr->pvars[c].first / N;
                vdata.js[c] = tpdata_sptr->pvars[c].first % N;
            }
            FILE_LOG(logINFO) << "Data struct time >> " << GETOPTTMS(st_5) << " [ms]"; 
        }
        else
        {
            auto st_5 = std::chrono::steady_clock::now();
            
            /// Lazy clustering strategy
            #ifdef LAZYCLUST
            NodeArcIdType LC = (tpdata_sptr->m * tpdata_sptr->n) / (algcfg.partition_factor * (tpdata_sptr->m + tpdata_sptr->n));
            double ct = (std::min(1.0, tpdata_sptr->minc) + tpdata_sptr->maxc - tpdata_sptr->minc) / (LC - 1) + tpdata_sptr->minc + std::min(1.0, tpdata_sptr->minc);
            FILE_LOG(logINFO) << "**Lazy clustering** ::: cost threshold >> " << ct; 
            std::unique_ptr<std::vector<std::vector<std::pair<NodeArcIdType, double>>>> cost_partitons(new std::vector<std::vector<std::pair<NodeArcIdType, double>>>(2));
            for(NodeArcIdType v = 0; v < tpdata_sptr->pvars.size(); v++)
            {
                if(tpdata_sptr->pvars[v].second + MYEPS < ct)
                    (*cost_partitons)[0].push_back(tpdata_sptr->pvars[v]);
                else
                    (*cost_partitons)[1].push_back(tpdata_sptr->pvars[v]);
            }
            auto st_srt = std::chrono::steady_clock::now();
            std::sort((*cost_partitons)[0].begin(), (*cost_partitons)[0].end(), 
                      [](const std::pair<NodeArcIdType, double>& a, const std::pair<NodeArcIdType, double>& b)
                      {
                          return a.second < b.second;
                      } );
            
            FILE_LOG(logINFO) << "Sizes of clusters >> [0] " << (*cost_partitons)[0].size() << ", [1] " << (*cost_partitons)[1].size();
            FILE_LOG(logINFO) << "Cluster 0 sorting time >> " << GETOPTTMS(st_srt) << " [ms]"; 
            #else
            /// Initial clustering
            NodeArcIdType LC = (tpdata_sptr->m * tpdata_sptr->n) / (algcfg.partition_factor * (tpdata_sptr->m + tpdata_sptr->n));
            std::unique_ptr<std::vector<std::vector<std::pair<NodeArcIdType, double>>>> cost_partitons(new std::vector<std::vector<std::pair<NodeArcIdType, double>>>(LC));
            
            for(NodeArcIdType v = 0; v < tpdata_sptr->pvars.size(); v++)
                (*cost_partitons)[((1 + tpdata_sptr->pvars[v].second - tpdata_sptr->minc) / (1 + tpdata_sptr->maxc - tpdata_sptr->minc) * (LC - 1))].push_back(tpdata_sptr->pvars[v]);
            #endif
            
            FILE_LOG(logINFO) << "Clustering time >> " << GETOPTTMS(st_5) << " [ms]"; 
            st_5 = std::chrono::steady_clock::now();
            
            /// init data struct
            vdata.cs.resize(VCT);
            vdata.is.resize(VCT);
            vdata.js.resize(VCT);
            
            NodeArcIdType c = 0;
            for(NodeArcIdType p = 0; p < cost_partitons->size(); p++)
            {
                for(NodeArcIdType v = 0; v < (*cost_partitons)[p].size(); v++)
                {
                    vdata.cs[c] = (*cost_partitons)[p][v].second;
                    vdata.is[c] = (*cost_partitons)[p][v].first / N;
                    vdata.js[c] = (*cost_partitons)[p][v].first % N;
                    ++c;
                }
            }
            FILE_LOG(logINFO) << "Data struct time >> " << GETOPTTMS(st_5) << " [ms]"; 
        }
    }
}

NodeArcIdType
TSimplex::compute_shields_v2(const TpInstance::tcosts& costs,
                             bool redp,
                             NodeArcIdType ori,
                             NodeArcIdType dest,
                             NodeArcIdType grsize,
                             Shielding::THEgrid& board,
                             std::vector<std::vector<NodeArcIdType>>& supp,
                             std::vector<CellVar>& varredcsts,
                             double shldeps)
{
    std::unique_ptr<std::vector<std::vector<bool>>> N_sptr;
    NodeArcIdType neigh_size = 0;
    NodeArcIdType left = 0;
    NodeArcIdType right = 0;
    NodeArcIdType up = 0;
    NodeArcIdType down = 0;
    
    N_sptr.reset(new std::vector<std::vector<bool>>(ori, std::vector<bool>(dest, false)));
    
    NodeArcIdType t = 0;
    for(NodeArcIdType i = 0; i < ori; i++)
    {
        if(( redp && (!tpdata_sptr->sources_map_bw[i] || (tpdata_sptr->sources_map_bw[i] && !(tpdata_sptr->sources[tpdata_sptr->sources_map_bw[i] - 1] > shldeps + MYEPS)))) ||
           (!redp && !(tpdata_sptr->sources[i] > shldeps + MYEPS)))
            continue;
        
        for(NodeArcIdType j = 0; j < supp[i].size(); j++)
        {
            /// Step 1
            t = supp[i][j];
            if(redp && !tpdata_sptr->destinations_map_bw[t])
                continue;
        
            if(!(*N_sptr)[i][t])
            {
                (*N_sptr)[i][t] = true;
                varredcsts.push_back(redp ? CellVar(tpdata_sptr->sources_map_bw[i] - 1, tpdata_sptr->destinations_map_bw[t] - 1, 
                                                                    costs[(tpdata_sptr->sources_map_bw[i] - 1) * tpdata_sptr->n + (tpdata_sptr->destinations_map_bw[t] - 1)], 0.0)
                                          : CellVar(i, t, costs[i * dest + t], 0.0));
                ++neigh_size;
            }
            /// Step 2
            for(NodeArcIdType h = 0; h < board.cell[i].neigh.size(); h++)
            {
                if((!redp && !(*N_sptr)[board.cell[i].neigh[h]][t]) || 
                   ( redp && !(*N_sptr)[board.cell[i].neigh[h]][t] && tpdata_sptr->sources_map_bw[board.cell[i].neigh[h]]))
                {
                    (*N_sptr)[board.cell[i].neigh[h]][t] = true;
                    varredcsts.push_back(redp ? CellVar(tpdata_sptr->sources_map_bw[board.cell[i].neigh[h]] - 1, tpdata_sptr->destinations_map_bw[t] - 1, 
                                                                        costs[(tpdata_sptr->sources_map_bw[board.cell[i].neigh[h]] - 1) * tpdata_sptr->n + (tpdata_sptr->destinations_map_bw[t] - 1)], 0.0)
                                              :  CellVar(board.cell[i].neigh[h], t,  costs[board.cell[i].neigh[h] * dest + t], 0.0));
                    ++neigh_size;
                }
            }
        }
        /// Step 3
        Shielding::getRectangle(board, supp, i, left, right, up, down);
        for(NodeArcIdType h = up; h <= down; h++)
        {
            for(NodeArcIdType k = left; k <= right; k++)
            {
                t = board.table[h][k];
                if((!redp && !(*N_sptr)[i][t] && tpdata_sptr->destinations[t] > shldeps + MYEPS) ||
                   ( redp && !(*N_sptr)[i][t] && tpdata_sptr->destinations_map_bw[t] && tpdata_sptr->destinations[tpdata_sptr->destinations_map_bw[t] - 1] > shldeps + MYEPS))
                {
                    (*N_sptr)[i][t] = true;
                    varredcsts.push_back(redp ? CellVar(tpdata_sptr->sources_map_bw[i] - 1, tpdata_sptr->destinations_map_bw[t] - 1, 
                                                                        costs[(tpdata_sptr->sources_map_bw[i] - 1) * tpdata_sptr->n + (tpdata_sptr->destinations_map_bw[t] - 1)], 0.0)
                                              : CellVar(i, t, costs[i * dest + t], 0.0));
                    ++neigh_size;
                }
            }
        }
    }
    
    return neigh_size;
}

std::pair<bool,bool>
TSimplex::computeReducedCostsPol_4(const ts_sol& quantities,
                                   const std::vector<double>& us,
                                   const std::vector<double>& vs,
                                   tplex_alg_data::var_data& vdata,
                                   std::vector<CellVar>& varredcsts,
                                   std::vector<CellVar>& entering_vars)
{
    NodeArcIdType N;
    NodeArcIdType i;
    NodeArcIdType j;
    unsigned long iter;
    NodeArcIdType v;
    std::size_t lim;
    double crc;
    bool is_opt = false;
    bool new_neigh = false;
    
    N = tpdata_sptr->n;
    
    /// init board
    if(vdata.board.table.empty() || vdata.board.cell.empty())
    {
        FILE_LOG(logINFO) << "Init shileding data ...";
        Shielding::THEgenerateSquareGrid(vdata.board,
                                         tpdata_sptr->reduced ? tpdata_sptr->m_orgn : tpdata_sptr->m, 
                                         std::sqrt(tpdata_sptr->reduced ? tpdata_sptr->m_orgn : tpdata_sptr->m));
        vdata.h = 0;
        FILE_LOG(logINFO) << "Done";
    }
    
    iter = 0;
    do
    {
        is_opt = false;
        
        if(vdata.h++ > algcfg.max_shield_neigh_macroiter || varredcsts.empty() || (iter > 0 && entering_vars.empty()))
        {
            #ifdef EXPTRACING_2
            auto st_tm = std::chrono::steady_clock::now();
            #endif
        
            /// get supp
            if(vdata.spat_sptr == nullptr)
                std::runtime_error("(vdata.spat_sptr == nullptr)");
            vdata.spat_sptr->computeTSpxShieldingSupp(quantities, vdata.supp, EPSQ);
            /// translate supp into old indexing
            /// DEV CODE START
            if(tpdata_sptr->reduced)
            {
                std::vector<std::vector<NodeArcIdType>> supp_trnsltd;
                supp_trnsltd.resize(tpdata_sptr->m_orgn);
                for(NodeArcIdType i = 0; i < tpdata_sptr->sources_map.size(); i++)
                    for(NodeArcIdType t = 0; t < vdata.supp[i].size(); t++)
                        supp_trnsltd[tpdata_sptr->sources_map[i]].push_back(tpdata_sptr->destinations_map[vdata.supp[i][t]]);
                vdata.supp.swap(supp_trnsltd);
            }
            /// DEV CODE END
            
            /// clear neighbor
            varredcsts.clear();
            
            /// compute neighbor
            this->compute_shields_v2(tpdata_sptr->costs,
                                     tpdata_sptr->reduced,
                                     tpdata_sptr->reduced ? tpdata_sptr->m_orgn : tpdata_sptr->m, 
                                     tpdata_sptr->reduced ? tpdata_sptr->n_orgn : tpdata_sptr->n,
                                     std::sqrt(tpdata_sptr->reduced ? tpdata_sptr->m_orgn : tpdata_sptr->m), 
                                     vdata.board, vdata.supp, varredcsts, EPSQ);
            new_neigh = true;
            vdata.h = 0;
            
            #ifdef EXPTRACING_2
            tmshielneigh += GETOPTTMS(st_tm);
            FILE_LOG(logINFO) << "Neighboor computation time " << tmshielneigh << " [ms], size " << varredcsts.size();
            #endif
        }
        /// get variable for pivoting
        v = 0;
        do
        {
            for( ;
                v < varredcsts.size(); 
                v++)
            {
                i = varredcsts[v].i;
                j = varredcsts[v].j;
                if((crc = tpdata_sptr->costs[i * N + j] - us[i] - vs[j]) < -MYEPS)
                {
                    entering_vars.push_back(CellVar(i, j, crc));
                    #ifdef EXPTRACING_2
                    comp_negrcs++;
                    rcs_neg++;
                    #endif
                }
                #ifdef EXPTRACING_2
                else if(crc > MYEPS)
                    rcs_pos++;
                else
                    rcs_0++;
                comp_rcs++;
                #endif
            }
            
            if(entering_vars.empty() && lim < varredcsts.size())
            {
                #ifdef LAZYCLUST
                lim = varredcsts.size();
                #else
                lim += lim;
                #endif
            }
            else
                break;
        }
        while(v < varredcsts.size());
        
        if(entering_vars.empty() && new_neigh)
             is_opt = true;
        
        ++iter;
    }
    while(entering_vars.empty() && !is_opt);

    std::sort(entering_vars.rbegin(), entering_vars.rend());

    return std::make_pair(new_neigh, is_opt);
}

std::pair<bool,bool>
TSimplex::computeReducedCostsPol_3(const ts_sol& quantities,
                                   const std::vector<double>& us,
                                   const std::vector<double>& vs,
                                   tplex_alg_data::var_data& vdata,
                                   std::vector<CellVar>& varredcsts,
                                   std::vector<CellVar>& entering_vars)
{
    /// local vars
    NodeArcIdType M;
    NodeArcIdType N;
    double crc;
    bool stop_rcs;
    bool allrccomp;
    bool rcmem_recomp;
    /// init local vars
    M = tpdata_sptr->m;
    N = tpdata_sptr->n;
    crc = std::numeric_limits<double>::quiet_NaN();
    allrccomp = rcmem_recomp = false;
    #ifdef EXPTRACING_2
    rcs_neg = rcs_0 = rcs_pos = 0;
    #endif
    
    /// if not empty, process the list of candidate vars
    if(!algcfg.multipiv && !varredcsts.empty())
    {
        stop_rcs = false;
        for(auto it = varredcsts.rbegin(); !stop_rcs && it != varredcsts.rend(); it++)
        {
            if((crc = tpdata_sptr->costs[N * it->i + it->j] - us[it->i] - vs[it->j]) < -MYEPS)
            {
                entering_vars.push_back(CellVar(it->i, it->j, crc));
                #ifdef EXPTRACING_2
                comp_negrcs++;
                rcs_neg++;
                #endif
                
                if(!entering_vars.empty())
                    stop_rcs = true;
            }
            #ifdef EXPTRACING_2
            else if(crc > MYEPS)
                rcs_pos++, rcs_get_pos++;
            else
                rcs_0++, rcs_get_pos++;
            comp_rcs++;
            #endif
            
            varredcsts.erase((it + 1).base());
        }
    }
    /// otherwise, bulid a new list of candidate vars
    if(entering_vars.empty())
    {
        /// compute reduced cost
        NodeArcIdType lim = (M + N) * algcfg.window_size_factor;
        NodeArcIdType u = 0;

        /// clear
        varredcsts.clear();
        do
        {
            for( ; u < lim && u < vdata.cs.size(); u++)
            {
                if((crc = vdata.cs[u] - us[vdata.is[u]] - vs[vdata.js[u]]) < -MYEPS)
                {
                    varredcsts.push_back(CellVar(vdata.is[u], vdata.js[u], crc));
                    #ifdef EXPTRACING_2
                    comp_negrcs++;
                    rcs_neg++;
                    #endif
                }
                #ifdef EXPTRACING_2
                else if(crc > MYEPS)
                    rcs_pos++;
                else
                    rcs_0++;
                comp_rcs++;
                #endif
            }
            if(varredcsts.empty() && lim < vdata.cs.size())
            {
                #ifdef LAZYCLUST
                lim = vdata.cs.size();
                #else
                lim += lim;
                #endif
            }
            else
                break;
        }
        while(varredcsts.empty());
        allrccomp = (u >= vdata.cs.size());
        rcmem_recomp = true;
        
        #ifdef EXPTRACING_2
        if(rcs_chg_sgn_base)
        {
            #ifdef RCS_TRACING
            rcs_cmp_stats.shlist_sizes.push_back(shortlist_size);
            rcs_cmp_stats.csn_rcs.push_back(rcs_get_pos);
            #endif
            double v = double(rcs_get_pos) / rcs_chg_sgn_base;
            rcs_chg_sgn_perc_ave += v;
            rcs_chg_sgn_perc_min = std::min(rcs_chg_sgn_perc_min, v);
            rcs_chg_sgn_perc_max = std::max(rcs_chg_sgn_perc_max, v);
            ++rcs_chg_sgn_ctr;
        }
        shortlist_size = rcs_chg_sgn_base = varredcsts.size();
        rcs_get_pos = 0;
        #endif
        
        if(!varredcsts.empty())
        {
            if(algcfg.multipiv)
            {
                entering_vars.assign(varredcsts.begin(), varredcsts.end());
                varredcsts.clear();
            }
            else
            {
                std::reverse(varredcsts.begin(), varredcsts.end());
                entering_vars.push_back(CellVar(varredcsts.back().i, varredcsts.back().j, varredcsts.back().rc));
                varredcsts.pop_back();
            }
        }                
    }
    
    return std::make_pair(rcmem_recomp, allrccomp);
}

std::pair<bool,bool>
TSimplex::computeReducedCostsPol_1(const ts_sol& quantities,
                                   const std::vector<double>& us,
                                   const std::vector<double>& vs,
                                   tplex_alg_data::var_data& vdata,
                                   std::vector<CellVar>& varredcsts,
                                   std::vector<CellVar>& entering_vars)
{
    /// local vars
    NodeArcIdType M;
    NodeArcIdType N;
    double crc;
    double lowest_rd;
    NodeArcIdTypeSGND lrd_i;
    NodeArcIdTypeSGND lrd_j;
    bool stoprcc;
    /// init local vars
    M = tpdata_sptr->m;
    N = tpdata_sptr->n;
    crc = std::numeric_limits<double>::quiet_NaN();
    lowest_rd = std::numeric_limits<double>::max();
    lrd_i = -1;
    lrd_j = -1;
    #ifdef EXPTRACING_2
    rcs_neg = rcs_0 = rcs_pos = 0;
    #endif

    stoprcc = false;
    for(NodeArcIdType i = 0; !stoprcc && i < M; i++)
    {
        for(NodeArcIdType j = 0; !stoprcc && j < N; j++)
        {
            if((crc = tpdata_sptr->costs[N * i + j] - us[i] - vs[j]) < -MYEPS)
            {
                lowest_rd = crc;
                lrd_i = i;
                lrd_j = j;
                stoprcc = true;
                
                /// tracing
                #ifdef EXPTRACING_2
                comp_negrcs++;
                rcs_neg++;
                #endif
            }
            #ifdef EXPTRACING_2
            else if(crc > MYEPS)
                rcs_pos++;
            else
                rcs_0++;
            comp_rcs++;
            #endif
        }
    }
    if(lrd_i >= 0 && lrd_j >= 0)
        entering_vars.push_back(CellVar(lrd_i, lrd_j, lowest_rd));
    
    return std::make_pair(true, !(lowest_rd + MYEPS < std::numeric_limits<double>::max()));    
}

std::pair<bool,bool>
TSimplex::computeReducedCostsPol_0(const ts_sol& quantities,
                                   const std::vector<double>& us,
                                   const std::vector<double>& vs,
                                   tplex_alg_data::var_data& vdata,
                                   std::vector<CellVar>& varredcsts,
                                   std::vector<CellVar>& entering_vars)
{
    /// local vars
    NodeArcIdType M;
    NodeArcIdType N;
    double crc;
    double lowest_rd;
    NodeArcIdTypeSGND lrd_i;
    NodeArcIdTypeSGND lrd_j;
    /// init local vars
    M = tpdata_sptr->m;
    N = tpdata_sptr->n;
    crc = std::numeric_limits<double>::quiet_NaN();
    lowest_rd = std::numeric_limits<double>::max();
    lrd_i = -1;
    lrd_j = -1;
    #ifdef EXPTRACING_2
    rcs_neg = rcs_0 = rcs_pos = 0;
    #endif

    for(NodeArcIdType i = 0; i < M; i++)
    {
        for(NodeArcIdType j = 0; j < N; j++)
        {
            if((crc = tpdata_sptr->costs[N * i + j] - us[i] - vs[j]) < -MYEPS)
            {
                if(crc + MYEPS < lowest_rd)
                {
                    lowest_rd = crc;
                    lrd_i = i;
                    lrd_j = j;
                }
                
                /// tracing
                #ifdef EXPTRACING_2
                comp_negrcs++;
                rcs_neg++;
                #endif
            }
            #ifdef EXPTRACING_2
            else if(crc > MYEPS)
                rcs_pos++;
            else
                rcs_0++;
            comp_rcs++;
            #endif
        }
    }
    if(lrd_i >= 0 && lrd_j >= 0)
        entering_vars.push_back(CellVar(lrd_i, lrd_j, lowest_rd));
    
    return std::make_pair(true, true);    
}

TSimplex::pivs_data
TSimplex::pivoting(const std::shared_ptr<tplex_alg_data>& tplexd_sptr, 
                   const std::shared_ptr<SpanningTree>& bspat_sptr, 
                   std::vector<CellVar>& entering_vars, 
                   double& objf_value, 
                   double& tot_q,
                   bool update_rwcl_sol)
{
    #ifdef EXPTRACING_2
    double objfv_on_entrance = objf_value;
    deg_bchng_ctr = 0;
    #endif

    //std::stringstream ss;
    std::vector<CellVar> exited_vars;
    std::vector<CellVar> zeroed_vars;
    std::vector<CellVar> unzeroed_vars;
    std::pair<double, NodeArcIdType> getminqrv;
    unsigned long saved_lpsrch;
    unsigned long failed_lpsrch;
    unsigned long succes_lpsrch;
    unsigned long iter;
    double min_q = std::numeric_limits<double>::quiet_NaN();
    double min_q_minus = std::numeric_limits<double>::quiet_NaN();
    double s1tm = std::numeric_limits<double>::quiet_NaN();
    double s2tm = std::numeric_limits<double>::quiet_NaN();
    bool is_multipiv = algcfg.multipiv && entering_vars.size() > 1;
    bool compute_loop;
    
    /// start chrono
    auto start_s1 = std::chrono::steady_clock::now();
    if(SUBTREES && algcfg.spatvarsel)
        bspat_sptr->computeSubtrees();
    
    succes_lpsrch = failed_lpsrch = saved_lpsrch = iter = 0;
    for(auto evit = entering_vars.begin(); evit != entering_vars.end(); evit++)
    {
        #ifdef INSVS
        if(tplexd_sptr->quantities.contains(evit->i * tpdata_sptr->n + evit->j))
        {
            evit->rc = std::numeric_limits<double>::quiet_NaN();
            continue;
        }
        #endif
        
        /// look for a delta-loop (new v)
        compute_loop = !is_multipiv || !algcfg.spatvarsel
                       || bspat_sptr->checkArc(std::make_pair(evit->i, tpdata_sptr->m + evit->j), algcfg.spatvarsel_greed_lvl
                                               #ifdef EXPTRACING_2
                                               , &odk_counter
                                               #endif
                                               );
        Loop loop(tpdata_sptr->n);
        if(compute_loop)
        {
            #ifdef EXPTRACING_2
            auto st_fl = std::chrono::steady_clock::now();
            bspat_sptr->findLoop(*evit, loop, &total_path_len_f1);
            #else
            bspat_sptr->findLoop(*evit, loop);
            #endif
            
            min_q = (getminqrv = loop.getMinQ(tplexd_sptr->quantities)).first;
            
            #ifdef EXPTRACING_2
            tm2fndloop += GETOPTTMS(st_fl);
            
            if(!(min_q > MYEPS))
                failed_lpsrch++;
            else
                succes_lpsrch++;
            #endif
        }
        else
        {
            #ifdef EXPTRACING_2
            saved_lpsrch++;
            #endif
            
            /// skip var
            if(is_multipiv)
            {
                evit->rc = std::numeric_limits<double>::quiet_NaN();
                continue;
            }
        }
        #ifdef SKIPTHETA0
        /// skip pivoting if q ~ 0
        if(algcfg.multipiv && min_q < MYEPS)
        {
            evit->rc = std::numeric_limits<double>::quiet_NaN();
            continue;
        }
        #endif
        
        #ifdef EXPTRACING_2
        total_loop_len += loop.size();
        total_loop_len_f1 += loop.size();
        ++cmpd_loops_cnt_f1;        
        auto st_fl = std::chrono::steady_clock::now();
        #endif

        #ifdef EXPTRACING_2
        if(!(std::round(min_q) > 0))
            ++espqbchng_counter, ++deg_bchng_ctr;
        #endif
        
        /// move quantities and erase zeroed cell
        unzeroed_vars.clear();
        loop.moveQuantity(min_q, tpdata_sptr->costs, tplexd_sptr->quantities, !is_multipiv ? exited_vars : zeroed_vars, unzeroed_vars, 
                          #ifdef EXPTRACING_2
                          &objf_value, 
                          #else
                          nullptr, 
                          #endif
                          nullptr, !is_multipiv);
        
        #ifdef EXPTRACING_2
        tm2fndloop += GETOPTTMS(st_fl);
        auto st_ct = std::chrono::steady_clock::now();
        #endif

        if(!is_multipiv)
        {
            /// update rows/cols data
            if(update_rwcl_sol)
            {
                tplexd_sptr->delVar(exited_vars[0]);
                tplexd_sptr->addVar(*evit, tpdata_sptr->costs[tpdata_sptr->n * evit->i + evit->j]);
            }
            /// update SPAT
            bspat_sptr->update(std::make_pair(exited_vars[0].i, tpdata_sptr->m + exited_vars[0].j), std::make_pair(evit->i, tpdata_sptr->m + evit->j)
                               #ifdef EXPTRACING_2
                               , false
                               , &tree_update_counter
                               #endif
                               );
        }
        else if(algcfg.spatvarsel)
        {
            /// update SPAT
            if(algcfg.spatvarsel_greed_lvl < 2)
            {
                for(auto it = unzeroed_vars.begin(); it  != unzeroed_vars.end(); it++)
                {
                    bspat_sptr->mergeSubtree(std::make_pair(it->i, tpdata_sptr->m + it->j)
                    #ifdef EXPTRACING_2
                    , false
                    , &node_clrng_counter
                    #endif
                    );
                }
            }
            
            bspat_sptr->colorSubtree(std::make_pair(zeroed_vars.back().i, tpdata_sptr->m + zeroed_vars.back().j)
            #ifdef EXPTRACING_2
            , 0, -1, false, &node_clrng_counter
            #endif
            );
        }
        
        #ifdef EXPTRACING_2
        tm2clrtree += GETOPTTMS(st_ct);
        #endif
        
        ++iter;
    }
    
    #ifdef EXPTRACING_2
    auto st_ct = std::chrono::steady_clock::now();
    #endif
    
    if(algcfg.spatvarsel)
        bspat_sptr->resetTreeColor();
    
    #ifdef EXPTRACING_2
    tm2clrtree += GETOPTTMS(st_ct);
    #endif
    
    s1tm = GETOPTTMS(start_s1);
    
    /// return if not multipivot
    if(!is_multipiv)
        return std::make_tuple(iter, saved_lpsrch, failed_lpsrch, succes_lpsrch, s1tm, 0.0);

    #ifdef EXPTRACING_2
    step1_improv += (objfv_on_entrance - objf_value);
    objfv_on_entrance = objf_value;
    #endif
    
    /// Step 2
    double tmp_objf_impr = std::numeric_limits<double>::quiet_NaN();
    double tmp_objf_impr_minus = std::numeric_limits<double>::quiet_NaN();
    
    /// start chrono
    auto start_s2 = std::chrono::steady_clock::now();
    
    /// V2 code
    iter = 0;
    for(auto it = entering_vars.begin(); it != entering_vars.end(); it++)
    {
        if(std::isnan(it->rc))
            continue;
        
        tmp_objf_impr = tmp_objf_impr_minus = 0.0;        
        Loop loop(tpdata_sptr->n);

        #ifdef EXPTRACING_2
        auto st_fl = std::chrono::steady_clock::now();
        bspat_sptr->findLoop(*it, loop, &total_path_len_f2);
        #else
        bspat_sptr->findLoop(*it, loop);
        #endif

        /// check if improve in direction plus
        min_q = loop.getMinQ(tplexd_sptr->quantities, true, true).first;
        loop.moveQuantity(min_q, tpdata_sptr->costs, tplexd_sptr->quantities, exited_vars, unzeroed_vars, &tmp_objf_impr, nullptr, false, true, true);
        
        /// check if improve in direction minus
        min_q_minus = loop.getMinQ(tplexd_sptr->quantities, false, true).first;
        loop.moveQuantity(min_q_minus, tpdata_sptr->costs, tplexd_sptr->quantities, exited_vars, unzeroed_vars, &tmp_objf_impr_minus, nullptr, false, true, false);
        
        /// logging ::: warning
        #ifdef EXPTRACING_2
        if(tmp_objf_impr > ERTV && tmp_objf_impr_minus > ERTV)
        {
            /// throw exception if there is no improvement in both directions
            std::stringstream erss;
            erss << "** WARNING ** Dir (+) nor (-) strictly improves,"
                 <<" -loop delta(OF) " << tmp_objf_impr_minus << " and theta " << min_q_minus
                 << ", +loop delta(OF) " << tmp_objf_impr << " and theta " << min_q;
            FILE_LOG(logINFO) << erss.str();
            //throw std::runtime_error(erss.str());
        }
        #endif
        /// main program swith
        if(tmp_objf_impr - tmp_objf_impr_minus < ERTV || (std::abs(tmp_objf_impr) < ERTV && std::abs(tmp_objf_impr_minus) < ERTV))
        {
            loop.moveQuantity(min_q, tpdata_sptr->costs, tplexd_sptr->quantities, exited_vars, unzeroed_vars,
                              #ifdef EXPTRACING_2
                              &objf_value, 
                              #else
                              nullptr,
                              #endif
                              nullptr,
                              true,
                              false, true);
        
            #ifdef EXPTRACING_2
            if(std::abs(tmp_objf_impr) > MYEPS)
            {
                step2_improv_plus += -1 * tmp_objf_impr;
                ++step2impr_ct;                
                ++step2imprpls_ct;
            }
           #endif
        }
        else
        {
            loop.moveQuantity(min_q_minus, tpdata_sptr->costs, tplexd_sptr->quantities, exited_vars, unzeroed_vars,
                              #ifdef EXPTRACING_2
                              &objf_value,
                              #else
                              nullptr,
                              #endif
                              nullptr,
                              true,
                              false, false);
        
            #ifdef EXPTRACING_2
            if(std::abs(tmp_objf_impr_minus) > MYEPS)
            {
                step2_improv_minus += -1 * tmp_objf_impr_minus;
                ++step2impr_ct;                
                ++step2imprmnd_ct;
            }
           #endif
        }
        
        #ifdef EXPTRACING_2
        tm2fndloop += GETOPTTMS(st_fl);
        auto st_ut = std::chrono::steady_clock::now();
        #endif
                
        /// add the new cell
        if(tplexd_sptr->quantities.contains(tpdata_sptr->n * it->i + it->j))
        {
            /// update rows/cols data
            if(update_rwcl_sol)
            {
                tplexd_sptr->delVar(exited_vars.back());
                tplexd_sptr->addVar(*it, tpdata_sptr->costs[tpdata_sptr->n * it->i + it->j]);
            }
            /// update SPAT
            bspat_sptr->update(std::make_pair(exited_vars.back().i, tpdata_sptr->m + exited_vars.back().j), 
                               std::make_pair(it->i, tpdata_sptr->m + it->j)
                               , false
                               #ifdef EXPTRACING_2
                               , &tree_update_counter
                               #endif
                               );
        }
        
        #ifdef EXPTRACING_2
        tm2udtree += GETOPTTMS(st_ut);
        total_loop_len += loop.size();
        total_loop_len_f2 += loop.size();
        ++cmpd_loops_cnt_f2;
        #endif
        
        ++iter;
    }
    s2tm = GETOPTTMS(start_s2);
    
    #ifdef EXPTRACING_2
    step2_improv += (objfv_on_entrance - objf_value);
    #endif
    
    return std::make_tuple(iter, saved_lpsrch, failed_lpsrch, succes_lpsrch, s1tm, s2tm);
}

tplex_alg_data::tplex_alg_data(const std::shared_ptr<TpInstance::TProblemData>& tpdsptr, bool to_fill)
    : quantities(ts_sol(tpdsptr->m * tpdsptr->n, std::numeric_limits<double>::quiet_NaN()))
    , us(std::vector<double>(tpdsptr->m, std::numeric_limits<double>::quiet_NaN()))
    , vs(std::vector<double>(tpdsptr->n, std::numeric_limits<double>::quiet_NaN()))
{

    if(to_fill)
    {
        rows.resize(tpdsptr->m, std::vector<CellVar>());
        for(auto it = rows.begin(); it != rows.end(); it++)
            it->reserve(tpdsptr->n);
        cols.resize(tpdsptr->n, std::vector<CellVar>());
        for(auto it = cols.begin(); it != cols.end(); it++)
            it->reserve(tpdsptr->m);
    }
    else
    {
        rows.resize(tpdsptr->m, std::vector<CellVar>(tpdsptr->n));
        cols.resize(tpdsptr->n, std::vector<CellVar>(tpdsptr->m));
        sources = tpdsptr->sources;
        destinations = tpdsptr->destinations;

        /// fill-in data struct1
        for(NodeArcIdType i = 0; i < tpdsptr->m; i++)
        {
            for(NodeArcIdType j = 0; j < tpdsptr->n; j++)
            {
                rows[i][j].i = cols[j][i].i = i;
                rows[i][j].j = cols[j][i].j = j;
                rows[i][j].cost = cols[j][i].cost = tpdsptr->costs[tpdsptr->n * i + j];
            }
        }
    }
}

void tplex_alg_data::sort()
{
    /// sort
    for(NodeArcIdType i = 0; i < rows.size(); i++)
        std::sort(rows[i].begin(), rows[i].end(), [](const CellVar& lhs, const CellVar& rhs) { return lhs.cost >= rhs.cost + MYEPS; });

    for(NodeArcIdType j = 0; j < cols.size(); j++)
        std::sort(cols[j].begin(), cols[j].end(), [](const CellVar& lhs, const CellVar& rhs) { return lhs.cost >= rhs.cost + MYEPS; });
}


inline tplex_alg_data::tplex_alg_data::tplex_alg_data(const tplex_alg_data& arg)
    : quantities(arg.quantities.size(), std::numeric_limits<double>::quiet_NaN())
{
    *this = arg;
}

inline void tplex_alg_data::tplex_alg_data::addVar(CellVar& ev, double c)
{
    rows[ev.i].push_back(CellVar(ev.i, ev.j, c, 0.0));
    cols[ev.j].push_back(CellVar(ev.i, ev.j, c, 0.0));
}
inline void tplex_alg_data::tplex_alg_data::delVar(CellVar& exv)
{
    for(auto it = rows[exv.i].rbegin(); it != rows[exv.i].rend(); it++)
        if(it->j == exv.j)
            rows[exv.i].erase((it + 1).base());
    for(auto it = cols[exv.j].rbegin(); it != cols[exv.j].rend(); it++)
        if(it->i == exv.i)
            cols[exv.j].erase((it + 1).base());
}

void tplex_alg_data::clearSol()
{
    quantities = ts_sol(rows.size() * cols.size(), std::numeric_limits<double>::quiet_NaN());
    for(NodeArcIdType i = 0; i < rows.size(); i++)
        rows[i].clear();
    for(NodeArcIdType j = 0; j < cols.size(); j++)
        cols[j].clear();
    solution.clear();
}
