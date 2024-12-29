///
/// src/algs/SpanningTreeImpl.cpp
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
#include <utility>
#include <random>
#include <cstdlib> 
#include <ctime> 
#include <stack> 
#include <set> 
#include <cmath> 
#include <fstream>
#include <iterator>
#include <iostream>
#include <time.h>

#include "TSimplexDatastructs.h"
#include "SpanningTree.h"
#include "util.h"

using namespace TSimplexData;


///
/// Class SpanningTree implementation
/// 
inline
SpanningTree::NodeType SpanningTree::detach(NodeType idx, NodeType jdx
                                                                       #ifdef EXPTRACING_2
                                                                       , unsigned long& op_ctr
                                                                       #endif
                                                                       )
{
    if(idx < 0 || jdx < 0)
        throw std::invalid_argument("(a < 0 || b < 0) is true");

    NodeType root_of_new_tree;
    /// case idx --> jdx
    if(idx == predecessor[jdx])
    {
        root_of_new_tree = jdx;
        predecessor[jdx] = -1;
        if(successor[idx] == jdx)
        {
            successor[idx] = youngerbro[jdx];
            if(youngerbro[jdx] >= 0)
            {
                elderbro[youngerbro[jdx]] = -1;
                /// tracement stuff
                #ifdef EXPTRACING_2
                ++op_ctr;
                #endif
            }
        }
        else
        {
            if(elderbro[jdx] >= 0)
            {
                youngerbro[elderbro[jdx]] = youngerbro[jdx];
                /// tracement stuff
                #ifdef EXPTRACING_2
                ++op_ctr;
                #endif
            }
            if(youngerbro[jdx] >= 0)
            {
                elderbro[youngerbro[jdx]] = elderbro[jdx];
                /// tracement stuff
                #ifdef EXPTRACING_2
                ++op_ctr;
                #endif
            }
        }
        elderbro[jdx] = youngerbro[jdx] = -1;
    }
    /// case jdx --> idx
    else if(jdx == predecessor[idx])
    {
        root_of_new_tree = idx;
        predecessor[idx] = -1;
        if(successor[jdx] == idx)
        {
            successor[jdx] = youngerbro[idx];
            if(youngerbro[idx] >= 0)
            {
                elderbro[youngerbro[idx]] = -1;
                /// tracement stuff
                #ifdef EXPTRACING_2
                ++op_ctr;
                #endif
            }
        }
        else
        {
            if(elderbro[idx] >= 0)
            {
                youngerbro[elderbro[idx]] = youngerbro[idx];
                /// tracement stuff
                #ifdef EXPTRACING_2
                ++op_ctr;
                #endif
            }
            if(youngerbro[idx] >= 0)
            {
                elderbro[youngerbro[idx]] = elderbro[idx];
                /// tracement stuff
                #ifdef EXPTRACING_2
                ++op_ctr;
                #endif
            }
        }
        elderbro[idx] = youngerbro[idx] = -1;
    }
    else
        throw std::runtime_error("Inconsistent tree " + std::to_string(idx) + " pred " + std::to_string(predecessor[idx]) +
                                 ", " + std::to_string(jdx) + " pred " + std::to_string(predecessor[jdx]));
    /// tracement stuff
    /// here I consider as touched node the parent node
    #ifdef EXPTRACING_2
    ++op_ctr;
    #endif
    
    return root_of_new_tree;
}

inline
SpanningTree::NodeType SpanningTree::attach_lhs(NodeType a, NodeType b, bool update_lvls
                                                                           #ifdef EXPTRACING_2
                                                                           , unsigned long* op_ctr
                                                                           #endif
                                                                           )
{
    revertAncestry(a
                   #ifdef EXPTRACING_2
                   , *op_ctr
                   #endif
                   );
    addSuccessor(b, a
                 #ifdef EXPTRACING_2
                 , *op_ctr
                 #endif
                 );
    if(update_lvls)
        updateLevels(a, level[b] + 1);
    
    return root_node;
}

inline
SpanningTree::NodeType SpanningTree::attach_rhs(NodeType a, NodeType b, bool update_lvls
                                                                           #ifdef EXPTRACING_2
                                                                           , unsigned long* op_ctr
                                                                           #endif
                                                                           )
{
    revertAncestry(b
                   #ifdef EXPTRACING_2
                   , *op_ctr
                   #endif
                   );
    addSuccessor(a, b
                 #ifdef EXPTRACING_2
                 , *op_ctr
                 #endif
                 );
    if(update_lvls)
        updateLevels(b, level[a] + 1);
        
    return root_node;
}

