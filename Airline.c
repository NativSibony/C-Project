#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define BY_TIME 1
#define BY_DATE 2
#define BY_SPEED 3
#include "Airline.h"

//initialaizing airline
int init_airline(Airline* airline)
{
	airline->num_of_flights = 0;
	airline->flight = NULL;
	getchar();
	airline->name = get_str_exact_len(2);
	fix_string(airline->name);
	airline->name[0] = toupper(airline->name[0]);//just in case...
	printf("How many airplanes in %s company?:\n", airline->name);
	if (!scanf("%d", &airline->num_of_planes))
		return 0;
	return 1;
}

//Initializing airline struct from a binary file 
int init_airline_from_file(Airline* airline, Airport_Manager* air_man, char* file_name)
{
	int arr[2];

	FILE* fp = fopen(file_name, "rb");
	if (!fp)
		return 0;
	fread(arr, sizeof(int), 2, fp);
	airline->num_of_planes = arr[0];
	airline->name = (char*)malloc(sizeof(Airline) * arr[1]);
	fread(airline->name, sizeof(char), arr[1], fp);
	if (!airline->flight)
		return 0;
	if (fread(arr, sizeof(int), 2, fp) != 2)
		return 0;
	airline->num_of_flights = arr[0];
	airline->flight = (Flight**)malloc(sizeof(Flight*) * airline->num_of_flights);
	airline->the_type = (sort_type)arr[1];
	init_flights(airline, air_man, fp);
	fclose(fp);
	return 1;
}
//Initializing inner struct 'Flight**' to airline
int init_flights(Airline* airline, Airport_Manager* air_man, FILE* fp)
{
	int i;
	char dest[MAX_LEN], orig[MAX_LEN];
	Airport* tmp_port1 = NULL, * tmp_port2 = NULL;
	for (i = 0; i < airline->num_of_flights; i++)
	{
		airline->flight[i] = (Flight*)malloc(sizeof(Flight));
		fread(dest, sizeof(char), CODE_LEN + 1, fp);
		fread(orig, sizeof(char), CODE_LEN + 1, fp);
		tmp_port1 = get_airport(air_man, dest);
		if (!tmp_port1)
			return 0;
		tmp_port2 = get_airport(air_man, orig);
		if (!tmp_port2)
			return 0;
		airline->flight[i]->from = (Airport*)malloc(sizeof(Airport));
		airline->flight[i]->from = tmp_port1;
		airline->flight[i]->to = tmp_port2;
		fread(&airline->flight[i]->time, sizeof(int), 1, fp);
		fread(&airline->flight[i]->date, sizeof(Date), 1, fp);
		fread(&airline->flight[i]->mph, sizeof(double), 1, fp);
	}
	return 1;
}
//Updating new data to the binary file
void write_flight_to_file(Airline* airline, char* file_name)
{
	FILE* fp = fopen(file_name, "wb");
	int i;
	int sort_type = (int)airline->the_type;
	int name_len = strlen(airline->name) + 1;//+1 for '\0'
	if (!fp)
		return;
	fwrite(&airline->num_of_planes, sizeof(int), 1, fp);
	fwrite(&name_len, sizeof(int), 1, fp);
	fwrite(airline->name, sizeof(char), name_len, fp);
	fwrite(&airline->num_of_flights, sizeof(int), 1, fp);
	fwrite(&sort_type, sizeof(int), 1, fp);
	for (i = 0; i < airline->num_of_flights; i++)
	{
		fwrite(airline->flight[i]->from->IATA, sizeof(char), CODE_LEN + 1, fp);
		fwrite(airline->flight[i]->to->IATA, sizeof(char), CODE_LEN + 1, fp);
		fwrite(&airline->flight[i]->time, sizeof(int), 1, fp);
		fwrite(&airline->flight[i]->date, sizeof(Date), 1, fp);
		fwrite(&airline->flight[i]->mph, sizeof(double), 1, fp);
	}
	fclose(fp);
}

