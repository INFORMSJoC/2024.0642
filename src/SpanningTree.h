///
/// src/algs/SpanningTree.h
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


#ifndef SPAT_H
#define SPAT_H

#include "Loop.h"
#include "TSimplexSolution.h"


#define UPDLVLS false
#define SUBTREES false


namespace TSimplexData
{    
    /// spanning tree data struct
    class SpanningTree
    {
    public:
        /// types
        typedef NodeArcIdTypeSGND NodeType;
        typedef NodeArcIdTypeSGND ColorType;
        typedef std::pair<NodeType,NodeType> ArcType;
        
        /// constructors and destructor
        //inline SpanningTree() : root_node(-1), nodes(0), m(0), is_path(false) { }
        inline SpanningTree(NodeArcIdType rn, NodeArcIdType e, NodeArcIdType mv)
            : root_node(rn)
            , nodes(e)
            , m(mv)
            , n(nodes - mv)
            , is_path(false)
        {
            level.resize(nodes, -1);
            subtree_level.resize(nodes, -1);
            predecessor.resize(nodes, -1);
            successor.resize(nodes, -1);
            elderbro.resize(nodes, -1);
            youngerbro.resize(nodes, -1);
            color.resize(nodes, 0);
            parent_color.resize(nodes, -1);
            path_mask.resize(nodes, false);
            if(SUBTREES)
                subtrees.resize(nodes, std::vector<NodeType>(nodes, -1));
        }
        inline ~SpanningTree() { }
        
        
        /// inline methods
        inline bool operator ==(const SpanningTree& arg)
        {
            return (
                nodes == arg.nodes &&
                (!UPDLVLS || level == arg.level) &&
                predecessor == arg.predecessor);
        }
        
        inline NodeType getRootNode()
        {
            return root_node;
        }
        
        inline NodeType getRoot(NodeType nd_idx)
        {
            if(nd_idx < 0)
                throw std::invalid_argument("(nd_idx < 0) is true");
            
            NodeType nd_prd = nd_idx;
            while((nd_prd = predecessor[nd_idx = nd_prd]) >= 0);
                
            return nd_idx;
        }
        
        inline NodeType getPredecessor(NodeType nd)
        {
            return predecessor[nd];
        }
        
        inline ColorType getColor(NodeType nd)
        {
            return color[nd];
        }
        
        inline ColorType getColorCount()
        {
            return next_color;
        }
        
        inline NodeType getM()
        {
            return m;
        }
        
        inline NodeType getLastNode()
        {
            if(!is_path)
                return -1;
            
            for(NodeArcIdType i = 0; i < successor.size(); i++)
                if(successor[i] < 0)
                    return i;
            
            throw std::runtime_error("Data structure is not a tree or a path.");
        }
        
        inline ArcType makeArc(NodeType pred, NodeType succ, NodeType eldb = -1)
        {
            /// set j as predecessor for every row node
            predecessor[succ] = pred;
            
            /// set the 1st row node i as the col j node successor
            if(eldb < 0)
            {
                successor[pred] = succ;
            }
            /// set elder and younger row node bro for each row node i
            else
            {
                elderbro[succ] = eldb;
                youngerbro[eldb] = succ;
            }    
            
            return std::make_pair(pred, succ);
        }
        
        inline bool isRowNode(NodeType nd)
        {
            return nd < NodeType(m);
        }
        
        inline bool isPath()
        {
            return is_path;
        }

        inline bool checkIsPath()
        {
            for(NodeArcIdType i = 0; i < nodes; i++)
                if(elderbro[i] >= 0)
                    return false;
            
            return true;
        }
        
        inline bool testNodes(const ArcType& arc)
        {
            return color[arc.first] == color[arc.second];
        }
        
