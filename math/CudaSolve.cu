/*
 * Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

#include <cuda_runtime.h>
#include <cusolverDn.h>

#include "cusolver_utils.h"

int cuda_solve(int verbose, int n, float* a, float* b, int*) {
    // Numer of right hand sides
    const int nrhs = 1;

    // Use float matrix and half precision factorization
    typedef float T;
    const cusolverPrecType_t matrix_precision = CUSOLVER_R_32F;
    // make sure that you specify matrix precision that matches to the data type
    assert(traits<T>::cusolver_precision_type == matrix_precision);
    const cusolverPrecType_t compute_lower_precision = CUSOLVER_R_16F;

    // Use GMRES refinement solver
    const cusolverIRSRefinement_t refinement_solver = CUSOLVER_IRS_REFINE_GMRES;

    T *hA=a;
    cusolver_int_t lda=n;
    T *hB=b;
    cusolver_int_t ldb=n;
    T *hX=(T*)malloc(n*n*sizeof(T));
    cusolver_int_t ldx=n;

    cudaStream_t stream;
    cudaEvent_t event_start, event_end;
    cusolverDnHandle_t handle;
    cusolverDnIRSParams_t gesv_params;
    cusolverDnIRSInfos_t gesv_info;
    if (verbose) {
        std::cout << "Initializing CUDA..." << std::endl;
    }
    CUDA_CHECK(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));
    CUDA_CHECK(cudaEventCreate(&event_start));
    CUDA_CHECK(cudaEventCreate(&event_end));
    CUSOLVER_CHECK(cusolverDnCreate(&handle));
    CUSOLVER_CHECK(cusolverDnSetStream(handle, stream));

    if (verbose) {
        std::cout << "Setting up gesv() parameters..." << std::endl;
    }
    // create solver parameters
    CUSOLVER_CHECK(cusolverDnIRSParamsCreate(&gesv_params));
    // set matrix precision and factorization precision
    CUSOLVER_CHECK(cusolverDnIRSParamsSetSolverPrecisions(gesv_params, matrix_precision,
                                                          compute_lower_precision));
    // set refinement solver
    CUSOLVER_CHECK(cusolverDnIRSParamsSetRefinementSolver(gesv_params, refinement_solver));
    // create solve info structure
    CUSOLVER_CHECK(cusolverDnIRSInfosCreate(&gesv_info));

    // matrix on device
    T *dA;
    cusolver_int_t ldda = ALIGN_TO(n * sizeof(T), device_alignment) / sizeof(T);
    // right hand side on device
    T *dB;
    cusolver_int_t lddb = ALIGN_TO(n * sizeof(T), device_alignment) / sizeof(T);
    // solution on device
    T *dX;
    cusolver_int_t lddx = ALIGN_TO(n * sizeof(T), device_alignment) / sizeof(T);
    // info indicator on device
    cusolver_int_t *dinfo;
    // work buffer
    void *dwork;
    // size of work buffer
    size_t dwork_size;
    // number of refinement iterations returned by solver
    cusolver_int_t iter;
    if (verbose) {
        std::cout << "Allocating memory on device..." << std::endl;
    }
    // allocate data
    CUDA_CHECK(cudaMalloc(&dA, ldda * n * sizeof(T)));
    CUDA_CHECK(cudaMalloc(&dB, lddb * nrhs * sizeof(T)));
    CUDA_CHECK(cudaMalloc(&dX, lddx * nrhs * sizeof(T)));
    CUDA_CHECK(cudaMalloc(&dinfo, sizeof(cusolver_int_t)));

    // copy input data
    size_t dpitch = ldda * sizeof(T);
    size_t spitch = lda * sizeof(T);
    size_t width = n * sizeof(T);
    size_t height = n;
    CUDA_CHECK(cudaMemcpy2D(dA, dpitch, hA, spitch, width, height,
        cudaMemcpyDefault));
    dpitch = lddb * sizeof(T);
    spitch = ldb * sizeof(T);
    height = nrhs;
    CUDA_CHECK(cudaMemcpy2D(dB, dpitch, hB, spitch, width,height,
        cudaMemcpyDefault));

    // get required device work buffer size
    CUSOLVER_CHECK(cusolverDnIRSXgesv_bufferSize(handle, gesv_params, n, nrhs, &dwork_size));
    if (verbose) {
        std::cout << "Workspace is " << dwork_size << " bytes" << std::endl;
    }
    CUDA_CHECK(cudaMalloc(&dwork, dwork_size));

    if (verbose) {
        std::cout << "Solving matrix on device..." << std::endl;
    }
    CUDA_CHECK(cudaEventRecord(event_start, stream));

    cusolverStatus_t gesv_status =
        cusolverDnIRSXgesv(handle, gesv_params, gesv_info, n, nrhs, dA, ldda, dB, lddb, dX, lddx,
                           dwork, dwork_size, &iter, dinfo);
    CUSOLVER_CHECK(gesv_status);

    CUDA_CHECK(cudaEventRecord(event_end, stream));
    // check solve status
    int info = 0;
    CUDA_CHECK(
        cudaMemcpyAsync(&info, dinfo, sizeof(cusolver_int_t), cudaMemcpyDeviceToHost, stream));
    CUDA_CHECK(cudaStreamSynchronize(stream));

    if (verbose) {
        std::cout << "Solve info is: " << info << ", iter is: " << iter << std::endl;
    }

    CUDA_CHECK(cudaMemcpy2D(hX, ldx * sizeof(T), dX, lddx * sizeof(T), n * sizeof(T), nrhs,
                            cudaMemcpyDefault));
    CUDA_CHECK(cudaGetLastError());
    memcpy(b, hX, n * sizeof(T));
    float solve_time = 0.f;
    CUDA_CHECK(cudaEventElapsedTime(&solve_time, event_start, event_end));
    if (verbose) {
        std::cout << "Solved matrix " << n << "x" << n << " with " << nrhs << " right hand sides in "
            << solve_time << " ms" << std::endl;
    }

    if (verbose) {
        std::cout << "Releasing resources..." << std::endl;
    }
    CUDA_CHECK(cudaFree(dwork));
    CUDA_CHECK(cudaFree(dinfo));
    CUDA_CHECK(cudaFree(dX));
    CUDA_CHECK(cudaFree(dB));
    CUDA_CHECK(cudaFree(dA));
    CUSOLVER_CHECK(cusolverDnDestroy(handle));
    CUDA_CHECK(cudaEventDestroy(event_start));
    CUDA_CHECK(cudaEventDestroy(event_end));
    CUDA_CHECK(cudaStreamDestroy(stream));
    free(hX);
    if (verbose) {
        std::cout << "Done!" << std::endl;
    }
    return 0;
}
