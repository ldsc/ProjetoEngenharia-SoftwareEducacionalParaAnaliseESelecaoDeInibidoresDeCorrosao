#include "MainWindow.h"

#include <memory>
#include <stdexcept>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFont>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnaliseCorrosao.h"
#include "CalculadoraDosagem.h"
#include "GraficoResultados.h"
#include "Inibidor.h"
#include "SimuladorCorrosao.h"

// ============================================================================
// Construcao da janela
// ============================================================================

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      indicePerguntaAtual(0),
      graficoResultados(nullptr)
{
    setWindowTitle("Software Educacional - Análise e Seleção de Inibidores de Corrosão");
    resize(1180, 780);
    setMinimumSize(980, 640);

    QWidget* central = new QWidget(this);
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(central);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);
    layoutPrincipal->setSpacing(0);

    layoutPrincipal->addWidget(CriarCabecalho());

    QTabWidget* abas = new QTabWidget(this);
    abas->addTab(CriarAbaFormulario(), "Formulário");
    abas->addTab(CriarAbaAmostrasPadrao(), "Amostras Padrão");
    abas->addTab(CriarAbaChat(), "Modo Perguntas");
    abas->addTab(CriarAbaGraficos(), "Gráficos");
    abas->addTab(CriarAbaMaterialApoio(), "Material de Apoio");
    layoutPrincipal->addWidget(abas, 1);

    setCentralWidget(central);

    perguntasChat = {
        "Qual o pH do fluido? (0 a 14; típico 4 a 8)",
        "Qual a concentração de CO2? (ppm; típico 0 a 1000)",
        "Qual a concentração de H2S? (ppm; típico 0 a 200)",
        "Qual a salinidade do fluido? (ppm; típico 1.000 a 250.000)",
        "Qual a temperatura do fluido? (°C; típico 20 a 150)",
        "Qual a pressão do sistema? (psi; típico 500 a 5.000)",
        "Qual a vazão do fluido? (m³/dia; típico 100 a 10.000)"
    };

    IniciarOuReiniciarChat();
}

QWidget* MainWindow::CriarCabecalho()
{
    QFrame* container = new QFrame();
    container->setObjectName("cabecalhoFrame");

    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(22, 16, 22, 16);
    layout->setSpacing(20);

    // ---------- Logo da UENF ----------
    QLabel* logoUenf = CarregarLogo("assets/uenf.png", "UENF");
    logoUenf->setAlignment(Qt::AlignCenter);

    // ---------- Titulo e subtitulo, centralizados ----------
    QVBoxLayout* colunaTitulo = new QVBoxLayout();
    colunaTitulo->setSpacing(4);

    QLabel* titulo = new QLabel("Software Educacional para Análise e\nSeleção de Inibidores de Corrosão");
    titulo->setObjectName("tituloLabel");
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setWordWrap(true);

    QLabel* subtitulo = new QLabel(
        "LENEP \u2022 Engenharia de Exploração e Produção de Petróleo \u2022 UENF\n"
        "Material de apoio à disciplina LEP01550 \u2013 Fundamentos de Corrosão aplicada à Indústria do Petróleo");
    subtitulo->setObjectName("subtituloLabel");
    subtitulo->setAlignment(Qt::AlignCenter);
    subtitulo->setWordWrap(true);

    colunaTitulo->addStretch();
    colunaTitulo->addWidget(titulo);
    colunaTitulo->addWidget(subtitulo);
    colunaTitulo->addStretch();

    layout->addWidget(logoUenf);
    layout->addLayout(colunaTitulo, 1);

    return container;
}

QLabel* MainWindow::CarregarLogo(const QString& caminhoArquivo, const QString& textoAlternativo) const
{
    QLabel* label = new QLabel();

    const QPixmap pixmap(caminhoArquivo);

    if (!pixmap.isNull())
    {
        label->setPixmap(pixmap.scaledToHeight(84, Qt::SmoothTransformation));
    }
    else
    {
        // Se a imagem ainda nao foi colocada em assets/, mostra um texto no lugar
        // para nao quebrar a interface. Basta colocar o arquivo .png/.jpg no caminho
        // indicado que o logo real passa a aparecer automaticamente.
        label->setText(textoAlternativo);
        label->setAlignment(Qt::AlignCenter);
        label->setMinimumSize(110, 70);
        label->setStyleSheet(
            "border: 1px solid rgba(255,255,255,120);"
            "border-radius: 8px;"
            "background-color: rgba(255,255,255,30);"
            "color: #ffffff;"
            "font-weight: bold;");
    }

    return label;
}

// ============================================================================
// Aba "Formulario"
// ============================================================================

