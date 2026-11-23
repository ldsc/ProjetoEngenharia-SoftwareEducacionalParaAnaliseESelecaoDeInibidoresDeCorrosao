#include "AnaliseCorrosao.h"

#include <cmath>
#include <iostream>

AnaliseCorrosao::AnaliseCorrosao(const Fluido& fluido)
    : fluido(fluido),
      taxaCorrosao(0.0),
      taxaCorrosaoFinal(0.0),
      tipoCorrosao("Indefinido"),
      severidade("Indefinida"),
      fatorSeveridade(1.0),
      fatorTemperatura(1.0),
      fatorCorrosivo(1.0)
{
    Analisar();
}

void AnaliseCorrosao::Analisar()
{
    taxaCorrosao = CalcularTaxaBaseCorrosao();
    tipoCorrosao = DeterminarTipoCorrosao();
    severidade = DeterminarSeveridade(taxaCorrosao);

    fatorSeveridade = CalcularFatorSeveridade(severidade);
    fatorTemperatura = CalcularFatorTemperatura();
    fatorCorrosivo = CalcularFatorCorrosivo();

    taxaCorrosaoFinal = taxaCorrosao * fatorSeveridade * fatorTemperatura * fatorCorrosivo;
}

double AnaliseCorrosao::CalcularTaxaBaseCorrosao() const
{
    return fluido.CO2() * std::exp(0.02 * fluido.Temperatura()) * fluido.FatorAcidez();
}

std::string AnaliseCorrosao::DeterminarTipoCorrosao() const
{
    if (fluido.H2S() > 10.0)
        return "Corrosão Ácida (Sour)";

    if (fluido.CO2() > 20.0)
        return "Corrosão Doce (Sweet)";

    return "Leve";
}

std::string AnaliseCorrosao::DeterminarSeveridade(double taxaCorrosao) const
{
    if (taxaCorrosao < 10.0)
        return "Baixa";

    if (taxaCorrosao < 50.0)
        return "Média";

    return "Alta";
}

double AnaliseCorrosao::CalcularFatorSeveridade(const std::string& severidade) const
{
    if (severidade == "Baixa")
        return 1.0;

    if (severidade == "Média")
        return 1.5;

    return 2.5;
}

double AnaliseCorrosao::CalcularFatorTemperatura() const
{
    if (fluido.Temperatura() < 40.0)
        return 1.0;

    if (fluido.Temperatura() <= 80.0)
        return 1.3;

    return 1.8;
}

double AnaliseCorrosao::CalcularFatorCorrosivo() const
{
    if (fluido.H2S() > 10.0)
        return 2.0;

    if (fluido.CO2() > 20.0)
        return 1.5;

    return 1.0;
}

double AnaliseCorrosao::TaxaCorrosao() const
{
    return taxaCorrosao;
}

double AnaliseCorrosao::TaxaCorrosaoFinal() const
{
    return taxaCorrosaoFinal;
}

std::string AnaliseCorrosao::TipoCorrosao() const
{
    return tipoCorrosao;
}

std::string AnaliseCorrosao::Severidade() const
{
    return severidade;
}

double AnaliseCorrosao::FatorSeveridade() const
{
    return fatorSeveridade;
}

double AnaliseCorrosao::FatorCorrecaoTemperatura() const
{
    return fatorTemperatura;
}

double AnaliseCorrosao::FatorCorrosivo() const
{
    return fatorCorrosivo;
}

void AnaliseCorrosao::Imprimir() const
{
    std::cout << "========== Análise de Corrosão ==========" << '\n';
    std::cout << "Tipo de corrosão:        " << tipoCorrosao << '\n';
    std::cout << "Severidade:              " << severidade << '\n';
    std::cout << "Taxa base de corrosão:   " << taxaCorrosao << '\n';
    std::cout << "Fator de severidade Fs:  " << fatorSeveridade << '\n';
    std::cout << "Fator térmico Ft:        " << fatorTemperatura << '\n';
    std::cout << "Fator corrosivo Fc:      " << fatorCorrosivo << '\n';
    std::cout << "Taxa final de corrosão:  " << taxaCorrosaoFinal << '\n';
}
