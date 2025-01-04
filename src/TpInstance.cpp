///
/// src/instance/TpInstance.cpp
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

#ifdef _WIN32
// windows code goes here
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <windows.h>
#endif

#include <limits>
#include <fstream>
#include <numeric>
#include <random>
#include <chrono>
#include <map>

#include "TpInstance.h"
#include "util.h"
#include "MyLog.h"

#define GETOPTTMS(start) double((std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()) / 1000.0)


TpInstance::TpInstance(std::string ifnm)
{
    std::ifstream ifs(ifnm);
    
    /// open input file
    if(!ifs.is_open())
    {
        throw std::runtime_error("File not found");
    }
    
    unsigned long long rnds; 
    ifname = ifnm;
    tp_data_sptr.reset(new TProblemData());
    NodeArcIdType m;
    NodeArcIdType n;
    
    ifs >> m;
    ifs >> n;
    ifs >> rnds;
    tp_data_sptr->cost_f = rnds;
    
    if(m * n > std::numeric_limits<NodeArcIdTypeSGND>::max())
        throw std::invalid_argument("Instance too large, max. m*n value is " + std::to_string(std::numeric_limits<NodeArcIdTypeSGND>::max()));
    
    tp_data_sptr->m = m;
    tp_data_sptr->n = n;
    tp_data_sptr->rndseed = rnds;
    
    if(m == 0 || n == 0)
        throw std::invalid_argument("Instance must have at least one source and one destination");
    
    tp_data_sptr->reduced = false;
    tp_data_sptr->srcs_at_zero = 0;
    tp_data_sptr->dsts_at_zero = 0;
    
    /// read sources form file
    tp_data_sptr->sources.resize(m, 0.0);
    tp_data_sptr->tot_src_quantity = 0.0;
    for(NodeArcIdType i = 0; i < m; i++)
    {
        ifs >> tp_data_sptr->sources[i];
        
        if(tp_data_sptr->sources[i] < MYEPS)
            ++tp_data_sptr->srcs_at_zero;
        
        tp_data_sptr->tot_src_quantity += tp_data_sptr->sources[i];
    }
    /// read destination demands from file
    tp_data_sptr->destinations.resize(n, 0.0);
    tp_data_sptr->tot_dst_quantity = 0.0;
    for(NodeArcIdType j = 0; j < n; j++)
    {
        ifs >> tp_data_sptr->destinations[j];
        
        if(tp_data_sptr->destinations[j] < MYEPS)
            ++tp_data_sptr->dsts_at_zero;
        
        tp_data_sptr->tot_dst_quantity += tp_data_sptr->destinations[j];
    }
    /// read cost matrix
    tp_data_sptr->pvars.resize(m * n);
    tp_data_sptr->costs.resize(m * n, 0.0);
    tp_data_sptr->minc = std::numeric_limits<double>::max();
    tp_data_sptr->maxc = std::numeric_limits<double>::lowest();
    tp_data_sptr->avgc = 0.0;
    double cf = 1.0 / (m * n);
    
    for(NodeArcIdType i = 0; i < m; i++)
    {
        for(NodeArcIdType j = 0; j < n; j++)
        {
            ifs >> tp_data_sptr->costs[n * i + j];
            tp_data_sptr->pvars[n * i + j] = std::make_pair(n * i + j, tp_data_sptr->costs[n * i + j]);
            tp_data_sptr->minc = std::min(tp_data_sptr->costs[n * i + j], tp_data_sptr->minc);
            tp_data_sptr->maxc = std::max(tp_data_sptr->costs[n * i + j], tp_data_sptr->maxc);
            tp_data_sptr->avgc += tp_data_sptr->costs[n * i + j] * cf;
        }
    }
    
    ifs.close();
}

TpInstance::~TpInstance()
{
}

bool TpInstance::TProblemData::hasSrcOrDstAt0()
{
    return srcs_at_zero || dsts_at_zero;
}

