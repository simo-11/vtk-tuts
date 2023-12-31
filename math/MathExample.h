#pragma once
enum LapackImpl {mkl,netlib};
extern int LapackSgesv(bool printSolution,LapackImpl lapackImpl);