        inline bool checkArc(const ArcType& arc, unsigned int greedy_lvl = 0, unsigned long* odk_ct_ptr = nullptr)
        {
            if(nodes == 0 || root_node < 0)
                throw std::runtime_error("Empty tree");

            if(arc.first < 0 || arc.second < 0)
                throw std::invalid_argument("(arc.first < 0 || arc.second < 0) is true");
            
            if(greedy_lvl)
            {
                /// it return true only if match the rule
                bool C1 = color[arc.first] == color[arc.second];
                bool C2 = color[arc.first] != color[arc.second] &&
                            ( (parent_color[color[arc.first]] == color[arc.second] && bool(subtree_level[arc.first] % 2)) ||
                              (parent_color[color[arc.second]] == color[arc.first] && bool(subtree_level[arc.second] % 2)) );
                bool C3 = color[arc.first] != color[arc.second] &&
                            ( (parent_color[color[arc.first]] == parent_color[color[arc.second]] && bool(subtree_level[arc.first] % 2) && bool(subtree_level[arc.second] % 2)) );
                
                /// When the oracle do not know
                #ifdef EXPTRACING_2
                bool ODK = color[arc.first] != color[arc.second] &&
                           parent_color[color[arc.first]] != color[arc.second] &&
                           parent_color[color[arc.second]] != color[arc.first] &&
                           parent_color[color[arc.first]] != parent_color[color[arc.second]];
                if(ODK)
                    (*odk_ct_ptr)++;
                #endif
                
                return (C1 || C2 || C3);
            }
            else
            {
                bool C1 = false;
                if(color[arc.second] != color[arc.first])
                {
                    bool cl2nd_is_prnt = false;
                    for(ColorType pcl = parent_color[color[arc.first]]; pcl > -1; pcl = parent_color[pcl])
                        if(pcl == color[arc.second])
                        {
                            cl2nd_is_prnt = true;
                            break;
                        }
                    bool cl1sr_is_prnt = false;
                    for(ColorType pcl = parent_color[color[arc.second]]; pcl > -1; pcl = parent_color[pcl]) 
                        if(pcl == color[arc.first])
                        {
                            cl1sr_is_prnt = true;
                            break;
                        }
                    
                    C1 = (cl2nd_is_prnt && !bool(subtree_level[arc.first] % 2)) || (cl1sr_is_prnt && !bool(subtree_level[arc.second] % 2));
                }
                
                bool C2 =   color[arc.first] != color[arc.second] &&
                            ((parent_color[color[arc.first]] == color[arc.second] && !bool(subtree_level[arc.first] % 2)) ||
                             (parent_color[color[arc.second]] == color[arc.first] && !bool(subtree_level[arc.second] % 2)));
                
                // C1 seems to be USELESS
                return !(C1 || C2);
            }
        }
        
        inline ColorType setColor(NodeType nd, ColorType cl)
        {
            return color[nd] = cl;
        }
        
        inline void resetRootLevel()
        {
            level[root_node] = 0;
        }
        
        inline void setIsPath(bool flag)
        {
            is_path = flag;
        }
        
        inline void resetTreeColor()
        {
            subtree_level = std::vector<NodeType>(nodes, -1);
            color = std::vector<ColorType>(nodes, 0);
            parent_color = std::vector<ColorType>(nodes, -1);
            next_color = 1;
        }
        
        /// declaration of class methods
        NodeArcIdType checkTree();
        
        void getDetachedNodes(std::vector<NodeType>&);
        
        #ifdef LOOPOP
        #ifdef EXPTRACING_2
        bool findLoop(const CellVar& evit, Loop& loop, unsigned long* totpslen = nullptr);
        bool findLoopOC(const CellVar& evit, Loop& loop, unsigned long* totpslen = nullptr);
        #else
        bool findLoop(const CellVar& evit, Loop& loop);
        bool findLoopOC(const CellVar& evit, Loop& loop);
        #endif
        #else
        #ifdef EXPTRACING_2
        bool findLoop(const CellVar& evit, Loop& loop, unsigned long* totpslen = nullptr);
        bool findLoopOP(const CellVar& evit, Loop& loop, unsigned long* totpslen = nullptr);
        #else
        bool findLoop(const CellVar& evit, Loop& loop);
        bool findLoopOP(const CellVar& evit, Loop& loop);
        #endif
        #endif
        std::pair<NodeArcIdType, NodeArcIdType> compute(const std::vector<std::pair<NodeArcIdType, NodeArcIdType>>& arcs);
        std::pair<NodeArcIdType, NodeArcIdType> compute(const std::vector<std::vector<CellVar>>& rows,
                                                        const std::vector<std::vector<CellVar>>& cols);
        void computeSubtrees();
        void computeTSpxMultips(const TpInstance::tcosts& cs, std::vector<double>& us, std::vector<double>& vs, NodeType nd = -1);
        std::pair<double,double> computeTSpxObjF(const TpInstance::tcosts& cs, const ts_sol& qs);
        std::shared_ptr<std::vector<std::pair<NodeArcIdType, NodeArcIdType>>> getTSpxSolV2();
        void getTSpxSol(const TpInstance::tcosts& cs, std::vector<CellVar>&);
        
        void computeTSpxShieldingSupp(const ts_sol& qs, std::vector<std::vector<NodeArcIdType>>& supp, double epsv = 1.0e-20);
        