bool TpInstance::TProblemData::reduce()
{
    reduced = false;
    if(srcs_at_zero || dsts_at_zero)
    {
        NodeArcIdType M;
        NodeArcIdType N;
        
        M = m;
        N = n;
        /// SOURCES
        /// process sources
        sources_map.resize(M, 0);
        for(NodeArcIdType i = 0; i < M; i++)
            sources_map[i] = i;
        
        NodeArcIdType i = 0;
        for(auto it = sources_map.begin(); it != sources_map.end(); it++, i++)
        {
            if(sources[i] < MYEPS)
                sources_map.erase(it--);
        }
        /// remove 0 srcs
        for(auto it = sources.begin(); it != sources.end(); it++)
        {
            if(*it < MYEPS)
                sources.erase(it--);
        }
        /// pop backward map
        sources_map_bw.resize(M, 0);
        for(NodeArcIdType i = 0; i < sources_map.size(); i++)
            sources_map_bw[sources_map[i]] = i + 1;
        /// DESTINATIONS
        /// process destionations
        destinations_map.resize(M, 0);
        for(NodeArcIdType j = 0; j < N; j++)
            destinations_map[j] = j;
        
        NodeArcIdType j = 0;
        for(auto it = destinations_map.begin(); it != destinations_map.end(); it++, j++)
        {
            if(destinations[j] < MYEPS)
                destinations_map.erase(it--);
        }
        /// remove 0 dsts
        for(auto it = destinations.begin(); it != destinations.end(); it++)
        {
            if(*it < MYEPS)
                destinations.erase(it--);
        }
        /// pop backward map
        destinations_map_bw.resize(N, 0);
        for(NodeArcIdType j = 0; j < destinations_map.size(); j++)
            destinations_map_bw[destinations_map[j]] = j + 1;
        
        swapped = false;
        if(sources_map.size() < destinations_map.size())
        {
            std::swap(M, N);
            std::swap(m, n);
            std::swap(tot_src_quantity, tot_dst_quantity);
            std::swap(srcs_at_zero, dsts_at_zero);
            sources.swap(destinations);
            sources_map.swap(destinations_map);
            sources_map_bw.swap(destinations_map_bw);
            swapped = true;
        }
        
        /// redo cost matrix
        std::vector<double> newcmtx;
        pvars.clear();
        newcmtx.reserve(sources_map.size() * destinations_map.size());
        pvars.reserve(sources_map.size() * destinations_map.size());
        minc = std::numeric_limits<double>::max();
        maxc = std::numeric_limits<double>::lowest();
        avgc = 0.0;
        double cf = 1.0 / (sources_map.size() * destinations_map.size());
        
        for(NodeArcIdType i = 0; i < sources_map.size(); i++)
        {
            for(NodeArcIdType j = 0; j < destinations_map.size(); j++)
            {
                newcmtx.push_back(costs[swapped
                                                        ? destinations_map[j] * M + sources_map[i]
                                                        : sources_map[i] * N + destinations_map[j]
                                                     ]);
                pvars.push_back(std::make_pair(i * destinations_map.size() + j, newcmtx.back()));
                minc = std::min(newcmtx.back(), minc);
                maxc = std::max(newcmtx.back(), maxc);
                avgc += newcmtx.back() * cf;
            }
        }
        costs = newcmtx;
        
        m_orgn = m;
        n_orgn = n;
        m = sources_map.size();
        n = destinations_map.size();
        reduced = true;
    }
    
    return reduced;
}

std::string TpInstance::getBaseName()
{
    return tp_data_sptr->name;
}

std::string TpInstance::getName()
{
    return ifname;
}

void TpInstance::toGenerate()
{
    to_generate = true;
}

void TpInstance::toWrite()
{
    to_write_to_file = true;
}

bool TpInstance::isToGenerate()
{
    return to_generate;
}

bool TpInstance::isToWrite()
{
    return to_write_to_file;
}

void TpInstance::setName(std::string nn)
{
    ifname = nn;
}

void TpInstance::write2file(std::string ofnm)
{
    std::ofstream ofs(ofnm);
    
    /// open input file
    if(!ofs.is_open())
    {
        throw std::runtime_error("File not found");
    }
    
    ofs << tp_data_sptr->m << " ";
    ofs << tp_data_sptr->n << " ";
    ofs << tp_data_sptr->rndseed;
    ofs << std::endl;

    for(NodeArcIdType i = 0; i < tp_data_sptr->m; i++)
    {
        ofs << (unsigned int)(tp_data_sptr->sources[i]);
        if(i + 1 < tp_data_sptr->m)
            ofs << " ";
    }
    ofs << std::endl;

    for(NodeArcIdType j = 0; j < tp_data_sptr->n; j++)
    {
        ofs << (unsigned int)(tp_data_sptr->destinations[j]);
        if(j + 1 < tp_data_sptr->n)
            ofs << " ";
    }
    ofs << std::endl;
    
    for(NodeArcIdType i = 0; i < tp_data_sptr->m; i++)
    {
        for(NodeArcIdType j = 0; j < tp_data_sptr->n; j++)
        {
            ofs << tp_data_sptr->costs[tp_data_sptr->n * i + j];
            if(j + 1 < tp_data_sptr->n)
                ofs << " ";
        }
        ofs << std::endl;
    }
    
    ofs.close();
}

const std::shared_ptr<TpInstance::TProblemData>& TpInstance::getInstanceData()
{
    return tp_data_sptr;
}

std::shared_ptr<TpInstance::TProblemData> TpInstance::generateData()
{
    //return generateData(true,
    return generateData(tp_data_sptr->destinations[1] > 0.5,
                        tp_data_sptr->costs[3], tp_data_sptr->costs[4],
                        tp_data_sptr->costs[0], tp_data_sptr->costs[1],
                        tp_data_sptr->sources[0], tp_data_sptr->destinations[0],
                        tp_data_sptr->rndseed, tp_data_sptr->sources[2]);
}