QWidget* MainWindow::CriarAbaFormulario()
{
    QWidget* aba = new QWidget();
    QHBoxLayout* layoutAba = new QHBoxLayout(aba);
    layoutAba->setContentsMargins(0, 0, 0, 0);
    layoutAba->setSpacing(0);

    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->setChildrenCollapsible(false);

        // ---------- Painel esquerdo: dados de entrada ----------
    QWidget* painelEntrada = new QWidget();
    QVBoxLayout* layoutPainel = new QVBoxLayout(painelEntrada);
    layoutPainel->setContentsMargins(16, 16, 8, 16);
    layoutPainel->setSpacing(8);

    QLabel* instrucao = new QLabel("Propriedades do fluido:");
    instrucao->setStyleSheet("font-weight: 700; color: #0b3d63; font-size: 13px;");
    layoutPainel->addWidget(instrucao);

    // Os campos ficam dentro de uma area de rolagem: se a janela for baixa,
    // nada e espremido nem cortado, apenas aparece a barra de rolagem.
    QWidget* conteudoCampos = new QWidget();
    conteudoCampos->setObjectName("conteudoCampos");
    conteudoCampos->setStyleSheet("#conteudoCampos { background: transparent; }");

    QVBoxLayout* layoutEntrada = new QVBoxLayout(conteudoCampos);
    layoutEntrada->setContentsMargins(0, 0, 8, 0);
    layoutEntrada->setSpacing(2);

    // Cria um campo: rotulo, spinbox e um botao "+" que abre/fecha a orientacao
    // (faixa de referencia teorica) logo abaixo da caixa.
    auto criarCampo = [layoutEntrada](const QString& rotuloTexto,
                                      const QString& sufixoUnidade,
                                      double minimo,
                                      double maximo,
                                      double padrao,
                                      const QString& textoFaixa) {
        QLabel* rotulo = new QLabel(rotuloTexto);
        rotulo->setStyleSheet("font-weight: 600; color: #0b3d63; font-size: 12px;");

        QDoubleSpinBox* spin = new QDoubleSpinBox();
        spin->setRange(minimo, maximo);
        spin->setDecimals(2);
        spin->setValue(padrao);
        spin->setSuffix(sufixoUnidade);
        spin->setMinimumHeight(30);
        spin->setToolTip(textoFaixa);

        // Botao "+" / "-" que mostra ou esconde a orientacao
        QToolButton* botaoDica = new QToolButton();
        botaoDica->setText("+  Ver faixa de referência");
        botaoDica->setCheckable(true);
        botaoDica->setCursor(Qt::PointingHandCursor);
        botaoDica->setToolButtonStyle(Qt::ToolButtonTextOnly);
        botaoDica->setStyleSheet(
            "QToolButton { border: none; background: transparent; color: #14639e;"
            "              font-size: 11px; font-weight: 600; padding: 3px 0px; }"
            "QToolButton:hover { color: #0b3d63; text-decoration: underline; }");

        QLabel* faixa = new QLabel(textoFaixa);
        faixa->setWordWrap(true);
        faixa->setVisible(false);   // comeca recolhida
        faixa->setStyleSheet(
            "background-color: #e6f1fb;"
            "color: #14497a;"
            "font-size: 11px;"
            "padding: 6px 8px;"
            "border-radius: 6px;"
            "border: 1px solid #cfe0ee;");

        QObject::connect(botaoDica, &QToolButton::toggled, faixa,
                         [botaoDica, faixa](bool aberto) {
            faixa->setVisible(aberto);
            botaoDica->setText(aberto ? QString::fromUtf8("\u2212  Ocultar faixa de referência")
                                      : QString::fromUtf8("+  Ver faixa de referência"));
        });

        layoutEntrada->addWidget(rotulo);
        layoutEntrada->addWidget(spin);
        layoutEntrada->addWidget(botaoDica, 0, Qt::AlignLeft);
        layoutEntrada->addWidget(faixa);
        layoutEntrada->addSpacing(8);

        return spin;
    };

    spinPH = criarCampo("pH", "", 0.0, 14.0, 7.0,
                         "Faixa válida: 0 – 14  |  Típico em água de produção: 4 – 8");
    spinCO2 = criarCampo("CO2", " ppm", 0.0, 1000000.0, 0.0,
                         "Unidade: ppm  |  Típico: 0 – 1.000 (regime doce/sweet se > 20 ppm)");
    spinH2S = criarCampo("H2S", " ppm", 0.0, 1000000.0, 0.0,
                         "Unidade: ppm  |  Típico: 0 – 200 (regime ácido/sour se > 10 ppm, ref. NACE MR0175)");
    spinSalinidade = criarCampo("Salinidade", " ppm", 0.0, 1000000.0, 0.0,
                         "Unidade: ppm  |  Típico: 1.000 – 250.000 (água do mar \u2248 35.000 ppm)");
    spinTemperatura = criarCampo("Temperatura", " °C", -273.15, 1000.0, 25.0,
                         "Unidade: °C  |  Típico em reservatórios de petróleo: 20 – 150 °C");
    spinPressao = criarCampo("Pressão", " psi", 0.0, 1000000.0, 101.325,
                         "Unidade: psi  |  Típico em sistemas de produção: 500 – 5.000 psi");
    spinVazao = criarCampo("Vazão", " m³/dia", 0.0, 1000000.0, 0.0,
                         "Unidade: m³/dia  |  Típico em poços de produção: 100 – 10.000 m³/dia");

    layoutEntrada->addStretch(1);

    QScrollArea* rolagemCampos = new QScrollArea();
    rolagemCampos->setWidgetResizable(true);
    rolagemCampos->setFrameShape(QFrame::NoFrame);
    rolagemCampos->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    rolagemCampos->setStyleSheet("QScrollArea { background: transparent; }");
    rolagemCampos->setWidget(conteudoCampos);
    layoutPainel->addWidget(rolagemCampos, 1);

    // Botao Simular fica FORA da rolagem: sempre visivel embaixo
    QPushButton* botaoSimular = new QPushButton("Simular");
    botaoSimular->setCursor(Qt::PointingHandCursor);
    botaoSimular->setMinimumHeight(36);
    botaoSimular->setStyleSheet("font-weight: 600;");
    connect(botaoSimular, &QPushButton::clicked, this, &MainWindow::OnSimularFormulario);
    layoutPainel->addWidget(botaoSimular);

    painelEntrada->setMinimumWidth(270);
    painelEntrada->setMaximumWidth(340);

    // ---------- Painel direito: resultados (bem maior/legivel) ----------
    QWidget* painelResultado = new QWidget();
    QVBoxLayout* layoutResultado = new QVBoxLayout(painelResultado);
    layoutResultado->setContentsMargins(14, 16, 16, 16);
    layoutResultado->setSpacing(8);

    QLabel* rotuloResultado = new QLabel("Resultado da simulação:");
    rotuloResultado->setStyleSheet("font-weight: 700; color: #0b3d63; font-size: 14px;");
    layoutResultado->addWidget(rotuloResultado);

    areaResultadoFormulario = new QTextEdit();
    areaResultadoFormulario->setReadOnly(true);
    areaResultadoFormulario->setMinimumHeight(440);
    areaResultadoFormulario->setStyleSheet("QTextEdit { font-size: 13px; }");
    layoutResultado->addWidget(areaResultadoFormulario, 1);

    splitter->addWidget(painelEntrada);
    splitter->addWidget(painelResultado);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({290, 880});

    layoutAba->addWidget(splitter);

    return aba;
}