        /// tree update stuff
        void update(const ArcType& delarc, const ArcType& addarc, bool update_lvls = false
                    #ifdef EXPTRACING_2
                    , unsigned long* op_ctr = nullptr
                    #endif
                    );
        
        NodeType detachArc(const ArcType& delarc
                           #ifdef EXPTRACING_2
                           , unsigned long& op_ctr
                           #endif
                           );
        NodeType attachArcLHS(const ArcType& addarc
                              #ifdef EXPTRACING_2
                              , unsigned long& op_ctr
                              #endif
                              );
        NodeType attachArcRHS(const ArcType& addarc
                              #ifdef EXPTRACING_2
                              , unsigned long& op_ctr
                              #endif
                              );
        NodeType attachArc(const ArcType& addarc
                           #ifdef EXPTRACING_2
                           , unsigned long& op_ctr
                           #endif
                           );
        NodeType addArc(const ArcType& addarc, bool color_arc_nodes = false
                        #ifdef EXPTRACING_2
                        , unsigned long* op_ctr = nullptr
                        #endif
                        );
        
        /// coloring stuff
        inline void colorNodes(const std::vector<double>& qs, double s = 0.0
                        #ifdef EXPTRACING_2
                        , unsigned long* node_ctr_ptr = nullptr
                        #endif
                        )
        {
            if(nodes == 0 || root_node < 0)
                throw std::runtime_error("Empty tree");
                    
            std::vector<std::pair<NodeType, bool>> stack;
            bool is_col;
            ColorType clr;
            NodeType cnd;
            NodeType bnd;
            
            /// reset colors
            this->resetTreeColor();
            clr = 0;
            /// init
            stack.push_back(std::make_pair(root_node, true));
            /// color
            do
            {
                bnd = successor[cnd = stack.back().first];
                is_col = stack.back().second;
                stack.pop_back();
                while(bnd >= 0)
                {
                    if((is_col ? std::round(qs[bnd * n + (cnd - m)]) : std::round(qs[cnd * n + (bnd - m)])) > s)
                    {
                        clr = (cnd == root_node ? 0 : getNewColor());
                        setColor(bnd, clr);
                        setColor(cnd, clr);
                        subtree_level[cnd] = 0;
                        subtree_level[bnd] = 1; 

                        #ifdef EXPTRACING_2
                        (*node_ctr_ptr) += 2;
                        #endif
                    }
                    else
                    {
                        parent_color[color[bnd]] = color[cnd];
                        subtree_level[bnd] = 0;
                    }
                    
                    stack.push_back(std::make_pair(bnd, !is_col));
                    bnd = youngerbro[bnd];
                }
            }
            while(!stack.empty());
          
            this->setNextColor(clr);
        }
        
        #ifdef EXPTRACING_2
        inline void mergeSubtree(const ArcType& unzeroarc, bool ignorelvl = false, unsigned long* node_ctr_ptr = nullptr)
        #else
        inline void mergeSubtree(const ArcType& unzeroarc, bool ignorelvl = false)
        #endif
        {
            if(nodes == 0 || root_node < 0)
                throw std::runtime_error("Empty tree");
            
            if(unzeroarc.first < 0 || unzeroarc.second < 0)
                throw std::invalid_argument("(zeroarc.first < 0 || zeroarc.second < 0) is true");
            
            this->colorSubtree(unzeroarc,
                               unzeroarc.first == predecessor[unzeroarc.second] ? subtree_level[unzeroarc.first] + 1 : subtree_level[unzeroarc.second] + 1,
                               unzeroarc.first == predecessor[unzeroarc.second] ? color[unzeroarc.first] : color[unzeroarc.second]
                               , ignorelvl
                               #ifdef EXPTRACING_2
                               , node_ctr_ptr
                               #endif
                               );
        }
        