inline
SpanningTree::NodeType SpanningTree::attach(NodeType a, NodeType b, bool update_lvls
                                                                       #ifdef EXPTRACING_2
                                                                       , unsigned long* op_ctr
                                                                       #endif
                                                                       )
{
    if(getRoot(a) == NodeType(root_node))
    {
        revertAncestry(b
                       #ifdef EXPTRACING_2
                       , *op_ctr
                       #endif
                       );
        addSuccessor(a, b
                     #ifdef EXPTRACING_2
                     , *op_ctr
                     #endif
                     );
        if(update_lvls)
            updateLevels(b, level[a] + 1);
    }
    else
    {
        revertAncestry(a
                       #ifdef EXPTRACING_2
                       , *op_ctr
                       #endif
                       );
        addSuccessor(b, a
                     #ifdef EXPTRACING_2
                     , *op_ctr
                     #endif
                     );
        if(update_lvls)
            updateLevels(a, level[b] + 1);
    }

    return root_node;
}

std::pair<NodeArcIdType, NodeArcIdType>
SpanningTree::compute(const std::vector<std::pair<NodeArcIdType, NodeArcIdType>>& arcs)
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");

    /// local vars
    std::vector<std::vector<CellVar>> rows(m);
    std::vector<std::vector<CellVar>> cols(n);
    NodeArcIdType i;
    NodeArcIdType j;
    for(auto it = arcs.begin(); it != arcs.end(); it++)
    {
        i = it->first, j = it->second;
        rows[i].push_back(CellVar(i, j, 0.0, 0.0)), cols[j].push_back(CellVar(i, j, 0.0, 0.0));
    }
    
    return this->compute(rows, cols);
}

std::pair<NodeArcIdType, NodeArcIdType>
SpanningTree::compute(const std::vector<std::vector<CellVar>>& rows,
                      const std::vector<std::vector<CellVar>>& cols)
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");
    /// ret value
    std::pair<NodeArcIdType, NodeArcIdType> retv(nodes, 0);
    /// local vars
    std::vector<std::tuple<NodeType, NodeType, bool>> loop;
    NodeType curr_idx;
    NodeType prnt_idx;
    NodeType prev_i;
    NodeType prev_mpj;
    bool by_col;
    
    level[root_node] = 0;
    
    /// main loop
    loop.reserve(nodes);
    loop.push_back(std::make_tuple(-1, cols.back().front().j, true));
    do
    {
        by_col = std::get<2>(loop.back());
        curr_idx = by_col ? std::get<1>(loop.back()) : std::get<0>(loop.back());
        prnt_idx = by_col ? std::get<0>(loop.back()) : std::get<1>(loop.back());
        loop.pop_back();
        
        if(by_col)
        {
            prev_i = -1;
            for(auto it = cols[curr_idx].begin(); it != cols[curr_idx].end(); it++)
            {                
                if(NodeType(it->i) == prnt_idx)
                    continue;

                loop.push_back(std::make_tuple(it->i, it->j, !by_col));
                ///spanning tree
                /// set j as predecessor for every row node
                predecessor[it->i] = m + curr_idx;
                level[it->i] = level[m + curr_idx] + 1;
                
                /// set the 1st row node i as the col j node successor
                if(prev_i < 0)
                    successor[m + curr_idx] = it->i;
                /// set elder and younger row node bro for each row node i
                else
                {
                    elderbro[it->i] = prev_i;
                    youngerbro[prev_i] = it->i;
                }    
                
                prev_i = it->i;
                
                #ifdef EXPTRACING_2
                ++retv.second;
                #endif
            }
        }
        else
        {
            prev_mpj = -1;
            for(auto it = rows[curr_idx].begin(); it != rows[curr_idx].end(); it++)
            {
                if(NodeType(it->j) == prnt_idx)
                    continue;
                
                loop.push_back(std::make_tuple(it->i, it->j, !by_col));
                ///spanning tree
                /// set i as predecessor of every col node
                predecessor[m + it->j] = curr_idx;
                level[m + it->j] = level[curr_idx] + 1;
                
                /// set the 1st col node as the row i successor
                if(prev_mpj < 0)
                    successor[curr_idx] = m + it->j;
                /// set elder and younger col node bro for each col node j
                else
                {
                    elderbro[m + it->j] = prev_mpj;
                    youngerbro[prev_mpj] = m + it->j;
                }
                
                prev_mpj = m + it->j;
                
                #ifdef EXPTRACING_2
                ++retv.second;
                #endif
            }
        }
    }
    while(loop.size());

    return retv;
}

