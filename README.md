# Phylogenetic tree metrics in C++

## Overview

This repository implements a simple C++ workflow for parsing a rooted binary tree from an edge list and calculating the Colless imbalance index.

The Colless index is a standard measure of topological imbalance in rooted bifurcating trees. It is based on the difference in descendant tip counts between left and right subtrees at each internal node.

This repository is intended as a minimal, readable implementation of tree parsing and imbalance calculation.

## Input format

The program expects a plain text edge list:

```text
parent child
parent child
...