        #ifdef EXPTRACING_2
        inline void colorSubtree(const ArcType& zeroarc, NodeType strl = 0, ColorType clr = -1, bool ignorelvl = false, unsigned long* node_ctr_ptr = nullptr)
        #else
        inline void colorSubtree(const ArcType& zeroarc, NodeType strl = 0, ColorType clr = -1, bool ignorelvl = false)
        #endif
        {
            if(nodes == 0 || root_node < 0)
                throw std::runtime_error("Empty tree");

            if(zeroarc.first < 0 || zeroarc.second < 0)
                throw std::invalid_argument("(zeroarc.first < 0 || zeroarc.second < 0) is true");
            
            std::vector<NodeType> stack;
            NodeType rnd;
            NodeType cnd;
            NodeType bnd;
            ColorType stc;
            
            stack.push_back(rnd = (zeroarc.first == predecessor[zeroarc.second] ? zeroarc.second : zeroarc.first));
            color[rnd] = stc = clr < 0 ? getNewColor() : clr;
            
            if(clr < 0)
                parent_color[stc] = color[predecessor[rnd]];
            subtree_level[rnd] = clr == 0 ? -1 : strl;
            do
            {
                bnd = successor[cnd = stack.back()];
                
                stack.pop_back();        
                while(bnd >= 0)
                {
                    if(ignorelvl || subtree_level[bnd] != 0)
                    {
                        color[bnd] = stc;
                        subtree_level[bnd] = clr == 0 ? -1 : subtree_level[cnd] + 1;
                        stack.push_back(bnd);
                        
                        #ifdef EXPTRACING_2
                        ++(*node_ctr_ptr);
                        #endif
                    }
                    else
                        parent_color[color[bnd]] = stc;
                    
                    bnd = youngerbro[bnd];
                }
            }
            while(!stack.empty());
            
            #ifdef EXPTRACING_2
            ++(*node_ctr_ptr);
            #endif
        }
        
        /// update levels
        inline void updateLevels(NodeType rnd, NodeArcIdType rndl)
        {
            if(rnd < 0)
                throw std::invalid_argument("(rnd < 0) is true");
                
            std::vector<NodeType> stack;
            NodeType cnd;
            NodeType bnd;
            stack.push_back(rnd);
            level[rnd] = rndl;
            do
            {
                bnd = successor[cnd = stack.back()];
                stack.pop_back();        
                while(bnd >= 0)
                {
                    level[bnd] = level[cnd] + 1;
                    stack.push_back(bnd);
                    bnd = youngerbro[bnd];
                }
            }
            while(!stack.empty());
        }
        
        /// to string methods
        std::string toStringNode(NodeType nd);
        std::string toString(NodeType nd = -1, ColorType cl = -1, bool allrts = false, std::vector<double>* qs_ptr = nullptr);
        std::string toString(Loop& loop, std::string sep = " --> ");
                
    private:
        NodeType root_node = -1;
        NodeArcIdType nodes = 0;
        NodeArcIdType m = 0;
        NodeArcIdType n = 0;
        
        bool is_path = false;
        
        std::vector<NodeType> level;
        std::vector<NodeType> subtree_level;
        std::vector<NodeType> predecessor;
        std::vector<NodeType> successor;
        std::vector<NodeType> elderbro;
        std::vector<NodeType> youngerbro;
        std::vector<ColorType> color;
        std::vector<ColorType> parent_color;
        std::vector<bool> path_mask;
        
        std::vector<std::vector<NodeType>> subtrees;
        
        ColorType next_color = 1;
        
        /// inline private methods
        inline ColorType getNewColor()
        {
            if(next_color + 1 >= parent_color.size())
                parent_color.resize(parent_color.size() + nodes, -1);
                
            return next_color++;
        }

        inline ColorType getNextColor()
        {
            return next_color;
        }
        
        inline void setNextColor(ColorType nc)
        {
            next_color = nc;
        }
        
        /// private methods
        inline NodeType detach(NodeType a, NodeType b
        #ifdef EXPTRACING_2
        , unsigned long& op_ctr
        #endif
        );
        inline NodeType attach_lhs(NodeType a, NodeType b, bool update_lvls = false
        #ifdef EXPTRACING_2
        , unsigned long* op_ctr = nullptr
        #endif
        );
        inline NodeType attach_rhs(NodeType a, NodeType b, bool update_lvls = false
        #ifdef EXPTRACING_2
        , unsigned long* op_ctr = nullptr
        #endif
        );
        inline NodeType attach(NodeType a, NodeType b, bool update_lvls = false
        #ifdef EXPTRACING_2
        , unsigned long* op_ctr = nullptr
        #endif
        );
        
        inline bool findPath2Root(NodeType node_idx, bool node_is_col, std::vector<LoopStone>& loop);
        inline bool findPath2Root1Step(NodeType& node_idx, bool& node_is_col, std::vector<LoopStone>& loop);
        inline void revertAncestry(NodeType nd_idx
        #ifdef EXPTRACING_2
        , unsigned long& op_ctr
        #endif
        );
        inline void addSuccessor(NodeType pred, NodeType new_succ
        #ifdef EXPTRACING_2
        , unsigned long& op_ctr
        #endif
        );
    };
}


#endif // SPAT_H
