#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "vrp_parse.h"

/*code for combinatorial functions from http://www.martinbroadhurst.com*/
static void swap(unsigned int *ar, unsigned int first, unsigned int second) {
	unsigned int temp = ar[first];
	ar[first] = ar[second];
	ar[second] = temp;
}

static void reverse(unsigned int *ar, size_t len) {
	unsigned int i, j;

	for (i = 0, j = len - 1; i < j; i++, j--) {
		swap(ar, i, j);
	}
}

unsigned int next_permutation(unsigned int *ar, size_t len) {
	unsigned int i1, i2;
	unsigned int result = 0;

	/* Find the rightmost element that is the first in a pair in ascending order */
	for (i1 = len - 2, i2 = len - 1; ar[i2] <= ar[i1] && i1 != 0; i1--, i2--)
		;
	if (ar[i2] <= ar[i1]) {
		/* If not found, array is highest permutation */
		reverse(ar, len);
	} else {
		/* Find the rightmost element to the right of i1 that is greater than ar[i1] */
		for (i2 = len - 1; i2 > i1 && ar[i2] <= ar[i1]; i2--)
			;
		/* Swap it with the first one */
		swap(ar, i1, i2);
		/* Reverse the remainder */
		reverse(ar + i1 + 1, len - i1 - 1);
		result = 1;
	}
	return result;
}

unsigned int next_combination(unsigned int *ar, size_t n, unsigned int k) {
	unsigned int finished = 0;
	unsigned int changed = 0;
	unsigned int i;

	if (k > 0) {
		for (i = k - 1; !finished && !changed; i--) {
			if (ar[i] < n - k + i + 1) {
				/* Increment this element */
				ar[i]++;
				if (i < k - 1) {
					/* Turn the elements after it into a linear sequence */
					unsigned int j;
					for (j = i + 1; j < k; j++) {
						ar[j] = ar[j - 1] + 1;
					}
				}
				changed = 1;
			}
			finished = i == 0;
		}
		if (!changed) {
			/* Reset to first combination */
			for (i = 0; i < k; i++) {
				ar[i] = i + 1;
			}
		}
	}
	return changed;
}

unsigned int next_k_permutation(unsigned int *ar, size_t n, unsigned int k) {
	unsigned int result = next_permutation(ar, k);
	if (result == 0) {
		result = next_combination(ar, n, k);
	}
	return result;
}

float euclid_distance(int x1, int y1, int x2, int y2) {
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

float route_distance(unsigned int* route, size_t route_size,
		location* locations, size_t location_size, unsigned short capacity) {

	float distance = 0;
	size_t i;
	short current_load = capacity;
	location* current, *prev = NULL;

	prev = &locations[route[0]];
	current_load -= locations[route[0]].demand;
	distance += euclid_distance(0, 0, prev->x, prev->y);

	for (i = 1; i < route_size; i++) {
		current = &locations[route[i]];
		current_load -= locations[route[i]].demand;

		if (current_load < 0) {
			distance += euclid_distance(prev->x, prev->y, 0, 0); /* hardcoded awesomeness 0.0*/
			distance += euclid_distance(0, 0, current->x, current->y); /* more awesomeness*/
			current_load = capacity - current->demand;
		} else
			distance += euclid_distance(prev->x, prev->y, current->x,
					current->y);

		prev = current;
	}

	distance += euclid_distance(prev->x, prev->y, 0, 0);

	return distance;
}
unsigned int* divide(unsigned int* permutation, size_t len,
		unsigned short depth) {
	unsigned short i, j, k, x, z = 1, found;
	unsigned int* problems;
	unsigned int problemsc = 1;

	for (i = 0; i < depth; i++)
		problemsc *= len - i;

	problems = (unsigned int*) malloc(sizeof(int) * problemsc*len );
	//printf("%d %zu\n", depth,len);
	for (i = 0; i < problemsc; i++) {

		if (depth > 1)
			next_k_permutation(permutation, len, depth);
		else
			permutation[0] = z++;
		memcpy(problems+i*len, permutation, len * sizeof(int));

		x = 1;
		for (j = depth; j < len;) {
			found = 0;
			for (k = 0; k < depth; k++)
				if (*(problems+i*len+k) == x) {
					found = 1;
					break;
				}
			if (!found) {
				*(problems+(i*len+j)) = x;
				j++;
			}
			x++;
		}

		//for (j = 0; j < len; j++)
			//printf("%d ", problems[i][j]);
		//printf("\n");
	}
	return problems;
}

float dvrp(vrp_info info, location* locations, unsigned int* permutation,
		unsigned short blockedc) {
	int i;
	float min_distance, distance;
	unsigned int* min_distance_permutation = (unsigned int*) malloc(
			info.num_visits * sizeof(int));

	min_distance = route_distance(permutation, info.num_visits, locations,
			info.num_locations, info.capacities);
	memcpy(min_distance_permutation, permutation,
			sizeof(int) * info.num_visits);

	while (next_permutation(permutation + blockedc, info.num_visits - blockedc)) {
		distance = route_distance(permutation, info.num_visits, locations,
				info.num_locations, info.capacities);
		if (distance < min_distance) {
			min_distance = distance;
			memcpy(min_distance_permutation, permutation,
					sizeof(int) * info.num_visits);
						//printf("Distance: %f\n", min_distance);
	//for (i = 0; i < info.num_visits; i++)
	//	printf("%d ", min_distance_permutation[i]);
	//printf("\n");
		}
	}
	printf("%f\n", min_distance);
	//for (i = 0; i < info.num_visits; i++)
		//printf("%d ", min_distance_permutation[i]);
	//printf("\n");
	return min_distance;
}
/*
int main(int argc, char **argv) {

	unsigned short i = 1, k, n;
	float temp, min = -1;
	unsigned int* permutation;
	vrp_info* info;
	location* locations;

	if (argc < 2)
		return EXIT_FAILURE;

	if (strcmp(argv[1], "divide") == 0) {
		scanf("%hu\n", &n);
		parse_vrp("", &locations, &info);
		k = info->num_visits;
		while (k < n)
			k *= k - i++;
		k = i;
		permutation = (unsigned int*) malloc(sizeof(int) * info->num_visits);
		for (i = 0; i < info->num_visits; i++)
			permutation[i] = i + 1;
		divide(permutation, info->num_visits, k);

	} else if (strcmp(argv[1], "solve") == 0) {
		scanf("%hu %hu\n", &n, &k);
		permutation = (unsigned int*) malloc(sizeof(int) * k);
	   	for (i = 0; i < k-1; i++)
			scanf("%u", &permutation[i]);
		scanf("%u\n", &permutation[k-1]);
		parse_vrp("", &locations, &info);
		dvrp(*info, locations, permutation, n);

	} else if (strcmp(argv[1], "merge") == 0) {
		scanf("%hu\n", &n);
		for (i = 0; i < n; i++) {
			scanf("%f\n", &temp);
			min = (min > temp || min == -1) ? temp : min;
		}
		printf("%f\n", min);
	} else
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
*/
