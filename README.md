# BidirectionalWoS
This is the implementation of the 2D reverse WoS algorithm in the paper ["A bidirectional formulation for Walk on Spheres"](https://cs.dartmouth.edu/~wjarosz/publications/qi22bidirectional.html).

Two test scenes (fig8 & fig9 in the paper) are provided in the code.

# How to run this code?
This code requires CMake to compile. 
```
git clone --recurse-submodules
cd BidirectionalWoS
mkdir build
cd build
cmake ..
make
./BidirectionalWoS
```

# Cite
Yang Qi, Dario Seyb, Benedikt Bitterli, Wojciech Jarosz. A bidirectional formulation for Walk on Spheres. Computer Graphics Forum (Proceedings of EGSR), 41(4), July 2022. 
```
@article{qi22bidirectional,
    author = "Qi, Yang and Seyb, Dario and Bitterli, Benedikt and Jarosz, Wojciech",
    title = "A bidirectional formulation for {Walk} on {Spheres}",
    journal = "Computer Graphics Forum (Proceedings of EGSR)",
    year = "2022",
    month = jul,
    volume = "41",
    number = "4",
    issn = "1467-8659",
    doi = "10.1111/cgf.14586",
    keywords = "Brownian motion, partial differential equations, PDEs, Monte Carlo",
    abstract = "Numerically solving partial differential equations (PDEs) is central to many applications in computer graphics and scientific modeling. Conventional methods for solving PDEs often need to discretize the space first, making them less efficient for complex geometry. Unlike conventional methods, the walk on spheres (WoS) algorithm recently introduced to graphics is a grid-free Monte Carlo method that can provide numerical solutions of Poisson equations without discretizing space. We draw analogies between WoS and classical rendering algorithms, and find that the WoS algorithm is conceptually equivalent to forward path tracing. Inspired by similar approaches in light transport, we propose a novel WoS reformulation that operates in the reverse direction, starting at source points and estimating the Green's function at ``sensor'' points. Implementations of this algorithm show improvement over classical WoS in solving Poisson equation with sparse sources. Our approach opens exciting avenues for future algorithms for PDE estimation which, analogous to light transport, connect WoS walks starting from sensors and sources and combine different strategies for robust solution algorithms in all cases."
}
```
