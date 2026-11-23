#ifndef ESTILO_H
#define ESTILO_H

#include <QString>

// Folha de estilos (QSS) com tema azul, usada em toda a aplicacao.
// Mantida em um unico lugar para facilitar ajustes de cor/identidade visual
// sem precisar mexer na logica das telas.
inline QString ObterFolhaDeEstilos()
{
    return QStringLiteral(R"(
QWidget {
    color: #1c2b39;
    font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
    font-size: 10.5pt;
}

QMainWindow {
    background-color: #eef3f8;
}

/* ---------- Cabecalho ---------- */

QFrame#cabecalhoFrame {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                       stop:0 #0b3d63, stop:1 #14639e);
    border-radius: 12px;
}

QLabel#tituloLabel {
    color: #ffffff;
    font-size: 17pt;
    font-weight: 700;
}

QLabel#subtituloLabel {
    color: #cfe3f5;
    font-size: 9.5pt;
}

QLabel#legendaLogoLabel {
    color: #eaf3fb;
    font-size: 8pt;
    font-weight: 600;
}

/* ---------- Abas ---------- */

QTabWidget::pane {
    border: 1px solid #cddceb;
    border-radius: 10px;
    background-color: #ffffff;
    top: -1px;
}

QTabBar::tab {
    background: #dce8f5;
    color: #0b3d63;
    padding: 9px 20px;
    margin-right: 3px;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
    font-weight: 600;
}

QTabBar::tab:selected {
    background: #0b3d63;
    color: #ffffff;
}

QTabBar::tab:hover {
    background: #14639e;
    color: #ffffff;
}

/* ---------- Formulario ---------- */

QLabel#rotuloFormulario {
    font-weight: 600;
    color: #0b3d63;
    padding-top: 4px;
}

QDoubleSpinBox, QLineEdit {
    background-color: #ffffff;
    border: 1px solid #c3d3e3;
    border-radius: 6px;
    padding: 5px 8px;
    min-height: 22px;
    selection-background-color: #14639e;
}

QDoubleSpinBox:focus, QLineEdit:focus {
    border: 1px solid #14639e;
}

/* ---------- Botoes ---------- */

QPushButton {
    background-color: #14639e;
    color: #ffffff;
    border: none;
    border-radius: 8px;
    padding: 9px 20px;
    font-weight: 600;
}

QPushButton:hover {
    background-color: #1a78bf;
}

QPushButton:pressed {
    background-color: #0b3d63;
}

QPushButton:disabled {
    background-color: #a9bccf;
    color: #eef3f8;
}

QPushButton#botaoReiniciarChat {
    background-color: #4a7fa8;
}

QPushButton#botaoReiniciarChat:hover {
    background-color: #5c94c0;
}

/* ---------- Areas de texto / chat ---------- */

QTextEdit {
    background-color: #ffffff;
    border: 1px solid #c3d3e3;
    border-radius: 10px;
    padding: 10px;
}

QScrollBar:vertical {
    background: #eef3f8;
    width: 10px;
    margin: 0px;
}

QScrollBar::handle:vertical {
    background: #a9c2d9;
    border-radius: 5px;
    min-height: 24px;
}

QScrollBar::handle:vertical:hover {
    background: #7fa7c9;
}

QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0px;
}
)");
}

#endif
