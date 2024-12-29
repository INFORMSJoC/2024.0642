///
/// src/algs/TSimplexSolution.h
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


#ifndef TSIMPLEXSOL_H
#define TSIMPLEXSOL_H

#include "TpInstance.h"

namespace TSimplexData
{
    /// 
    /// T. simplex algorithm solution data
    /// 
    class tsimplex_sparse_sol : private std::unordered_map<NodeArcIdType, double>
    {
    public:
        inline tsimplex_sparse_sol(NodeArcIdType aMN, double) : std::unordered_map<NodeArcIdType, double>(), MN(aMN) {}
        inline ~tsimplex_sparse_sol() {}
        
        inline tsimplex_sparse_sol(const tsimplex_sparse_sol& ref)
        {
            *this = ref;
        }
        
        inline const NodeArcIdType& size() const
        {
            return MN;
        }
        
        inline void resize(const NodeArcIdType&, const double&) { }
        
        inline const double& get(const NodeArcIdType& e) const
        {
            auto it = this->find(e);
            if(it != this->end()) 
            {
                return it->second;
            }
            else
            {
                throw std::runtime_error("No element " + std::to_string(e));
            }
        }
         
        inline double& get(const NodeArcIdType& e)
        {
            auto it = this->find(e);
            if(it != this->end()) 
            {
                return it->second;
            }
            else
            {
                throw std::runtime_error("No element " + std::to_string(e));
            }
        }
         
        inline void set(const NodeArcIdType& e, const double& v)
        {
            auto it = this->find(e);
            if(it == this->end())
            {
                this->insert(std::make_pair(e, v));
            }
            else
            {
                it->second = v;
            }
        }
        
        inline bool contains(const NodeArcIdType& e) const
        {
            return (this->find(e) != this->end());
        }
        
        inline void remove(const NodeArcIdType& e)
        {
            this->erase(e);
        }
        
    private:
        NodeArcIdType MN = 0;
    };
    class tsimplex_dense_sol : private std::vector<double>
    {
    public:
        inline tsimplex_dense_sol(std::vector<double>::size_type aMN, double v) : std::vector<double>(aMN, v) {}
        
        using std::vector<double>::size;
        using std::vector<double>::resize;
        
        inline const double& get(const NodeArcIdType& e) const
        {
            return std::vector<double>::operator[](e);
        }
         
        inline double& get(const NodeArcIdType& e)
        {
            return std::vector<double>::operator[](e);
        }
         
        inline void set(const NodeArcIdType& e, const double& v)
        {
            std::vector<double>::operator[](e) = v;
        }
        
        inline bool contains(const NodeArcIdType& e) const
        {
            return !std::isnan(std::vector<double>::operator[](e));
        }
        
        inline void remove(const NodeArcIdType& e)
        {
            std::vector<double>::operator[](e) = std::numeric_limits<double>::quiet_NaN();
        }
    };
    #ifdef TSSOLSPRS
    typedef class tsimplex_sparse_sol ts_sol;
    #else
    typedef class tsimplex_dense_sol ts_sol;
    #endif
}

#endif // 