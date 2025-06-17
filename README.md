# B-Tree para analise da Biblia

## Introdução

Este projeto realiza uma analise em uma B-Tree, medindo o tempo (μs) de inserção e remoção de cada token lido do arquivo `blivre.txt`. Conforme a árvore cresce, capturamos operações estruturais (split, merge, rebalanceamento) e exportamos os resultados em CSV para análise de comportamento amortizado e picos de latência.

## Como Rodar

1. **Compilar e Executar o Código em C**

   Abra um terminal no diretório raiz do projeto e execute:

   ```bash
   # Compilar para Windows (MinGW ou similar)
   gcc btree.c -o btree.exe

   # Executar (gera B-tree.csv a partir de utils/blivre.txt)
   ./btree.exe
   ```

   O programa processa `utils/blivre.txt` e cria `B-tree.csv` no diretório raiz.

2. **Gerar Gráficos com Python**

   Acesse a pasta de gráficos e rode o script:

   ```bash
   cd graficos
   pip install pandas matplotlib     # se ainda não estiverem instalados
   python gerar_grafico.py          # carrega ../B-tree.csv e gera as imagens
   ```

   As figuras geradas aparecerão em `graficos/`:

   * `insercao.png`
   * `remocao.png`
   * `comparacao.png`
   * `acumulado.png`

## Estrutura do Projeto

```
├── graficos/                    # Scripts e imagens de saída
│   ├── gerar_grafico.py        # Gera gráficos a partir de ../B-tree.csv
│   ├── insercao.png            # Desempenho de inserção
│   ├── remocao.png             # Desempenho de remoção
│   ├── comparacao.png          # Comparativo inserção vs. remoção
│   └── acumulado.png           # Tempo acumulado das operações
├── utils/                       # Dados de entrada
│   └── blivre.txt              # Texto da Bíblia Livre em formato TXT
|   ├── btree.c                  # Implementação em C da estrutura e benchmark
|   ├── btree.exe                # Executável
└── B-tree.csv                   # CSV gerado com tempos (operation,node_count,time_us)
```

## Pseudocódigo Simplificado

```plaintext
// Inicialização
create_btree():
  root ← create_node(leaf)
  node_count ← 1

// Inserção
insert_key(tree, key):
  se root cheia:
    new_root ← split_root(tree)
    root ← new_root
  inserir_rec(root, key)

// Remoção
remove_key(tree, key):
  remover_rec(root, key)
  se root vazia:
    root ← ajustar_raiz(root)

// Leitura e benchmark
main():
  tokens ← ler_tokens("utils/blivre.txt")
  for cada token em tokens:
    medir_tempo(e → insert_key(tree, token))
    log(insert, node_count, us)
  shuffle(tokens)
  for cada token em tokens:
    medir_tempo(e → remove_key(tree, token))
    log(delete, node_count, us)
```

## Resultados e Interpretação dos Gráficos

### Desempenho de Inserção (`insercao.png`)

<img src="https://github.com/A-juli07/B_Tree-bible/blob/main/graficos/insercao.png" alt="Gráfico 1" width="80%" style="display: inline-block;">

* **Formato do gráfico**: tempo de cada operação de inserção (μs) versus número de nós da árvore.
* **Observações**:

  * Picos mais elevados (< 120 μs) ocorrem nos primeiros splits, quando a altura da B-Tree aumenta de 1 para 2 e 3 níveis.
  * Após \~200 000 tokens, a latência de inserção se estabiliza entre **1 e 10 μs**, indicando comportamento amortizado constante.
  * Pontos isolados acima de 20 μs correspondem a operações de reorganização de página (split).

### Desempenho de Remoção (`remocao.png`)

<img src="https://github.com/A-juli07/B_Tree-bible/blob/main/graficos/remocao.png" alt="Gráfico 2" width="80%" style="display: inline-block;">

* **Formato do gráfico**: tempo de cada operação de remoção (μs) versus número de nós.
* **Observações**:

  * Picos de remoção chegam a **\~150 μs**, associados a eventos de *merge* ou redistribuição de chaves.
  * A maior parte das remoções ocorre em **1–5 μs**, quando o nó ainda contém chaves suficientes.
  * Spikes isolados correspondem a merges de níveis superiores da árvore.

### Comparativo Inserção vs Remoção (`comparacao.png`)

<img src="https://github.com/A-juli07/B_Tree-bible/blob/main/graficos/comparacao.png" alt="Gráfico 3" width="80%" style="display: inline-block;">

* **Formato do gráfico**: ambas as curvas (verde para inserção, roxa para remoção) sobrepostas.
* **Observações**:

  * Inserção apresenta picos mais frequentes e elevados, refletindo splits.
  * Remoção exibe picos menos intensos, mas também recorrentes durante merges.
  * Fora dos picos, ambas as operações mantêm latências baixas e similares.

### Tempo Acumulado (`acumulado.png`)

<img src="https://github.com/A-juli07/B_Tree-bible/blob/main/graficos/acumulado.png" alt="Gráfico 4" width="80%" style="display: inline-block;">

* **Formato do gráfico**: somatório dos tempos de inserção e remoção ao longo do benchmark.
* **Observações**:

  * A curva de inserção (verde) cresce quase linearmente até **\~4 000 000 μs** (\~4 s) para \~247 000 nós.
  * A curva de remoção (roxa) acompanha crescimento linear, aproximando-se de **\~1 500 000 μs** (\~1,5 s).
  * A diferença absoluta reflete o custo extra de splits comparado a merges.

## Conclusão

A B‑Tree de ordem 3 apresenta comportamento amortizado eficiente, com latências baixas na maioria das operações e picos pontuais bem distribuídos. Inserções são levemente mais custosas que remoções, mas ambos os processos permanecem na mesma ordem de grandeza.

## Ambiente de Execução

* **Sistema Operacional**: Windows 11 Home 64 bits
* **Processador**: 11th Gen Intel(R) Core(TM) i7-1165G7 @ 2.80GHz   1.69 GHz
* **Memória**: 16 GB DDR4
* **Armazenamento**: SSD 500 GB

---

Ana Julia Vieira Pereira Andrade da Costa