Airport* get_airport(Airport_Manager* air_man, char* code)
{
	Airport* my_air = NULL;
	int i;
	for (i = 0; i < air_man->num_of_aPorts; i++)
	{
		my_air = &air_man->air[i];
		if (strcmp(code, air_man->air[i].IATA) == 0)
			return my_air;
	}
	return NULL;
}

//adding flight to airport manager
int add_flight(Airline* airline, const Airport_Manager* air_man)
{
	airline->flight = (Flight**)realloc(airline->flight, sizeof(Flight*) * (airline->num_of_flights + 1));
	if (!airline->flight)
		return 0;
	int res = get_flight_from_user(airline, air_man);
	if (!res)
	{
		printf("Error, could not add flight.");
		return 0;
	}
	airline->num_of_flights = airline->num_of_flights + 1;
	//Default sortion by time for the first run
	//In case search is activate by time so you can avoid an exeption
	qsort(airline->flight, airline->num_of_flights, sizeof(Flight*), compare_by_time);
	airline->the_type = 0;
	return 1;
}
//getting flight values from user
int get_flight_from_user(Airline* airline, const Airport_Manager* air_man)
{
	int pos1, pos2, res;
	double mph;
	int time;
	airline->flight[airline->num_of_flights] = (Flight*)malloc(sizeof(Flight));
	if (!airline->flight[airline->num_of_flights])
		return 0;
	res = is_origin_dest_valid(air_man, &pos1, &pos2);
	if (res == 0)
		return 0;
	airline->flight[airline->num_of_flights]->from = (Airport*)malloc(sizeof(Airport));
	if (!airline->flight[airline->num_of_flights]->from)
		return 0;
	*airline->flight[airline->num_of_flights]->from = air_man->air[pos1];
	airline->flight[airline->num_of_flights]->to = (Airport*)malloc(sizeof(Airport));
	if (!airline->flight[airline->num_of_flights]->from)
		return 0;
	*airline->flight[airline->num_of_flights]->to = air_man->air[pos2];
	get_date(&airline->flight[airline->num_of_flights]->date);
	is_valid_mph(&mph);
	airline->flight[airline->num_of_flights]->mph = mph;
	is_valid_flight_time(&time);
	airline->flight[airline->num_of_flights]->time = time;
	return 1;
}
//checking if origin and destination insertion is valid
int is_origin_dest_valid(const Airport_Manager* air_man, int* pos1, int* pos2)
{
	int one, two;
	int bool = 0;
	getchar();
	one = get_origin_iata(air_man);
	while (!bool)
	{
		if (one == -1)
		{
			printf("Origin IATA was not found or illeagal! (All Capitals)\n");
			fflush(stdin);
			one = get_origin_iata(air_man);
		}
		else
			bool = 1;
	}
	bool = 0;
	two = get_dest_iata(air_man);
	while (!bool)
	{
		if (two == -1)
		{
			printf("Destination IATA was not found or illeagal! (All Capitals)\n");
			fflush(stdin);
			two = get_dest_iata(air_man);
		}
		else
			bool = 1;
	}
	if (is_identical_airport(&air_man->air[one], &air_man->air[two]) == 1)
	{
		printf("Destination and Origin cannot be identical!\n");
		return 0;
	}
	else
		printf("\nFlight was added successfully\n");
	*pos1 = one;
	*pos2 = two;
	return 1;
}
//getting iata of origin airport
int get_origin_iata(const Airport_Manager* air_man)
{
	int i;
	char origin[MAX_LEN];
	char* str;
	printf("Enter origin IATA code:\n");
	fgets(origin, MAX_LEN + 1, stdin);
	str = _strdup(origin);
	str[strcspn(str, "\n")] = 0;
	if (!is_valid_code(str) || !is_all_capitals(str))
		return -1;
	for (i = 0; i < air_man->num_of_aPorts; i++)
		if (is_identical_code(&air_man->air[i], str) == 1)
			return i;
	return -1;
}
//get iata of destination
int get_dest_iata(const Airport_Manager* air_man)
{
	int i;
	char dest[MAX_LEN];
	char* str;
	printf("Enter destination IATA code:\n");
	fgets(dest, MAX_LEN, stdin);
	str = _strdup(dest);
	str[strcspn(str, "\n")] = 0;
	if (!is_valid_code(str) || !is_all_capitals(str))
		return -1;
	for (i = 0; i < air_man->num_of_aPorts; i++)
		if (is_identical_code(&air_man->air[i], str) == 1)
			return i;
	return -1;
}
//getting date from user
int get_date(Date* date)
{
	printf("Enter date (DD/MM/YYYY format): ");
	int dd, mm, yy;
	int bool = 0;
	while (!bool)
	{
		if (!scanf("%d/%d/%d", &dd, &mm, &yy))
			return 0;
		if (!is_valid_date(dd, mm, yy, date))
		{
			printf("Enter date (DD/MM/YYYY format): ");
		}
		else
			bool = 1;
	}
	return 1;
}
//validating date format 
int is_valid_date(int dd, int mm, int yy, Date* date)
{
	char str[12];
	int bool = 0;
	while (!bool)
	{
		//check year
		if (yy >= 2020 && yy <= 2050)
		{
			//check month
			if (mm >= 1 && mm <= 12)
			{
				//check days
				if ((dd >= 1 && dd <= 31) && (mm == 1 || mm == 3 || mm == 5 || mm == 7 || mm == 8 || mm == 10 || mm == 12))
					printf("Date is valid.\n");
				else if ((dd >= 1 && dd <= 30) && (mm == 4 || mm == 6 || mm == 9 || mm == 11))
					printf("Date is valid.\n");
				else if ((dd >= 1 && dd <= 28) && (mm == 2))
					printf("Date is valid.\n");
				else
				{
					printf("Day is invalid.\n");
					return 0;
				}
			}
			else
			{
				printf("Month is not valid.\n");
				return 0;
			}
		}
		else
		{
			printf("Year is not valid.\n");
			return 0;
		}
		bool = 1;
	}
	sprintf(str, "%d/%d/%d", dd, mm, yy);
	sscanf(str, "%d/%d/%d", &date->dd, &date->mm, &date->yy);
	return 1;
}
//validating mph
void is_valid_mph(double* mph)
{
	int bool = 0;
	printf("Please enter MPH of the plane: ");
	scanf("%lf", mph);
	while (!bool)
	{
		if (*mph < 1 || *mph > 10000)
		{
			printf("MPH must be greater than 0 and lower than 10,000\n");
			printf("Please enter MPH of the plane: ");
			scanf("%lf", mph);
		}
		else
			bool = 1;
	}
}
// checking if flight time is valid
int is_valid_flight_time(int* num)
{
	int bool = 0;
	printf("Enter flight time: ");
	scanf("%d", num);
	while (!bool)
	{
		if (*num > 23 || *num < 0)
		{
			printf("Wrong input! Time must be between 0 - 23\n");
			printf("Enter flight time: ");
			scanf("%d", num);
		}
		else
			bool = 1;
	}
	return 1;
}
//print airline
void print_airline(Airline* airline)
{
	int i;
	char* str[] = { "Time","Date","Speed" };
	double dist;
	for (i = 0; i < airline->num_of_flights; i++)
	{
		printf("---------------------Flight Number %d---------------------\n", i + 1);
		printf("Flight Date: %d/%d/%d\n", airline->flight[i]->date.dd, airline->flight[i]->date.mm, airline->flight[i]->date.yy);
		printf("From: %s (IATA: %s)\n", airline->flight[i]->from->country, airline->flight[i]->from->IATA);
		printf("To: %s (IATA: %s)\n", airline->flight[i]->to->country, airline->flight[i]->to->IATA);
		dist = distance(airline->flight[i]->from->lat, airline->flight[i]->from->lon,
			airline->flight[i]->to->lat, airline->flight[i]->to->lon);
		printf("Flight Time: %d\n", airline->flight[i]->time);
		printf("Flight Distance: %.2lf Miles\n", dist);
		printf("Flight Duration: ");
		flight_duration(airline->flight[i]->mph, dist);
		printf("\n");
	}
	if (airline->num_of_flights > 0)
		printf("Flights Sorted By:\t%s", str[airline->the_type]);
}
//free flights & airports from the program 
void free_airline(void* airline)
{
	Airline* air = (Airline*)airline;
	int i;
	for (i = 0; i < air->num_of_flights; i++)
	{
		free(air->flight[i]);
	}
	free(air->flight);
}

