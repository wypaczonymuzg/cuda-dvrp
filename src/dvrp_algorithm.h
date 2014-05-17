/*
 * dvrp_algorithm.h
 *
 *  Created on: May 17, 2014
 *      Author: mozg
 */

#ifndef DVRP_ALGORITHM_H_
#define DVRP_ALGORITHM_H_
#include "vrp_parse.h"

static void swap(unsigned int *ar, unsigned int first, unsigned int second);
static void reverse(unsigned int *ar, size_t len);
unsigned int next_permutation(unsigned int *ar, size_t len);
unsigned int next_combination(unsigned int *ar, size_t n, unsigned int k);
unsigned int next_k_permutation(unsigned int *ar, size_t n, unsigned int k);
float euclid_distance(int x1, int y1, int x2, int y2);
float route_distance(unsigned int* route, size_t route_size,
		location* locations, size_t location_size, unsigned short capacity);
unsigned int* divide(unsigned int* permutation, size_t len,
		unsigned short depth) ;
float dvrp(vrp_info info, location* locations, unsigned int* permutation,
		unsigned short blockedc);

#endif /* DVRP_ALGORITHM_H_ */
