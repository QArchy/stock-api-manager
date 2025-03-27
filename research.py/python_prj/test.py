# install dependencies and select solver
# pip install -q amplpy ccxt matplotlib networkx numpy pandas

SOLVER = "cbc"

from amplpy import AMPL, ampl_notebook

ampl = ampl_notebook(
    modules=["cbc"],  # modules to install
    license_uuid="default",  # license to use
)  # instantiate AMPL object

import os
import sys
from time import time
from timeit import default_timer as timer

import ccxt
import matplotlib.pyplot as plt
import networkx as nx
import numpy as np
import pandas as pd

print("Available exchanges:\n")
for i, exchange in enumerate(ccxt.exchanges):
    print(f"{i+1:3d}) {exchange.ljust(30)}", end="" if (i + 1) % 4 else "\n")
print("\n")

# global variables used in subsequent cells

# create an exchange object
exchange = ccxt.kucoin()


def get_exchange_dg(exchange, minimum_in_degree=1):
    """
    Return a directed graph constructed from the market symbols on a specified exchange.
    """
    markets = exchange.load_markets()
    symbols = markets.keys()

    # create an edge for all market symbols
    dg = nx.DiGraph()
    for base, quote in [symbol.split("/") for symbol in symbols]:
        dg.add_edge(quote, base, color="k", width=1)

    # remove base currencies with in_degree less than minimum_in_degree
    remove_nodes = [
        node
        for node in dg.nodes
        if dg.out_degree(node) == 0 and dg.in_degree(node) < minimum_in_degree
    ]
    dg.remove_nodes_from(remove_nodes)

    # color quote currencies in gold
    for node in dg.nodes():
        dg.nodes[node]["color"] = "gold" if dg.out_degree(node) > 0 else "lightblue"

    return dg


def draw_dg(dg, rad=0.0):
    """
    Draw directed graph of markets symbols.
    """
    n_nodes = len(dg.nodes)
    size = int(2.5 * np.sqrt(n_nodes))
    fig = plt.figure(figsize=(size, size))
    pos = nx.circular_layout(dg)
    nx.draw(
        dg,
        pos,
        with_labels=True,
        node_color=[dg.nodes[node]["color"] for node in dg.nodes()],
        edge_color=[dg.edges[u, v]["color"] for u, v in dg.edges],
        width=[dg.edges[u, v]["width"] for u, v in dg.edges],
        node_size=1000,
        font_size=8,
        arrowsize=15,
        connectionstyle=f"arc3, rad={rad}",
    )
    nx.draw_networkx_edge_labels(
        dg, pos, edge_labels={(src, dst): f"{src}/{dst}" for src, dst in dg.edges()}
    )
    
    return plt.gca()


minimum_in_degree = 5
exchange_dg = get_exchange_dg(exchange, minimum_in_degree)
ax = draw_dg(exchange_dg, 0.01)
ax.set_title(
    exchange.name + "\n" + f"Minimum in Degree (Base Currencies) = {minimum_in_degree}"
)
ax.figure.savefig('Trade/crypto_arbitrage_graph.png', dpi=300, bbox_inches='tight')
print(f"Number of nodes = {len(exchange_dg.nodes()):3d}")
print(f"Number of edges = {len(exchange_dg.edges()):3d}")