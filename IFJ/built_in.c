// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "built_in.h"

/*
 * boolVal vestavena funkce, vrati term hodnotu konvertovanou na bool
 * @param TToken *data - struktura pro vsechny mozne datove typy (obsahuje union SData)
 * @return - bool
 */
bool bool_val(TToken *data){
	switch(data->type){
		case _BOOLVALUE:
			return data->value.boolVal;
			break;

		case _INTVALUE:
			if(data->value.intVal != 0)
				return true;
			else
				return false;
			break;

		case _DOUBLEVALUE:
			if(data->value.doubleVal != 0.0)
				return true;
			else
				return false;
			break;
	
		case _STRINGVALUE:
			if(data->value.stringVal != NULL && data->value.stringVal[0] != '\0')
				return true;
			else
				return false;
			break;
	
		case _NULLVALUE:
			return false;
			break;

		default:
			return false;
			break;
	}
}

/*
 * vrati hodnotu term konvertovanou na retezec
 * @param TToken *data - struktura pro vsechny mozne datove typy (obsahuje union SData)
 * @param int *errno - v pripade chyby vraci chybu
 * @return - char *
 */
char *str_val(TToken *data, int *errno){
	char *temp = NULL;
	int pom = 0;
	switch(data->type){
		case _INTVALUE:		
			if((temp = malloc(_LENGTH)) == NULL){
				*errno = 99;
				return NULL;
			}
			snprintf(temp,_LENGTH,"%d",data->value.intVal);	//do temp ulozi celociselnou hodnotu
			pom=strlen(temp);
			if((temp=realloc(temp,pom+1)) == NULL){
				*errno = 99;
				return NULL;
			}
			return temp;
			break;
		
		case _DOUBLEVALUE:
			if((temp = malloc(_LENGTH)) == NULL){
				*errno = 99;
				return NULL;
			}
			snprintf(temp,_LENGTH,"%g",data->value.doubleVal);	//do temp ulozi hodnotu typu double
			pom=strlen(temp);
			if((temp=realloc(temp,pom+1)) == NULL){
				*errno = 99;
				return NULL;
			}
			return temp;
			break;
		
		case _STRINGVALUE:
			if(data->value.stringVal != NULL){
				if((temp = malloc(strlen(data->value.stringVal) + 1)) == NULL){
					*errno = 99;
					return NULL;
				}
				strcpy(temp, data->value.stringVal);
				return temp;
			}
			else{
				return NULL;
			}
			break;

		case _BOOLVALUE:
			if(data->value.boolVal == true){
				if((temp = malloc(2)) == NULL){
					*errno = 99;
					return temp;
				}
				strcpy(temp,"1");
				return temp;
			}
			else{
				if((temp = malloc(1)) == NULL){
					*errno = 99;
					return NULL;
				}
				strcpy(temp,"");
				return temp;
			}
			break;

		case _NULLVALUE:
			if((temp = malloc(1)) == NULL){
				*errno = 99;
				return NULL;
			}
			strcpy(temp,"");
			return temp;
			break;

		default:
			return NULL;
			break;
	}
}

/*
 * vrati hodnotu konvertovanou na double
 * @param TToken *data - token obsahujici union s daty
 * @param int *errno - odkazem predana chyba
 * @return - double
 */
double double_val(TToken *data, int *errno){
	int i = 0;
	double vysl = 0.0;
	int exp_znamenko = 1;
	int exp = 0;
	switch(data->type){
		case _BOOLVALUE:
			if(data->value.boolVal == true)
				return 1.0;
			else 
				return 0.0;
			break;

		case _INTVALUE:
			return (double)data->value.intVal;
			break;

		case _DOUBLEVALUE:
			return data->value.doubleVal;
			break;

		case _STRINGVALUE:
			if(data->value.stringVal == NULL){
				*errno = 99;
				return 0.0;
			}
			while(data->value.stringVal[i] == ' ' ||
				!isprint(data->value.stringVal[i])){	//preskoci bile uvozujici znaky
				if(data->value.stringVal[i] == '\0'){
					return 0.0;	//nebylo nacteno zadne cislo
				}
				i++;
			}
			
			if(isdigit(data->value.stringVal[i])){
				while(isdigit(data->value.stringVal[i])){	//nacte cislo
					vysl = vysl * 10 + (data->value.stringVal[i] - '0');
					i++;
				}
				if(data->value.stringVal[i] == '.'){	//123.
					i = i+1;
					if(isdigit(data->value.stringVal[i])){	//123.123
						for(int b = -1; isdigit(data->value.stringVal[i]); b--){
							vysl = vysl + ((data->value.stringVal[i] - '0') * pow(10,b));
							i++;
						}
					}
					else{
						*errno = 11;
						return 0.0; //nevyhovujici
					}

					if(data->value.stringVal[i] == 'e' || data->value.stringVal[i] == 'E'){	//123.123e
						i = i+1;
						if(data->value.stringVal[i] == '+'){	//123.123e+
						}
						else if(data->value.stringVal[i] == '-'){	//123.123e-
							exp_znamenko = -1;
						}
						else if(isdigit(data->value.stringVal[i])){	//123.123e123
							while(isdigit(data->value.stringVal[i])){
								exp = exp * 10 + (data->value.stringVal[i] - '0');
								i++;
							}
							return (vysl * pow(10, (exp * exp_znamenko)));
						}
						else{
							*errno = 11; //nevyhovujici 
							return 0.0;
						}
						i = i+1;
						if(isdigit(data->value.stringVal[i])){
							while(isdigit(data->value.stringVal[i])){	//123.123e+-123
								exp = exp * 10 + (data->value.stringVal[i] - '0');						
								i++;
							}
							return (vysl * pow(10, (exp * exp_znamenko)));
						}
						else{
							*errno = 11; //nevyhovujici 
							return 0.0;
						}
					}
					else{
						return vysl;	//pokud nacetno cislo a nasleduje jiny znak mozna error
					}
					
				}
				else if(data->value.stringVal[i] == 'e' || data->value.stringVal[i] == 'E'){	//123e
					i = i+1;
					if(data->value.stringVal[i] == '+'){	//123e+
					}
					else if(data->value.stringVal[i] == '-'){	//123e-
						exp_znamenko = -1;
					}
					else if(isdigit(data->value.stringVal[i])){	//123e123
						while(isdigit(data->value.stringVal[i])){
							exp = exp * 10 + (data->value.stringVal[i] - '0');
							i++;
						}
						return (vysl * pow(10, (exp * exp_znamenko)));
					}
					else{
						*errno = 11; //nevyhovujici 
						return 0.0;
					}
					i = i+1;
					if(isdigit(data->value.stringVal[i])){
						while(isdigit(data->value.stringVal[i])){	//123e+-123
							exp = exp * 10 + (data->value.stringVal[i] - '0');						
							i++;
						}
						return (vysl * pow(10, (exp * exp_znamenko)));
					}
					else{
						*errno = 11; //nevyhovujici 
						return 0.0;
					}
				}
				else{
					return vysl;	//pokud nacetno cislo a nasleduje jiny znak mozna error
				}
			}
			else{
			//	*errno = 11; //nevyhovujici
				return 0.0; 
			}
			break;

		case _NULLVALUE:
			return 0.0;
			break;

		default:
			*errno = 99;
			return 0.0;
			break;
	}
}


