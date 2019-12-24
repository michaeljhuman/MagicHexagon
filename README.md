# MagicHexagon
C++ code for solving the order 3 magic hexagon.  VS 2017

Simple attack.  Builds permutations that allow outer edges to sum to 38.  
Stops building a permutation as soon as it's known it will lead to an invalid solution.

Once outer edge solution is found, and inner edge solution is found.  
Either there's only one valid outer solution, or the first one it finds is valid.