std::shared_ptr<TpInstance::TProblemData> TpInstance::generateData(bool ap, unsigned int minq, unsigned int maxq, double minc, double maxc, NodeArcIdType marg, NodeArcIdType narg, unsigned long long rndsarg, unsigned int iid)
{
    if(ap && narg != marg)
        throw std::invalid_argument("For AP M must equal N");
    
    FILE_LOG(logINFO) << "Generate instance with random seed " << rndsarg << " ...";
    std::shared_ptr<TpInstance::TProblemData> tpdata_sptr(new TpInstance::TProblemData());
    NodeArcIdType idx;
    double absdiff;
    double q2add;
    double src_cf;
    double dst_cf;
    tpdata_sptr->m = marg;
    tpdata_sptr->n = narg;
    tpdata_sptr->rndseed = rndsarg;
    
    /// start chrono
    auto start = std::chrono::steady_clock::now();
    
    src_cf = std::max(1.0, std::ceil(double(narg) / double(marg)));
    dst_cf = std::max(1.0, std::ceil(double(marg) / double(narg)));
    
    /// generate src and dst quantities
    std::default_random_engine rndngen(rndsarg);
    /// src q
    std::uniform_int_distribution<unsigned int> srcq_distr(minq, maxq * src_cf);
    tpdata_sptr->tot_src_quantity = 0.0;
    for(NodeArcIdType i = 0; i < tpdata_sptr->m; i++)
    {
        tpdata_sptr->sources.push_back(ap ? 1.0 : srcq_distr(rndngen));
        tpdata_sptr->tot_src_quantity += tpdata_sptr->sources.back();
    }
    /// dst q
    std::uniform_int_distribution<unsigned int> dstq_distr(minq, maxq * dst_cf);
    tpdata_sptr->tot_dst_quantity = 0.0;
    for(NodeArcIdType j = 0; j < tpdata_sptr->n; j++)
    {
        tpdata_sptr->destinations.push_back(ap ? 1.0 : dstq_distr(rndngen));
        tpdata_sptr->tot_dst_quantity += tpdata_sptr->destinations.back();
    }
    /// balance qs
    if(!ap)
    {
        std::uniform_int_distribution<NodeArcIdType> rndi(0, tpdata_sptr->m - 1);
        std::uniform_int_distribution<NodeArcIdType> rndj(0, tpdata_sptr->n - 1);
        while((absdiff = std::abs(tpdata_sptr->tot_src_quantity - tpdata_sptr->tot_dst_quantity)) > MYEPS)
        {
            std::uniform_int_distribution<unsigned int> diffqntsdistr(minq, (unsigned int)absdiff);
            
            if(tpdata_sptr->tot_src_quantity + MYEPS < tpdata_sptr->tot_dst_quantity)
            {
                idx = rndi(rndngen);
                q2add = std::min((double)diffqntsdistr(rndngen), src_cf * maxq - tpdata_sptr->sources[idx]);
                tpdata_sptr->sources[idx] += q2add;
                tpdata_sptr->tot_src_quantity += q2add;
            }
            else
            {
                idx = rndj(rndngen);
                q2add = std::min((double)diffqntsdistr(rndngen), dst_cf * maxq - tpdata_sptr->destinations[idx]);
                tpdata_sptr->destinations[idx] += q2add;
                tpdata_sptr->tot_dst_quantity += q2add;
            }
        }
    }
    /// init cost matrix
    tpdata_sptr->minc = std::numeric_limits<double>::max();
    tpdata_sptr->maxc = std::numeric_limits<double>::min();
    tpdata_sptr->avgc = 0.0;
    tpdata_sptr->pvars.resize(tpdata_sptr->m * tpdata_sptr->n);
    std::uniform_int_distribution<unsigned int> rndc(minc, maxc);
    
    double cf = 1.0 / (tpdata_sptr->m * tpdata_sptr->n);
    NodeArcIdType c = 0;
    tpdata_sptr->costs.resize(tpdata_sptr->m * tpdata_sptr->n, std::numeric_limits<double>::quiet_NaN());
    
    for(auto it = tpdata_sptr->costs.begin(); it != tpdata_sptr->costs.end(); it++, c++)
    {
        *it = (double)rndc(rndngen);
        tpdata_sptr->pvars[c] = std::make_pair(c, *it);
        tpdata_sptr->minc = std::min(*it, tpdata_sptr->minc);
        tpdata_sptr->maxc = std::max(*it, tpdata_sptr->maxc);
        tpdata_sptr->avgc += cf * (*it);
    }
    
    FILE_LOG(logINFO) << "Instance generation time = " << GETOPTTMS(start) << " [ms]";
    
    tpdata_sptr->name = "tp_" + std::to_string(int(minq)) +
                        "_" + std::to_string(int(maxq)) + 
                        "_" + std::to_string(int(minc)) +
                        "_" + std::to_string(int(maxc)) +
                        "_" + std::to_string(tpdata_sptr->m) +
                        "_" + std::to_string(tpdata_sptr->n) + 
                        "_" + std::to_string(iid);

    return tpdata_sptr;
}