void SpanningTree::computeSubtrees()
{
    NodeType node_idx;
    NodeType pnode_idx;
    NodeType lvl;
    
    for(NodeArcIdType i = 0; i < nodes; i++)
    {
        node_idx = i;
        lvl = 0;
        subtrees[i][node_idx] = lvl;
        while((pnode_idx = predecessor[node_idx]) >= 0)
        {
            subtrees[i][pnode_idx] = ++lvl;
            node_idx = pnode_idx;
        }
    }
}

void SpanningTree::computeTSpxMultips(const TpInstance::tcosts& cs, std::vector<double>& us, std::vector<double>& vs, NodeType nd)
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree, " + std::to_string(nodes) + " nodes, " + std::to_string(root_node) + " root");
            
    std::vector<std::pair<NodeType, bool>> stack;
    bool is_col;
    NodeType cnd;
    NodeType bnd;
    
    stack.push_back(std::make_pair(nd < 0 ? root_node : nd, nd < 0 ? true : nd < m ? false : true));
    if(nd < 0)
        vs[stack.back().second ? root_node - m : root_node] = 0.0;
    do
    {
        bnd = successor[cnd = stack.back().first];
        is_col = stack.back().second;
        stack.pop_back();
        while(bnd >= 0)
        {
            if(is_col)
                us[bnd] = cs[bnd * n + (cnd - m)] - vs[cnd - m];
            else
                vs[bnd - m] = cs[cnd * n + (bnd - m)] - us[cnd];
            
            stack.push_back(std::make_pair(bnd, !is_col));
            bnd = youngerbro[bnd];
        }
    }
    while(!stack.empty());    
}

void SpanningTree::computeTSpxShieldingSupp(const ts_sol& qs, std::vector<std::vector<NodeArcIdType>>& supp, double epsv)
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");
            
    std::vector<std::pair<NodeType, bool>> stack;
    bool is_col;
    NodeType cnd;
    NodeType bnd;
    
    supp.clear();
    supp.resize(m);
    
    stack.push_back(std::make_pair(root_node, true));
    do
    {
        bnd = successor[cnd = stack.back().first];
        is_col = stack.back().second;
        stack.pop_back();
        while(bnd >= 0)
        {
            if(is_col)
            {
                if(qs.get(bnd * n + (cnd - m)) > epsv + MYEPS)
                    supp[bnd].push_back(cnd - m);
            }
            else
            {
                if(qs.get(cnd * n + (bnd - m)) > epsv + MYEPS)
                    supp[cnd].push_back(bnd - m);
            }
            
            stack.push_back(std::make_pair(bnd, !is_col));
            bnd = youngerbro[bnd];
        }
    }
    while(!stack.empty());
}

std::pair<double,double> SpanningTree::computeTSpxObjF(const TpInstance::tcosts& cs, const ts_sol& qs)
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");
            
    std::vector<std::pair<NodeType, bool>> stack;
    bool is_col;
    NodeType cnd;
    NodeType bnd;
    double objf;
    double tsqt;
    double q;
    
    stack.push_back(std::make_pair(root_node, true));
    tsqt = objf = 0.0;
    do
    {
        bnd = successor[cnd = stack.back().first];
        is_col = stack.back().second;
        stack.pop_back();
        while(bnd >= 0)
        {
            if(is_col)
                objf += cs[bnd * n + (cnd - m)] * (q = std::round(qs.get(bnd * n + (cnd - m))));
            else
                objf += cs[cnd * n + (bnd - m)] * (q = std::round(qs.get(cnd * n + (bnd - m))));
            
            tsqt += q;
            
            stack.push_back(std::make_pair(bnd, !is_col));
            bnd = youngerbro[bnd];
        }
    }
    while(!stack.empty());
    
    return std::make_pair(objf, tsqt);
}