void free_airport(void* air_man)
{
	Airport_Manager* sherman = (Airport_Manager*)air_man;
	free(sherman->air);
}
//printing flights
void print_flights_in_line(Airline* airline)
{
	int res, bool = 0;
	char* orig = NULL, * dest = NULL;
	getchar();
	orig = get_str_exact_len(3);
	while (!bool)
	{
		if (orig == NULL)
		{
			printf("Origin IATA was not found or illeagal! (All Capitals)\n");
			orig = get_str_exact_len(3);
		}
		else
			bool = 1;
	}
	bool = 0;
	dest = get_str_exact_len(4);
	while (!bool)
	{
		if (dest == NULL)
		{
			printf("Destination IATA was not found or illeagal! (All Capitals)\n");
			dest = get_str_exact_len(4);
		}
		else
			bool = 1;
	}
	res = count_flights_in_line(airline->flight, orig, dest, airline->num_of_flights);
	printf("\nThere is %d amount of flights between %s to %s", res, orig, dest);
}
//Comparators
int compare_by_time(const void* fl1, const void* fl2)
{
	Flight* f1 = *(Flight**)fl1;
	Flight* f2 = *(Flight**)fl2;
	return f1->time - f2->time;
}

int compare_by_date(const void* fl1, const void* fl2)
{
	Flight* pF1 = *(Flight**)fl1;
	Flight* pF2 = *(Flight**)fl2;
	if (pF1->date.yy < pF2->date.yy)
		return -1;
	else if (pF1->date.yy > pF2->date.yy)
		return 1;
	if (pF1->date.yy == pF2->date.yy)
	{
		if (pF1->date.mm < pF2->date.mm)
			return -1;
		else if (pF1->date.mm > pF2->date.mm)
			return 1;
		else if (pF1->date.dd < pF2->date.dd)
			return -1;
		else if (pF1->date.dd > pF2->date.dd)
			return 1;
		else
			return 0;
	}
	return 0;
}

