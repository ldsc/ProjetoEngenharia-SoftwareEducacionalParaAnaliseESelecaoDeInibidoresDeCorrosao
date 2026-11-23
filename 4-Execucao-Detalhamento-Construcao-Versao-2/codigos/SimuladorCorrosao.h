#ifndef SIMULADORCORROSAO_H
#define SIMULADORCORROSAO_H

#include <string>
#include <vector>

#include "Fluido.h"
#include "AnaliseCorrosao.h"
#include "Inibidor.h"

class SimuladorCorrosao
{
private:
    std::vector<Fluido> amostras;

    void ImprimirCabecalho() const;
    void ImprimirTituloSecao(const std::string& titulo) const;

    void ImprimirResumoFluido(const Fluido& fluido) const;
    void ImprimirResumoCorrosao(const AnaliseCorrosao& analise) const;
    void ImprimirResumoInibidor(const Inibidor& inibidor) const;
    void ImprimirResumoDosagem(const Fluido& fluido,
                            const AnaliseCorrosao& analise,
                            const Inibidor& inibidor) const;

    void AnalisarEExibirResultados(const Fluido& fluido, const std::string& titulo) const;

    void ExecutarSimulacao(const Fluido& fluido, int numeroAmostra) const;

    static double LerDouble(const std::string& mensagem);
    Fluido LerDadosDoUsuario() const;

public:
    SimuladorCorrosao();

    // Retorna o conjunto de amostras padrao (tipicas de industria de oleo e gas),
    // usado tanto pelo menu de console quanto pela interface grafica.
    static std::vector<Fluido> AmostrasPadrao();

    int Executar();
    void ExecutarInterativo() const;
};

#endif
