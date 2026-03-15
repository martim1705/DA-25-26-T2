cmake_minimum_required(VERSION 3.26)

# O nome do projeto (com o formato do ZIP final)
# project(DA2026_PRJ1_T02_G00)

project(DA-25-26-T2)
# Mantemos a norma C++14  como nas TP
set(CMAKE_CXX_STANDARD 14)

# Cria o executável com todos os ficheiros
add_executable(project
        main.cpp
        CentralManager.cpp
        Parser.cpp
        MaxFlow.cpp

        # Truque do CLion: listar os .h aqui não afeta a compilação,
        # mas faz com que eles apareçam na barra lateral do editor
        CentralManager.h
        Parser.h
        MaxFlow.h
        DataEntities.h
        Graph.h
)
