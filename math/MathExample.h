#pragma once
enum SolverImpl {mkl,cuda};
extern int MathSolve(int verbose,SolverImpl impl, int n);
extern int lapack_solve(int verbose, int n, float* a, float* b, int* ipiv);
extern int cuda_solve(int verbose, int n, float* a, float* b, int* ipiv);
