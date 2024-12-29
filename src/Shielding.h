///
/// src/algs/Shielding.h
///
/// Written by Roberto Bargetto
///        DIGEP
///        Politecnico di Torino
///        Corso Duca degli Abruzzi, 10129, Torino
///        Italy
///
/// ------------------- 
///     Data structures
///            tplex_alg_data::THEgrid::X;
///            tplex_alg_data::THEgrid;
///     and functions
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

#include "TpInstance.h"


namespace Shielding
{
/// shielding stuff
// Serve per tenere traccia
// degli indici di riga e colonna nella griglia di ogni origine e dei suoi vicini
struct X
{
    NodeArcIdType row;
    NodeArcIdType col;
    std::vector<NodeArcIdType> neigh;
};

struct BV : public X
{
    double val;
    double cost = 0;
    NodeArcIdTypeSGND degree;
};

// table contiene i valori della griglia, cell[i] le relative informazioni per ogni origine i.
struct THEgrid
{
    std::vector<std::vector<NodeArcIdType>> table;
    std::vector<X> cell;
};

void THEgenerateSquareGrid(THEgrid& board, NodeArcIdType ori, NodeArcIdType grsize);
void generateSquareGrid(std::vector<std::vector<NodeArcIdTypeSGND>>& grid, NodeArcIdTypeSGND ori, NodeArcIdTypeSGND grsize);
void getRectangle(THEgrid& board,
                  std::vector<std::vector<NodeArcIdType>>& supp,
                  NodeArcIdType i,
                  NodeArcIdType& left,
                  NodeArcIdType& right, 
                  NodeArcIdType& up,
                  NodeArcIdType& down);
}

