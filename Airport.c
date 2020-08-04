#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Airport.h"

//Generic printing and releasing data
void general_array_function(void* arr, int size, int type_size, void (*action)(void*))
{
	action(arr);
}
//checking for identical codes
int is_identical_airport(Airport* one, Airport* two)
{
	if (strcmp(one->IATA, two->IATA) == 0)
		return 1;
	return 0;
}
// checking if code is compatible
int is_identical_code(Airport* air, char* code)
{
	if (strcmp(air->IATA, code) == 0)
		return 1;
	return 0;
}

// converting degrees to radians 
double to_radians(const double degree)
{
	long double one_deg = (PI) / 180;
	return (one_deg * degree);
}
// calculate distance
double distance(double lat1, double long1, double lat2, double long2)
{
	//Convert the lat and lon
	//from degree to radians. 
	lat1 = to_radians(lat1);
	long1 = to_radians(long1);
	lat2 = to_radians(lat2);
	long2 = to_radians(long2);

	//Haversine Formula 
	double dlong = long2 - long1;
	double dlat = lat2 - lat1;

	double ans = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlong / 2), 2);

	ans = 2 * asin(sqrt(ans));

	//Use R = 6371 for KM 
	double R = 3956;//in miles

	//Calculate the result 
	ans = ans * R;

	return ans;
}
//getting airport from user
int get_airport_from_user(Airport* air, int pos)
{
	int i;
	Airport res;
	get_code(res.IATA, air, pos);
	for (i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			res.name = get_str_exact_len(i);
			fix_string(res.name);
			res.name[0] = toupper(res.name[0]);//First letter to upepr
		}
		else
		{
			res.country = get_str_exact_len(i);
			fix_string(res.country);
			res.country[0] = toupper(res.country[0]);//First letter to upepr
		}
	}
	get_coordinates(&res.lat, &res.lon);
	air[pos] = res;
	return 1;
}
//getting exact length of string
char* get_str_exact_len(int opt)
{
	char* the_str = NULL;
	int len;
	char input_str[MAX_LEN];
	//Ask for a string from the user
	if (opt == 0)
		printf("Enter airport name: ");
	else if (opt == 1)
		printf("Enter airport country: ");
	else if (opt == 2)
		printf("Enter airline name: ");
	else if (opt == 3)
		printf("Enter Origine to check from: ");
	else
		printf("Enter Destination to check to: ");
	my_gets(input_str, sizeof(input_str));
	//find string size and add 1 for the '\0'
	len = strlen(input_str) + 1;
	//allocate a place for the string in the right location in the array 
	the_str = (char*)malloc(len * sizeof(char));
	//Copy the string to the right location in the array 
	if (the_str != NULL)
		strcpy(the_str, input_str);
	if (opt == 3 || opt == 4)
		if (!is_valid_code(input_str) || !is_all_capitals(input_str))
			return NULL;
	return the_str;
}
//getting fixed string
char* my_gets(char* buf, size_t size)
{
	fflush(stdin);
	if (buf != NULL && size > 0)
	{		
		if (fgets(buf, size, stdin))
		{
			buf[strcspn(buf, "\n")] = '\0';
			return buf;
		}
		*buf = '\0';
	}
	return NULL;
}
//getting iata code
int get_code(char* code, Airport* air, int pos)
{
	int bool = 0, i;
	int res;
	char input_buf[MAX_LEN] = { '\0' };
	res = getchar();
	while (!bool)
	{
		int flag_ident = 0;
		printf("Enter 3 letter IATA code: ");
		if (!fgets(input_buf, MAX_LEN - 1, stdin))
			return 0;
		input_buf[strcspn(input_buf, "\n")] = 0;
		for (i = 0; i < pos; i++)
		{
			if (is_identical_code(&air[i], input_buf) == 1)
			{
				printf("Code already exist! Try Again...\n");
				flag_ident = 1;
			}
		}
		if (!flag_ident && !bool)
		{
			if (is_valid_code(input_buf) == 0 || is_all_capitals(input_buf) == 0)
				printf("Wrong input! Try Again...\n");
			else
				bool = 1;
		}
	}
	strcpy(code, input_buf);
	return 1;
}
//checking if all letters in iata are capitals
int is_all_capitals(char* str)
{
	// counting for upper case
	char ch;
	int i = 0;
	ch = str[i];
	while (i < CODE_LEN)
	{
		ch = str[i++];
		if (!isupper(ch))
			return 0;
	}
	return 1;
}
//checking if iata code is in right length
int is_valid_code(char* code)
{
	if (strlen(code) <= 2 || strlen(code) >= 4)
		return 0;
	return 1;
}
//getting longtitude and latitude coordinates
int get_coordinates(double* lat, double* lon) {
	int bool = 0;
	double temp_lat = 0, temp_lon = 0;
	while (!bool) {
		printf("Enter latitude: ");
		if (!scanf("%lf", &temp_lat))
			return 0;
		printf("Enter longitude: ");
		if (!scanf("%lf", &temp_lon))
			return 0;
		//TO_DO -> split the validation of the coordinates
		if (is_valid_coordinates(temp_lat, 1) == 0)
			printf("\nWrong Input! Try Again...\n");
		else if (is_valid_coordinates(temp_lon, 2) == 0)
			printf("\nWrong Input! Try Again...\n");
		else
			bool = 1;
	}
	*lon = temp_lon;
	*lat = temp_lat;
	return 1;
}
//checking if coordinates are in range
int is_valid_coordinates(double value, int type) {
	if (type == 1)
	{
		if (value < -90 || value > 90)//checking for lat
			return 0;
		return 1;
	}
	else
		if (value > 180 || value < -180)//checking for lon
			return 0;
	return 1;
}

void fix_string(char* str) {
	int no_space = 1;
	char* pLook = str;
	char* pWrite = str;
	while (*pLook) {
		if (*pLook != ' ') {
			*pWrite = *pLook;
			pWrite++;
			no_space = 1;
		}
		else if ((*pLook == ' ') && (no_space)) {
			*pWrite = *pLook;
			pWrite++;
			no_space = 0;
		}
		pLook++;
	}
	*pWrite = '\0';
}

