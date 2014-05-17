/**
 * Copyright 1993-2012 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 */
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "dvrp_algorithm.h"
#include "vrp_parse.h"

}

static const int WORK_SIZE = 256;

/**
 * This macro checks return value of the CUDA runtime call and exits
 * the application if the call failed.
 */

#define CUDA_CHECK_RETURN(value) {											\
	cudaError_t _m_cudaStat = value;										\
	if (_m_cudaStat != cudaSuccess) {										\
		fprintf(stderr, "Error %s at line %d in file %s\n",					\
				cudaGetErrorString(_m_cudaStat), __LINE__, __FILE__);		\
		exit(1);															\
	} }
//**********************************************************

//***********************************************************

__device__ static void d_swap(unsigned int *ar, unsigned int first,
		unsigned int second) {
	unsigned int temp = ar[first];
	ar[first] = ar[second];
	ar[second] = temp;
}

__device__ static void d_reverse(unsigned int *ar, size_t len) {
	unsigned int i, j;

	for (i = 0, j = len - 1; i < j; i++, j--) {
		d_swap(ar, i, j);
	}
}

__device__ float d_euclid_distance(int x1, int y1, int x2, int y2) {
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

__device__ unsigned int d_next_permutation(unsigned int *ar, size_t len) {
	unsigned int i1, i2;
	unsigned int result = 0;

	/* Find the rightmost element that is the first in a pair in ascending order */
	for (i1 = len - 2, i2 = len - 1; ar[i2] <= ar[i1] && i1 != 0; i1--, i2--)
		;
	if (ar[i2] <= ar[i1]) {
		/* If not found, array is highest permutation */
		d_reverse(ar, len);
	} else {
		/* Find the rightmost element to the right of i1 that is greater than ar[i1] */
		for (i2 = len - 1; i2 > i1 && ar[i2] <= ar[i1]; i2--)
			;
		/* Swap it with the first one */
		d_swap(ar, i1, i2);
		/* Reverse the remainder */
		d_reverse(ar + i1 + 1, len - i1 - 1);
		result = 1;
	}
	return result;
}

__device__ float d_route_distance(unsigned int* route, size_t route_size,
		location* locations, size_t location_size, unsigned short capacity) {

	float distance = 0;
	size_t i;
	short current_load = capacity;
	location* current, *prev = NULL;

	prev = &locations[route[0]];
	current_load -= locations[route[0]].demand;
	distance += d_euclid_distance(0, 0, prev->x, prev->y);

	for (i = 1; i < route_size; i++) {
		current = &locations[route[i]];
		current_load -= locations[route[i]].demand;

		if (current_load < 0) {
			distance += d_euclid_distance(prev->x, prev->y, 0, 0); /* hardcoded awesomeness 0.0*/
			distance += d_euclid_distance(0, 0, current->x, current->y); /* more awesomeness*/
			current_load = capacity - current->demand;
		} else
			distance += d_euclid_distance(prev->x, prev->y, current->x,
					current->y);

		prev = current;
	}

	distance += d_euclid_distance(prev->x, prev->y, 0, 0);

	return distance;
}

__device__ float d_dvrp(unsigned int* permutation, vrp_info *info,
		location* locations, unsigned short blockedc) {

	//int i;
	float min_distance, distance;
	//unsigned int* min_distance_permutation;
	//cudaMalloc((unsigned int*)min_distance_permutation,info.num_visits * sizeof(int));

	//cudaMalloc((void** ) &kernelArray,
	//sizeof(int) * info->num_visits * WORK_SIZE));

	min_distance = d_route_distance(permutation, info->num_visits, locations,
			info->num_locations, info->capacities);
	//memcpy(min_distance_permutation, permutation,
	//sizeof(int) * info.num_visits);

	while (d_next_permutation(permutation + blockedc,
			info->num_visits - blockedc)) {
		distance = d_route_distance(permutation, info->num_visits, locations,
				info->num_locations, info->capacities);
		if (distance < min_distance) {
			min_distance = distance;
			//memcpy(min_distance_permutation, permutation,
			//sizeof(int) * info.num_visits);
			//printf("Distance: %f\n", min_distance);
			//for (i = 0; i < info.num_visits; i++)
			//	printf("%d ", min_distance_permutation[i]);
			//printf("\n");
		}
	}
	//printf("%f\n", min_distance);
	//for (i = 0; i < info.num_visits; i++)
	//printf("%d ", min_distance_permutation[i]);
	//printf("\n");
	return min_distance;

}

/**
 * CUDA kernel function that reverses the order of bits in each element of the array.
 */
__global__ void kernel_dvrp(float* dst, unsigned int* permutation,
		vrp_info* info, location* locations, unsigned short blockedc,
		int problemsc) {

	float dist = 0;
	int id = blockIdx.x * WORK_SIZE + threadIdx.x;
	extern __shared__ unsigned int sh_permutation[];

	for (int i = 0; i < info->num_visits; i++) {
		sh_permutation[threadIdx.x * info->num_visits + i] =
				permutation[id + i];
	}
	//sh_permutation=(unsigned int*) malloc(sizeof(unsigned int)*info->num_visits*WORK_SIZE);
	//memcpy(sh_permutation,permutation + blockIdx.x * info->num_visits*WORK_SIZE,info->num_visits*WORK_SIZE);

	//printf("threadidx.x  = %d\n", threadIdx.x);
	/*
	 if (threadIdx.x == 1) {
	 printf("threadidx.x  = %d\n", threadIdx.x);
	 printf("info num_visits = %d\n", info->num_visits);
	 for (int i = 0; i < 256; i++) {
	 for (int j = 0; j < 12; j++)
	 printf("%d ", *(permutation + i * info->num_visits + j));
	 printf("\n");
	 }
	 }
	 */
	//unsigned int *idata = ((unsigned int*) permutation
	//+ threadIdx.x * info.num_visits);
	//printf("threadidx.x  = %d\n",threadIdx.x);
	if (id < problemsc) {
		dist = d_dvrp(sh_permutation + threadIdx.x * info->num_visits, info,
				locations, blockedc);

		dst[id] = dist;
		//dst[id] ma poprawne wartosci
		//printf("problem = %d\t distance = %f \t dst[id] =  %f\n", id, dist,dst[id]);
	}
	/*
	 if (id == 0) {
	 for (int i = 0; i < problemsc; i++) {
	 printf("problem = %d\t distance = %f \n", i, dst[i]);
	 }

	 }
	 __syncthreads();
	 */
}

/**
 * Host function that prepares data array and passes it to the CUDA kernel.
 */
int main(void) {
	unsigned int* kernelArray = NULL;
	unsigned int i = 1, k, n;

	unsigned int* permutation;
	unsigned int* problems;
	float* d_odata;
	float* h_odata;
	unsigned int problemsc = 1;

	vrp_info* info, *d_info;
	location* locations, *d_locations;

	//divide

	n = WORK_SIZE;
	parse_vrp("", &locations, &info);
	k = info->num_visits;
	while (k < n)
		k *= k - i++;
	k = i;
	permutation = (unsigned int*) malloc(sizeof(int) * info->num_visits);
	for (i = 0; i < info->num_visits; i++)
		permutation[i] = i + 1;

	for (i = 0; i < k; i++)
		problemsc *= info->num_visits - i;

	printf("problemsc = %d\n", problemsc);

	problems = (unsigned int*) malloc(sizeof(int*) * problemsc);

	problems = divide(permutation, info->num_visits, k);

	printf("k = %d \t n = %d \n", k, n);

	for (int i = 0; i < problemsc; i++) {
		printf("problems[%d] = ", i);
		for (int j = 0; j < info->num_visits; j++) {
			printf("%d ", *(problems + i * info->num_visits + j));
		}
		printf("\n");
	}

	//solve

	printf("kernelArray cudaMalloc\n");
	//for (int i = 0; i < problemsc / WORK_SIZE; i++) {
	CUDA_CHECK_RETURN(
			cudaMalloc((unsigned int** ) &kernelArray,
					sizeof(int) * info->num_visits * WORK_SIZE));
	printf("d_info cudaMalloc\n");
	CUDA_CHECK_RETURN(cudaMalloc((void** ) &d_info, sizeof(vrp_info)));
	printf("d_locations cudaMalloc\n");
	CUDA_CHECK_RETURN(
			cudaMalloc((void** ) &d_locations,
					sizeof(location) * info->num_locations));

	printf("d_odata cudaMalloc\n");
	CUDA_CHECK_RETURN(cudaMalloc((void** ) &d_odata, sizeof(float) * problemsc));

	printf("kernelArray from problems cudaMemcpy\n");
	CUDA_CHECK_RETURN(
			cudaMemcpy(kernelArray, problems,
					sizeof(unsigned int) * info->num_visits * WORK_SIZE,
					cudaMemcpyHostToDevice));

	printf("d_info from info cudaMemcpy\n");
	CUDA_CHECK_RETURN(
			cudaMemcpy(d_info, info, sizeof(vrp_info), cudaMemcpyHostToDevice));

	printf("d_locations from locations cudaMemcpy\n");
	CUDA_CHECK_RETURN(
			cudaMemcpy(d_locations, locations,
					sizeof(location) * info->num_locations,
					cudaMemcpyHostToDevice));

	printf("sharedMemory size\n");
	unsigned int sharedMemory = WORK_SIZE * info->num_visits
			* sizeof(unsigned int) + sizeof(vrp_info)
			+ sizeof(location) * info->num_locations;

	printf("kernel call\n");
	kernel_dvrp<<<6, WORK_SIZE, sharedMemory>>>(d_odata, kernelArray, d_info,
			d_locations, k, problemsc);

	//printf("out of kernel10\n");
	CUDA_CHECK_RETURN(cudaThreadSynchronize());

	//CUDA_CHECK_RETURN(cudaDeviceSynchronize());
	printf("get cudaerrors\n");
	CUDA_CHECK_RETURN(cudaGetLastError());

	printf("h_odata malloc\n");
	h_odata = (float*) malloc(sizeof(float) * problemsc);
	printf("h_odata from d_odata cudaMemcpy\n");
	CUDA_CHECK_RETURN(
			cudaMemcpy(h_odata, d_odata, sizeof(float) * problemsc,
					cudaMemcpyDeviceToHost));

	cudaFree(d_odata);

	printf("print h_odata : \n");
	for (int j = 0; j < problemsc; j++) {
		printf("h_odata[%d] =  %f\n", j, h_odata[j]);
	}

	printf("end of program\n");
	return EXIT_SUCCESS;
}
