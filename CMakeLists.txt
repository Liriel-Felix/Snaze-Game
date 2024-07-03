cmake_minimum_required(VERSION 3.10)

# Nome do projeto
project(t1)

# Define a versão do C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adiciona os arquivos fonte
add_executable(t1
    src/main.cpp
    src/game.cpp
)

# Inclui o diretório de cabeçalhos
target_include_directories(t1 PUBLIC "${PROJECT_SOURCE_DIR}/src")