std::shared_ptr<std::vector<std::pair<NodeArcIdType, NodeArcIdType>>> SpanningTree::getTSpxSolV2()
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");
    
    std::shared_ptr<std::vector<std::pair<NodeArcIdType, NodeArcIdType>>> sol_sptr;
    std::vector<std::pair<NodeType, bool>> stack;
    bool is_col;
    NodeType cnd;
    NodeType bnd;
    
    sol_sptr.reset(new std::vector<std::pair<NodeArcIdType, NodeArcIdType>>());
    stack.push_back(std::make_pair(root_node, true));
    do
    {
        bnd = successor[cnd = stack.back().first];
        is_col = stack.back().second;
        stack.pop_back();
        while(bnd >= 0)
        {
            if(is_col)
                sol_sptr->push_back(std::make_pair(bnd, cnd - m));
            else
                sol_sptr->push_back(std::make_pair(cnd, bnd - m));
            
            stack.push_back(std::make_pair(bnd, !is_col));
            bnd = youngerbro[bnd];
        }
    }
    while(!stack.empty());
    
    return sol_sptr;
}

void SpanningTree::getTSpxSol(const TpInstance::tcosts& cs, std::vector<CellVar>& sol)
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");
            
    std::vector<std::pair<NodeType, bool>> stack;
    bool is_col;
    NodeType cnd;
    NodeType bnd;
    
    sol.clear();
    stack.push_back(std::make_pair(root_node, true));
    do
    {
        bnd = successor[cnd = stack.back().first];
        is_col = stack.back().second;
        stack.pop_back();
        while(bnd >= 0)
        {
            if(is_col)
                sol.push_back(CellVar(bnd, cnd - m, cs[bnd * n + (cnd - m)], 0.0));
            else
                sol.push_back(CellVar(cnd, bnd - m, cs[cnd * n + (bnd - m)], 0.0));
            
            stack.push_back(std::make_pair(bnd, !is_col));
            bnd = youngerbro[bnd];
        }
    }
    while(!stack.empty());
}

inline
bool SpanningTree::findPath2Root1Step(NodeType& node_idx, bool& node_is_col, std::vector<LoopStone>& loop)
{
    NodeType pnode_idx = -1;
    
    if(node_idx >= 0 && (pnode_idx = predecessor[node_idx]) >= 0)
    {
        if(node_is_col)
            loop.push_back(LoopStone(0, pnode_idx, node_idx - m));
        else
            loop.push_back(LoopStone(0, node_idx, pnode_idx - m));
    }
    
    node_idx = pnode_idx;
    node_is_col = !node_is_col;
    
    return (node_idx == root_node || node_idx < 0);
}

#ifdef LOOPOP
#ifdef EXPTRACING_2
bool SpanningTree::findLoopOC(const CellVar& evit, Loop& left_loop, unsigned long* totpslen)
#else
bool SpanningTree::findLoopOC(const CellVar& evit, Loop& left_loop)
#endif
#else
#ifdef EXPTRACING_2
bool SpanningTree::findLoop(const CellVar& evit, Loop& left_loop, unsigned long* totpslen)
#else
bool SpanningTree::findLoop(const CellVar& evit, Loop& left_loop)
#endif
#endif
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");
    
    /// expcept if loop not empty
    if(left_loop.size())
        throw std::invalid_argument("TSimplex::computeLoop(..) >> (loop.size() > 0) is true");
    
    /// local vars
    std::vector<LoopStone> right_loop;
    NodeType nidx_lhsp = m + evit.j;
    NodeType nidx_rhsp = evit.i;
    bool nisc_lhsp = true;
    bool nisc_rhsp = false;
    bool lhsp_has_it = false;
    
    /// init. paths
    left_loop.reserve(m / 2);
    right_loop.reserve(m / 2);
    left_loop.push_back(LoopStone(-1, evit.i, evit.j));
    path_mask[nidx_lhsp] = path_mask[nidx_rhsp] = true;
    
    do
    {
        /// compute left path
        findPath2Root1Step(nidx_lhsp, nisc_lhsp, left_loop);
        if(nidx_lhsp >= 0)
        {
            if(path_mask[nidx_lhsp])
            {
                break;
            }
            else
                path_mask[nidx_lhsp] = true;
        }
        /// compute right path
        findPath2Root1Step(nidx_rhsp, nisc_rhsp, right_loop);
        if(nidx_rhsp >= 0)
        {
            if(path_mask[nidx_rhsp])
            {
                lhsp_has_it = true;
                break;
            }
            else
                path_mask[nidx_rhsp] = true;
        }
    }
    while(true);
    /// clear apth mask
    path_mask = std::vector<bool>(nodes, false);
    
    #ifdef EXPTRACING_2
    if(totpslen != nullptr)
        *totpslen += right_loop.size() + 1 + left_loop.size() + 1; 
    #endif
    
    std::vector<LoopStone>& p2cut = lhsp_has_it ? left_loop : right_loop;
    LoopStone& apx_nd = lhsp_has_it ? right_loop.back() : left_loop.back();
    NodeArcIdType s = 0;
    for( ; s < p2cut.size(); s++)
        if(p2cut[s].i == apx_nd.i || p2cut[s].j == apx_nd.j)
            break;
    ++s;    
    
    p2cut.erase(p2cut.begin() + s, p2cut.end());
    
    if(!(!lhsp_has_it && (left_loop.front().i == left_loop.back().i || left_loop.front().j == left_loop.back().j)))
    {
        left_loop.insert(left_loop.end(), right_loop.rbegin(), right_loop.rend());
    }
    
    if(left_loop.size() < 4)
        throw std::runtime_error("There's no loop for var " + evit.toString() + ", maybe it's a basic var, loop " + left_loop.toString());
    
    return (left_loop.size() > 3);
}

