#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>

#define PI 3.14159265359
#define CODE_LEN 3
#define MAX_LEN 100
#define DATE 11
typedef struct {
	char* name;
	char* country;
	char IATA[CODE_LEN + 1];//TO CHANGE BACK TO "CODE_LEN"
	double lat;
	double lon;
}Airport;
//General 
void general_array_function(void* arr, int size, int type_size, void (*print)(void*));

//Check
int is_identical_airport(Airport* one, Airport* two);
int is_identical_code(Airport* air, char* code);
int is_valid_coordinates(double value, int type);
int is_all_capitals(char* str);
int is_valid_code(char* code);

//Utils
int get_airport_from_user(Airport* air, int pos);
char* get_str_exact_len(int opt);
int get_coordinates(double* lat, double* lon);
int get_code(char* code, Airport* air, int pos);
char* my_gets(char* buf, size_t size);
double to_radians(const double degree);
double distance(double lat1, double long1, double lat2, double long2);
void fix_string(char* str);