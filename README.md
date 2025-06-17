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
├── btree.c                      # Implementação em C da estrutura e benchmark
├── btree.exe                    # Executável (Windows)
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

## Insights do Benchmark do Benchmark do Benchmark

* **Inserção**: picos de até \~360 μs em splits iniciais; após \~200 000 tokens, latência estabiliza em 1–10 μs.
* **Remoção**: picos de \~150 μs em merges/redistribuições; geralmente 1–5 μs quando nó está balanceado.
* **Tempo Acumulado**: inserção e remoção crescem quase linearmente, totalizando cerca de 3,4 s cada para \~247 000 tokens.

## Conclusão

A B‑Tree de ordem 3 apresenta comportamento amortizado eficiente, com latências baixas na maioria das operações e picos pontuais bem distribuídos. Inserções são levemente mais custosas que remoções, mas ambos os processos permanecem na mesma ordem de grandeza.

## Ambiente de Execução

* **Sistema Operacional**: Windows 11 Home 64 bits
* **Processador**: 11th Gen Intel(R) Core(TM) i7-1165G7 @ 2.80GHz   1.69 GHz
* **Memória**: 16 GB DDR4
* **Armazenamento**: SSD 500 GB

---

Ana Julia Vieira Pereira Andrade da Costa
