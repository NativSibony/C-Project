#pragma once
/*
 ============================================
 Project Name : HW3_NativSibonyAndNoaAviel.c
 Authors      : Nativ Sibony & Noa Aviel
 Description  : Home Work 3 in C
 ============================================
 */
#define _CRT_SECURE_NO_WARNINGS
#define ADD_FLIGHT 1
#define ADD_AIR 2
#define PRINT_AIRLINE 3
#define PRINT_AIR_MAN 4
#define PRINT_BETWEEN 5
#define SORT 6
#define SEARCH 7
#define EXIT -1

#define B_FILE "company.bin"
#define T_FILE "airport_authority.txt"
#include "AirportManager.h"
#include "Airline.h"

int menu();

int main()
{
	Airport_Manager air_man;
	Airline airline;

	//Initialize from user input	--> HW2
	//init_airport_manager(&air_man);
	//init_airline(&airline);


	//Initialize from a given files	--> HW3
	init_airport_from_file(&air_man, T_FILE);
	init_airline_from_file(&airline, &air_man, B_FILE);

	int option;
	int stop = 0;
	do
	{
		option = menu();
		switch (option)
		{
		case ADD_FLIGHT:
			add_flight(&airline, &air_man);
			break;
		case ADD_AIR:
			add_airport(&air_man);
			break;
		case PRINT_AIRLINE:
			print_airline(&airline);
			break;
		case PRINT_AIR_MAN:
			general_array_function(&air_man, 1, sizeof(Airport_Manager), print_airport_manager);
			break;
		case PRINT_BETWEEN:
			print_flights_in_line(&airline);
			break;
		case SORT:
			sort_arr(&airline);
			break;
		case SEARCH:
			search_flight(&airline);
			break;
		case EXIT:
			if (airline.num_of_flights > 2)
				take_off(airline.flight[0], airline.flight[1], airline.flight[2], NULL);
			write_airport_to_file(&air_man, T_FILE);
			write_flight_to_file(&airline, B_FILE);
			general_array_function(&airline, 1, sizeof(Airline), free_airline);
			general_array_function(&air_man, 1, sizeof(Airport_Manager), free_airport);
			printf("Bye bye\n");
			stop = 1;
			break;
		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);
	return 0;
}
int menu()
{
	int option;
	printf("\n\n");
	printf("Please choose one of the following options\n");
	printf("%d - Add flight to the airline.\n", ADD_FLIGHT);
	printf("%d - Adding an airport.\n", ADD_AIR);
	printf("%d - Print airline data\n", PRINT_AIRLINE);
	printf("%d - Print airport data.\n", PRINT_AIR_MAN);
	printf("%d - Print number of flights between 2 airports.\n", PRINT_BETWEEN);
	printf("%d - Sorting flights by given feature in ascending order.\n", SORT);
	printf("%d - Search for an airline flight by the feature that the array is sorted.\n", SEARCH);
	printf("%d - Quit\n", EXIT);
	if (scanf("%d", &option) == 0)
		return -1;
	return option;
}