int compare_by_speed(const void* fl1, const void* fl2)
{
	Flight* pF1 = *(Flight**)fl1;
	Flight* pF2 = *(Flight**)fl2;
	if (pF1->mph > pF2->mph)
		return 1;
	if (pF1->mph < pF2->mph)
		return -1;
	return 0;
}
//Sort Array Menu
void sort_arr(Airline* airline)
{
	int size = airline->num_of_flights;
	int option;
	int stop = 0;
	do
	{
		option = mini_menu(0);
		switch (option)
		{
		case BY_TIME:
			qsort(airline->flight, size, sizeof(Flight*), compare_by_time);
			airline->the_type = 0;
			stop = 1;
			printf("Array sorted by time successfully!");
			break;
		case BY_DATE:
			qsort(airline->flight, size, sizeof(Flight*), compare_by_date);
			airline->the_type = 1;
			stop = 1;
			printf("Array sorted by date successfully!");
			break;
		case BY_SPEED:
			qsort(airline->flight, size, sizeof(Flight*), compare_by_speed);
			airline->the_type = 2;
			stop = 1;
			printf("Array sorted by speed successfully!");
			break;
		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);
}

int mini_menu(int indicator)
{
	if (indicator == 0)
		printf("Choose which way to sort the array:\n");
	else
		printf("Choose which way to search:\n");
	int option;
	printf("1. By Time.\n");
	printf("2. By Date.\n");
	printf("3. By Speed.\n");
	if (scanf("%d", &option) == 0)
		return -1;
	return option;
}

void search_flight(Airline* airline)
{
	int size = airline->num_of_flights;
	int option;
	Flight* the_flight;
	Flight* temp = (Flight*)malloc(sizeof(Flight));
	int stop = 0;
	sort_type curr_type = airline->the_type;
	do
	{
		option = mini_menu(1);
		switch (option)
		{
		case BY_TIME:
			if (airline->num_of_flights > 1)
			{

				if (curr_type == (BY_TIME - 1))
				{
					printf("\nEnter flight time to search\n");
					scanf("%d", &temp->time);
					the_flight = *(Flight**)bsearch(&temp, airline->flight, size, sizeof(Flight*), compare_by_time);
					print_specific_flight(the_flight);
				}
				else
					printf("Array is not sorted by time!\nPlease first sort the array by time and search again...\n");
			}
			else
				printf("There is only one flight in the program.");
			stop = 1;
			break;
		case BY_DATE:
			if (airline->num_of_flights > 1)
			{
				if (curr_type == (BY_DATE - 1))
				{
					printf("\nEnter flight date to search\n");
					get_date(&temp->date);
					the_flight = *(Flight**)bsearch(&temp, airline->flight, size, sizeof(Flight*), compare_by_date);
					print_specific_flight(the_flight);
				}
				else
					printf("Array is not sorted by date!\nPlease first sort the array by date and search again...\n");
			}
			else
				printf("There is only one flight in the program.");
			stop = 1;
			break;
		case BY_SPEED:
			if (airline->num_of_flights > 1)
			{
				if (curr_type == (BY_SPEED - 1))
				{
					printf("\nEnter flight speed to search\n");
					scanf("%lf", &temp->mph);
					the_flight = *(Flight**)bsearch(&temp, airline->flight, size, sizeof(Flight*), compare_by_speed);
					print_specific_flight(the_flight);
				}
				else
					printf("Array is not sorted by speed!\nPlease first sort the array by speed and search again...\n");
			}
			else
				printf("There is only one flight in the program.");
			stop = 1;
			break;
		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);
	free(temp);
}

void print_specific_flight(Flight* the_flight)
{
	double dist;
	int res = is_flight_exist(the_flight, the_flight->from->IATA, the_flight->to->IATA);
	if (the_flight != NULL && res == 1)
	{
		printf("\nFlight Was Found Successfully!\n\n");
		printf("-------------Flight Details-------------\n");
		printf("Flight Date: %d/%d/%d\n", the_flight->date.dd, the_flight->date.mm, the_flight->date.yy);
		printf("From: %s (IATA: %s)\n", the_flight->from->country, the_flight->from->IATA);
		printf("To: %s (IATA: %s)\n", the_flight->to->country, the_flight->to->IATA);
		dist = distance(the_flight->from->lat, the_flight->from->lon,
			the_flight->to->lat, the_flight->to->lon);
		printf("Flight Time: %d\n", the_flight->time);
		printf("Flight Distance: %.2lf Miles\n", dist);
		printf("Flight Duration: ");
		flight_duration(the_flight->mph, dist);
		printf("\n");
	}
	else
		printf("Flight Was Not Found...\n");
}

void take_off(Flight* flight, ...)
{
	va_list params;
	Flight* curr_flight;
	va_start(params, flight);
	curr_flight = flight;

	while (curr_flight != NULL)
	{
		printf("Flight from %s to %s\nIs taking off at: %d\n", curr_flight->from->country,
			curr_flight->to->country, curr_flight->time);
		curr_flight = va_arg(params, Flight*);
	}
	free(curr_flight);
	va_end(params);
}
