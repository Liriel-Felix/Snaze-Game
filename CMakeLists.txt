cmake_minimum_required(VERSION 3.10)

# Nome do projeto
project(snaze-game)

# Define a versão do C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adiciona os arquivos fonte
add_executable(snaze-game
    src/main.cpp
    src/game.cpp
)

# Inclui o diretório de cabeçalhos
target_include_directories(snaze-game PUBLIC "${PROJECT_SOURCE_DIR}/src")

# Linka o ncurses
target_link_libraries(snaze-game ncurses)
