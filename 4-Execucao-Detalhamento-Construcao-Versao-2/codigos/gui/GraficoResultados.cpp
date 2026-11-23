#include "GraficoResultados.h"

#include <algorithm>
#include <cmath>

#include <QFont>
#include <QLinearGradient>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QPen>
#include <QRect>

GraficoResultados::GraficoResultados(QWidget* parent)
    : QWidget(parent),
      temDados(false),
      valorPH(0.0),
      valorCO2(0.0),
      valorH2S(0.0),
      valorSalinidade(0.0),
      valorTemperatura(0.0),
      valorPressao(0.0),
      valorVazao(0.0),
      taxaCorrosaoBase(0.0),
      taxaCorrosaoFinal(0.0),
      eficienciaInibidor(0.0),
      doseRecomendada(0.0)
{
    setMinimumHeight(900);
    setAutoFillBackground(true);

    QPalette paleta = palette();
    paleta.setColor(QPalette::Window, QColor("#ffffff"));
    setPalette(paleta);
}

QSize GraficoResultados::minimumSizeHint() const
{
    return QSize(640, 900);
}

QSize GraficoResultados::sizeHint() const
{
    return QSize(760, 980);
}

void GraficoResultados::Limpar()
{
    temDados = false;
    fatoresCorrecao.clear();
    update();
}

void GraficoResultados::AtualizarDados(const Fluido& fluido,
                                        const AnaliseCorrosao& analise,
                                        const Inibidor& inibidor,
                                        double fatorVazao,
                                        double dose)
{
    valorPH = fluido.PH();
    valorCO2 = fluido.CO2();
    valorH2S = fluido.H2S();
    valorSalinidade = fluido.Salinidade();
    valorTemperatura = fluido.Temperatura();
    valorPressao = fluido.Pressao();
    valorVazao = fluido.Vazao();

    tipoCorrosao = QString::fromStdString(analise.TipoCorrosao());
    severidade = QString::fromStdString(analise.Severidade());
    taxaCorrosaoBase = analise.TaxaCorrosao();
    taxaCorrosaoFinal = analise.TaxaCorrosaoFinal();

    fatoresCorrecao.clear();
    fatoresCorrecao.append({"Severidade\n(Fs)",
                             analise.FatorSeveridade(),
                             QString::number(analise.FatorSeveridade(), 'f', 2),
                             QColor("#1e88e5")});
    fatoresCorrecao.append({"Temperatura\n(Ft)",
                             analise.FatorCorrecaoTemperatura(),
                             QString::number(analise.FatorCorrecaoTemperatura(), 'f', 2),
                             QColor("#8e24aa")});
    fatoresCorrecao.append({"Corrosivo\n(Fc)",
                             analise.FatorCorrosivo(),
                             QString::number(analise.FatorCorrosivo(), 'f', 2),
                             QColor("#fb8c00")});
    fatoresCorrecao.append({"Vazao\n(Fq)",
                             fatorVazao,
                             QString::number(fatorVazao, 'f', 2),
                             QColor("#00897b")});

    nomeInibidor = QString::fromStdString(inibidor.Nome());
    eficienciaInibidor = inibidor.Eficiencia() * 100.0;

    doseRecomendada = dose;

    temDados = true;
    update();
}

QColor GraficoResultados::CorPorSeveridade(const QString& severidade) const
{
    if (severidade == "Baixa")
        return QColor("#2e7d32");

    if (severidade == "Média")
        return QColor("#f9a825");

    return QColor("#c62828");
}