void TpInstance::TProblemData::logStrictInfo()
{
    //FILE_LOG(logINFO) << "Instance file: " << ifname;
    if(swapped)
    {
        FILE_LOG(logINFO) << "Data swapped";
    }
    FILE_LOG(logINFO) << "Max. possible M*N value is " << std::numeric_limits<NodeArcIdType>::max();
    FILE_LOG(logINFO) << "Max. possible obj f value is " << std::numeric_limits<double>::max();
    FILE_LOG(logINFO) << "T. problem has " << sources.size() << " supply facilities and " << destinations.size() << " demand spots";
    FILE_LOG(logINFO) << "Total supply quantities " << std::fixed << tot_src_quantity;
    FILE_LOG(logINFO) << "Supplies with 0 quantity " << std::fixed << srcs_at_zero;
    FILE_LOG(logINFO) << "Total demand quantities " << std::fixed << tot_dst_quantity;
    FILE_LOG(logINFO) << "Destinations with 0 quantity " << std::fixed << dsts_at_zero;
    FILE_LOG(logINFO) << "Average t cost " << avgc;
    FILE_LOG(logINFO) << "Max t cost " << maxc;
    FILE_LOG(logINFO) << "Min t cost " << minc;
}

void TpInstance::logInfo()
{
    FILE_LOG(logINFO) << "Instance file: " << ifname;
    FILE_LOG(logINFO) << "Max. M*N value is " << std::numeric_limits<NodeArcIdType>::max();
    FILE_LOG(logINFO) << "Max. possible obj f value is " << std::numeric_limits<double>::max();
    FILE_LOG(logINFO) << "T. problem has " << tp_data_sptr->sources.size() << " supply facilities and " << tp_data_sptr->destinations.size() << " demand spots";
    FILE_LOG(logINFO) << "Cost matrix has " << tp_data_sptr->m << " rows and " << tp_data_sptr->n << " columns";
    FILE_LOG(logINFO) << "Average t cost " << tp_data_sptr->avgc;
    FILE_LOG(logINFO) << "Max cost " << tp_data_sptr->maxc;
    FILE_LOG(logINFO) << "Min cost " << tp_data_sptr->minc;
    
    unsigned long tot_supply = 0;
    unsigned long tot_demand = 0;

    for(NodeArcIdType i = 0; i < tp_data_sptr->sources.size(); i++)
        tot_supply += static_cast<unsigned int>(tp_data_sptr->sources[i]);
    
    for(NodeArcIdType i = 0; i < tp_data_sptr->destinations.size(); i++)
        tot_demand += static_cast<unsigned int>(tp_data_sptr->destinations[i]);
    
    if(LOGLEVEL > logDEBUG2)
    {
        std::stringstream ss;
        ss << "Q. of supplies: [";
        /// log supplies
        for(NodeArcIdType i = 0; i < tp_data_sptr->sources.size(); i++)
        {
            tot_supply += static_cast<unsigned int>(tp_data_sptr->sources[i]);
            ss << " " << tp_data_sptr->sources[i];
        }
        ss << "]";

        FILE_LOG(logDEBUG2) << ss.str();
        
        ss.str("");
        ss << "Q. of destinations: [";
        /// log supplies
        for(NodeArcIdType i = 0; i < tp_data_sptr->destinations.size(); i++)
        {
            tot_demand += static_cast<unsigned int>(tp_data_sptr->destinations[i]);
            ss << " " << tp_data_sptr->destinations[i];
        }
        ss << "]";

        FILE_LOG(logDEBUG2) << ss.str();
        
        ss.str("");
        ss << "Matrix of costs: [";
        /// log supplies
        for(NodeArcIdType i = 0; i < tp_data_sptr->sources.size(); i++)
        {
            ss << " [";
            for(NodeArcIdType j = 0; j < tp_data_sptr->destinations.size(); j++)
            {
                ss << " " << tp_data_sptr->costs[tp_data_sptr->n * i + j];
            }
            ss << "] ";
        }
        ss << "]";

        FILE_LOG(logDEBUG2) << ss.str();
    }
    
    FILE_LOG(logINFO)   << (tot_demand == tot_supply ? "Balanced " : "Unbalanced ") 
                        << "supply-demand quantities, tot. supply = " << tot_supply 
                        << ", tot. demand = " << tot_demand;
}

















