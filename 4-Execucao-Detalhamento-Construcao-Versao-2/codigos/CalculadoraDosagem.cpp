#include "CalculadoraDosagem.h"

#include <stdexcept>

double CalculadoraDosagem::CalcularDose(const Fluido& fluido,
                                       const AnaliseCorrosao& analise,
                                       const Inibidor& inibidor)
{
    if (inibidor.Eficiencia() <= 0.0)
        throw std::invalid_argument("A eficiencia do inibidor deve ser maior que zero.");

    const double fatorVazao = FatorDeVazao(fluido);

    return (constanteEmpirica *
            analise.TaxaCorrosao() *
            analise.FatorSeveridade() *
            analise.FatorCorrecaoTemperatura() *
            analise.FatorCorrosivo() *
            fatorVazao) / inibidor.Eficiencia();
}

double CalculadoraDosagem::FatorDeVazao(const Fluido& fluido)
{
    return 1.0 + (fluido.Vazao() / 1000.0);
}
