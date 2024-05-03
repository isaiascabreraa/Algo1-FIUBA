#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dia_en_la_uni.h"
#include "papeleo.h"

int main(){

srand((unsigned)time(NULL));

juego_t juego;
char personaje_tp1 = ' ';

seleccionar_personaje( &personaje_tp1);

inicializar_juego( &juego, personaje_tp1);

imprimir_terreno( juego);

return 0;
}
