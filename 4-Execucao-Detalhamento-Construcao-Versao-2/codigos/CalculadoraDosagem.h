#ifndef CALCULADORADOSAGEM_H
#define CALCULADORADOSAGEM_H

#include "Fluido.h"
#include "AnaliseCorrosao.h"
#include "Inibidor.h"

class CalculadoraDosagem
{
private:
    static constexpr double constanteEmpirica = 0.1;

public:
    static double CalcularDose(const Fluido& fluido,
                                const AnaliseCorrosao& analise,
                                const Inibidor& inibidor);

    static double FatorDeVazao(const Fluido& fluido);
};

#endif
