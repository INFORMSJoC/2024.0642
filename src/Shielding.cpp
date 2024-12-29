///
/// src/algs/Shielding.cpp
///
/// Written by Roberto Bargetto
///        DIGEP
///        Politecnico di Torino
///        Corso Duca degli Abruzzi, 10129, Torino
///        Italy
///
/// ------------------- 
///     Implementation of functions
///            void THEgenerateSquareGrid(tplex_alg_data::THEgrid& board, NodeArcIdType ori, NodeArcIdType grsize);
///            void generateSquareGrid(std::vector<std::vector<NodeArcIdTypeSGND>>& grid, NodeArcIdTypeSGND ori, NodeArcIdTypeSGND grsize);
///            void getRectangle(tplex_alg_data::THEgrid& board, std::vector<std::vector<NodeArcIdType>>& supp,
///                              NodeArcIdType i, NodeArcIdType& left, NodeArcIdType& right, NodeArcIdType& up, NodeArcIdType& down);
///     except for minor adaptations, are originally written by Rosario Scatamacchia, DIGEP, Politecnico di Torino
///     Work licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// ------------------- 
///
/// Copyright 2023 by Roberto Bargetto
/// roberto.bargetto@polito.it or roberto.bargetto@gmail.com
///
/// This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
/// See the license at http://creativecommons.org/licenses/by-nc-sa/4.0/
///


#include "Shielding.h"

//using namespace Shielding;

void Shielding::generateSquareGrid(std::vector<std::vector<NodeArcIdTypeSGND>>& a, NodeArcIdTypeSGND ori, NodeArcIdTypeSGND grsize)
{
    a = std::vector<std::vector<NodeArcIdTypeSGND>>(ori);
    NodeArcIdTypeSGND k;
    bool ex;
    
    for (NodeArcIdTypeSGND i = 0; i < ori; i++)
    {
        k = 0;
        ex = false;
        while(k < ori && !ex)
        {
            if (i == k)
                ex = true;
            k += grsize;
        }
        
        if(!ex) a[i].push_back(i - 1);

        k = 0;
        ex = false;
        while (k < ori && !ex)
        {
            if (i == k + grsize -1)
                ex = true;
            k += grsize;
        }
        if(!ex)
            a[i].push_back(i + 1);

        if(i - grsize >= 0)
            a[i].push_back(i - grsize);
        if(i + grsize < ori) 
            a[i].push_back(i + grsize);
    }
}

// Genera una griglia con dimensioni grsize x grsize riempita con numeri 0, 1,..., ori - 1 (ori: numero di origini)  
void Shielding::THEgenerateSquareGrid(THEgrid& board, NodeArcIdType ori, NodeArcIdType grsize)
{
    board.table = std::vector<std::vector<NodeArcIdType>>(grsize, std::vector<NodeArcIdType>(grsize, 0));
    NodeArcIdType it = 0;
    for(NodeArcIdType h = 0; h < grsize; h++)
        for(NodeArcIdType k = 0; k < grsize; k++)
            board.table[h][k] = it++;

    board.cell = std::vector<X>(ori);
    NodeArcIdType k;
    bool ex;
    NodeArcIdType indR = 0;
    NodeArcIdType indC = 0;
    for(NodeArcIdType i = 0; i < ori; i++)
    {
        if (indC >= grsize)
        {
            indC = 0; 
            indR++;
        }
        board.cell[i].row = indR;
        board.cell[i].col = indC;
        indC++;

        k = 0;
        ex = false;
        while (k < ori && !ex)
        {
            if (i == k)
                ex = true;
            k += grsize;
        }
        
        if (!ex)
            board.cell[i].neigh.push_back(i - 1);

        k = 0;
        ex = false;
        while (k < ori && !ex)
        {
            if (i + 1 == k + grsize)
                ex = true;
            k += grsize;
        }

        if (!ex)
            board.cell[i].neigh.push_back(i + 1);

        if (i >= grsize)
            board.cell[i].neigh.push_back(i - grsize);
            
        if (i + grsize < ori)
            board.cell[i].neigh.push_back(i + grsize);
    }
}


void Shielding::getRectangle(THEgrid& board,
                             std::vector<std::vector<NodeArcIdType>>& supp,
                             NodeArcIdType i,
                             NodeArcIdType& left,
                             NodeArcIdType& right, 
                             NodeArcIdType& up,
                             NodeArcIdType& down)
{
    NodeArcIdType a;
    NodeArcIdType mini = 0;
    NodeArcIdType maxi = 0;

    right = board.table.size() - 1;
    left = 0;
    down = board.table.size() - 1;
    up = 0;

    for (NodeArcIdType k = 0; k < board.cell[i].neigh.size(); k++)
    {
        a = board.cell[i].neigh[k];

        if (board.cell[i].row == board.cell[a].row)
        {
            if (board.cell[i].col < board.cell[a].col)
            {
                /// update right
                mini = board.table.size();

                for (NodeArcIdType h = 0; h < supp[a].size(); h++)
                {
                    if (board.cell[supp[a][h]].col <= mini)
                    {
                        right = board.cell[supp[a][h]].col;
                        mini = right;
                    }
                }
            }
            else 
            {
                /// update left
                maxi = 0;
                
                for (NodeArcIdType h = 0; h < supp[a].size(); h++)
                {
                    if (board.cell[supp[a][h]].col >= maxi)
                    {
                        left = board.cell[supp[a][h]].col;
                        maxi = left;
                    }

                }
            }
        }
        else
        {
            if (board.cell[i].row < board.cell[a].row)
            {
                /// update down
                mini = board.table.size();
                
                for(NodeArcIdType h = 0; h < supp[a].size(); h++)
                {
                    if (board.cell[supp[a][h]].row <= mini)
                    {
                        down = board.cell[supp[a][h]].row;
                        mini = down;
                    }
                }
            }
            else
            {
                /// update up
                maxi = 0;
                
                for(NodeArcIdType h = 0; h < supp[a].size(); h++)
                {
                    if (board.cell[supp[a][h]].row >= maxi)
                    {
                        up = board.cell[supp[a][h]].row;
                        maxi = up;
                    }
                }
            }
        }
    }
}
