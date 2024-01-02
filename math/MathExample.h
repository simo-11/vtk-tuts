#pragma once
enum SolverImpl {mkl,cuda};
extern float* getRandomA(int n);
extern float* getRandomB(int n);
extern int MathSolve(int verbose,SolverImpl impl, int n, float* a, float* b);
extern int lapack_solve(int verbose, int n, float* a, float* b, int* ipiv);
extern int cuda_solve(int verbose, int n, float* a, float* b, int* ipiv);