void MainWindow::OnSimularFormulario()
{
    try
    {
        const Fluido fluido = MontarFluido(
            spinPH->value(),
            spinCO2->value(),
            spinH2S->value(),
            spinSalinidade->value(),
            spinTemperatura->value(),
            spinPressao->value(),
            spinVazao->value());

        areaResultadoFormulario->setHtml(GerarRelatorioHtml(fluido));
        AtualizarPainelGraficos(fluido);
    }
    catch (const std::invalid_argument& erro)
    {
        QMessageBox::warning(this, "Dados inválidos", QString::fromStdString(erro.what()));
    }
}

// ============================================================================
// Aba "Amostras Padrao"
// ============================================================================

QWidget* MainWindow::CriarAbaAmostrasPadrao()
{
    QWidget* aba = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(aba);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(10);

    QLabel* instrucao = new QLabel(
        "Amostras padrão, típicas da indústria de óleo e gás, prontas para simular sem "
        "precisar digitar nenhum dado (o mesmo conjunto usado no modo console):");
    instrucao->setWordWrap(true);
    instrucao->setStyleSheet("font-weight: 600; color: #0b3d63;");
    layout->addWidget(instrucao);

    QHBoxLayout* linhaSelecao = new QHBoxLayout();
    linhaSelecao->setSpacing(10);

    comboAmostras = new QComboBox();
    comboAmostras->setMinimumHeight(30);

    const std::vector<Fluido> amostras = SimuladorCorrosao::AmostrasPadrao();

    // Lista de nomes exibidos exclusivamente na interface
    const std::vector<QString> nomesAmostras = {
        "Sour acentuado",
        "Sweet severa",
        "Sweet intensa / T alta",
        "Leve / baixo risco",
        "Leve / catodico (baixa)",
        "Leve / catodico (media)",
        "Extremo combinado"
    };

    for (std::size_t i = 0; i < amostras.size(); ++i)
    {
        QString nome = (i < nomesAmostras.size()) ? nomesAmostras[i] : "";

        comboAmostras->addItem(
            QString("Amostra %1 - %2 (pH %3 | CO2 %4 ppm | H2S %5 ppm)")
                .arg(static_cast<int>(i + 1))
                .arg(nome)
                .arg(amostras[i].PH(), 0, 'f', 2)
                .arg(amostras[i].CO2(), 0, 'f', 1)
                .arg(amostras[i].H2S(), 0, 'f', 1));
    }

    QPushButton* botaoSimularUma = new QPushButton("Simular amostra selecionada");
    botaoSimularUma->setCursor(Qt::PointingHandCursor);
    botaoSimularUma->setMinimumHeight(30);
    connect(botaoSimularUma, &QPushButton::clicked, this, &MainWindow::OnRodarAmostraSelecionada);

    QPushButton* botaoSimularTodas = new QPushButton("Simular todas as amostras padrão");
    botaoSimularTodas->setCursor(Qt::PointingHandCursor);
    botaoSimularTodas->setMinimumHeight(30);
    connect(botaoSimularTodas, &QPushButton::clicked, this, &MainWindow::OnRodarTodasAmostras);

    linhaSelecao->addWidget(comboAmostras, 1);
    linhaSelecao->addWidget(botaoSimularUma);
    linhaSelecao->addWidget(botaoSimularTodas);
    layout->addLayout(linhaSelecao);

    areaResultadoAmostras = new QTextEdit();
    areaResultadoAmostras->setReadOnly(true);
    areaResultadoAmostras->setMinimumHeight(440);
    areaResultadoAmostras->setStyleSheet("QTextEdit { font-size: 13px; }");
    layout->addWidget(areaResultadoAmostras, 1);

    return aba;
}

void MainWindow::OnRodarAmostraSelecionada()
{
    const std::vector<Fluido> amostras = SimuladorCorrosao::AmostrasPadrao();
    const int indice = comboAmostras->currentIndex();

    if (indice < 0 || indice >= static_cast<int>(amostras.size()))
        return;

    const Fluido& fluido = amostras[static_cast<std::size_t>(indice)];

    areaResultadoAmostras->setHtml(
        QString("<h2 style='color:#0b3d63;'>Amostra %1</h2>").arg(indice + 1) +
        GerarRelatorioHtml(fluido));

    AtualizarPainelGraficos(fluido);
}

