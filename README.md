# ♟️ Jogo de Xadrez em C com Persistência de Dados (Console)

Este é um projeto completo de um Jogo de Xadrez desenvolvido em C para rodar no console (linha de comando). O sistema implementa as regras básicas do xadrez e adiciona um conjunto de funcionalidades robustas de gerenciamento de usuários e persistência de partidas, simulando um ambiente de jogo mais completo.

## ✨ Funcionalidades Principais

* **Jogo Completo:** Implementação das regras básicas de movimentação para todas as peças (`moverPeca`), incluindo promoção de peão.
* **Inteligência Artificial (IA):** O jogador Preto é controlado por uma IA com uma heurística de avaliação de tabuleiro (`avaliarTabuleiro`) e escolha da melhor jogada (`jogadaComputador`).
* **Menu Interativo:** Menu principal para iniciar novas partidas, retomar partidas salvas, ver o ranking e gerenciar usuários.
* **Gerenciamento de Usuários:**
    * **Cadastro e Login:** Permite que usuários se cadastrem e autentiquem (`criarCadastro`, `efetuarLogin`).
    * **Ranking:** Sistema de pontuação baseado em vitórias (3 pontos) e empates (1 ponto), com ordenação e exibição (`mostrarRanking`).
* **Persistência de Dados:**
    * **Salvamento de Partida:** Salva o estado atual do tabuleiro e o turno para o arquivo `partida_salva.txt`.
    * **Ranking/Contas:** Os dados de usuários (nome, senha, vitórias, empates, derrotas) são persistidos no arquivo `usuarios.txt`.

## 🛠️ Tecnologias e Implementação

* **Linguagem:** C (C11).
* **Compilador:** Utilizado `GCC`.
* **Ambiente:** Console/Terminal.
* **Estruturas de Dados:** Uso de `struct` para representar `Pecas`, `Casa` e `Usuario`.
* **Memória:** Gerenciamento explícito de memória com `malloc` e `free` para as peças do tabuleiro (`liberarTabuleiro`).

### Estruturas e Variáveis Globais

O jogo utiliza variáveis globais para manter o estado atual:

* `Casa* tabuleiro[8][8]`: O tabuleiro de xadrez.
* `char turnoAtual`: Indica o turno atual ('B' ou 'P').
* `int jogoEmAndamento`: Flag para o loop principal da partida.
* `char corVencedor`: Armazena a cor vencedora ao fim da partida.
* `int usaLogin`: Controla se o ranking deve ser atualizado.

## 📁 Arquivos do Projeto

| Arquivo | Descrição |
| :--- | :--- |
| `xadrez.c` | **Código-fonte principal** contendo toda a lógica do jogo, IA, CRUD de usuários e menus. |
| `usuarios.txt` | Armazena dados de login e estatísticas dos usuários para o ranking. |
| `partida_salva.txt` | Armazena o estado do jogo para retomada posterior. |

## 🚀 Como Compilar e Rodar

### Pré-requisitos

* Um compilador C (como GCC) instalado no sistema.

### Compilação

Para compilar o código utilizando o GCC, use o seguinte comando no terminal:

```bash
gcc xadrez.c -o xadrez

 