void GraficoResultados::paintEvent(QPaintEvent* evento)
{
    Q_UNUSED(evento);

    QPainter pintor(this);
    pintor.setRenderHint(QPainter::Antialiasing, true);
    pintor.setRenderHint(QPainter::TextAntialiasing, true);

    const int margem = 20;
    const QRect area = rect().adjusted(margem, margem, -margem, -margem);

    QFont fonteTitulo("Segoe UI", 13, QFont::Bold);
    pintor.setFont(fonteTitulo);
    pintor.setPen(QColor("#0b3d63"));

    const QRect areaTituloGeral(area.left(), area.top(), area.width(), 30);
    pintor.drawText(areaTituloGeral, Qt::AlignLeft | Qt::AlignVCenter,
                     "Análise Gráfica dos Resultados");

    if (!temDados)
    {
        DesenharMensagemVazia(pintor);
        return;
    }

    int y = areaTituloGeral.bottom() + 12;

    // ---------- Secao 0: perfil do fluido simulado (parametros de entrada) ----------
    const int alturaResumo = 100;
    const QRect areaResumo(area.left(), y, area.width(), alturaResumo);
    DesenharResumoFluido(pintor, areaResumo);
    y = areaResumo.bottom() + 18;

    // ---------- Secao 1: fatores de correcao da taxa de corrosao ----------
    const int alturaBarras = 210;
    const QRect areaBarras(area.left(), y, area.width(), alturaBarras);
    DesenharGraficoBarrasVerticais(pintor, areaBarras, fatoresCorrecao);
    y = areaBarras.bottom() + 18;

    // ---------- Secao 1.5: evolucao da taxa por etapa (novo grafico) ----------
    const int alturaEvolucao = 220;
    const QRect areaEvolucao(area.left(), y, area.width(), alturaEvolucao);
    DesenharEvolucaoTaxa(pintor, areaEvolucao);
    y = areaEvolucao.bottom() + 18;

    // ---------- Secao 2: comparativo de taxas + gauge de eficiencia ----------
    const int alturaSecao2 = 190;
    const int espacoEntreColunas = 20;
    const int larguraColuna = (area.width() - espacoEntreColunas) / 2;

    const QRect areaTaxas(area.left(), y, larguraColuna, alturaSecao2);
    const QRect areaGauge(areaTaxas.right() + espacoEntreColunas, y, larguraColuna, alturaSecao2);

    DesenharComparativoTaxas(pintor, areaTaxas, taxaCorrosaoBase, taxaCorrosaoFinal,
                              CorPorSeveridade(severidade));
    DesenharGaugeEficiencia(pintor, areaGauge, eficienciaInibidor, nomeInibidor);

    y = areaTaxas.bottom() + 18;

    // ---------- Secao 3: dose recomendada ----------
    const int alturaDose = 90;
    const QRect areaDose(area.left(), y, area.width(), alturaDose);
    DesenharCartaoDose(pintor, areaDose, doseRecomendada);
}

void GraficoResultados::DesenharMensagemVazia(QPainter& pintor) const
{
    pintor.setPen(QColor("#7f97ab"));
    pintor.setFont(QFont("Segoe UI", 11));
    pintor.drawText(rect().adjusted(20, 60, -20, -20), Qt::AlignHCenter | Qt::AlignTop,
                     "Rode uma simulação na aba \"Formulário\", \"Amostras Padrão\" ou \"Modo Perguntas\"\n"
                     "para visualizar aqui os gráficos dos resultados.");
}

void GraficoResultados::DesenharTituloSecao(QPainter& pintor, const QRect& area, const QString& texto) const
{
    pintor.setFont(QFont("Segoe UI", 10, QFont::DemiBold));
    pintor.setPen(QColor("#0b3d63"));
    pintor.drawText(area, Qt::AlignLeft | Qt::AlignVCenter, texto);
}

void GraficoResultados::DesenharResumoFluido(QPainter& pintor, const QRect& area) const
{
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QColor("#f4f8fc"));
    pintor.drawRoundedRect(area, 10, 10);

    DesenharTituloSecao(pintor, QRect(area.left() + 14, area.top() + 8, area.width() - 28, 20),
                         "Perfil do Fluido Simulado (parâmetros fornecidos)");

    struct Item
    {
        QString rotulo;
        QString valor;
    };

    const QVector<Item> itens = {
        {"pH", QString::number(valorPH, 'f', 2)},
        {"CO2", QString("%1 ppm").arg(QString::number(valorCO2, 'f', 1))},
        {"H2S", QString("%1 ppm").arg(QString::number(valorH2S, 'f', 1))},
        {"Salinidade", QString("%1 ppm").arg(QString::number(valorSalinidade, 'f', 0))},
        {"Temperatura", QString("%1 °C").arg(QString::number(valorTemperatura, 'f', 1))},
        {"Pressão", QString("%1 psi").arg(QString::number(valorPressao, 'f', 0))},
        {"Vazão", QString("%1 m³/dia").arg(QString::number(valorVazao, 'f', 0))}
    };

    const int margemInterna = 14;
    const int espacamento = 8;
    const int numItens = itens.size();
    const int larguraCartao =
        (area.width() - margemInterna * 2 - espacamento * (numItens - 1)) / numItens;

    const QRect areaCartoes(area.left() + margemInterna, area.top() + 34,
                             area.width() - margemInterna * 2, area.height() - 44);

    int x = areaCartoes.left();
    for (const Item& item : itens)
    {
        const QRect cartao(x, areaCartoes.top(), larguraCartao, areaCartoes.height());

        pintor.setPen(Qt::NoPen);
        pintor.setBrush(QColor("#ffffff"));
        pintor.drawRoundedRect(cartao, 8, 8);

        pintor.setPen(QColor("#5c7387"));
        pintor.setFont(QFont("Segoe UI", 8, QFont::DemiBold));
        pintor.drawText(QRect(cartao.left(), cartao.top() + 6, cartao.width(), 16),
                         Qt::AlignHCenter | Qt::AlignVCenter, item.rotulo);

        pintor.setPen(QColor("#14639e"));
        pintor.setFont(QFont("Segoe UI", 10, QFont::Bold));
        pintor.drawText(cartao.adjusted(2, 22, -2, -6), Qt::AlignHCenter | Qt::AlignVCenter, item.valor);

        x += larguraCartao + espacamento;
    }
}

