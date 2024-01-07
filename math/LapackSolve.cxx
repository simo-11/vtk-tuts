/*
Calls LAPACKE_sgesv using col_major
*/
#include <stdlib.h>
#include <stdio.h>
#include "MathExample.h"
#include "mkl_lapacke.h"
enum errorCode {IPIV_ALLOCATE=1};
int lapack_allocate(Workspaces* ws) {
    ws->ipiv = (int*)malloc(ws->n * sizeof(int));
    if (ws->ipiv == nullptr) {
        return IPIV_ALLOCATE;
    }
    return 0;
}
int lapack_free(Workspaces* ws) {
    if (ws->ipiv != nullptr) {
        free(ws->ipiv);
    }
    ws->ipiv = nullptr;
    return 0;
}
namespace {
    char eb[120];
}
const char* get_lapack_error_reason(int n) {
    switch (n) {
    case IPIV_ALLOCATE: return "IPIV_ALLOCATE";
    default: 
        snprintf(eb, sizeof(eb), "Uknown code %d, add to %s", n,__FILE__);
        return eb;
    }
}

int lapack_solve(int verbose, int n, float* a, float* b, Workspaces* ws) {
    int info = LAPACKE_sgesv(LAPACK_COL_MAJOR, n, 1, a, n, ws->ipiv,
        b, n);
    /* Check for the exact singularity */
    if (info > 0) {
        if (verbose) {
            printf("The diagonal element of the triangular factor of A,\n");
            printf("U(%i,%i) is zero, so that A is singular;\n", info, info);
            printf("the solution could not be computed.\n");
        }
    }
    return info;
}