void MainWindow::OnRodarTodasAmostras()
{
    const std::vector<Fluido> amostras = SimuladorCorrosao::AmostrasPadrao();

    areaResultadoAmostras->setHtml(GerarRelatorioAmostrasHtml(amostras));

    if (!amostras.empty())
    {
        const int indice = comboAmostras->currentIndex();
        const std::size_t indiceValido =
            (indice >= 0 && indice < static_cast<int>(amostras.size()))
                ? static_cast<std::size_t>(indice)
                : 0;

        // O painel de graficos mostra sempre um fluido por vez: ao rodar todas as
        // amostras, ele reflete a amostra atualmente selecionada no combo acima.
        AtualizarPainelGraficos(amostras[indiceValido]);
    }
}

// ============================================================================
// Aba "Modo Perguntas" (chat)
// ============================================================================

QWidget* MainWindow::CriarAbaChat()
{
    QWidget* aba = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(aba);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    areaChat = new QTextEdit();
    areaChat->setReadOnly(true);
    layout->addWidget(areaChat, 1);

    QHBoxLayout* linhaEntrada = new QHBoxLayout();
    linhaEntrada->setSpacing(10);

    campoRespostaChat = new QLineEdit();
    campoRespostaChat->setPlaceholderText("Digite um número e pressione Enter...");
    connect(campoRespostaChat, &QLineEdit::returnPressed, this, &MainWindow::OnEnviarRespostaChat);

    botaoEnviarChat = new QPushButton("Enviar");
    botaoEnviarChat->setCursor(Qt::PointingHandCursor);

    connect(botaoEnviarChat, &QPushButton::clicked, this, &MainWindow::OnEnviarRespostaChat);

    linhaEntrada->addWidget(campoRespostaChat, 1);
    linhaEntrada->addWidget(botaoEnviarChat);
    layout->addLayout(linhaEntrada);

    botaoReiniciarChat = new QPushButton("Nova simulação");
    botaoReiniciarChat->setObjectName("botaoReiniciarChat");
    botaoReiniciarChat->setCursor(Qt::PointingHandCursor);
    connect(botaoReiniciarChat, &QPushButton::clicked, this, &MainWindow::OnReiniciarChat);
    layout->addWidget(botaoReiniciarChat);

    return aba;
}

void MainWindow::IniciarOuReiniciarChat()
{
    respostasChat.clear();
    indicePerguntaAtual = 0;

    areaChat->clear();
    campoRespostaChat->clear();
    campoRespostaChat->setEnabled(true);
    botaoEnviarChat->setEnabled(true);

    ExibirMensagemSoftware(
        "Olá! Vou te fazer algumas perguntas sobre o fluido para simular a corrosão "
        "e sugerir um inibidor.");
    ExibirMensagemSoftware(perguntasChat[indicePerguntaAtual]);
}

void MainWindow::OnReiniciarChat()
{
    IniciarOuReiniciarChat();
}

// ============================================================================
// Aba "Graficos"
// ============================================================================

QWidget* MainWindow::CriarAbaGraficos()
{
    QWidget* conteudo = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(conteudo);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(12);

    explicacaoGraficos = new QLabel();
    explicacaoGraficos->setWordWrap(true);
    explicacaoGraficos->setTextFormat(Qt::RichText);
    explicacaoGraficos->setStyleSheet(
        "background-color: #e6f1fb;"
        "color: #16324f;"
        "padding: 12px 16px;"
        "border-radius: 10px;"
        "font-size: 12px;");
    explicacaoGraficos->setText(
        "<b>Explicação didática</b><br>"
        "Rode uma simulação (Formulário, Amostras Padrão ou Modo Perguntas) para ver aqui "
        "uma explicação do tipo de corrosão identificado e do inibidor selecionado.");
    layout->addWidget(explicacaoGraficos);

    graficoResultados = new GraficoResultados();
    layout->addWidget(graficoResultados);

    QScrollArea* areaRolagem = new QScrollArea();
    areaRolagem->setWidgetResizable(true);
    areaRolagem->setFrameShape(QFrame::NoFrame);
    areaRolagem->setWidget(conteudo);

    return areaRolagem;
}

// ============================================================================
// Aba "Material de Apoio"
// ============================================================================

