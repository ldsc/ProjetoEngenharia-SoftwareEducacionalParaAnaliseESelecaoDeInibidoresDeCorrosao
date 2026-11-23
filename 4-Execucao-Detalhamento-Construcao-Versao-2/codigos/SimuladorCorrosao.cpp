#include "SimuladorCorrosao.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

#include "CalculadoraDosagem.h"

SimuladorCorrosao::SimuladorCorrosao()
{
    amostras = AmostrasPadrao();
}

std::vector<Fluido> SimuladorCorrosao::AmostrasPadrao()
{
    return {
        Fluido(5.50, 60.0, 25.0, 80000.0, 65.0, 4000.0, 5000.0),   // Sour acentuado
        Fluido(5.20, 40.0, 5.0, 120000.0, 75.0, 3800.0, 8000.0),   // Sweet severa
        Fluido(5.80, 120.0, 3.0, 20000.0, 95.0, 5000.0, 4000.0),   // Sweet intensa / T alta
        Fluido(6.80, 5.0, 2.0, 5000.0, 35.0, 2000.0, 1000.0),      // Leve / baixo risco
        Fluido(6.90, 8.0, 1.0, 180000.0, 55.0, 3000.0, 3000.0),    // Leve / catodico (baixa)
        Fluido(6.50, 10.0, 2.0, 50000.0, 45.0, 2500.0, 2500.0),    // Leve / catodico (media)
        Fluido(4.80, 90.0, 40.0, 150000.0, 110.0, 8000.0, 9000.0)  // Extremo combinado
    };
}

int SimuladorCorrosao::Executar()
{
    try
    {
        ImprimirCabecalho();

        for (std::size_t i = 0; i < amostras.size(); ++i)
        {
            ExecutarSimulacao(amostras[i], static_cast<int>(i + 1));
        }
    }
    catch (const std::exception& erro)
    {
        std::cerr << "Erro na simulação: " << erro.what() << '\n';
        return 1;
    }

    return 0;
}

void SimuladorCorrosao::ImprimirCabecalho() const
{
    std::cout << "============================================================" << '\n';
    std::cout << "     SIMULADOR DE CORROSÃO - INDÚSTRIA DE ÓLEO E GÁS         " << '\n';
    std::cout << "============================================================" << '\n';
}

void SimuladorCorrosao::ImprimirTituloSecao(const std::string& titulo) const
{
    std::cout << '\n';
    std::cout << "------------------------------------------------------------" << '\n';
    std::cout << titulo << '\n';
    std::cout << "------------------------------------------------------------" << '\n';
}

void SimuladorCorrosao::ImprimirResumoFluido(const Fluido& fluido) const
{
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "pH:                   " << fluido.PH() << '\n';
    std::cout << "CO2:                  " << fluido.CO2() << " ppm" << '\n';
    std::cout << "H2S:                  " << fluido.H2S() << " ppm" << '\n';
    std::cout << "Salinidade:           " << fluido.Salinidade() << " ppm" << '\n';
    std::cout << "Temperatura:          " << fluido.Temperatura() << " °C" << '\n';
    std::cout << "Pressão:              " << fluido.Pressao() << " psi" << '\n';
    std::cout << "Vazão:                " << fluido.Vazao() << " m³/dia" << '\n';
    std::cout << "Fator de temperatura: " << fluido.FatorTemperatura() << '\n';
    std::cout << "Pressão parcial CO2:  " << fluido.PressaoParcialCO2() << '\n';
    std::cout << "Fator de acidez:      " << fluido.FatorAcidez() << '\n';
}

void SimuladorCorrosao::ImprimirResumoCorrosao(const AnaliseCorrosao& analise) const
{
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Tipo de corrosão:         " << analise.TipoCorrosao() << '\n';
    std::cout << "Severidade:               " << analise.Severidade() << '\n';
    std::cout << "Taxa base de corrosão:    " << analise.TaxaCorrosao() << '\n';
    std::cout << "Fator de severidade Fs:   " << analise.FatorSeveridade() << '\n';
    std::cout << "Fator térmico Ft:         " << analise.FatorCorrecaoTemperatura() << '\n';
    std::cout << "Fator corrosivo Fc:       " << analise.FatorCorrosivo() << '\n';
    std::cout << "Taxa final de corrosão:   " << analise.TaxaCorrosaoFinal() << '\n';
}

