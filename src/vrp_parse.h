/*
 * vrp_parser.h
 *
 *  Created on: May 6, 2014
 *      Author: bryst
 */

#ifndef VRP_PARSER_H_
#define VRP_PARSER_H_

#define DEPOT 1
#define VISIT 0
#define TRUE 1
#define FALSE 0

typedef struct vrp_info {

	unsigned short num_depots;
	unsigned short num_capacities;
	unsigned short num_visits;
	unsigned short num_locations;
	unsigned short num_vehicles;
	unsigned short capacities;

} vrp_info;

typedef struct location {

	unsigned short id;
	unsigned short type;
	int x, y;
	unsigned short time_from, time_to;
	short demand;
	unsigned short duration;
	unsigned short time_avail;
} location;

typedef struct depot {

	unsigned short id;
	int x, y; /*LOCATION_COORD_SECTION*/
	unsigned short time_from, time_to; /*DEPOT_TIME_WINDOW_SECTION*/
	unsigned short location; /*DEPOT_LOCATION_SECTION*/

} depot;

typedef struct visit {

	unsigned short id;
	int x, y; /*LOCATION_COORD_SECTION*/
	short demand; /*DEMAND_SECTION*/
	unsigned short location; /*VISIT_LOCATION_SECTION*/
	unsigned short duration; /*DURATION_SECTION*/
	unsigned short time_avail; /*TIME_AVAIL_SECTION*/

} visit;

int parse_vrp(char * filename, location**, vrp_info**);

#endif /* VRP_PARSER_H_ */