/*
 * standardni strlen()
 * @param char *str - retezec
 * @return int - delka retezce
 */
int str_len(TToken *data, int *errno){
	char *temp_str = NULL;
	temp_str = str_val(data, errno);

	if(temp_str == NULL){
		return 0;
	}

	int temp = 0;
	
	for(int i = 0; temp_str[i] != '\0'; i++){
		temp++;
	}
	free(temp_str);
	
	return temp;
}


/*
 * vrati podretezec v retezci mezi dvema pozicemi v retezci
 * @param char *str - retezec
 * @param int index_from - pozice kde zacina podretezec
 * @param int index_to - pozice za poslednim znakem
 * @param int *errno - predani chyby
 * @return char * - vrati dany podretezec
 */
char *get_substring(char *str, int index_from, int index_to, int *errno){
	if(index_from < 0 || index_to < 0 || index_from > index_to){
		*errno = 13;
		return NULL;
	}

	if(index_to >= (int)strlen(str) || index_to > (int)strlen(str)){
		*errno = 13;
		return NULL;
	}

	char *temp_str = NULL;
	if((temp_str = malloc((index_to - index_from) + 1)) == NULL){
		*errno = 99;
		return NULL;
	}
	
	for(int i = index_from; i < index_to; i++){
		temp_str[i-index_from] = str[i];
	}

	temp_str[(index_to - index_from)] = '\0';

	return temp_str;
}


/*
 * funkce prevede z term na int
 * @param TToken *data - struktura s term
 * @return int - vysledne cele cislo
 */
int int_val(TToken *data){
	int temp = 0;
	int counter = 0;
	switch(data->type){
		case _INTVALUE:
			return data->value.intVal;
			break;

		case _DOUBLEVALUE:
			return (int)data->value.doubleVal;
			break;

		case _BOOLVALUE:
			if(data->value.boolVal)
				return 1;
			else
				return 0;
			break;

		case _STRINGVALUE:
			if(data->value.stringVal == NULL){
				return 0;
			}

			for(int i = 0; data->value.stringVal[i] != '\0'; i++){
				if(i == 0)
					while(data->value.stringVal[i] == ' ' ||
					!isprint(data->value.stringVal[i])){
						if(data->value.stringVal[i] == '\0'){
							return 0;
						}
						i++;
					}

				if(data->value.stringVal[i] >= '0' &&
				data->value.stringVal[i] <= '9'){
					temp = temp * 10 + (data->value.stringVal[i] - '0');
					counter++;
				}
				else if(counter){
					return temp;
				}
				else
					return 0;
			}

			return temp;
			break;

		default:
			return 0;
			break;

	}
}

/*
 * funkce pro konkatenaci retezce ("retezec1retezec2")
 * @param char *str1 - prvni retezec
 * @param char *str2 - druhy retezec
 * @return char * - konkatenovany retezec
 */
char *konkatenace(char *str1, char *str2){
	if(str1 == NULL || str2 == NULL){
		return NULL;
	}

	char *temp = NULL;
	if((temp = malloc((strlen(str1) + strlen(str2)) + 1)) == NULL){
		return NULL;
	}

	strcpy(temp, str1);
	strcat(temp, str2);
	return temp;
}

/*
 * zabudovana funkce getstring()
 * @return char * - jeden radek nacteny ze stdin
 */
char *get_string(void){
	char c = 0;
	int i;
	char *temp_string = NULL;
	if((temp_string = malloc(sizeof(char) * BUFFER)) == NULL){
		return NULL; //nepovedlo se alokovat
	}

	for(i = 0; (c = fgetc(stdin)) != '\n' && c != EOF; i++){
		temp_string[i] = c;
		if((i != 0) && (((i + 1) % BUFFER) == 0)){
			if((temp_string = realloc(temp_string, (i + 1) + BUFFER)) == NULL){
				return NULL; //nepovedlo se alokovat
			}
		}
	}

	if(!(i % BUFFER)){
		if((temp_string = realloc(temp_string, i + 1)) == NULL){
			return NULL; //nepovedlo se alokovat
		}
	}

	temp_string[i] = '\0';
	return temp_string;
}
