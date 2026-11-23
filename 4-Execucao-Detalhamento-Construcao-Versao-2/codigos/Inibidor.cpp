#include "Inibidor.h"

Inibidor::Inibidor(const std::string& nome,
                     double eficiencia,
                     const std::string& descricao)
    : nome(nome),
      eficiencia(eficiencia),
      descricao(descricao)
{
}

std::string Inibidor::Nome() const
{
    return nome;
}

double Inibidor::Eficiencia() const
{
    return eficiencia;
}

std::string Inibidor::Descricao() const
{
    return descricao;
}

InibidorOrganico::InibidorOrganico()
    : Inibidor("Inibidor Orgânico",
                0.85,
                "Forma filme adsorvido na superfície metálica, reduzindo o contato metal-fluido.")
{
}

std::string InibidorOrganico::Tipo() const
{
    return "Orgânico";
}

bool InibidorOrganico::EhAplicavel(const Fluido& fluido) const
{
    return fluido.CO2() > 20.0 && fluido.PH() < 6.0;
}

InibidorH2S::InibidorH2S()
    : Inibidor("Inibidor para H2S",
                0.93,
                "Indicado para ambientes sour com presença significativa de H2S.")
{
}

std::string InibidorH2S::Tipo() const
{
    return "H2S";
}

bool InibidorH2S::EhAplicavel(const Fluido& fluido) const
{
    return fluido.H2S() > 10.0;
}

InibidorCatodico::InibidorCatodico()
    : Inibidor("Inibidor Catódico",
                0.80,
                "Reduz a reação catódica, sendo indicado para ambientes de alta salinidade.")
{
}

std::string InibidorCatodico::Tipo() const
{
    return "Catódico";
}

bool InibidorCatodico::EhAplicavel(const Fluido& fluido) const
{
    return fluido.Salinidade() > 15000.0;
}

InibidorGeral::InibidorGeral()
    : Inibidor("Inibidor Geral",
                0.70,
                "Aplicado em cenários leves ou como alternativa padrão.")
{
}

std::string InibidorGeral::Tipo() const
{
    return "Geral";
}

bool InibidorGeral::EhAplicavel(const Fluido&) const
{
    return true;
}

std::unique_ptr<Inibidor> SeletorInibidor::Selecionar(const Fluido& fluido)
{
    if (fluido.H2S() > 10.0)
        return std::make_unique<InibidorH2S>();

    if (fluido.CO2() > 20.0 && fluido.PH() < 6.0)
        return std::make_unique<InibidorOrganico>();

    if (fluido.Salinidade() > 15000.0)
        return std::make_unique<InibidorCatodico>();

    return std::make_unique<InibidorGeral>();
}
