/*
 * vrp_parse.c
 *
 *  Created on: May 17, 2014
 *      Author: mozg
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vrp_parse.h"

#define DEPOT 1
#define VISIT 0
#define TRUE 1
#define FALSE 0

int parse_vrp(char * filename, location** locations, vrp_info** info) {

	/*FILE* fp;*/
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int line_num = 0;

	/*helper variables used during parsing*/
	int i;
	int id;
	int x, y;
	int val;
	char st[50];

	visit* visits;
	depot* depots;

	/*fp = fopen(filename, "r");
	if (fp == NULL) {
		return -1;
	}*/

	*info = malloc(sizeof(vrp_info));

	/*printf("VRP PARSER ******************************************************************************************************\n");*/

	while ((read = getline(&line, &len, stdin)) != -1) {

		line_num++;

		sscanf(line, "%*s %d", &val);

		switch (line_num) {

		case 5:
			(*info)->num_depots = val;
			break;
		case 6:
			(*info)->num_capacities = val;
			break;
		case 7:
			(*info)->num_visits = val;
			break;
		case 8:
			(*info)->num_locations = val;
			break;
		case 9:
			(*info)->num_vehicles = val;
			break;
		case 10:
			(*info)->capacities = val;
			break;

		}

		if (line_num == 10)
			break;

	}

	/*printf("\n");
	printf("NUM_DEPOTS: %d\n", (*info)->num_depots);
	printf("NUM_CAPACITIES: %d\n", (*info)->num_capacities);
	printf("NUM_VISITS: %d\n", (*info)->num_visits);
	printf("NUM_LOCATIONS: %d\n", (*info)->num_locations);
	printf("NUM_VEHICLES: %d\n", (*info)->num_vehicles);
	printf("CAPACITIES: %d\n", (*info)->capacities);
	printf("\n");*/

	visits = malloc((*info)->num_visits * sizeof(visit));
	depots = malloc((*info)->num_depots * sizeof(depot));
	*locations = malloc(((*info)->num_depots + (*info)->num_visits) * sizeof(location));

	for (i = 0; i < (*info)->num_depots; i++) {
		depots[i].id = i;
	}

	for (i = 0; i < (*info)->num_visits; i++) {
		visits[i].id = i + (*info)->num_depots;
	}

	/*continuing parsing after line 10*/

	while ((read = getline(&line, &len, stdin)) != -1) {

		sscanf(line, "%s", st);

		if (strcmp(st, "DATA_SECTION") == 0) {
			/*printf("DATA_SECTION\n");*/

		} else if (strcmp(st, "DEPOTS") == 0) {
			/*printf("DEPOTS\n");*/

			for (i = 0; i < (*info)->num_depots; i++) {

				read = getline(&line, &len, stdin);
				sscanf(line, "%d", &val);
				depots[i].id = val;
			}

		} else if (strcmp(st, "DEMAND_SECTION") == 0) {
			/*printf("DEMAND_SECTION\n");*/

			for (i = 0; i < (*info)->num_visits; i++) {

				read = getline(&line, &len, stdin);
				sscanf(line, "%d -%d", &id, &val);
				visits[i].demand = val;
			}
		} else if (strcmp(st, "LOCATION_COORD_SECTION") == 0) {
			/*printf("LOCATION_COORD_SECTION\n");*/

			for (i = 0; i < (*info)->num_depots; i++) {

				read = getline(&line, &len, stdin);
				sscanf(line, "%d %d %d", &id, &x, &y);
				depots[i].x = x;
				depots[i].y = y;
			}

			for (i = 0; i < (*info)->num_visits; i++) {

				read = getline(&line, &len, stdin);
				sscanf(line, "%d %d %d", &id, &x, &y);
				visits[i].x = x;
				visits[i].y = y;
			}

		} else if (strcmp(st, "DEPOT_LOCATION_SECTION") == 0) {
			/*printf("DEPOT_LOCATION_SECTION\n");*/

			for (i = 0; i < (*info)->num_depots; i++) {

				read = getline(&line, &len, stdin);
				sscanf(line, "%d %d", &id, &val);
				depots[i].location = val;
			}

		} else if (strcmp(st, "VISIT_LOCATION_SECTION") == 0) {
			/*printf("DEPOT_LOCATION_SECTION\n");*/

			for (i = 0; i < (*info)->num_visits; i++) {

				read = getline(&line, &len, stdin);
				sscanf(line, "%d %d", &id, &val);
				visits[i].location = val;
			}
		} else if (strcmp(st, "DURATION_SECTION") == 0) {
			/*printf("DURATION_SECTION\n");*/

			for (i = 0; i < (*info)->num_visits; i++) {

				read = getline(&line, &len, stdin);
				sscanf(line, "%d %d", &id, &val);
				visits[i].duration = val;
			}
		} else if (strcmp(st, "DEPOT_TIME_WINDOW_SECTION") == 0) {
			/*printf("DEPOT_TIME_WINDOW_SECTION\n");*/

			for (i = 0; i < (*info)->num_depots; i++) {

				read = getline(&line, &len, stdin);
				sscanf(line, "%d %d %d", &id, &x, &y);
				depots[i].time_from = x;
				depots[i].time_to = y;
			}
		} else if (strcmp(st, "TIME_AVAIL_SECTION") == 0) {
			/*printf("TIME_AVAIL_SECTION\n");*/

			for (i = 0; i < (*info)->num_visits; i++) {

				read = getline(&line, &len, stdin);
				sscanf(line, "%d %d", &id, &val);
				visits[i].time_avail = val;
			}
			break;
		}
	}

	/*printf("\n\nDEPOTS\n\n");
	for (i = 0; i < (*info)->num_depots; i++) {

		printf("id\t%d\n", depots[i].id);
		printf("coord\t%d,%d\n", depots[i].x, depots[i].y);
		printf("time\t%d - %d\n", depots[i].time_from, depots[i].time_to);
		printf("loc\t%d\n", depots[i].location);

	}

	printf("\n\nVISITS\n\n");
	for (i = 0; i < 2; i++) {

		printf("id\t%d\n", visits[i].id);
		printf("coord\t%d,%d\n", visits[i].x, visits[i].y);
		printf("dem\t%d\n", visits[i].demand);
		printf("loc\t%d\n", visits[i].location);
		printf("dur\t%d\n", visits[i].duration);
		printf("time_a\t%d\n", visits[i].time_avail);

		printf("\n");
	}*/

	for (i = 0; i < (*info)->num_depots; i++) {

		(*locations)[i].id = depots[i].id;
		(*locations)[i].type = DEPOT;
		(*locations)[i].x = depots[i].x;
		(*locations)[i].y = depots[i].y;
		/*locations[i].location_section = depots[i].location;*/
		(*locations)[i].time_from = depots[i].time_from;
		(*locations)[i].time_to = depots[i].time_to;
		(*locations)[i].demand = 0;
		(*locations)[i].duration = 0;
		(*locations)[i].time_avail = 0;

	}

	for (i = 0; i < (*info)->num_visits; i++) {

		(*locations)[i + (*info)->num_depots].id = visits[i].id;
		(*locations)[i + (*info)->num_depots].type = VISIT;
		(*locations)[i + (*info)->num_depots].x = visits[i].x;
		(*locations)[i + (*info)->num_depots].y = visits[i].y;
		/*locations[i + info->num_depots].location_section = visits[i].location;*/
		(*locations)[i + (*info)->num_depots].time_from = 0;
		(*locations)[i + (*info)->num_depots].time_to = 0;
		(*locations)[i + (*info)->num_depots].demand = visits[i].demand;
		(*locations)[i + (*info)->num_depots].duration = visits[i].duration;
		(*locations)[i + (*info)->num_depots].time_avail = visits[i].time_avail;

	}

	free(depots);
	free(visits);

	if (line)
		free(line);

	return 0;
}


