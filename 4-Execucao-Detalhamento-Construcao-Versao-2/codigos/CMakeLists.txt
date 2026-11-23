CMAKE_MINIMUM_REQUIRED(VERSION 3.27)

project(SoftwareDeCorrosao
        LANGUAGES C CXX)

SET(CMAKE_CXX_STANDARD 20)

# ---------------------------------------------------------------------------
# Executavel de console (menu no terminal) - ja existia, mantido como esta.
# ---------------------------------------------------------------------------
SET(SOURCES
    main.cpp
    Fluido.cpp
    AnaliseCorrosao.cpp
    Inibidor.cpp
    CalculadoraDosagem.cpp
    SimuladorCorrosao.cpp
    CGnuplot.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# ---------------------------------------------------------------------------
# Executavel com interface grafica (Qt).
# Requer o Qt instalado (Qt6 ou Qt5). Se o CMake nao encontrar o Qt sozinho,
# aponte o caminho da instalacao, por exemplo:
#   cmake -DCMAKE_PREFIX_PATH="C:/Qt/6.7.0/mingw_64" ..
# ---------------------------------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets QUIET)

if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets QUIET)
endif()

if (Qt6_FOUND OR Qt5_FOUND)
    SET(FONTES_GUI
        gui/main_gui.cpp
        gui/MainWindow.cpp
        gui/GraficoResultados.cpp
        Fluido.cpp
        AnaliseCorrosao.cpp
        Inibidor.cpp
        CalculadoraDosagem.cpp
        SimuladorCorrosao.cpp
    )

    add_executable(SoftwareCorrosaoGUI ${FONTES_GUI})

    target_include_directories(SoftwareCorrosaoGUI PRIVATE ${CMAKE_SOURCE_DIR})

    if (Qt6_FOUND)
        target_link_libraries(SoftwareCorrosaoGUI PRIVATE Qt6::Widgets)
    else()
        target_link_libraries(SoftwareCorrosaoGUI PRIVATE Qt5::Widgets)
    endif()

    # Copia a pasta assets/ (logos) para perto do executavel apos cada build.
    add_custom_command(TARGET SoftwareCorrosaoGUI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets
                $<TARGET_FILE_DIR:SoftwareCorrosaoGUI>/assets)
else()
    message(STATUS "Qt nao encontrado: o alvo SoftwareCorrosaoGUI nao sera compilado. "
                    "Instale o Qt (Widgets) para habilitar a interface grafica.")
endif()