inline
bool SpanningTree::findPath2Root(NodeType node_idx, bool node_is_col, std::vector<LoopStone>& loop)
{
    NodeType pnode_idx;
    do
    {
        if((pnode_idx = predecessor[node_idx]) < 0)
            break;
        
        if(node_is_col)
            loop.push_back(LoopStone(0, pnode_idx, node_idx - m));
        else
            loop.push_back(LoopStone(0, node_idx, pnode_idx - m));
        
        node_idx = pnode_idx;
        node_is_col = !node_is_col;
    }
    while(true);
    
    return (node_idx == root_node);
}

//#define DEBUGLOOPOC
#ifdef LOOPOP
#ifdef EXPTRACING_2
bool SpanningTree::findLoop(const CellVar& evit, Loop& left_loop, unsigned long* totpslen)
#else
bool SpanningTree::findLoop(const CellVar& evit, Loop& left_loop)
#endif
#else 
#ifdef EXPTRACING_2
bool SpanningTree::findLoopOP(const CellVar& evit, Loop& left_loop, unsigned long* totpslen)
#else
bool SpanningTree::findLoopOP(const CellVar& evit, Loop& left_loop)
#endif
#endif
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");
    
    /// expcept if loop not empty
    if(left_loop.size())
        throw std::invalid_argument("TSimplex::computeLoop(..) >> (loop.size() > 0) is true");
    
    /// local vars
    std::vector<LoopStone> right_loop;
    
    /// compute left path
    left_loop.reserve(m);
    left_loop.push_back(LoopStone(-1, evit.i, evit.j));
    findPath2Root(m + evit.j, true, left_loop);
    
    /// compute right path
    right_loop.reserve(m);
    findPath2Root(evit.i, false, right_loop);
    
    #ifdef EXPTRACING_2
    if(totpslen != nullptr)
        *totpslen += right_loop.size() + 1 + left_loop.size() + 1; 
    #endif
    
    /// look for the apwx node
    auto it1 = left_loop.rbegin(), it2 = right_loop.rbegin();
    for( ; it1 != left_loop.rend() && it2 != right_loop.rend(); it1++, it2++)
        if(!(it1->i == it2->i && it1->j == it2->j))
            break;
    /// clip path
    left_loop.erase((it1).base(), left_loop.end());
    right_loop.erase((it2).base(), right_loop.end());
    
    /// cat partial paths
    left_loop.insert(left_loop.end(), right_loop.rbegin(), right_loop.rend());
    if(left_loop.size() < 4)
        throw std::runtime_error("There's no loop for var " + evit.toString() + ", maybe it's a basic var, loop " + left_loop.toString());
    
    #ifdef DEBUGLOOPOC
    Loop test_loop(left_loop.getN());
    this->findLoop(evit, test_loop);
    if(test_loop != left_loop)
    {
        FILE_LOG(logDEBUG) << "Test loop = " << test_loop.toString();
        FILE_LOG(logDEBUG) << "Left loop = " << left_loop.toString();
        
        throw std::runtime_error("Error: Test loop != loop");
    }
    #endif

    return (left_loop.size() > 3);
}

