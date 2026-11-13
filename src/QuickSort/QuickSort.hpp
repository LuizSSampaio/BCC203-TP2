#pragma once

#include <fstream>
#include <string>
#include "Item.hpp"
#include "PivotBuffer.hpp"

class QuickSortExterno{
private:
    static void Executar(std::fstream& ArqLi, std::fstream& ArqEi, std::fstream& ArqLEs, int Esq, int Dir);

    static void Particao(std::fstream& ArqLi, std::fstream& ArqEi, std::fstream& ArqLEs,
                         PivotBuffer& pivot, int Esq, int Dir, int& i, int& j);

    static void LeSup(std::fstream& ArqLEs, Item& UltLido, int& Ls, bool& OndeLer);

    static void LeInf(std::fstream& ArqLi, Item& UltLido, int& Li, bool& OndeLer);

    static void EscreveMax(std::fstream& ArqLEs, const Item& R, int& Es);

    static void EscreveMin(std::fstream& ArqEi, const Item& R, int& Ei);

public:

    static void Ordenar (const std::string& binFilePath, int quantity);


};