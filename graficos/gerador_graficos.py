import pandas as pd
import matplotlib.pyplot as plt
import time

# Configurar backend para modo não interativo (mais rápido)
plt.switch_backend('Agg')

# Estilo global
plt.rcParams['font.family'] = 'DejaVu Sans'
plt.rcParams['grid.alpha'] = 0.3
plt.rcParams['grid.linestyle'] = '--'
plt.rcParams['axes.titlepad'] = 20
plt.rcParams['axes.labelpad'] = 15

# Cores para as operações
VERDE = '#2ca02c'
ROXO  = '#9467bd'

# Função para carregar dados

def carregar_dados(arquivo):
    return pd.read_csv(
        arquivo,
        usecols=['operation', 'node_count', 'time_us'],
        dtype={'operation': 'category', 'node_count': 'int32', 'time_us': 'int32'}
    )

# Função genérica para gerar gráficos de operação

def gerar_grafico(df, titulo, arquivo, cor, label, max_time=None):
    fig, ax = plt.subplots(figsize=(12, 6))
    df = df.sort_values('node_count')
    # Downsampling se necessário
    if len(df) > 10000:
        step = len(df) // 5000
        x = df['node_count'][::step]
        y = df['time_us'][::step]
    else:
        x = df['node_count']
        y = df['time_us']
    ax.plot(x, y, color=cor, linewidth=1.5, label=label)
    ax.set_title(titulo, fontsize=16)
    ax.set_xlabel("Número de Nós", fontsize=14)
    ax.set_ylabel("Tempo (μs)", fontsize=14)
    ax.grid(True)
    ax.legend()
    # Ajuste de tempo máximo se especificado
    if max_time is not None:
        ax.set_ylim(0, max_time)
    fig.tight_layout()
    fig.savefig(arquivo, dpi=150)
    plt.close(fig)

# Função para gerar gráfico comparativo

def gerar_comparativo(ins, rem, arquivo, max_time=None):
    fig, ax = plt.subplots(figsize=(12, 6))
    ins = ins.sort_values('node_count')
    rem = rem.sort_values('node_count')
    # Downsampling
    if len(ins) > 10000:
        step = len(ins) // 5000
        ax.plot(ins['node_count'][::step], ins['time_us'][::step], color=VERDE, linewidth=1.5, label="Inserção")
        ax.plot(rem['node_count'][::step], rem['time_us'][::step], color=ROXO, linewidth=1.5, label="Remoção")
    else:
        ax.plot(ins['node_count'], ins['time_us'], color=VERDE, linewidth=1.5, label="Inserção")
        ax.plot(rem['node_count'], rem['time_us'], color=ROXO, linewidth=1.5, label="Remoção")
    ax.set_title("Comparativo Inserção vs Remoção", fontsize=16)
    ax.set_xlabel("Número de Nós", fontsize=14)
    ax.set_ylabel("Tempo (μs)", fontsize=14)
    ax.grid(True)
    ax.legend()
    if max_time is not None:
        ax.set_ylim(0, max_time)
    fig.tight_layout()
    fig.savefig(arquivo, dpi=150)
    plt.close(fig)

# Função para gerar gráfico de tempo acumulado

def gerar_acumulado(ins, rem, arquivo, max_time=None):
    ins = ins.sort_values('node_count').copy()
    rem = rem.sort_values('node_count').copy()
    ins['acumulado'] = ins['time_us'].cumsum()
    rem['acumulado'] = rem['time_us'].cumsum()
    fig, ax = plt.subplots(figsize=(12, 6))
    # Downsampling acumulado
    if len(ins) > 10000:
        step = len(ins) // 5000
        ax.plot(ins['node_count'][::step], ins['acumulado'][::step], color=VERDE, linewidth=1.5, label="Inserção")
        ax.plot(rem['node_count'][::step], rem['acumulado'][::step], color=ROXO, linewidth=1.5, label="Remoção")
    else:
        ax.plot(ins['node_count'], ins['acumulado'], color=VERDE, linewidth=1.5, label="Inserção")
        ax.plot(rem['node_count'], rem['acumulado'], color=ROXO, linewidth=1.5, label="Remoção")
    ax.set_title("Tempo Acumulado", fontsize=16)
    ax.set_xlabel("Número de Nós", fontsize=14)
    ax.set_ylabel("Tempo Acumulado (μs)", fontsize=14)
    ax.grid(True)
    ax.legend()
    if max_time is not None:
        ax.set_ylim(0, max_time)
    fig.tight_layout()
    fig.savefig(arquivo, dpi=150)
    plt.close(fig)

# Entrada principal

def main():
    df = carregar_dados("../B-tree.csv")
    ins = df[df['operation'] == 'insert']
    rem = df[df['operation'] == 'delete']
    # Inserção sem limite
    gerar_grafico(ins, "Desempenho Inserção", "insercao.png", VERDE, "Inserção")
    # Remoção com limite 1000
    gerar_grafico(rem, "Desempenho Remoção", "remocao.png", ROXO, "Remoção", max_time=150)
    # Comparativo com limite 1000
    gerar_comparativo(ins, rem, "comparacao.png", max_time=300)
    # Acumulado sem limite
    gerar_acumulado(ins, rem, "acumulado.png")

if __name__ == "__main__":
    main()