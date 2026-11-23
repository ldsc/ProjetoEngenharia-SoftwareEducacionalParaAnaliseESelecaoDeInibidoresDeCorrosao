#ifndef ANALISECORROSAO_H
#define ANALISECORROSAO_H

#include <string>

#include "Fluido.h"

class AnaliseCorrosao
{
private:
    const Fluido& fluido;

    double taxaCorrosao;
    double taxaCorrosaoFinal;

    std::string tipoCorrosao;
    std::string severidade;

    double fatorSeveridade;
    double fatorTemperatura;
    double fatorCorrosivo;

    void Analisar();

    double CalcularTaxaBaseCorrosao() const;
    std::string DeterminarTipoCorrosao() const;
    std::string DeterminarSeveridade(double taxaCorrosao) const;

    double CalcularFatorSeveridade(const std::string& severidade) const;
    double CalcularFatorTemperatura() const;
    double CalcularFatorCorrosivo() const;

public:
    explicit AnaliseCorrosao(const Fluido& fluido);

    double TaxaCorrosao() const;
    double TaxaCorrosaoFinal() const;

    std::string TipoCorrosao() const;
    std::string Severidade() const;

    double FatorSeveridade() const;
    double FatorCorrecaoTemperatura() const;
    double FatorCorrosivo() const;

    void Imprimir() const;
};

#endif