void GraficoResultados::DesenharGraficoBarrasVerticais(QPainter& pintor,
                                                         const QRect& area,
                                                         const QVector<Barra>& barras) const
{
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QColor("#f4f8fc"));
    pintor.drawRoundedRect(area, 10, 10);

    DesenharTituloSecao(pintor, QRect(area.left() + 14, area.top() + 8, area.width() - 28, 20),
                         "Fatores de Correção da Taxa de Corrosão");

    if (barras.isEmpty())
        return;

    const QRect areaGrafico(area.left() + 24, area.top() + 40,
                             area.width() - 48, area.height() - 78);

    double valorMax = 1.0;
    for (const Barra& barra : barras)
        valorMax = std::max(valorMax, barra.valor);
    valorMax *= 1.25;

    // ---------- Linhas de grade horizontais de referencia ----------
    const int numLinhas = 4;
    for (int i = 0; i <= numLinhas; ++i)
    {
        const double fracao = static_cast<double>(i) / numLinhas;
        const int yLinha = areaGrafico.bottom() - static_cast<int>(fracao * areaGrafico.height());

        pintor.setPen(QPen(QColor("#dbe6f0"), 1));
        pintor.drawLine(areaGrafico.left(), yLinha, areaGrafico.right(), yLinha);

        pintor.setPen(QColor("#8296a8"));
        pintor.setFont(QFont("Segoe UI", 8));
        pintor.drawText(QRect(area.left(), yLinha - 8, areaGrafico.left() - area.left() - 6, 16),
                         Qt::AlignRight | Qt::AlignVCenter,
                         QString::number(fracao * valorMax, 'f', 1));
    }

    // ---------- Barras ----------
    const int numBarras = barras.size();
    const int espacamento = 18;
    const int larguraBarra = std::min(60, (areaGrafico.width() - espacamento * (numBarras - 1)) / numBarras);
    const int larguraTotal = larguraBarra * numBarras + espacamento * (numBarras - 1);
    int x = areaGrafico.left() + (areaGrafico.width() - larguraTotal) / 2;

    for (const Barra& barra : barras)
    {
        const double fracao = barra.valor / valorMax;
        const int alturaBarra = static_cast<int>(fracao * areaGrafico.height());

        const QRect retanguloBarra(x, areaGrafico.bottom() - alturaBarra, larguraBarra, alturaBarra);

        QLinearGradient gradiente(retanguloBarra.topLeft(), retanguloBarra.bottomLeft());
        gradiente.setColorAt(0.0, barra.cor.lighter(115));
        gradiente.setColorAt(1.0, barra.cor);

        pintor.setPen(Qt::NoPen);
        pintor.setBrush(gradiente);
        pintor.drawRoundedRect(retanguloBarra, 6, 6);

        pintor.setPen(QColor("#1c2b3a"));
        pintor.setFont(QFont("Segoe UI", 9, QFont::Bold));
        pintor.drawText(QRect(x - 10, retanguloBarra.top() - 20, larguraBarra + 20, 18),
                         Qt::AlignHCenter | Qt::AlignBottom, barra.valorTexto);

        pintor.setPen(QColor("#3c4f61"));
        pintor.setFont(QFont("Segoe UI", 8));
        pintor.drawText(QRect(x - 20, areaGrafico.bottom() + 4, larguraBarra + 40, 32),
                         Qt::AlignHCenter | Qt::AlignTop, barra.rotulo);

        x += larguraBarra + espacamento;
    }

    pintor.setPen(QPen(QColor("#9db3c6"), 1.2));
    pintor.drawLine(areaGrafico.bottomLeft(), areaGrafico.bottomRight());
}

