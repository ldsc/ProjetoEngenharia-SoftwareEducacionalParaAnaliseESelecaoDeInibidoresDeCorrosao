#ifndef GRAFICORESULTADOS_H
#define GRAFICORESULTADOS_H

#include <QColor>
#include <QSize>
#include <QString>
#include <QVector>
#include <QWidget>

#include "../AnaliseCorrosao.h"
#include "../Fluido.h"
#include "../Inibidor.h"

class QPainter;
class QPaintEvent;

// Widget que desenha, via QPainter (sem depender do modulo Qt Charts),
// um painel com graficos visuais dos resultados da simulacao de corrosao:
//  - Perfil do fluido simulado (parametros de entrada fornecidos pelo usuario);
//  - Barras com os fatores de correcao da taxa de corrosao (Fs, Ft, Fc, Fq);
//  - Comparativo entre a taxa de corrosao base e a taxa final;
//  - Um "gauge" (medidor circular) com a eficiencia do inibidor selecionado;
//  - Um cartao de destaque com a dose recomendada.
class GraficoResultados : public QWidget
{
    Q_OBJECT

public:
    explicit GraficoResultados(QWidget* parent = nullptr);

    // Atualiza os dados exibidos com o resultado de uma simulacao e
    // solicita o redesenho do widget.
    void AtualizarDados(const Fluido& fluido,
                         const AnaliseCorrosao& analise,
                         const Inibidor& inibidor,
                         double fatorVazao,
                         double dose);

    // Volta ao estado inicial (antes de qualquer simulacao ser executada).
    void Limpar();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* evento) override;

private:
    struct Barra
    {
        QString rotulo;
        double valor;
        QString valorTexto;
        QColor cor;
    };

    bool temDados;

    // ---------- Perfil do fluido (parametros de entrada) ----------
    double valorPH;
    double valorCO2;
    double valorH2S;
    double valorSalinidade;
    double valorTemperatura;
    double valorPressao;
    double valorVazao;

    // ---------- Dados calculados da ultima simulacao ----------
    QString tipoCorrosao;
    QString severidade;
    double taxaCorrosaoBase;
    double taxaCorrosaoFinal;

    QVector<Barra> fatoresCorrecao;

    QString nomeInibidor;
    double eficienciaInibidor;

    double doseRecomendada;

    // ---------- Metodos auxiliares de desenho ----------
    QColor CorPorSeveridade(const QString& severidade) const;

    void DesenharTituloSecao(QPainter& pintor, const QRect& area, const QString& texto) const;

    void DesenharResumoFluido(QPainter& pintor, const QRect& area) const;

    void DesenharGraficoBarrasVerticais(QPainter& pintor,
                                         const QRect& area,
                                         const QVector<Barra>& barras) const;

    void DesenharEvolucaoTaxa(QPainter& pintor, const QRect& area) const;

    void DesenharComparativoTaxas(QPainter& pintor,
                                   const QRect& area,
                                   double taxaBase,
                                   double taxaFinal,
                                   const QColor& cor) const;

    void DesenharGaugeEficiencia(QPainter& pintor,
                                  const QRect& area,
                                  double percentual,
                                  const QString& rotulo) const;

    void DesenharCartaoDose(QPainter& pintor,
                             const QRect& area,
                             double dose) const;

    void DesenharMensagemVazia(QPainter& pintor) const;
};

#endif