void SimuladorCorrosao::ImprimirResumoInibidor(const Inibidor& inibidor) const
{
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Nome:         " << inibidor.Nome() << '\n';
    std::cout << "Tipo:         " << inibidor.Tipo() << '\n';
    std::cout << "Eficiência:   " << inibidor.Eficiencia() * 100.0 << " %" << '\n';
    std::cout << "Descrição:    " << inibidor.Descricao() << '\n';
}

void SimuladorCorrosao::ImprimirResumoDosagem(const Fluido& fluido,
                                            const AnaliseCorrosao& analise,
                                            const Inibidor& inibidor) const
{
    const double fatorVazao = CalculadoraDosagem::FatorDeVazao(fluido);
    const double dose = CalculadoraDosagem::CalcularDose(fluido, analise, inibidor);

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Fator de vazão Fq:   " << fatorVazao << '\n';
    std::cout << "Dose recomendada:    " << dose << " ppm" << '\n';
}

void SimuladorCorrosao::AnalisarEExibirResultados(const Fluido& fluido, const std::string& titulo) const
{
    std::cout << '\n';
    std::cout << "============================================================" << '\n';
    std::cout << titulo << '\n';
    std::cout << "============================================================" << '\n';

    AnaliseCorrosao analise(fluido);
    std::unique_ptr<Inibidor> inibidor = SeletorInibidor::Selecionar(fluido);

    ImprimirTituloSecao("1. Propriedades do fluido");
    ImprimirResumoFluido(fluido);

    ImprimirTituloSecao("2. Análise de corrosão");
    ImprimirResumoCorrosao(analise);

    ImprimirTituloSecao("3. Inibidor selecionado");
    ImprimirResumoInibidor(*inibidor);

    ImprimirTituloSecao("4. Dosagem recomendada");
    ImprimirResumoDosagem(fluido, analise, *inibidor);
}

void SimuladorCorrosao::ExecutarSimulacao(const Fluido& fluido, int numeroAmostra) const
{
    AnalisarEExibirResultados(fluido, "AMOSTRA " + std::to_string(numeroAmostra));
}

double SimuladorCorrosao::LerDouble(const std::string& mensagem)
{
    double valor;

    while (true)
    {
        std::cout << mensagem;

        if (std::cin >> valor)
        {
            return valor;
        }

        if (std::cin.eof())
        {
            throw std::runtime_error("Entrada de dados encerrada antes do esperado.");
        }

        std::cout << "Entrada inválida. Digite um número." << '\n';
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

Fluido SimuladorCorrosao::LerDadosDoUsuario() const
{
    std::cout << '\n';
    std::cout << "============================================================" << '\n';
    std::cout << "              INFORME OS DADOS DO FLUIDO                    " << '\n';
    std::cout << "============================================================" << '\n';
    std::cout << "(faixas típicas de referência entre parênteses; use ponto decimal)" << '\n';
    std::cout << '\n';

    while (true)
    {
        const double pH = LerDouble("pH (0 a 14; típico 4 a 8): ");
        const double co2 = LerDouble("CO2 em ppm (típico 0 a 1000; sweet se > 20): ");
        const double h2s = LerDouble("H2S em ppm (típico 0 a 200; sour se > 10): ");
        const double salinidade = LerDouble("Salinidade em ppm (típico 1.000 a 250.000): ");
        const double temperatura = LerDouble("Temperatura em °C (típico 20 a 150): ");
        const double pressao = LerDouble("Pressão em psi (típico 500 a 5.000): ");
        const double vazao = LerDouble("Vazão em m³/dia (típico 100 a 10.000): ");

        try
        {
            return Fluido(pH, co2, h2s, salinidade, temperatura, pressao, vazao);
        }
        catch (const std::invalid_argument& erro)
        {
            std::cout << '\n';
            std::cout << "Dados inválidos: " << erro.what() << '\n';
            std::cout << "Por favor, informe os dados novamente." << '\n';
            std::cout << '\n';
        }
    }
}

void SimuladorCorrosao::ExecutarInterativo() const
{
    const Fluido fluido = LerDadosDoUsuario();
    AnalisarEExibirResultados(fluido, "SIMULAÇÃO PERSONALIZADA");
}