QWidget* MainWindow::CriarAbaMaterialApoio()
{
    const QString estiloCartaoClaro =
        "<table width='100%' cellpadding='14' cellspacing='0' style='margin:10px 0;'>"
        "<tr><td style='background-color:#f4f8fc; color:#1c2b39; border-radius:10px;'>%1</td></tr>"
        "</table>";

    QString html;

    html += "<h2 style='color:#0b3d63;'>Material de Apoio \u2014 Fundamentos Teóricos</h2>";
    html += "<p style='color:#3c4f61;'>Resumo das fórmulas e dos conceitos usados pelo simulador, "
            "como referência rápida para a disciplina "
            "<b>LEP01550 \u2013 Fundamentos de Corrosão aplicada à Indústria do Petróleo</b>.</p>";

    // ---------- 1. Formulas do modelo ----------
    html += QString(estiloCartaoClaro).arg(
        "<b style='color:#0b3d63; font-size:14px;'>1. Fórmulas do modelo</b><br><br>"
        "<b>Fator de acidez:</b> FatorAcidez = máx(0; 7 \u2212 pH)<br><br>"
        "<b>Taxa base de corrosão:</b><br>"
        "Taxa_base = CO2 \u00d7 e<sup>0,02 \u00d7 T</sup> \u00d7 FatorAcidez<br>"
        "<span style='font-size:11px; color:#5c7387;'>"
        "(CO2 em ppm; T = temperatura em °C. Se pH \u2265 7, FatorAcidez = 0 e a taxa base "
        "também é zero \u2014 o modelo assume corrosão relevante apenas em meio ácido.)</span><br><br>"
        "<b>Fator de severidade (Fs)</b> \u2014 conforme a taxa base:<br>"
        "&nbsp;&nbsp;\u2022 Taxa &lt; 10 \u2192 Severidade Baixa \u2192 Fs = 1,0<br>"
        "&nbsp;&nbsp;\u2022 10 \u2264 Taxa &lt; 50 \u2192 Severidade Média \u2192 Fs = 1,5<br>"
        "&nbsp;&nbsp;\u2022 Taxa \u2265 50 \u2192 Severidade Alta \u2192 Fs = 2,5<br><br>"
        "<b>Fator térmico (Ft)</b> \u2014 conforme a temperatura:<br>"
        "&nbsp;&nbsp;\u2022 T &lt; 40 °C \u2192 Ft = 1,0<br>"
        "&nbsp;&nbsp;\u2022 40 °C \u2264 T \u2264 80 °C \u2192 Ft = 1,3<br>"
        "&nbsp;&nbsp;\u2022 T &gt; 80 °C \u2192 Ft = 1,8<br><br>"
        "<b>Fator corrosivo (Fc):</b><br>"
        "&nbsp;&nbsp;\u2022 H2S &gt; 10 ppm (sour) \u2192 Fc = 2,0<br>"
        "&nbsp;&nbsp;\u2022 CO2 &gt; 20 ppm, sem H2S relevante (sweet) \u2192 Fc = 1,5<br>"
        "&nbsp;&nbsp;\u2022 Caso contrário \u2192 Fc = 1,0<br><br>"
        "<b>Taxa final de corrosão:</b><br>"
        "Taxa_final = Taxa_base \u00d7 Fs \u00d7 Ft \u00d7 Fc &nbsp;(mm/ano)<br><br>"
        "<b>Fator de vazão (Fq):</b><br>"
        "Fq = 1 + (Vazão / 1000) &nbsp;<span style='color:#5c7387;'>(Vazão em m³/dia)</span><br><br>"
        "<b>Dose recomendada de inibidor:</b><br>"
        "Dose = (K \u00d7 Taxa_base \u00d7 Fs \u00d7 Ft \u00d7 Fc \u00d7 Fq) / Eficiência<br>"
        "<span style='font-size:11px; color:#5c7387;'>"
        "(K = 0,1 é a constante empírica do modelo; Eficiência é a eficiência do inibidor, entre 0 e 1.)"
        "</span>");

    // ---------- 2. Tipos de corrosao ----------
    html += QString(estiloCartaoClaro).arg(
        "<b style='color:#0b3d63; font-size:14px;'>2. Tipos de corrosão considerados</b><br><br>"
        "<b>Corrosão Ácida (Sour)</b> \u2014 H2S acima de 10 ppm.<br>"
        "O H2S dissolvido forma ácido sulfídrico e pode gerar sulfeto de ferro (FeS) na superfície "
        "do aço, além de favorecer a fragilização por hidrogênio (HIC/SSC) em aços mais duros. "
        "Ambientes sour costumam seguir critérios de normas como a NACE MR0175 / ISO 15156.<br><br>"
        "<b>Corrosão Doce (Sweet)</b> \u2014 CO2 acima de 20 ppm, sem H2S relevante.<br>"
        "O CO2 dissolvido forma ácido carbônico (H2CO3), que ataca o aço-carbono e pode causar "
        "corrosão localizada severa (como a chamada \"mesa corrosion\"). É o mecanismo descrito "
        "pelos modelos clássicos de De Waard &amp; Milliams e pela norma NORSOK M-506.<br><br>"
        "<b>Leve</b> \u2014 CO2 e H2S abaixo dos limiares acima.<br>"
        "A corrosão tende a ser mais branda, dominada por fatores gerais (salinidade, temperatura, "
        "oxigênio dissolvido) em vez de um mecanismo ácido específico.");

    // ---------- 3. Tipos de inibidor ----------
    html += QString(estiloCartaoClaro).arg(
        "<b style='color:#0b3d63; font-size:14px;'>3. Tipos de inibidor e critério de seleção</b><br><br>"
        "<b>Inibidor para H2S</b> (eficiência 93%) \u2014 selecionado quando H2S &gt; 10 ppm.<br>"
        "Combina ação filmogênica com sequestro de H2S, reduzindo a disponibilidade de H2S livre "
        "em solução e o risco de fragilização por hidrogênio.<br><br>"
        "<b>Inibidor Orgânico</b> (eficiência 85%) \u2014 selecionado quando CO2 &gt; 20 ppm e pH &lt; 6.<br>"
        "Filmogênico (\"film-forming\"): se adsorve na superfície metálica formando uma película "
        "protetora que separa o metal do fluido corrosivo.<br><br>"
        "<b>Inibidor Catódico</b> (eficiência 80%) \u2014 selecionado quando salinidade &gt; 15.000 ppm.<br>"
        "Atua sobre a reação catódica, formando precipitados/filmes que bloqueiam os sítios "
        "catódicos da superfície \u2014 útil quando a alta condutividade do eletrólito favorece a corrosão.<br><br>"
        "<b>Inibidor Geral</b> (eficiência 70%) \u2014 usado quando nenhum critério acima se aplica.<br>"
        "Proteção básica de amplo espectro, com eficiência menor que os inibidores especializados.");

    // ---------- 4. Referencias ----------
    html += QString(estiloCartaoClaro).arg(
        "<b style='color:#0b3d63; font-size:14px;'>4. Referências de apoio</b><br><br>"
        "\u2022 NACE MR0175 / ISO 15156 \u2014 materiais resistentes a trincamento em ambientes "
        "sour (H2S).<br>"
        "\u2022 NORSOK M-506 \u2014 modelo de predição de taxa de corrosão por CO2.<br>"
        "\u2022 Modelo de De Waard &amp; Milliams \u2014 modelo semi-empírico clássico de corrosão "
        "por CO2.<br><br>"
        "<span style='font-size:11px; color:#5c7387;'>As fórmulas deste software são uma "
        "simplificação didática para fins de ensino, inspirada nesses conceitos \u2014 não "
        "substituem os modelos completos das normas para uso em projeto real. Consulte o "
        "material da disciplina para o equacionamento completo.</span>");

    QLabel* label = new QLabel();
    label->setTextFormat(Qt::RichText);
    label->setWordWrap(true);
    label->setContentsMargins(20, 18, 20, 18);
    label->setText(html);

    QScrollArea* areaRolagem = new QScrollArea();
    areaRolagem->setWidgetResizable(true);
    areaRolagem->setFrameShape(QFrame::NoFrame);
    areaRolagem->setWidget(label);

    return areaRolagem;
}