void GraficoResultados::DesenharEvolucaoTaxa(QPainter& pintor, const QRect& area) const
{
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QColor("#f4f8fc"));
    pintor.drawRoundedRect(area, 10, 10);

    DesenharTituloSecao(pintor, QRect(area.left() + 14, area.top() + 8, area.width() - 28, 20),
                         "Evolução da Taxa de Corrosão por Etapa");

    if (fatoresCorrecao.size() < 3)
        return;

    // Mostra, passo a passo, como a taxa base vai sendo multiplicada por cada
    // fator de correcao ate chegar na taxa final (efeito cumulativo/"waterfall").
    const double fs = fatoresCorrecao[0].valor;
    const double ft = fatoresCorrecao[1].valor;
    const double fc = fatoresCorrecao[2].valor;

    const double v0 = taxaCorrosaoBase;
    const double v1 = v0 * fs;
    const double v2 = v1 * ft;
    const double v3 = v2 * fc;

    struct Etapa
    {
        QString rotulo;
        double valor;
    };

    const QVector<Etapa> etapas = {
        {"Taxa base", v0},
        {"\u00d7 Fs (severidade)", v1},
        {"\u00d7 Ft (temperatura)", v2},
        {"\u00d7 Fc (corrosivo) = Final", v3}
    };

    const int alturaBarra = 24;
    const int espacoEntreBarras = 12;
    const int larguraRotulo = 150;
    const int larguraMaxima = std::max(60, area.width() - larguraRotulo - 90);
    const double valorMax = std::max(v3, 1.0) * 1.1;

    int y = area.top() + 40;

    const QColor corInicial("#607d8b");
    const QColor corFinal = CorPorSeveridade(severidade);

    for (int i = 0; i < etapas.size(); ++i)
    {
        const Etapa& etapa = etapas[i];

        pintor.setFont(QFont("Segoe UI", 9));
        pintor.setPen(QColor("#1c2b3a"));
        pintor.drawText(QRect(area.left() + 14, y, larguraRotulo, alturaBarra),
                         Qt::AlignLeft | Qt::AlignVCenter, etapa.rotulo);

        const int largura = std::max(4, static_cast<int>((etapa.valor / valorMax) * larguraMaxima));
        const QRect retangulo(area.left() + 14 + larguraRotulo, y, largura, alturaBarra);

        const double fracaoEtapa = static_cast<double>(i) / static_cast<double>(etapas.size() - 1);
        const QColor corBarra(
            static_cast<int>(corInicial.red() + (corFinal.red() - corInicial.red()) * fracaoEtapa),
            static_cast<int>(corInicial.green() + (corFinal.green() - corInicial.green()) * fracaoEtapa),
            static_cast<int>(corInicial.blue() + (corFinal.blue() - corInicial.blue()) * fracaoEtapa));

        QLinearGradient gradiente(retangulo.topLeft(), retangulo.topRight());
        gradiente.setColorAt(0.0, corBarra);
        gradiente.setColorAt(1.0, corBarra.lighter(125));

        pintor.setPen(Qt::NoPen);
        pintor.setBrush(gradiente);
        pintor.drawRoundedRect(retangulo, 6, 6);

        pintor.setPen(QColor("#1c2b3a"));
        pintor.setFont(QFont("Segoe UI", 9, QFont::Bold));
        pintor.drawText(QRect(retangulo.right() + 8, y, 80, alturaBarra),
                         Qt::AlignLeft | Qt::AlignVCenter, QString::number(etapa.valor, 'f', 2));

        y += alturaBarra + espacoEntreBarras;
    }
}

