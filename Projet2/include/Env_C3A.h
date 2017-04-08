#ifndef DICT_HEADER
#define DICT_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define false 0
#define true 1

/**
* Env_C3Aironnement de variables (Table de Hash)
**/
struct Env_C3A
{
    unsigned long *keys;
    int *values;
    int length;
};

unsigned long Env_C3A_hash(char *str);

/**
* Initialise un nouvel Env_C3Aironnement
* env : Env_C3Aironnement à initialiser
*/
struct Env_C3A* Env_C3A_init();
/**
* Affiche un environnement 
* env : environnement à afficher
**/
void Env_C3A_print(struct Env_C3A *env);
/**
* Test si la clef key existe 
* env : environnement à tester 
* key : clef dont il est nécessaire de tester l'existence
*/
char Env_C3A_key_exists(struct Env_C3A *env, char* key);


char Env_C3A_key_exists_hash(struct Env_C3A *env, unsigned long hash);
/**
* Ajoute une nouvelle entrée dans l'environnement
* env : environnement dans lequel ajouter 
* key : clef de la valeur à ajouter 
* value : valeur à ajouter 
*/
void Env_C3A_add_value(struct Env_C3A *env, char* key, int value);

void Env_C3A_add_value_hash(struct Env_C3A *env, unsigned long hash, int value);

/**
* Récupère une valeur à partir de sa clef 
* env : environnement dans lequel récupérer 
* key : clef de la valeur à récupérer
*/
int Env_C3A_get_value(struct Env_C3A *env, char* key);

/**
* Change/ajoute une valeur 
* env: environnement à modifier 
* key : clef de la valeur 
* value: valeur 
*/
void Env_C3A_set_value(struct Env_C3A *env, char* key, int value);

void Env_C3A_set_value_hash(struct Env_C3A *env, unsigned long hash, int value);




void Env_C3A_free(struct Env_C3A* env);

void Env_C3A_pop(struct Env_C3A* env, unsigned long* hash, int* value);

#endif