void MainWindow::OnEnviarRespostaChat()
{
    const QString textoDigitado = campoRespostaChat->text().trimmed();

    if (textoDigitado.isEmpty())
        return;

    // Aceita tanto "6.5" quanto "6,5" como separador decimal.
    QString textoNormalizado = textoDigitado;
    textoNormalizado.replace(',', '.');

    bool conversaoOk = false;
    const double valor = textoNormalizado.toDouble(&conversaoOk);

    if (!conversaoOk)
    {
        ExibirMensagemSoftware("Isso não parece um número válido. Tente novamente (ex: 6.5).");
        campoRespostaChat->clear();
        return;
    }

    ExibirMensagemUsuario(textoDigitado);
    campoRespostaChat->clear();

    respostasChat.append(valor);
    indicePerguntaAtual++;

    if (indicePerguntaAtual < perguntasChat.size())
    {
        ExibirMensagemSoftware(perguntasChat[indicePerguntaAtual]);
        return;
    }

    // Todas as 7 respostas foram coletadas: pH, CO2, H2S, salinidade, temperatura, pressao, vazao.
    try
    {
        const Fluido fluido = MontarFluido(
            respostasChat[0], respostasChat[1], respostasChat[2],
            respostasChat[3], respostasChat[4], respostasChat[5], respostasChat[6]);

        ExibirMensagemSoftware(GerarRelatorioHtml(fluido));
        AtualizarPainelGraficos(fluido);
        ExibirMensagemSoftware("Simulação concluída! Clique em 'Nova simulação' para testar outro fluido.");

        campoRespostaChat->setEnabled(false);
        botaoEnviarChat->setEnabled(false);
    }
    catch (const std::invalid_argument& erro)
    {
        ExibirMensagemSoftware(
            QString("Dados inválidos: %1<br>Vamos recomeçar do zero.").arg(QString::fromStdString(erro.what())));
        IniciarOuReiniciarChat();
    }
}

void MainWindow::ExibirMensagemSoftware(const QString& texto)
{
    areaChat->append(QString(
        "<table width='100%' cellspacing='0' style='margin:6px 0;'><tr>"
        "<td align='left' width='80%'>"
        "<table cellpadding='8' style='background-color:#e6f1fb; border-radius:10px;'>"
        "<tr><td style='color:#0b3d63;'>%1</td></tr></table>"
        "</td>"
        "<td width='20%'></td>"
        "</tr></table>").arg(texto));
}

void MainWindow::ExibirMensagemUsuario(const QString& texto)
{
    areaChat->append(QString(
        "<table width='100%' cellspacing='0' style='margin:6px 0;'><tr>"
        "<td width='20%'></td>"
        "<td align='right' width='80%'>"
        "<table cellpadding='8' align='right' style='background-color:#14639e; border-radius:10px;'>"
        "<tr><td style='color:#ffffff;'>%1</td></tr></table>"
        "</td>"
        "</tr></table>").arg(texto));
}

// ============================================================================
// Logica compartilhada (formulario, amostras padrao e chat levam ao mesmo lugar)
// ============================================================================

Fluido MainWindow::MontarFluido(double pH,
                                 double co2,
                                 double h2s,
                                 double salinidade,
                                 double temperatura,
                                 double pressao,
                                 double vazao)
{
    // O construtor de Fluido ja valida os dados e lanca
    // std::invalid_argument se algo estiver fora do esperado.
    return Fluido(pH, co2, h2s, salinidade, temperatura, pressao, vazao);
}