void SpanningTree::getDetachedNodes(std::vector<NodeType>& detached_nodes)
{
    detached_nodes = std::vector<NodeType>();
    for(NodeArcIdType i = 0; i < nodes; i++)
    {
        if(NodeType(i) != root_node && predecessor[i] < 0)
            detached_nodes.push_back(i);
    }
    
    return;
} 

NodeArcIdType SpanningTree::checkTree()
{
    NodeArcIdType detached_nodes = 0;
    for(NodeArcIdType i = 0; i < nodes; i++)
    {
        if(NodeType(i) != root_node && predecessor[i] < 0)
            ++detached_nodes;
    }
    
    return detached_nodes;
}

inline
void SpanningTree::revertAncestry(NodeType nd_idx
                                                   #ifdef EXPTRACING_2
                                                   , unsigned long& op_ctr
                                                   #endif
)
{
    if(nd_idx < 0)
        throw std::invalid_argument("(nd_idx < 0) is true");
    /// find path for node idx to the (sub)tree root
    std::vector<NodeType> path;
    path.reserve(m);
    NodeType nd_prd = nd_idx;
    do path.push_back(nd_prd);
    while((nd_prd = predecessor[nd_idx = nd_prd]) >= 0);
    /// revert from root to node idx: *it is pred. of *next(it), detach and make *next(it) pred. of of *it
    for(auto it = path.rbegin(); std::next(it) != path.rend(); it++)
    {
        detach(*it, *std::next(it)
               #ifdef EXPTRACING_2
               , op_ctr
               #endif
               );
        addSuccessor(*std::next(it), *it
                     #ifdef EXPTRACING_2
                     , op_ctr
                     #endif
                     );
    }
}

inline
void SpanningTree::addSuccessor(NodeType pred, NodeType new_succ
                                                 #ifdef EXPTRACING_2
                                                 , unsigned long& op_ctr
                                                 #endif
)
{
    if(predecessor[new_succ] >= 0)
        throw std::runtime_error("Trying to make successor node already having a predecessor");
        
    predecessor[new_succ] = pred;
    youngerbro[new_succ] = successor[pred];
    if(successor[pred] >= 0)
    {
        elderbro[successor[pred]] = new_succ;
        /// tracement stuff
        #ifdef EXPTRACING_2
        ++op_ctr;
        #endif
    }
    successor[pred] = new_succ;
    
    /// tracement stuff
    /// here I consider as touched node the parent node
    #ifdef EXPTRACING_2
    ++op_ctr;
    #endif
}

void SpanningTree::update(const ArcType& delarc, const ArcType& addarc, bool update_lvls
                                           #ifdef EXPTRACING_2
                                           , unsigned long* op_ctr
                                           #endif
                                           )
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");
    
    /// dettach arc
    detach(delarc.first, delarc.second
           #ifdef EXPTRACING_2
           , *op_ctr
           #endif
           );
    
    /// attach new arc
    attach(addarc.first, addarc.second, update_lvls
           #ifdef EXPTRACING_2
           , op_ctr
           #endif
           );    
}

SpanningTree::NodeType SpanningTree::detachArc(const ArcType& delarc
                                                                          #ifdef EXPTRACING_2
                                                                          , unsigned long& op_ctr
                                                                          #endif
                                                                          )
{
    return detach(delarc.first, delarc.second
                  #ifdef EXPTRACING_2
                  , op_ctr
                  #endif
                  );
}
    
SpanningTree::NodeType SpanningTree::attachArcLHS(const ArcType& addarc
                                                                             #ifdef EXPTRACING_2
                                                                             , unsigned long& op_ctr
                                                                             #endif
                                                                             )
{
    return attach_lhs(addarc.first, addarc.second
                      #ifdef EXPTRACING_2
                      , false
                      , &op_ctr
                      #endif
                      );
}

SpanningTree::NodeType SpanningTree::attachArcRHS(const ArcType& addarc
                                                                             #ifdef EXPTRACING_2
                                                                             , unsigned long& op_ctr
                                                                             #endif
                                                                             )
{
    return attach_rhs(addarc.first, addarc.second
                      #ifdef EXPTRACING_2
                      , false
                      , &op_ctr
                      #endif
                      );
}

