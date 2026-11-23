#include "SimuladorCorrosao.h"

#include <iostream>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
    // Garante que os acentos (á, ã, ç, é, ...) apareçam corretamente
    // no console do Windows, que por padrão usa uma code page diferente de UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    SimuladorCorrosao simulador;
    int opcao = -1;

    while (opcao != 0)
    {
        std::cout << '\n';
        std::cout << "============================================================" << '\n';
        std::cout << "        SIMULADOR DE CORROSÃO - INDÚSTRIA DE ÓLEO E GÁS      " << '\n';
        std::cout << "============================================================" << '\n';
        std::cout << "1 - Rodar simulação com amostras padrão" << '\n';
        std::cout << "2 - Informar dados de um fluido e simular" << '\n';
        std::cout << "0 - Sair" << '\n';
        std::cout << "Escolha uma opção: ";

        if (!(std::cin >> opcao))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Opção inválida." << '\n';
            continue;
        }

        switch (opcao)
        {
            case 1:
                simulador.Executar();
                break;

            case 2:
                try
                {
                    simulador.ExecutarInterativo();
                }
                catch (const std::exception& erro)
                {
                    std::cout << '\n';
                    std::cout << "Não foi possível concluir a simulação: " << erro.what() << '\n';
                    opcao = 0;
                }
                break;

            case 0:
                std::cout << "Encerrando o simulador." << '\n';
                break;

            default:
                std::cout << "Opção inválida." << '\n';
                break;
        }
    }

    return 0;
}
