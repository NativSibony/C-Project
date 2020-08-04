#include "Flight.h"
//calculating flight duration
void flight_duration(double mph, double dist)
{
	double time = dist / mph;
	int time_mins = (int)floor(time * 60.0);
	int hours = time_mins / 60;
	int minutes = time_mins % 60;
	printf("%d:%d hours\n", hours, minutes);
}
//checking if flight is in system
int is_flight_exist(Flight* flight, char* origin, char* destination)
{
	if (strcmp(flight->from->IATA, origin) != 0 || strcmp(flight->to->IATA, destination) != 0)
		return 0;
	return 1;
}
//counting amount of flight in line between 2 airports
int count_flights_in_line(Flight** flight, char* origin, char* destination, int size)
{
	int i, count = 0;
	for (i = 0; i < size; i++)
	{
		if (is_flight_exist(flight[i], origin, destination) == 1)
			count++;
	}
	return count;
}

