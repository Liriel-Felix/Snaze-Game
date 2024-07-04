# Introdução

O projeto "snaze" é uma implementação do clássico jogo da cobrinha (Snake) utilizando C++. O objetivo do jogo é guiar uma serpente por um ambiente de labirinto, coletando alimentos para crescer de tamanho enquanto evita colidir com as paredes ou com seu próprio corpo. 
O jogo oferece múltiplos níveis, cada um representado por um arquivo de configuração que define a estrutura do labirinto e a posição inicial da serpente e dos alimentos. Os jogadores podem ajustar opções como o número inicial de vidas e a quantidade total de alimentos necessários para completar o jogo.

# Autores

- Nome: < José Diogo de Almeida Costa e Liriel Antônia de Lima Felix >
- Email: < diogo.almeida.130@ufrn.edu.br e liriel.felix.116@ufrn.edu.br>
- Turma: < Programação I >

# Problemas encontrados ou limitações

Nossa maior dificuldade foi com relação a utilização da biblioteca sfml para construção gráfica do jogo, acabamos não conseguindo aplicar a interface a partir dela.

# Instruções para Compilar e Executar o Programa

Para a compilação do projeto, é necessário via terminal estar no diretório src/ e então seguir o caminho:

## Criação e configuração da pasta build:
```
mkdir build
cd build
cmake ..
make
```

Esse procedimento irá gerar dentro da pasta build um executavél de nome GameOfLife.

## Depois de criar o executável, você pode executá-lo com o seguinte comando:

```
./snaze-game --life <numero-vidas> --food <numero-comidas>
```
