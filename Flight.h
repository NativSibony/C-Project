#pragma once
#include "AirportManager.h"

typedef struct {
	int dd;
	int mm;
	int yy;
}Date;

typedef struct {
	Airport* from;//origin
	Airport* to;//destination *cant be equale to origin airport
	int time;
	Date date;//Changed by instructions
	double mph;
}Flight;

//Check
int is_flight_exist(Flight* flight, char* origin, char* destination);

//Calcs
void flight_duration(double mph, double dist);
int count_flights_in_line(Flight** flight, char* origin, char* destination, int size);