SpanningTree::NodeType SpanningTree::attachArc(const ArcType& addarc
                                                                          #ifdef EXPTRACING_2
                                                                          , unsigned long& op_ctr
                                                                          #endif
                                                                          )
{
    return attach(addarc.first, addarc.second
                  #ifdef EXPTRACING_2
                  , false
                  , &op_ctr
                  #endif
                  );
}

SpanningTree::NodeType SpanningTree::addArc(const ArcType& addarc, bool color_arc_nodes
                                                                       #ifdef EXPTRACING_2
                                                                       , unsigned long* op_ctr
                                                                       #endif
                                                                       )
{
    if(addarc.second != root_node && predecessor[addarc.second] < 0)
    {
        addSuccessor(addarc.first, addarc.second
                     #ifdef EXPTRACING_2
                     , *op_ctr
                     #endif
                     );
    }
    else if(addarc.first != root_node && predecessor[addarc.first] < 0)
    {
        addSuccessor(addarc.second, addarc.first
                     #ifdef EXPTRACING_2
                     , *op_ctr
                     #endif
                     );
    }
    else
    {
        throw std::runtime_error("Already attached arc (" + 
                                 std::to_string(addarc.first) + "," + 
                                 std::to_string(addarc.second) + 
                                 "), SpanningTree::addArc(..)");
    }
    
    if(color_arc_nodes)
        color[addarc.first] = color[addarc.second] = (addarc.first == root_node || addarc.second == root_node ? 0 : getNewColor());
    
    return root_node;
}


std::string SpanningTree::toStringNode(NodeType nd)
{
    std::stringstream ss;
    if(nd >= 0)
    {
        ss << "("
           << nd 
           << ")[lvl " << level[nd] 
           << ", stlvl " << subtree_level[nd] 
           << ", clr " << color[nd] << " p" << parent_color[color[nd]] 
           << "]";
    }
    
    return ss.str();
}

std::string SpanningTree::toString(NodeType nd, ColorType cl, bool rt, std::vector<double>* qs_ptr)
{
    if(nodes == 0 || root_node < 0)
        throw std::runtime_error("Empty tree");
    
    std::stringstream ss;
    if(nd >= 0)
    {
        ss << "(" << nd << ")[" << level[nd] << "," << subtree_level[nd] << "," << color[nd] << ":" << parent_color[color[nd]] << "]";
        return ss.str();
    }
    
    NodeArcIdType idx = 0;
    for(NodeArcIdType r = rt ? 0 : root_node; r < (rt ? predecessor.size() : root_node + 1); r++)
    {
        if(predecessor[r] < 0)
        {
            ss << "Root: " << r << std::endl;
            std::vector<NodeType> stack;
            NodeType cnd;
            stack.push_back(r);
            
            do
            {
                cnd = stack.back();
                stack.pop_back();
                if(cl < 0 || color[cnd] == cl)
                    ss << idx << ": (" << cnd << ")[" << level[cnd] << "," << subtree_level[cnd] << "," << color[cnd] << "]";
                ++idx;
                
                NodeType bnd = successor[cnd];
                ss << (bnd >= 0 ? " -->" : " :: leaf");
                while(bnd >= 0)
                {
                    if(cl < 0 || color[cnd] == cl)
                    {
                        ss << " (" << bnd << ")[" << level[bnd] << "," << subtree_level[bnd] << "," << color[bnd] << "]";
                        if(qs_ptr != nullptr)
                            ss << "{" << (bnd >= NodeType(m) ? (*qs_ptr)[cnd * n + (bnd - m)] : (*qs_ptr)[bnd * n + (cnd - m)]) << "}";
                    }
                    stack.push_back(bnd);
                    bnd = youngerbro[bnd];
                }
                
                if(cl < 0 || color[cnd] == cl)
                    ss << "\n";
            }
            while(!stack.empty());
        }
    }
    
    return ss.str();
}

std::string SpanningTree::toString(Loop& loop, std::string sep)
{
    //throw std::runtime_error("Not implemented method");
    std::string str("");
    for(auto it = loop.begin(); it != loop.end(); it++)
    {
        str += "" + this->toStringNode(m + it->j) + "-" 
                + this->toStringNode(it->i) + "";
        
        if(std::next(it) != loop.end())
            str += sep;
    }
    return str;    
}

