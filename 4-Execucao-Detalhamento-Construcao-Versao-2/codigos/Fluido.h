#ifndef FLUIDO_H
#define FLUIDO_H

#include <string>

class Fluido
{
private:
    double pH;
    double co2;
    double h2s;
    double salinidade;
    double temperatura;
    double pressao;
    double vazao;

    void Validar() const;

public:
    Fluido();

    Fluido(double pH,
                    double co2,
                    double h2s,
                    double salinidade,
                    double temperatura,
                    double pressao,
                    double vazao);

    double PH() const;
    double CO2() const;
    double H2S() const;
    double Salinidade() const;
    double Temperatura() const;
    double Pressao() const;
    double Vazao() const;

    void DefinirPH(double pH);
    void DefinirCO2(double co2);
    void DefinirH2S(double h2s);
    void DefinirSalinidade(double salinidade);
    void DefinirTemperatura(double temperatura);
    void DefinirPressao(double pressao);
    void DefinirVazao(double vazao);

    double FatorTemperatura() const;
    double PressaoParcialCO2() const;
    double FatorAcidez() const;

    void Imprimir() const;
};

#endif
