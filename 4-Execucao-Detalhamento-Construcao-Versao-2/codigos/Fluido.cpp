#include "Fluido.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

Fluido::Fluido()
    : pH(7.0),
      co2(0.0),
      h2s(0.0),
      salinidade(0.0),
      temperatura(25.0),
      pressao(101.325),
      vazao(0.0)
{
}

Fluido::Fluido(double pH,
                                 double co2,
                                 double h2s,
                                 double salinidade,
                                 double temperatura,
                                 double pressao,
                                 double vazao)
    : pH(pH),
      co2(co2),
      h2s(h2s),
      salinidade(salinidade),
      temperatura(temperatura),
      pressao(pressao),
      vazao(vazao)
{
    Validar();
}

void Fluido::Validar() const
{
    if (pH < 0.0 || pH > 14.0)
        throw std::invalid_argument("pH deve estar entre 0 e 14.");

    if (co2 < 0.0)
        throw std::invalid_argument("CO2 não pode ser negativo.");

    if (h2s < 0.0)
        throw std::invalid_argument("H2S não pode ser negativo.");

    if (salinidade < 0.0)
        throw std::invalid_argument("Salinidade não pode ser negativa.");

    if (temperatura < -273.15)
        throw std::invalid_argument("Temperatura não pode ser menor que o zero absoluto.");

    if (pressao < 0.0)
        throw std::invalid_argument("Pressão não pode ser negativa.");

    if (vazao < 0.0)
        throw std::invalid_argument("Vazão não pode ser negativa.");
}

double Fluido::PH() const
{
    return pH;
}

double Fluido::CO2() const
{
    return co2;
}

double Fluido::H2S() const
{
    return h2s;
}

double Fluido::Salinidade() const
{
    return salinidade;
}

double Fluido::Temperatura() const
{
    return temperatura;
}

double Fluido::Pressao() const
{
    return pressao;
}

double Fluido::Vazao() const
{
    return vazao;
}

void Fluido::DefinirPH(double valor)
{
    pH = valor;
    Validar();
}

void Fluido::DefinirCO2(double valor)
{
    co2 = valor;
    Validar();
}

void Fluido::DefinirH2S(double valor)
{
    h2s = valor;
    Validar();
}

void Fluido::DefinirSalinidade(double valor)
{
    salinidade = valor;
    Validar();
}

void Fluido::DefinirTemperatura(double valor)
{
    temperatura = valor;
    Validar();
}

void Fluido::DefinirPressao(double valor)
{
    pressao = valor;
    Validar();
}

void Fluido::DefinirVazao(double valor)
{
    vazao = valor;
    Validar();
}

double Fluido::FatorTemperatura() const
{
    return std::exp(0.02 * temperatura);
}

double Fluido::PressaoParcialCO2() const
{
    return co2 * pressao;
}

double Fluido::FatorAcidez() const
{
    const double acidez = 7.0 - pH;

    if (acidez < 0.0)
        return 0.0;

    return acidez;
}

void Fluido::Imprimir() const
{
    std::cout << "========== Propriedades do Fluido ==========" << '\n';
    std::cout << "pH:                   " << pH << '\n';
    std::cout << "CO2:                  " << co2 << " ppm" << '\n';
    std::cout << "H2S:                  " << h2s << " ppm" << '\n';
    std::cout << "Salinidade:           " << salinidade << " ppm" << '\n';
    std::cout << "Temperatura:          " << temperatura << " °C" << '\n';
    std::cout << "Pressão:              " << pressao << " psi" << '\n';
    std::cout << "Vazão:                " << vazao << " m³/dia" << '\n';
    std::cout << "Fator temperatura:    " << FatorTemperatura() << '\n';
    std::cout << "Pressão parcial CO2:  " << PressaoParcialCO2() << '\n';
    std::cout << "Fator de acidez:      " << FatorAcidez() << '\n';
}
