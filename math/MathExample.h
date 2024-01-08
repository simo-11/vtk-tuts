#pragma once
#include <cuda_runtime.h>
#include <cusolverDn.h>
#include "cusolver_utils.h"
enum SolverImpl {mkl,cuda};
struct Workspaces {
	int n;
	/* for mkl */
	int* ipiv;
	/* for cuda */
	float* hx;
	cudaStream_t stream;
	cusolverDnHandle_t handle;
	cusolverDnIRSParams_t gesv_params;
	cusolverDnIRSInfos_t gesv_info;
	float* dA;
	cusolver_int_t ldda;
	// right hand side on device
	float* dB;
	cusolver_int_t lddb;
	// solution on device
	float* dX;
	cusolver_int_t lddx;
	// info indicator on device
	cusolver_int_t* dinfo;
	// work buffer
	void* dwork;
	// size of work buffer
	size_t dwork_size;
	// number of refinement iterations returned by solver
	cusolver_int_t iter;
};
extern float* getRandomA(int n);
extern float* getRandomB(int n);
extern int MathSolve(int verbose,SolverImpl impl, int n, float* a, float* b, 
	Workspaces* ws);
extern int lapack_allocate(Workspaces* ws);
extern int lapack_free(Workspaces* ws);
extern int lapack_solve(int verbose, int n, float* a, float* b, Workspaces* ws);
extern const char* get_lapack_error_reason(int n);
extern int cuda_allocate(Workspaces* ws);
extern int cuda_free(Workspaces* ws);
extern int cuda_solve(int verbose, int n, float* a, float* b, Workspaces* ws);
extern const char* get_cuda_error_reason(int n);

class vtkObject;

class MathExampleUI
{
public:
	static void setup(vtkObject* caller, unsigned long eventId, 
			void* clientData, void* callData);
	static void draw(vtkObject* caller, unsigned long eventId, 
		void* clientData, void* callData);
};
