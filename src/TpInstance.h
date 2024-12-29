///
/// src/instance/TpInstance.h
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

#ifndef TPINST_H
#define TPINST_H

#include <string>
#include <memory>
#include <vector>
#include <limits>
#include <cmath>
#include <iostream>

#define F4DDR 1e4

//SGN_NODEARCID_TYPE

#define NODEARC_INT_TYPE long
typedef NODEARC_INT_TYPE NodeArcIdTypeSGND;
typedef unsigned NODEARC_INT_TYPE NodeArcIdType;

typedef double TpCostType;
typedef double TpQuantityType;

class TpInstance
{
public:
    typedef std::vector<TpCostType> tcosts;

    struct TProblemData
    {
        unsigned long long rndseed;
        unsigned int cost_f = 0;
        bool reduced = false;
        bool swapped = false;
        
        TpQuantityType tot_dst_quantity = std::numeric_limits<TpQuantityType>::quiet_NaN();
        TpQuantityType tot_src_quantity = std::numeric_limits<TpQuantityType>::quiet_NaN();

        double minc = std::numeric_limits<double>::quiet_NaN();
        double maxc = std::numeric_limits<double>::quiet_NaN();
        double avgc = std::numeric_limits<double>::quiet_NaN();
        
        NodeArcIdType m;
        NodeArcIdType n;
        
        std::vector<TpQuantityType> sources;
        std::vector<TpQuantityType> destinations;
        tcosts costs;
        std::vector<std::pair<NodeArcIdType, double>> pvars;
        
        NodeArcIdType m_orgn;
        NodeArcIdType n_orgn;
        NodeArcIdType srcs_at_zero;
        NodeArcIdType dsts_at_zero;
        std::vector<NodeArcIdType> sources_map;
        std::vector<NodeArcIdType> destinations_map;
        std::vector<NodeArcIdType> sources_map_bw;
        std::vector<NodeArcIdType> destinations_map_bw;
        
        std::string name = "";
        std::string initial_base_fname = "current_base.txt";
        std::string solution_output_fname = "current_sol.txt";
        bool dump_down_sol = false;
        unsigned long long max_macroiter = (std::numeric_limits<unsigned long long>::max)();
        
        /// method to remove srcs and dsts with 0 q.
        bool reduce();
        bool hasSrcOrDstAt0();
        void logStrictInfo();
    };
    
    typedef struct TProblemData* TProblemDataPtr;
    typedef struct TProblemData& TProblemDataRef;
        
    TpInstance(std::string);
    ~TpInstance();

    std::string getBaseName();
    std::string getName();

    bool isToGenerate();
    bool isToWrite();
    void toGenerate();
    void toWrite();
    void logInfo();
    void setName(std::string);
    void write2file(std::string);
    
    const std::shared_ptr<TProblemData>& getInstanceData();
    std::shared_ptr<TProblemData> generateData();
    std::shared_ptr<TProblemData> generateData(bool, unsigned int, unsigned int, double, double, NodeArcIdType, NodeArcIdType, unsigned long long, unsigned int iid = 1);

private:
    std::string ifname;
    bool to_generate = false;
    bool to_write_to_file = false;

    std::shared_ptr<TProblemData> tp_data_sptr;
};

#endif // TPINST_H