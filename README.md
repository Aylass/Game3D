# Game3D
Trabalho da cadeira de Computação Gráfica. Intuito de exercitar o desenvolvimento de projeto em forma 3d.

Comandos:
P: terceira pessoa
SPACE: movimentação do coelho
SETAS
DIREITA: rotaciona o coelho para direita
ESQUERDA: rotaciona o coelho para esquerda
CIMA: olha para cima em primeira pessoa
BAIXO: olha para baixo em primeira pessoa
ESC: sai do jogo


Estruturas de Dados:
O jogo roda em torno de uma Grid(matriz) de 50 por 50. Em cada entrada dela existe 4 pontos q delimitam aquela área.
Ela auxilia nos cálculos de colisão com as beiradas do mapa e as estradas.
Os objetos são guardados em vetores que correspondem com os tipos de objetos:
tipo:cenoura vetor: Cenouras[]
tipo:objeto  vetor: Arvores[]
tipo:objeto  vetor:Lobos[]