QString MainWindow::GerarRelatorioHtml(const Fluido& fluido)
{
    const AnaliseCorrosao analise(fluido);
    const std::unique_ptr<Inibidor> inibidor = SeletorInibidor::Selecionar(fluido);
    const double fatorVazao = CalculadoraDosagem::FatorDeVazao(fluido);
    const double dose = CalculadoraDosagem::CalcularDose(fluido, analise, *inibidor);

    const QString severidade = QString::fromStdString(analise.Severidade());
    QString corSeveridade = "#1e88e5";
    if (severidade == "Baixa")
        corSeveridade = "#2e7d32";
    else if (severidade == "Média")
        corSeveridade = "#f9a825";
    else if (severidade == "Alta")
        corSeveridade = "#c62828";

    // Monta um "cartao" usando uma <table>, nao uma <div>: o QTextEdit (motor de
    // texto rico do Qt) so aplica cor de fundo e cor de texto de forma confiavel
    // quando ambas estao no MESMO elemento (uma <td>). Com <div>, o texto que vem
    // depois do titulo acaba "escapando" do bloco e a cor de fundo/texto se perde
    // (foi o que deixava a secao de dosagem em branco).
    auto cartao = [](const QString& corFundo, const QString& corTexto, const QString& conteudoHtml) {
        return QString(
                   "<table width='100%' cellpadding='12' cellspacing='0' "
                   "style='margin:12px 0;'>"
                   "<tr><td style='background-color:%1; color:%2; border-radius:10px;'>"
                   "%3"
                   "</td></tr></table>")
            .arg(corFundo, corTexto, conteudoHtml);
    };

    QString html;

    html += cartao("#f4f8fc", "#1c2b39",
                QString(
                    "<b style='color:#0b3d63; font-size:13px;'>1. Propriedades do fluido</b><br><br>"
                    "pH: <b>%1</b><br>"
                    "CO2: <b>%2</b> ppm<br>"
                    "H2S: <b>%3</b> ppm<br>"
                    "Salinidade: <b>%4</b> ppm<br>"
                    "Temperatura: <b>%5</b> °C<br>"
                    "Pressão: <b>%6</b> psi<br>"
                    "Vazão: <b>%7</b> m³/dia")
                    .arg(fluido.PH(), 0, 'f', 2)
                    .arg(fluido.CO2(), 0, 'f', 2)
                    .arg(fluido.H2S(), 0, 'f', 2)
                    .arg(fluido.Salinidade(), 0, 'f', 2)
                    .arg(fluido.Temperatura(), 0, 'f', 2)
                    .arg(fluido.Pressao(), 0, 'f', 2)
                    .arg(fluido.Vazao(), 0, 'f', 2));

    html += cartao("#f4f8fc", "#1c2b39",
                QString(
                    "<b style='color:#0b3d63; font-size:13px;'>2. Análise de corrosão</b><br><br>"
                    "Tipo de corrosão: <b>%1</b><br>"
                    "Severidade: <span style='color:%2; font-weight:700;'>%3</span><br>"
                    "Taxa base de corrosão: %4<br>"
                    "Fator de severidade (Fs): %5<br>"
                    "Fator térmico (Ft): %6<br>"
                    "Fator corrosivo (Fc): %7<br>"
                    "<b>Taxa final de corrosão: %8 mm/ano</b>")
                    .arg(QString::fromStdString(analise.TipoCorrosao()))
                    .arg(corSeveridade)
                    .arg(severidade)
                    .arg(analise.TaxaCorrosao(), 0, 'f', 2)
                    .arg(analise.FatorSeveridade(), 0, 'f', 2)
                    .arg(analise.FatorCorrecaoTemperatura(), 0, 'f', 2)
                    .arg(analise.FatorCorrosivo(), 0, 'f', 2)
                    .arg(analise.TaxaCorrosaoFinal(), 0, 'f', 2));

    html += cartao("#f4f8fc", "#1c2b39",
                QString(
                    "<b style='color:#0b3d63; font-size:13px;'>3. Inibidor selecionado</b><br><br>"
                    "Nome: <b>%1</b><br>"
                    "Tipo: %2<br>"
                    "Eficiência: <b>%3 %</b><br>"
                    "Descrição: %4")
                    .arg(QString::fromStdString(inibidor->Nome()))
                    .arg(QString::fromStdString(inibidor->Tipo()))
                    .arg(inibidor->Eficiencia() * 100.0, 0, 'f', 2)
                    .arg(QString::fromStdString(inibidor->Descricao())));

    html += cartao("#0b3d63", "#ffffff",
                QString(
                    "<b style='font-size:13px;'>4. Dosagem recomendada</b><br><br>"
                    "Fator de vazão (Fq): %1<br>"
                    "<span style='font-size:18px; font-weight:700;'>Dose recomendada: %2 ppm</span>")
                    .arg(fatorVazao, 0, 'f', 2)
                    .arg(dose, 0, 'f', 2));

    return html;
}

QString MainWindow::GerarRelatorioAmostrasHtml(const std::vector<Fluido>& amostras)
{
    QString html;

    for (std::size_t i = 0; i < amostras.size(); ++i)
    {
        html += QString(
                    "<table width='100%' cellpadding='10' cellspacing='0' style='margin:12px 0 4px 0;'>"
                    "<tr><td style='background-color:#14639e; color:#ffffff; border-radius:8px; "
                    "font-size:14px; font-weight:700;'>Amostra %1</td></tr></table>")
                    .arg(static_cast<int>(i + 1));
        html += GerarRelatorioHtml(amostras[i]);
        html += "<hr style='border:none; border-top:1px dashed #cddceb; margin:18px 0;'>";
    }

    return html;
}