std::pair<double, NodeArcIdType> Loop::getMinQ(const ts_sol& quantities, bool incr, bool zvcr, bool prob0)
{
    if(this->size() < 4)
        throw std::runtime_error("Not a loop size < 4");
        
    NodeArcIdType c;
    std::size_t idx = this->size() - 1;
    std::size_t idxp = this->size() - 1;
    std::size_t i;
    double min_q;
    bool plus;
    bool var_plus_0q = false;
    min_q = std::numeric_limits<double>::max();
    plus = incr ? false : true;
    i = this->size() - 1;
    for(auto it = this->rbegin(); it != this->rend(); it++, i--)
    {
        c = n * it->i + it->j;
        if(!plus && quantities.contains(c) && min_q > quantities.get(c) + MYEPS)
            min_q = quantities.get(c), idx = i;

        if(!zvcr && plus && 
           quantities.contains(c) &&
           !(quantities.get(c) > MYEPS))
            var_plus_0q = true, idxp = i;
        
        if(prob0 && (!(min_q > MYEPS) || var_plus_0q))
            break;
        
        plus = !plus;
    }
    
    if(var_plus_0q)
        return std::make_pair(0.0, idx < idxp ? idx : idxp);
    
    return std::make_pair(min_q, idx);
}

std::string Loop::toString(const NodeArcIdType N, const std::vector<double>* q_ptr)
{
    std::string str("");
    for(auto it = this->begin(); it != this->end(); it++)
    {
        str += it->toString(N, q_ptr);
        if(std::next(it) != this->end())
            str += "; ";
    }
    return str;
}

NodeArcIdType Loop::moveQuantity(double min_q,
                                                           const TpInstance::tcosts& costs,
                                                           ts_sol& quantities,
                                                           std::vector<CellVar>& exited_vars,
                                                           std::vector<CellVar>& unzeroed_vars,
                                                           double* objf_value, double* tot_q,
                                                           bool remov_if_0, bool just_check, bool incr)
{
    
    NodeArcIdType zeroed;
    NodeArcIdType i;
    NodeArcIdType j;
    NodeArcIdType c;
    double tmp_q = std::numeric_limits<double>::max();
    double tmp_c = std::numeric_limits<double>::max();
    //double outofbv = remov_if_0 ? std::numeric_limits<double>::quiet_NaN() : 0.0;
    bool plus;
    bool tst_q;

    zeroed = 0;
    plus = incr ? false : true;
    for(auto it = this->rbegin(); it != this->rend(); it++)
    {
        c = n * (i = it->i) + (j = it->j);
        if((tst_q = quantities.contains(c)))
            tmp_q = quantities.get(c);
        tmp_c = costs[c];
        
        /// sum
        if(plus)
        {
            if(objf_value != nullptr)
                *objf_value += min_q * tmp_c; 
            if(tot_q != nullptr)
                *tot_q += min_q;
            
            if(!just_check)
            {
                quantities.set(c, !tst_q ? (min_q) : (tmp_q + min_q));
                if(tst_q && !(tmp_q > MYEPS))
                    unzeroed_vars.push_back(CellVar(i, j, tmp_c, 0.0));
            }
        }
        /// sub
        else
        {
            if(objf_value != nullptr)
                *objf_value -= min_q * tmp_c; 
            if(tot_q != nullptr)
                *tot_q -= min_q;
            
            if(!just_check)
            {
                if(tmp_q > min_q + MYEPS)
                    quantities.set(c, tmp_q - min_q);
                else
                {
                    if(remov_if_0)
                        quantities.remove(c);
                    else
                        quantities.set(c, 0.0);
                }
                
                if((remov_if_0 && !quantities.contains(c)) ||
                   (!remov_if_0 && !(quantities.get(c) > MYEPS)))
                {
                    if(++zeroed > 1)
                        quantities.set(c,
                                       #ifdef EPSQIS0
                                       0.0
                                       #else
                                       EPSQ
                                       #endif
                                       );
                    else
                        exited_vars.push_back(CellVar(i, j, tmp_c, std::numeric_limits<double>::lowest()));
                }
            }
        }
    
        plus = !plus;
    }
        
    return (NodeArcIdType)(zeroed > 0);
}