void GraficoResultados::DesenharComparativoTaxas(QPainter& pintor,
                                                   const QRect& area,
                                                   double taxaBase,
                                                   double taxaFinal,
                                                   const QColor& cor) const
{
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QColor("#f4f8fc"));
    pintor.drawRoundedRect(area, 10, 10);

    DesenharTituloSecao(pintor, QRect(area.left() + 14, area.top() + 8, area.width() - 28, 20),
                         "Taxa de Corrosão: Base vs Final (mm/ano)");

    pintor.setFont(QFont("Segoe UI", 8));
    pintor.setPen(QColor("#3c4f61"));
    pintor.drawText(QRect(area.left() + 14, area.top() + 26, area.width() - 28, 16),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     QString("Tipo: %1   |   Severidade: %2").arg(tipoCorrosao, severidade));

    const int alturaBarra = 26;
    const int espacoEntreBarras = 14;
    const int larguraMaxima = std::max(40, area.width() - 150);
    const double valorMax = std::max({taxaBase, taxaFinal, 1.0}) * 1.15;

    int y = area.top() + 56;

    auto desenharLinha = [&](const QString& rotulo, double valor, const QColor& corBarra)
    {
        pintor.setFont(QFont("Segoe UI", 9));
        pintor.setPen(QColor("#1c2b3a"));
        pintor.drawText(QRect(area.left() + 14, y, 100, alturaBarra),
                         Qt::AlignLeft | Qt::AlignVCenter, rotulo);

        const int largura = std::max(4, static_cast<int>((valor / valorMax) * larguraMaxima));
        const QRect retangulo(area.left() + 120, y, largura, alturaBarra);

        QLinearGradient gradiente(retangulo.topLeft(), retangulo.topRight());
        gradiente.setColorAt(0.0, corBarra);
        gradiente.setColorAt(1.0, corBarra.lighter(130));

        pintor.setPen(Qt::NoPen);
        pintor.setBrush(gradiente);
        pintor.drawRoundedRect(retangulo, 6, 6);

        pintor.setPen(QColor("#1c2b3a"));
        pintor.setFont(QFont("Segoe UI", 9, QFont::Bold));
        pintor.drawText(QRect(retangulo.right() + 8, y, 90, alturaBarra),
                         Qt::AlignLeft | Qt::AlignVCenter, QString::number(valor, 'f', 2));

        y += alturaBarra + espacoEntreBarras;
    };

    desenharLinha("Taxa base", taxaBase, QColor("#607d8b"));
    desenharLinha("Taxa final", taxaFinal, cor);
}

void GraficoResultados::DesenharGaugeEficiencia(QPainter& pintor,
                                                 const QRect& area,
                                                 double percentual,
                                                 const QString& rotulo) const
{
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QColor("#f4f8fc"));
    pintor.drawRoundedRect(area, 10, 10);

    DesenharTituloSecao(pintor, QRect(area.left() + 14, area.top() + 8, area.width() - 28, 20),
                         "Eficiência do Inibidor Selecionado");

    const int diametro = std::min(area.width() - 40, area.height() - 70);
    const QRect areaCirculo(area.center().x() - diametro / 2, area.top() + 38, diametro, diametro);

    const int espessura = std::max(10, diametro / 12);
    const QRect areaArco = areaCirculo.adjusted(espessura / 2, espessura / 2, -espessura / 2, -espessura / 2);

    pintor.setPen(QPen(QColor("#dbe6f0"), espessura, Qt::SolidLine, Qt::RoundCap));
    pintor.setBrush(Qt::NoBrush);
    pintor.drawArc(areaArco, 0, 360 * 16);

    QColor corGauge("#1e88e5");
    if (percentual >= 85.0)
        corGauge = QColor("#2e7d32");
    else if (percentual < 75.0)
        corGauge = QColor("#f9a825");

    const double fracao = std::clamp(percentual / 100.0, 0.0, 1.0);
    const int anguloSpan = static_cast<int>(-fracao * 360 * 16);

    pintor.setPen(QPen(corGauge, espessura, Qt::SolidLine, Qt::RoundCap));
    pintor.drawArc(areaArco, 90 * 16, anguloSpan);

    pintor.setPen(QColor("#0b3d63"));
    pintor.setFont(QFont("Segoe UI", 16, QFont::Bold));
    pintor.drawText(areaCirculo, Qt::AlignCenter,
                     QString("%1%").arg(QString::number(percentual, 'f', 1)));

    pintor.setFont(QFont("Segoe UI", 8));
    pintor.setPen(QColor("#3c4f61"));
    pintor.drawText(QRect(area.left(), areaCirculo.bottom() + 2, area.width(), 18),
                     Qt::AlignHCenter | Qt::AlignTop, rotulo);
}

void GraficoResultados::DesenharCartaoDose(QPainter& pintor,
                                            const QRect& area,
                                            double dose) const
{
    QLinearGradient gradiente(area.topLeft(), area.topRight());
    gradiente.setColorAt(0.0, QColor("#0b3d63"));
    gradiente.setColorAt(1.0, QColor("#14639e"));

    pintor.setPen(Qt::NoPen);
    pintor.setBrush(gradiente);
    pintor.drawRoundedRect(area, 10, 10);

    pintor.setPen(QColor("#ffffff"));
    pintor.setFont(QFont("Segoe UI", 10, QFont::DemiBold));
    pintor.drawText(QRect(area.left() + 20, area.top() + 12, area.width() - 40, 20),
                     Qt::AlignLeft | Qt::AlignVCenter, "Dosagem Recomendada de Inibidor");

    pintor.setFont(QFont("Segoe UI", 22, QFont::Bold));
    pintor.drawText(QRect(area.left() + 20, area.top() + 30, area.width() - 40, area.height() - 40),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     QString("%1 ppm").arg(QString::number(dose, 'f', 2)));
}
