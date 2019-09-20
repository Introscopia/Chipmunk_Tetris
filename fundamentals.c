#include "fundamentals.h"

const double PHI         = 1.618033988749894848204586834365638117720309179805762862135;
const double TWO_PI      = 6.283185307179586476925286766559005768394338798750211641949;
const double PI          = 3.141592653589793238462643383279502884197169399375105820974;
const double HALF_PI     = 1.570796326794896619231321691639751442098584699687552910487;
const double THIRD_PI    = 1.047197551196597746154214461093167628065723133125035273658;
const double QUARTER_PI  = 0.785398163397448309615660845819875721049292349843776455243;
const double ONE_OVER_PI = 0.318309886183790671537767526745028724068919291480912897495;

double sq( double a ){
	return a * a;
}

int random( int min, int max ){
    return (rand() % (max-min)) + min;
}

float lerp(float start, float stop, float amt) {
    return start + (stop-start) * amt;
}

float map(float value, float start1, float stop1,  float start2, float stop2) {
	return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

int constrain( int a, int min, int max ){
	if( a < min ) return min;
	else if( a > max ) return max;
	else return a;
}
float constrainf( float a, float min, float max ){
	if( a < min ) return min;
	else if( a > max ) return max;
	else return a;
}

double degrees( double radians ){
	return radians * 57.29577951308232087679815481410517033240547246656432154916;//ONE_OVER_PI * 180;
}


void strspl( char *string, const char *delimiters, char ***list, int *size ){
	int ss = strlen( string );
	*list = (char**) malloc( ceil(0.5*ss) * sizeof(char*) );
	int sd = strlen( delimiters );
	*size = 0;

	bool *checks = (bool*) malloc( ss * sizeof(bool) );
	for( int i = 0; i < ss; ++i ){
		checks[i] = 0;
		for (int j = 0; j < sd; ++j){
			if( string[i] == delimiters[j] ){
				checks[i] = 1;
				break;
			}
		}
	}

	bool looking_for_first = 1;
	for( int i = 0; i < ss; ++i ){
		if( looking_for_first ){
			if( checks[i] ){
				string[i] = '\0';
			}
			else{
				(*list)[0] = string+i;
				(*size)++;
				looking_for_first = 0;
			}
		}
		else{
			if( checks[i] ){
				string[i] = '\0';
				if( i < ss-1 ){
					if( !checks[i+1] ){
						(*list)[*size] = string+i+1;
						(*size)++;
					}
				}
			}
		}
	}
	*list = (char**) realloc( *list, (*size) * sizeof(char*) );
	//apparently for strtok the string MUST be declared as "char string[]" in the calling function
	// it can't be a literal and it can't be "char *string"...
	/* TEST:
	char **list;
	int size = 0;
	char string[] = "split me baby one more time";
	split_string( string, " ", &list, &size );
	for (int i = 0; i < size; ++i){
	    printf("%s\n", list[i] );
	}
	*/
	// char * p = strtok (string, delimiters);
	// int i = 0;
	// while (p != NULL){
	// 	(*list)[i] = p;
	// 	p = strtok (NULL, delimiters);
	// 	++i;
	// }
}

int strcchr( char *string, char C ){ // String Count character
	int i = 0;
	int count = 0;
	while( string[i] != '\0' ){
		if( string[i] == C ) ++count;
	}
	return count;
}

// sub-string
char * substr( char *string, int start, int stop ){
	char *sub = (char*) calloc( stop-start +1, sizeof(char) );
	for (int i = start; i < stop; ++i){
		sub[i-start] = string[i];
	}
	sub[ stop-start ] = '\0';
	return sub;
}

bool cursor_in_rect( SDL_MouseButtonEvent button, SDL_Rect rect ){
	return ( button.x > rect.x && button.x < rect.x + rect.w ) && ( button.y > rect.y && button.y < rect.y + rect.h );
}

bool coordinates_in_rect( float x, float y, SDL_Rect rect ){
	return ( x > rect.x && x < rect.x + rect.w ) && ( y > rect.y && y < rect.y + rect.h );
}

bool is_alphanumeric( char c ){
  if( is_lower_case( c ) || is_upper_case( c ) || is_numeral( c ) || c == '-' ) return 1;// (int)c == 32 || // 32 is ' '
  else return 0;
}
bool is_lower_case( char c ){
  int n = (int)c;
  if( n >= 97 && n <= 122) return 1;
  else return 0;
}
bool is_upper_case( char c ){
  int n = (int)c;
  if( n >= 65 && n <= 90) return 1;
  else return 0;
}
bool is_numeral( char c ){
  int n = (int)c;
  if( (n >= 48 && n <= 57) || c == '.' ) return 1; // 46 is '.'
  else return 0;
}