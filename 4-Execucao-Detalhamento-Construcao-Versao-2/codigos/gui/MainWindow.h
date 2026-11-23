#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include <QMainWindow>
#include <QVector>
#include <QString>

#include "Fluido.h"

class QLabel;
class QDoubleSpinBox;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QComboBox;
class GraficoResultados;
class AnaliseCorrosao;
class Inibidor;

// Janela principal do software educacional.
// Possui tres formas de entrada de dados, que levam ao mesmo calculo:
//  - Aba "Formulario": caixinhas (QDoubleSpinBox) para cada propriedade do fluido.
//  - Aba "Amostras Padrao": conjunto de fluidos pre-definidos (como no console).
//  - Aba "Modo Perguntas": um fluxo estilo chat, uma pergunta por vez.
// Os resultados de qualquer uma delas tambem atualizam a aba "Graficos".
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void OnSimularFormulario();
    void OnEnviarRespostaChat();
    void OnReiniciarChat();
    void OnRodarAmostraSelecionada();
    void OnRodarTodasAmostras();

private:
    // ---------- Construcao da interface ----------
    QWidget* CriarCabecalho();
    QWidget* CriarAbaFormulario();
    QWidget* CriarAbaAmostrasPadrao();
    QWidget* CriarAbaChat();
    QWidget* CriarAbaGraficos();
    QWidget* CriarAbaMaterialApoio();

    QLabel* CarregarLogo(const QString& caminhoArquivo, const QString& textoAlternativo) const;

    // ---------- Logica compartilhada ----------
    // Tenta criar um Fluido a partir dos 7 valores informados.
    // Lanca std::invalid_argument se algum valor for invalido (mesma validacao do console).
    static Fluido MontarFluido(double pH,
                                double co2,
                                double h2s,
                                double salinidade,
                                double temperatura,
                                double pressao,
                                double vazao);

    // Roda AnaliseCorrosao + SeletorInibidor + CalculadoraDosagem e monta um relatorio em HTML
    // (com estilo visual: cartoes, cores por severidade etc).
    static QString GerarRelatorioHtml(const Fluido& fluido);

    // Concatena o relatorio HTML de varias amostras, uma apos a outra (usado na aba
    // "Amostras Padrao" ao simular todas de uma vez).
    static QString GerarRelatorioAmostrasHtml(const std::vector<Fluido>& amostras);

    // Roda a mesma simulacao (AnaliseCorrosao + SeletorInibidor + CalculadoraDosagem)
    // e envia os resultados para o painel de graficos ser redesenhado.
    void AtualizarPainelGraficos(const Fluido& fluido);

    // ---------- Widgets do formulario ----------
    QDoubleSpinBox* spinPH;
    QDoubleSpinBox* spinCO2;
    QDoubleSpinBox* spinH2S;
    QDoubleSpinBox* spinSalinidade;
    QDoubleSpinBox* spinTemperatura;
    QDoubleSpinBox* spinPressao;
    QDoubleSpinBox* spinVazao;
    QTextEdit* areaResultadoFormulario;

    // ---------- Widgets da aba "Amostras Padrao" ----------
    QComboBox* comboAmostras;
    QTextEdit* descricaoCenario;
    QTextEdit* areaResultadoAmostras;


    // ---------- Widgets do modo chat ----------
    QTextEdit* areaChat;
    QLineEdit* campoRespostaChat;
    QPushButton* botaoEnviarChat;
    QPushButton* botaoReiniciarChat;

    // ---------- Estado do modo chat ----------
    QVector<QString> perguntasChat;
    QVector<double> respostasChat;
    int indicePerguntaAtual;

    // ---------- Widget da aba "Graficos" ----------
    GraficoResultados* graficoResultados;
    QLabel* explicacaoGraficos;

    void ExibirMensagemSoftware(const QString& texto);
    void ExibirMensagemUsuario(const QString& texto);
    void IniciarOuReiniciarChat();
};

#endif
