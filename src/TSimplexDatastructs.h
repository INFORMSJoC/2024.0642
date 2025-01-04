///
/// src/algs/TSimplexDatastructs.h
///
/// Written by Roberto Bargetto
///        DIGEP
///        Politecnico di Torino
///        Corso Duca degli Abruzzi, 10129, Torino
///        Italy
///
/// Copyright 2023 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
///
/// This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// See the license at http://creativecommons.org/licenses/by-nc-sa/4.0/
///


#ifndef TSIMPLEXDATA_H
#define TSIMPLEXDATA_H

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

#include "SpanningTree.h"
#include "Shielding.h"
#include "TpInstance.h"
#include "util.h"

#define EPSQ ((2.0 - 20.0 * MYEPS) * MYEPS)

namespace TSimplexData
{
    struct tplex_alg_data
    {    
        /// var data struct
        struct var_data
        {
            std::vector<double> cs;
            std::vector<NodeArcIdType> is;
            std::vector<NodeArcIdType> js;
            
            Shielding::THEgrid board;
            std::vector<std::vector<NodeArcIdType>> supp;
            
            std::shared_ptr<SpanningTree> spat_sptr;
            
            unsigned long h = 0;
            
            NodeArcIdType next_arc = 0;
            bool next_flag = true;
            std::vector<double> vs_max;
            std::vector<NodeArcIdType> b;
            //std::vector<NodeArcIdTypeSGND> uplr;
            std::vector<NodeArcIdType> Lrow;
            std::vector<NodeArcIdType> Lmax;
            std::vector<NodeArcIdType> row_rotors;
            std::vector<NodeArcIdType> row_rotors_0;
            std::vector<bool> rotor_flags;
            std::vector<bool> rcneg_flags;
            std::vector<bool> row_flags;
        };
        /// struct attribute
        /// algorithm utility
        /// solution
        std::vector<CellVar> solution;
        size_t partial_basis_size;
        /// solution by rows
        std::vector<std::vector<CellVar>> rows;
        /// solution by cols
        std::vector<std::vector<CellVar>> cols;
        
        ts_sol quantities;
        std::vector<double> sources;
        std::vector<double> destinations;
        
        std::vector<double> us;
        std::vector<double> vs;
        var_data vdata;
    
        /// struct constructor
        tplex_alg_data(const std::shared_ptr<TpInstance::TProblemData>&, bool to_fill = false);
        tplex_alg_data(const tplex_alg_data&);
        /// struct method
        void sort();
        void addVar(CellVar& env, double c);
        void delVar(CellVar& exv);
        void clearSol();
        std::string toStringSol();
    };
}


#endif // TSIMPLEXDATA_H