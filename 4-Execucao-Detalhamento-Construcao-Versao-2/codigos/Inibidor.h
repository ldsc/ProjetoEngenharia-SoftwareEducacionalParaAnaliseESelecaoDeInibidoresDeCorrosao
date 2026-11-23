#ifndef INIBIDOR_H
#define INIBIDOR_H

#include <memory>
#include <string>

#include "Fluido.h"

class Inibidor
{
protected:
    std::string nome;
    double eficiencia;
    std::string descricao;

public:
    Inibidor(const std::string& nome,
              double eficiencia,
              const std::string& descricao);

    virtual ~Inibidor() = default;

    std::string Nome() const;
    double Eficiencia() const;
    std::string Descricao() const;

    virtual std::string Tipo() const = 0;
    virtual bool EhAplicavel(const Fluido& fluido) const = 0;
};

class InibidorOrganico : public Inibidor
{
public:
    InibidorOrganico();

    std::string Tipo() const override;
    bool EhAplicavel(const Fluido& fluido) const override;
};

class InibidorH2S : public Inibidor
{
public:
    InibidorH2S();

    std::string Tipo() const override;
    bool EhAplicavel(const Fluido& fluido) const override;
};

class InibidorCatodico : public Inibidor
{
public:
    InibidorCatodico();

    std::string Tipo() const override;
    bool EhAplicavel(const Fluido& fluido) const override;
};

class InibidorGeral : public Inibidor
{
public:
    InibidorGeral();

    std::string Tipo() const override;
    bool EhAplicavel(const Fluido& fluido) const override;
};

class SeletorInibidor
{
public:
    static std::unique_ptr<Inibidor> Selecionar(const Fluido& fluido);
};

#endif
