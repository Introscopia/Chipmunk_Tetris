#ifndef FUNDAMENTALS_H_INCLUDED
#define FUNDAMENTALS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <SDL.h>

extern const double PHI;
extern const double TWO_PI;
extern const double PI;
extern const double HALF_PI;
extern const double THIRD_PI;
extern const double QUARTER_PI;
extern const double ONE_OVER_PI;

//const SDL_Color black = {0, 0, 0, 255};
//const SDL_Color white = {255, 255, 255, 255};

Uint32 rmask, gmask, bmask, amask;

typedef char bool;

double sq( double a );

/* RANDOM: min inclusive, max not inclusive
	int co  [7];
    for (int i = 0; i < 7; ++i) co[i] = 0;
    for (int i = 0; i < 1000; ++i){
       int x = random(1, 8);
       co[x-1]++;
    }
    for (int i = 0; i < 7; ++i) printf("%d: %d, ", i+1, co[i] );
    puts(".");
*/
int random( int min, int max );

float lerp(float start, float stop, float amt);

float map(float value, float start1, float stop1,  float start2, float stop2);

int constrain( int a, int min, int max );

float constrainf( float a, float min, float max );

double degrees( double radians );

// String split
void strspl( char *string, const char *delimiters, char ***list, int *size );
// String Count character
int strcchr( char *string, char C );
// sub-string
char * substr( char *string, int start, int stop );

bool cursor_in_rect( SDL_MouseButtonEvent button, SDL_Rect rect );
bool coordinates_in_rect( float x, float y, SDL_Rect rect );

bool is_alphanumeric( char c );
bool is_lower_case( char c );
bool is_upper_case( char c );
bool is_numeral( char c );

typedef struct {
    int i, j;
} index2d;

#endif