namespace
{

// Pequenas explicacoes didaticas, usadas na aba "Graficos" para contextualizar
// o tipo de corrosao identificado e o inibidor selecionado na ultima simulacao.

QString DescricaoTipoCorrosao(const QString& tipo)
{
    if (tipo == "Corrosão Ácida (Sour)")
        return "Ocorre quando o H2S dissolvido é significativo (acima de 10 ppm, neste modelo). "
               "O H2S forma ácido sulfídrico em solução e pode gerar sulfeto de ferro (FeS) na "
               "superfície do aço, além de favorecer a fragilização por hidrogênio (HIC/SSC) em "
               "aços mais duros \u2014 por isso ambientes sour costumam seguir critérios da norma "
               "NACE MR0175 / ISO 15156.";

    if (tipo == "Corrosão Doce (Sweet)")
        return "Ocorre quando há CO2 dissolvido em quantidade relevante (acima de 20 ppm, neste "
               "modelo) sem H2S significativo. O CO2 forma ácido carbônico (H2CO3), que ataca o "
               "aço-carbono e pode gerar corrosão localizada severa (como a \"mesa corrosion\"). "
               "É o mecanismo descrito pelos modelos clássicos de De Waard &amp; Milliams e pela "
               "norma NORSOK M-506.";

    return "As concentrações de CO2 e H2S estão abaixo dos limiares considerados críticos neste "
           "modelo. A corrosão tende a ser mais branda, dominada por fatores gerais (salinidade, "
           "temperatura, oxigênio dissolvido) em vez de um mecanismo ácido específico.";
}

QString DescricaoTipoInibidor(const QString& tipo)
{
    if (tipo == "H2S")
        return "Inibidores para ambientes sour costumam combinar ação filmogênica com sequestro "
               "de H2S, reduzindo a disponibilidade de H2S livre em solução e o risco de "
               "fragilização por hidrogênio.";

    if (tipo == "Orgânico")
        return "Inibidores orgânicos filmogênicos (\"film-forming\") se adsorvem na superfície "
               "metálica formando uma película protetora que separa o metal do fluido corrosivo.";

    if (tipo == "Catódico")
        return "Atuam principalmente sobre a reação catódica, formando precipitados/filmes que "
               "bloqueiam os sítios catódicos da superfície \u2014 abordagem comum em ambientes de "
               "alta salinidade, onde a condutividade do eletrólito favorece a corrosão.";

    return "Inibidor de uso geral, indicado quando nenhum mecanismo corrosivo específico "
           "predomina; oferece proteção básica de amplo espectro, com eficiência tipicamente "
           "menor que a de inibidores especializados.";
}

} // namespace

void MainWindow::AtualizarPainelGraficos(const Fluido& fluido)
{
    const AnaliseCorrosao analise(fluido);
    const std::unique_ptr<Inibidor> inibidor = SeletorInibidor::Selecionar(fluido);
    const double fatorVazao = CalculadoraDosagem::FatorDeVazao(fluido);
    const double dose = CalculadoraDosagem::CalcularDose(fluido, analise, *inibidor);

    graficoResultados->AtualizarDados(fluido, analise, *inibidor, fatorVazao, dose);

    const QString tipoCorrosao = QString::fromStdString(analise.TipoCorrosao());
    const QString tipoInibidor = QString::fromStdString(inibidor->Tipo());

    explicacaoGraficos->setText(
        QString(
            "<b>Tipo de corrosão identificado: %1</b><br>%2<br><br>"
            "<b>Inibidor selecionado: %3 (tipo %4)</b><br>%5")
            .arg(tipoCorrosao)
            .arg(DescricaoTipoCorrosao(tipoCorrosao))
            .arg(QString::fromStdString(inibidor->Nome()))
            .arg(tipoInibidor)
            .arg(DescricaoTipoInibidor(tipoInibidor)));
}
namespace {

struct AmostrasPadrao
{
    QString nome;
    double pH, co2, h2s, salinidade, temperatura, pressao, vazao;
    QString comentario;
};

const QVector<AmostrasPadrao>& ObterAmostrasPadrao()
{
    static const QVector<AmostrasPadrao> cenarios = {
        { "1 - Sour acentuado", 5.50, 60.0, 25.0, 80000.0, 65.0, 4000.0, 5000.0,
          "Testa o ramo do Inibidor para H2S (H2S > 10 ppm). Ambiente acido e sour, "
          "severidade Alta, dose de inibidor significativa." },

        { "2 - Sweet severa", 5.20, 40.0, 5.0, 120000.0, 75.0, 3800.0, 8000.0,
          "Testa o Inibidor Organico (CO2 > 20 ppm e pH < 6). Mesmo com salinidade alta, "
          "a condicao de CO2/pH e avaliada primeiro na arvore de decisao." },

        { "3 - Sweet intensa, T alta", 5.80, 120.0, 3.0, 20000.0, 95.0, 5000.0, 4000.0,
          "Testa o limite superior de temperatura (T > 80C, Ft = 1.8) combinado com CO2 "
          "no topo da faixa tipica. Gera a maior dose entre os cenarios nao extremos." },

        { "4 - Leve, baixo risco", 6.80, 5.0, 2.0, 5000.0, 35.0, 2000.0, 1000.0,
          "Caso trivial: todos os parametros brandos. Deve cair no Inibidor Geral com a "
          "menor dose calculada, comprovando que o software nao superdimensiona." },

        { "5 - Leve, catodico (baixa)", 6.90, 8.0, 1.0, 180000.0, 55.0, 3000.0, 3000.0,
          "Isola a condicao de salinidade alta (>15000 ppm) como unico gatilho do "
          "Inibidor Catodico, com severidade Baixa." },

        { "6 - Leve, catodico (media)", 6.50, 10.0, 2.0, 50000.0, 45.0, 2500.0, 2500.0,
          "Mesmo ramo do cenario 5 (Inibidor Catodico), mas com severidade Media, para "
          "comparar o efeito do fator de severidade (Fs) na dose final." },

        { "7 - Extremo combinado", 4.80, 90.0, 40.0, 150000.0, 110.0, 8000.0, 9000.0,
          "Teste de estresse: satisfaz as tres condicoes de selecao de inibidor ao mesmo "
          "tempo e usa os limites superiores de T, P e vazao. Deve prevalecer o Inibidor "
          "para H2S (primeira condicao da arvore de decisao)." },
    };
    return cenarios;
}

} // namespace