/*
Calls vtkMath
*/
#include <stdlib.h>
#include <stdio.h>
#include <vtkMath.h>
#include "MathExample.h"
enum errorCode { IPIV_ALLOCATE, A_ALLOCATE, X_ALLOCATE};
int vtk_allocate(Workspaces* ws) {
    ws->ipiv = (int*)malloc(ws->n * sizeof(int));
    if (ws->ipiv == nullptr) {
        return IPIV_ALLOCATE;
    }
    ws->A = (double**)malloc(ws->n* ws->n * sizeof(double));
    if (ws->ipiv == nullptr) {
        return A_ALLOCATE;
    }
    ws->x = (double*)malloc(ws->n * sizeof(double));
    if (ws->ipiv == nullptr) {
        return X_ALLOCATE;
    }
    return 0;
}
int vtk_free(Workspaces* ws) {
    if (ws->ipiv != nullptr) {
        free(ws->ipiv);
    }
    ws->ipiv = nullptr;
    if (ws->A != nullptr) {
        free(ws->A);
    }
    ws->A = nullptr;
    if (ws->x != nullptr) {
        free(ws->x);
    }
    ws->x = nullptr;
    return 0;
}
namespace {
    char eb[120];
}
const char* get_vtk_error_reason(int n) {
    switch (n) {
    case IPIV_ALLOCATE: return "HX_ALLOCATE";
    case A_ALLOCATE: return "A_ALLOCATE";
    case X_ALLOCATE: return "X_ALLOCATE";
    default:
        snprintf(eb, sizeof(eb), "Uknown code %d, add to %s", n,__FILE__);
        return eb;
    }
}

int vtk_solve(int verbose, int n, float* a, float* b, Workspaces* ws) {
    double** A = ws->A;
    double* x = ws->x;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[j][i] = a[n*i+j];
        }
        x[i] = b[i];
    }
    if (vtkMath::LUFactorLinearSystem(A, ws->ipiv, n) == 0)
    {
        return 1;
    }
    vtkMath::LUSolveLinearSystem(A, ws->ipiv, x, n);
    for (int i = 0; i < n; i++) {
        b[i] = x[i];
    }
    return 0;
}
