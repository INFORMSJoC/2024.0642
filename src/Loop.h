///
/// src/algs/Loop.h
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


#ifndef LOOP_H
#define LOOP_H

#include "TpInstance.h"
#include "TSimplexSolution.h"

namespace TSimplexData
{    
    /// loop stone data struct
    struct LoopStone
    {
        NodeArcIdTypeSGND prev_stone_pos = -1;
        NodeArcIdType i = 0;
        NodeArcIdType j = 0;
        
        inline LoopStone() { }
        inline LoopStone(NodeArcIdTypeSGND psp, NodeArcIdType ia, NodeArcIdType ja)
            : prev_stone_pos(psp), i(ia), j(ja) { }
        
        inline bool operator==(const LoopStone& rhs) const { return i == rhs.i && j == rhs.j; }
        
        inline std::string toString(const NodeArcIdType N = 0, const std::vector<double>* q_ptr = nullptr)
        { 
            return "(" + std::to_string(i) + "," + std::to_string(j) + ")" + (!N ? "" : "{" + std::to_string((*q_ptr)[i * N + j]) + "}");
        }
    };
    /// Tableau cell data struct
    struct CellVar : public LoopStone
    {
        double cost = std::numeric_limits<double>::quiet_NaN();
        double rc = std::numeric_limits<double>::quiet_NaN();
        
        inline CellVar() { }
        inline CellVar(NodeArcIdType ia, NodeArcIdType ja, double rca) : LoopStone::LoopStone(-1, ia, ja), rc(rca) { }
        inline CellVar(NodeArcIdType ia, NodeArcIdType ja, double ca, double rca) : LoopStone::LoopStone(-1, ia, ja), cost(ca), rc(rca) { }
        
        inline bool operator<(const CellVar& arg) const { return rc > arg.rc; /* rc < arg.rc; */ }
        inline bool operator==(const CellVar& arg) const { return i == arg.i && j == arg.j; }
        
        inline std::string toString(bool wrc = false) const { return "(" + std::to_string(i) + "," + std::to_string(j) + ")"; }
    };
    /// loop data struct
    struct Loop : public std::vector<LoopStone>
    {
        inline Loop(NodeArcIdType na) : n(na) { }
        
        std::pair<double, NodeArcIdType> getMinQ(const ts_sol& quantities, bool new_incr = true, bool zvcr = true, bool prob0 = false);
        std::string toString(const NodeArcIdType N = 0, const std::vector<double>* q_ptr = nullptr);
        NodeArcIdType moveQuantity(double minq,
                                   const TpInstance::tcosts& costs,
                                   ts_sol& quantities,
                                   std::vector<CellVar>& exited_vars,
                                   std::vector<CellVar>& unzeroed_vars,
                                   double* objf_value = nullptr,
                                   double* tot_q = nullptr,
                                   bool remov_if_0 = true, bool just_check = false, bool incr = true);
                                  
        inline NodeArcIdType getN() { return n; }
        
    private:
        const NodeArcIdType n;
    };
}

#endif // LOOP_H
