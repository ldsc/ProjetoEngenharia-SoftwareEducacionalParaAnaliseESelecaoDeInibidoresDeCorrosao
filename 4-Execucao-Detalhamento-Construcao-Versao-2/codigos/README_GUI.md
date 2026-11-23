# Interface grafica (Qt) - como compilar no VS Code

O projeto agora tem **dois executáveis**:

- `SoftwareDeCorrosao` — o programa de console com menu (já existia).
- `SoftwareCorrosaoGUI` — a interface gráfica (Qt Widgets), material de apoio à
  disciplina **LEP01545 – Fundamentos de Corrosão aplicada à Indústria do
  Petróleo** (LENEP/UENF), com as abas:
  - **Formulário**: preenche as propriedades do fluido manualmente (com a
    faixa de referência teórica e a unidade de cada parâmetro à mostra).
  - **Amostras Padrão**: roda fluidos pré-definidos, sem precisar digitar
    nada (o mesmo conjunto do menu de console).
  - **Modo Perguntas**: fluxo estilo chat, uma pergunta por vez.
  - **Gráficos**: visualização gráfica dos resultados (perfil do fluido,
    fatores de correção, taxa de corrosão, eficiência do inibidor e dose).

O `CMakeLists.txt` so tenta compilar a GUI se encontrar o Qt instalado; caso
contrario, ele avisa no log do CMake e segue compilando normalmente so o
executavel de console.

## 1. Instalar o Qt

Escolha uma opcao (qualquer uma serve, Qt6 ou Qt5):

- **Windows**: baixe o instalador em https://www.qt.io/download-qt-installer
  e marque o componente "Qt Widgets" de alguma versao (ex: Qt 6.7.0, MinGW).
- **Ubuntu/Debian**: `sudo apt install qt6-base-dev` (ou `qtbase5-dev` para Qt5)
- **macOS**: `brew install qt`

## 2. Compilar pelo VS Code

Com a extensao **CMake Tools** instalada:

1. Abra a pasta do projeto no VS Code.
2. `Ctrl+Shift+P` -> "CMake: Configure".
   - Se o Qt nao for encontrado automaticamente (Windows principalmente),
     rode "CMake: Edit CMake Cache" ou configure manualmente com:
     ```
     cmake -DCMAKE_PREFIX_PATH="C:/Qt/6.7.0/mingw_64" -B build
     ```
     (troque o caminho pelo local onde voce instalou o Qt).
3. `Ctrl+Shift+P` -> "CMake: Build" (ou selecione o alvo `SoftwareCorrosaoGUI`
   na barra inferior do VS Code antes de compilar).
4. Rode `SoftwareCorrosaoGUI` pelo botao de play do CMake Tools, ou execute
   o binario gerado em `build/`.

## 3. Logo da UENF

Salve o arquivo oficial em:

```
assets/uenf.png
```

Fonte oficial recomendada — manual de marca da UENF (arquivos em alta
resolução): https://uenf.br/portal/wp-content/uploads/2024/11/manual_marca_UENF_2024.pdf
ou o site www.uenf.br

O LENEP e o curso de Engenharia de Exploração e Produção de Petróleo aparecem
apenas como texto no cabeçalho (não há mais uma segunda logo), então não é
necessário nenhum arquivo `assets/lenep.png` — o `lenep.png` que já estava na
pasta pode ficar ali sem uso ou ser removido, como preferir.

Não precisa mexer em nada no código — a interface carrega `assets/uenf.png`
automaticamente. Enquanto o arquivo não existir, aparece um retângulo com o
texto "UENF" no lugar, só para a tela não ficar quebrada.

A pasta `assets/` é copiada automaticamente para perto do executável a cada
build (configurado no `CMakeLists.txt`), então rode "CMake: Build" de novo
depois de trocar o logo.

## Observacao importante

Este projeto foi escrito e revisado cuidadosamente seguindo os padroes
normais do Qt Widgets, mas **nao foi compilado com o Qt de verdade** no
ambiente onde foi gerado (sem Qt instalado e sem acesso a internet para
instalar). Se aparecer algum erro de compilacao no seu VS Code, copie a
mensagem de erro e me envie — ajusto na hora.
