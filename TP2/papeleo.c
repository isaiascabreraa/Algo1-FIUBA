#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "utiles.h"
#include "papeleo.h"
#include "dia_en_la_uni.h"

#define MAX_OBSTACULOS 100
#define MAX_HERRAMIENTAS 100
#define MAX_OBJETOS 100
#define MAX_PAPELEOS 10
#define MAX_NIVELES 3

#define MAX_ELEMENTOS 100
#define MAX_FILAS_MATRIZ 25
#define MAX_COLUMNAS_MATRIZ 25

static const char OLAF = 'O';
static const char STITCH = 'S';
static const char RAYO_MCQUEEN = 'R';
static const char JASMIN = 'J';

const int TOPE_LV1 = 22;
const int TOPE_LV2 = 17;
const int TOPE_LV3 = 12;

const char PAREDES = 'P', MIKE = 'W', FUEGOS = 'F', MEDIAS = 'M', BOTELLAS_DE_GRITOS = 'G', INTERRUPTORES_AHUYENTA_RANDALL = 'I';
const char PAPELEO = 'X', ESPACIO = '_';

const int PAPELEO_LV1 = 2, PAPELEO_LV2 = 3, PAPELEO_LV3 = 4;

const int FUEGOS_LV1 = 10, FUEGOS_LV2 = 5, FUEGOS_LV3 = 3;
const int MEDIAS_LV1 = 5, MEDIAS_LV2 = 4, MEDIAS_LV3 = 3;
const int BOTELLAS_LV1 = 4, BOTELLAS_LV2 = 3, BOTELLAS_LV3 = 2;
const int INTERRUPTORES_LV1 = 1, INTERRUPTORES_LV2 = 1, INTERRUPTORES_LV3 = 0;

const int MARTILLOS_LV1 = 4, MARTILLOS_LV2 = 5, MARTILLOS_LV3 = 6;
const int EXTINTORES_LV1 = 4, EXTINTORES_LV2 = 2, EXTINTORES_LV3 = 2;
const int MOVIMIENTOS_LV1 = 40, MOVIMIENTOS_LV2 = 30, MOVIMIENTOS_LV3 = 20;

const int CONDICION_FALLIDA = -1;
const int CONDICION_CUMPLIDA = 0;
const int NO_SE_REPITEN = 0, SE_REPITEN = -1;
const int NO_SE_SUPERPONEN = 0, SE_SUPERPONEN = -1;
const int NO_SON_ADYACENTES = -1, SON_ADYACENTES = 0, no_necesitan_ser_adyacentes = 0;
const int LIMITE_REPETICIONES = 2; //Valor que indica el limite de veces que puede repetirse una posicion dentro de su mismo vector.

const int OLAF_ELIMINA_FUEGOS_LV1 = 2, OLAF_ELIMINA_FUEGOS_LV2 = 1; 
const int STITCH_ELIMINA_PAPELELO_LV3 = 1;


//Pre: El personaje obtenido del TP1 debe ser jasmin.
//Post: Otorga un martillo extra por cada nivel en el que estemos.
void obsequio_jasmin(int nivel_actual, char personaje_tp1, int* martillos){
    if(personaje_tp1 == JASMIN){
        *martillos = *martillos + 1;
    }
}

//Pre: El personaje obtenido del TP1 debe ser el rayo mcqueen.
//Post: Otorga diez movimientos iniciales extras en el primer nivel.
void obsequio_rayo_mcqueen(int nivel_actual, char personaje_tp1, int* movimientos){
    if(nivel_actual == 1 && personaje_tp1 == RAYO_MCQUEEN){
        *movimientos = *movimientos + 10;
    }
}

//Pre: El personaje obtenido del TP1 debe ser stitch.
//Post: Otorga un papeleo menos a recolectar al estar en el nivel tres.
void obsequio_stitch(int* tope_papeleos, int nivel_actual, char personaje_tp1){
    
    if(nivel_actual == 3 && personaje_tp1 == STITCH){
        *tope_papeleos = *tope_papeleos - STITCH_ELIMINA_PAPELELO_LV3;
    }
}

//Pre: El personaje obtenido del TP1 debe ser olaf.
//Post: Otorga una reduccion de dos y de uno, de los fuegos de los primeros dos niveles, respectivamente.
void obsequio_olaf(int* cantidad_fuegos, int nivel_actual, char personaje_tp1){
    
    if(nivel_actual == 1 && personaje_tp1 == OLAF){
        *cantidad_fuegos = *cantidad_fuegos - OLAF_ELIMINA_FUEGOS_LV1;
    }else if(nivel_actual == 2 && personaje_tp1 == OLAF){
        *cantidad_fuegos = *cantidad_fuegos - OLAF_ELIMINA_FUEGOS_LV2;
    }
}

//Pre: -
//Post: Verifica que la posicion de un objeto no sea la misma que la de otros.
bool esta_la_posicion_ocupada( coordenada_t objeto_uno, coordenada_t objeto_dos){
    
    return(objeto_uno.fil == objeto_dos.fil && objeto_uno.col == objeto_dos.col);
}

//Pre: -
//Post: Verifica que los fuegos se enecuentren de forma adyacente a las paredes.
bool son_fuegos_adyacentes( coordenada_t objeto_uno, coordenada_t paredes){
    
    return( abs(objeto_uno.fil - paredes.fil) + abs(objeto_uno.col - paredes.col) == 1);
}

//Pre: -
//Post: Verifica que los papeleos cumplan las condiciones necesarias.
bool papeleos_cumple_las_condiciones( int verificar_paredes, int verificar_obstaculos, int verificar_herramientas, int verificar_papeleos, 
    coordenada_t posicion, coordenada_t pos_inicial_jugador){
    
    return(verificar_paredes == CONDICION_CUMPLIDA && verificar_obstaculos == CONDICION_CUMPLIDA && verificar_herramientas == CONDICION_CUMPLIDA && 
    verificar_papeleos == CONDICION_CUMPLIDA && !esta_la_posicion_ocupada( posicion, pos_inicial_jugador));
}

//Pre: -
//Post: Verifica que los objetos cumplan las condiciones necesarias.
bool objetos_cumple_las_condiciones( int verificar_paredes, int verificar_obstaculos, int verificar_herramientas, coordenada_t posicion, 
    coordenada_t pos_inicial_jugador){
    
    return(verificar_paredes == CONDICION_CUMPLIDA && verificar_obstaculos == CONDICION_CUMPLIDA && 
    verificar_herramientas == CONDICION_CUMPLIDA && !esta_la_posicion_ocupada( posicion, pos_inicial_jugador));
}

bool no_son_el_mismo_tipo(char tipo_uno, char tipo_dos){
    
    return((tipo_uno == FUEGOS || tipo_uno == MEDIAS) && (tipo_dos == BOTELLAS_DE_GRITOS || tipo_dos == INTERRUPTORES_AHUYENTA_RANDALL));
}

//Pre: -
//Post: Devuelve una coordenada que se genera aleatoriamente entre determinados numeros segun el nivel actual.
coordenada_t asignar_coordenadas(coordenada_t coordenada, int nivel_actual){
    
    if(nivel_actual == 1){
        coordenada.fil = rand()%20 + 1; 
        coordenada.col = rand()%20 + 1;

    }else if(nivel_actual == 2){
        coordenada.fil = rand()%15 + 1; 
        coordenada.col = rand()%15 + 1;

    }else{
        coordenada.fil = rand()%10 + 1; 
        coordenada.col = rand()%10 + 1;
    }
    return coordenada;
}

//Pre: -
//Post: Verifica que los objetos no se superpongan con las paredes.
int verificar_si_coinciden_paredes( coordenada_t objeto_uno, char tipo_objeto_uno, coordenada_t paredes[MAX_ELEMENTOS], int tope_paredes){
    
    int superposicion = NO_SE_SUPERPONEN;
    int locacion = NO_SON_ADYACENTES;
    int j = 0;
    
    while(j < tope_paredes && (superposicion == NO_SE_SUPERPONEN || locacion == NO_SON_ADYACENTES) ){
        
        if( esta_la_posicion_ocupada(objeto_uno, paredes[j])){
            superposicion = SE_SUPERPONEN;
        }

        if(tipo_objeto_uno == FUEGOS && son_fuegos_adyacentes( objeto_uno, paredes[j])){
            locacion = SON_ADYACENTES;
        
        }else if(tipo_objeto_uno != FUEGOS){
            locacion = no_necesitan_ser_adyacentes;
        }
        j++;
    }
   int verificacion = superposicion + locacion;
   return verificacion;
}

//Pre: -
//Post: Verifica que diferentes objetos no se superpongan entre ellos.
int verificar_si_coinciden_objetos( coordenada_t objeto_uno, char tipo_uno, objeto_t objeto_dos[MAX_ELEMENTOS], int tope_objeto_dos){
    
    int superposicion = NO_SE_SUPERPONEN;
    int repeticiones = NO_SE_REPITEN;
    int j = 0;
    
    while(j < tope_objeto_dos && superposicion == NO_SE_SUPERPONEN && repeticiones == NO_SE_REPITEN){
        
        if( esta_la_posicion_ocupada( objeto_uno, objeto_dos[j].posicion) && no_son_el_mismo_tipo(tipo_uno, objeto_dos[j].tipo)){
            superposicion = SE_SUPERPONEN;
        
        }else if( esta_la_posicion_ocupada( objeto_uno, objeto_dos[j].posicion) && !no_son_el_mismo_tipo(tipo_uno, objeto_dos[j].tipo)){
            repeticiones = SE_REPITEN;
        }
        j++;
    }
    return (superposicion + repeticiones);
}

//Pre: -
//Post: Verifica que al recorrer el vector al que pertenece cada papeleo no se encuentren dos papeleos en el mismo lugar) 
int verificar_si_coinciden_papeleos( coordenada_t papeleo_uno, papeleo_t papeleo_dos[MAX_PAPELEOS], int tope_papeleos){
    
    int repeticiones = NO_SE_REPITEN;
    int j = 0;

    while(j < tope_papeleos && repeticiones == NO_SE_REPITEN){
        
        if( esta_la_posicion_ocupada( papeleo_uno, papeleo_dos[j].posicion)){
            repeticiones = SE_REPITEN;
        }
        j++;
    }
    return repeticiones;
}

//Pre: -
//Post: Devuelven valores segun se vaya corroborando que los papeleos no se superpongan con los demas elementos.
void verificaciones_papeleos(coordenada_t posicion, nivel_t nivel, char tipo, int* verificar_paredes, int* verificar_obstaculos, int* verificar_herramientas, int* verificar_papeleos){
    
    *verificar_paredes = verificar_si_coinciden_paredes( posicion, tipo, nivel.paredes, nivel.tope_paredes);
    
    *verificar_obstaculos = verificar_si_coinciden_objetos( posicion, tipo, nivel.obstaculos, nivel.tope_obstaculos);
    
    *verificar_herramientas = verificar_si_coinciden_objetos( posicion, tipo, nivel.herramientas, nivel.tope_herramientas);
    
    *verificar_papeleos = verificar_si_coinciden_papeleos( posicion, nivel.papeleos, nivel.tope_papeleos);
}

//Pre: -
//Post: Devuelve valores segun se vaya corroborando que las herramientas no se superpongan con los demas elementos.
void verificaciones_objetos(coordenada_t posicion, nivel_t nivel, char tipo, int* verificar_paredes, int* verificar_obstaculos, int* verificar_herramientas){

    *verificar_paredes = verificar_si_coinciden_paredes( posicion, tipo, nivel.paredes, nivel.tope_paredes);
    
    *verificar_obstaculos = verificar_si_coinciden_objetos( posicion, tipo, nivel.obstaculos, nivel.tope_obstaculos);
    
    *verificar_herramientas = verificar_si_coinciden_objetos( posicion, tipo, nivel.herramientas, nivel.tope_herramientas);
}

//Pre: -
//Post: Verifica que se cumplan todas las condiciones para asignarle un numero aleatorio a uno de los papeleos.
int verificar_coincidencias_papeleos(coordenada_t posicion, nivel_t nivel, char tipo){
    
    int verificar_paredes = 0, verificar_obstaculos = 0, verificar_herramientas = 0, verificar_papeleos = 0;

    verificaciones_papeleos( posicion, nivel, tipo, &verificar_paredes, &verificar_obstaculos, &verificar_herramientas, &verificar_papeleos);
    
    if( papeleos_cumple_las_condiciones( verificar_paredes, verificar_obstaculos, verificar_herramientas, verificar_papeleos,
        posicion, nivel.pos_inicial_jugador)){
        
        return CONDICION_CUMPLIDA;
    } 
    return CONDICION_FALLIDA;
}

//Pre: -
//Post: Verifica que se cumplan todas las condiciones para asignarle un numero aleatorio a una de las herramientas.
int verificar_coincidencias_objetos(coordenada_t posicion, nivel_t nivel, char tipo){
    
    int verificar_paredes = 0, verificar_obstaculos = 0, verificar_herramientas = 0;
    
    verificaciones_objetos( posicion, nivel, tipo, &verificar_paredes, &verificar_obstaculos, &verificar_herramientas);

    if( objetos_cumple_las_condiciones(verificar_paredes, verificar_obstaculos, verificar_herramientas, 
        posicion, nivel.pos_inicial_jugador)){
        
        return CONDICION_CUMPLIDA;
    }
    return CONDICION_FALLIDA;
}

//Pre: -
//Post: Le asigna a las variables de jugador un valor segun el nivel en el que estemos.
void definir_nivel_jugador( jugador_t* jugador, int nivel_actual){
    
    if( nivel_actual == 1){        
        
        (*jugador).movimientos = MOVIMIENTOS_LV1;           
        (*jugador).martillos = MARTILLOS_LV1;
        (*jugador).extintores = EXTINTORES_LV1;
    
    }else if( nivel_actual == 2){
        
        (*jugador).movimientos = MOVIMIENTOS_LV2;
        (*jugador).martillos = MARTILLOS_LV2;
        (*jugador).extintores = EXTINTORES_LV2;

    }else{
        
        (*jugador).movimientos = MOVIMIENTOS_LV3;
        (*jugador).martillos = MARTILLOS_LV3;
        (*jugador).extintores = EXTINTORES_LV3;
    }
}

//Pre: -
//Post: Define la posicion del jugador.
void definir_posicion_jugador(jugador_t* jugador, nivel_t nivel){

    (*jugador).posicion.fil = nivel.pos_inicial_jugador.fil;
    (*jugador).posicion.col = nivel.pos_inicial_jugador.col;
}

//Pre: -
//Post: Carga todos los valores necesarios en lo que respecta al juegador.
void cargar_jugador(jugador_t* jugador, nivel_t nivel, int nivel_actual, char personaje_tp1){
    
    (*jugador).ahuyenta_randall = false;

    definir_nivel_jugador( jugador, nivel_actual);
    definir_posicion_jugador( jugador, nivel);

    obsequio_jasmin( nivel_actual, personaje_tp1, &(*jugador).martillos);
    obsequio_rayo_mcqueen( nivel_actual, personaje_tp1, &(*jugador).movimientos);
}

//Pre: -
//Post: Genera una coordenada aleatoria para los elementos del vector papeleos.
void asignar_posiciones_papeleos( nivel_t* nivel, int nivel_actual){

    for(int i = 0; i < (*nivel).tope_papeleos ; i++){
        int verificar = 0;
        do{
            coordenada_t coordenada = asignar_coordenadas((*nivel).papeleos[i].posicion, nivel_actual);
            verificar = verificar_coincidencias_papeleos(coordenada, *nivel, PAPELEO);

            (*nivel).papeleos[i].posicion.fil = coordenada.fil;
            (*nivel).papeleos[i].posicion.col = coordenada.col;
        
        }while( verificar != CONDICION_CUMPLIDA);
    }
}

//Pre: -
//Post: Genera una coordenada alaetoria para los elementos de un vector de objetos.
void asignar_posiciones_objetos( nivel_t* nivel, objeto_t objetos[MAX_ELEMENTOS], int tope, int nivel_actual){

    for(int i = 0; i < tope ; i++){
        int verificar = 0;

        do{
            coordenada_t coordenada = asignar_coordenadas( objetos[i].posicion, nivel_actual);
            verificar = verificar_coincidencias_objetos(coordenada, *nivel, objetos[i].tipo);
            
            objetos[i].posicion.fil = coordenada.fil;
            objetos[i].posicion.col = coordenada.col;

        }while( verificar != CONDICION_CUMPLIDA);
    }
}
//Pre: -
//Post: Devuelve el valor de las filas y columnas de mi matriz segun el nivel actual.
int determinar_tope_matriz(int nivel_actual){

     if(nivel_actual == 1){
        return TOPE_LV1;
    }else if(nivel_actual == 2){
        return TOPE_LV2;
    }else{
        return TOPE_LV3;
    }
}

//Pre: -
//Post: Posiciona las diferentes coordenadas de cada elemento en una matriz.
void posicionar_coordenadas(juego_t juego, int nivel_actual, char primer_terreno[MAX_FILAS_MATRIZ][MAX_COLUMNAS_MATRIZ]){

    int numero_nivel = nivel_actual - 1;
    
    for(int i = 0; i < juego.niveles[numero_nivel].tope_paredes ; i++){
        primer_terreno[juego.niveles[numero_nivel].paredes[i].fil][juego.niveles[numero_nivel].paredes[i].col] = PAREDES; 
    }

    for(int i = 0; i < juego.niveles[numero_nivel].tope_obstaculos ; i++){
        primer_terreno[juego.niveles[numero_nivel].obstaculos[i].posicion.fil][juego.niveles[numero_nivel].obstaculos[i].posicion.col] = 
        juego.niveles[numero_nivel].obstaculos[i].tipo; 
    }

    for(int i = 0; i < juego.niveles[numero_nivel].tope_herramientas ; i++){
        primer_terreno[juego.niveles[numero_nivel].herramientas[i].posicion.fil][juego.niveles[numero_nivel].herramientas[i].posicion.col] = 
        juego.niveles[numero_nivel].herramientas[i].tipo; 
    }

    for(int i = 0; i < juego.niveles[numero_nivel].tope_papeleos ; i++){
        primer_terreno[juego.niveles[numero_nivel].papeleos[i].posicion.fil][juego.niveles[numero_nivel].papeleos[i].posicion.col] = PAPELEO;
    }

    primer_terreno[juego.niveles[numero_nivel].pos_inicial_jugador.fil][juego.niveles[numero_nivel].pos_inicial_jugador.col] = MIKE;
}

//Pre: -
//Post: Segun el nivel en el que estemos y el personaje elegido, les asigna un determiando valor a las variables de papeleos.
void definir_cantidad_papeleos(int* tope_papeleos, int nivel_actual, char personaje_tp1){
    
    if(nivel_actual == 1){
        *tope_papeleos = PAPELEO_LV1;
    
    }else if(nivel_actual == 2){
        *tope_papeleos = PAPELEO_LV2;
    
    }else if(nivel_actual == 3){
        *tope_papeleos = PAPELEO_LV3;
    }

    obsequio_stitch( tope_papeleos, nivel_actual, personaje_tp1);
}

//Pre: -
//Post: Define los valores de los diferentes elementos del vector papeleos.
void definir_id_papeleos( papeleo_t papeleos[MAX_PAPELEOS], int tope_papeleos){

    for(int i = 1; i <= tope_papeleos; i++){
        (papeleos[i]).id_papeleo = i;
        
        (papeleos[i]).recolectado = false;
    }    
}

//Pre: -
//Post:  Carga todos los valores necesarios en lo que respecta a papeleos.
void cargar_papeleos( nivel_t* nivel, int nivel_actual, char personaje_tp1){

    definir_cantidad_papeleos( &(*nivel).tope_papeleos, nivel_actual, personaje_tp1);
    asignar_posiciones_papeleos( nivel, nivel_actual);
    definir_id_papeleos( (*nivel).papeleos, (*nivel).tope_papeleos);
}

//Pre: -
//Post: Segun el nivel en el que estemos, les asigna un determinado valor a las variables de herramientas.
void definir_cantidad_herramientas(int* tope_botellas, int* tope_interruptores, int* tope_herramientas, int nivel_actual){

    if(nivel_actual == 1){
        
        *tope_botellas = BOTELLAS_LV1;
        *tope_interruptores = INTERRUPTORES_LV1;
    
    }else if(nivel_actual == 2){
        
        *tope_botellas = BOTELLAS_LV2;
        *tope_interruptores = INTERRUPTORES_LV2;
    
    }else if(nivel_actual == 3){
        
        *tope_botellas = BOTELLAS_LV3;
        *tope_interruptores = INTERRUPTORES_LV3;
    }
    *tope_herramientas = *tope_botellas + *tope_interruptores;
}

//Pre: -
//Post:  Le asigna un tipo de caracter (en vector herramientas) a dos tipos diferentes de obstaculos en el juego.
void definir_tipo_herramientas( objeto_t herramientas[MAX_HERRAMIENTAS], int tope_herramientas, int tope_botellas){

    for(int i = 0; i < tope_botellas; i++){
        (herramientas[i]).tipo = BOTELLAS_DE_GRITOS;    
    }
    for(int i = tope_botellas; i < tope_herramientas; i++){
        (herramientas[i]).tipo = INTERRUPTORES_AHUYENTA_RANDALL;
    }
}

//Pre: -
//Post: Carga todos los valores necesarios en lo que respecta a las herramientas.
void cargar_herramientas( nivel_t* nivel, int nivel_actual){

    int tope_botellas = 0;
    int tope_interruptores = 0;

    definir_cantidad_herramientas( &tope_botellas, &tope_interruptores, &(*nivel).tope_herramientas, nivel_actual);
    definir_tipo_herramientas( (*nivel).herramientas, (*nivel).tope_herramientas, tope_botellas);
    asignar_posiciones_objetos( nivel, (*nivel).herramientas, (*nivel).tope_herramientas, nivel_actual);
}

//Pre: -
//Post: Segun el nivel en el que estemos y el personaje elegido, les asigna un determinado valor a las variables de obstaculos.
void definir_cantidad_obstaculos(int* cantidad_fuegos, int* cantidad_medias, int* tope_obstaculos, int nivel_actual, char personaje_tp1){
    
    if(nivel_actual == 1){
        
        *cantidad_fuegos = FUEGOS_LV1;
        *cantidad_medias = MEDIAS_LV1;
    
    }else if(nivel_actual == 2){
        
        *cantidad_fuegos = FUEGOS_LV2;
        *cantidad_medias = MEDIAS_LV2;
    
    }else if(nivel_actual == 3){
        
        *cantidad_fuegos = FUEGOS_LV3;
        *cantidad_medias = MEDIAS_LV3;
    }
    obsequio_olaf( cantidad_fuegos, nivel_actual, personaje_tp1);
    *tope_obstaculos = *cantidad_fuegos + *cantidad_medias;
}

//Pre: -
//Post: Le asigna un tipo de caracter (en vector obstaculos) a dos tipos diferentes de obstaculos en el juego.
void definir_tipo_obstaculos( objeto_t obstaculos[MAX_OBSTACULOS], int tope_obstaculos, int cantidad_fuegos){

    for(int i = 0; i < cantidad_fuegos; i++){
        (obstaculos[i]).tipo = FUEGOS;
    }
    
    for(int i = cantidad_fuegos; i < tope_obstaculos; i++){
        (obstaculos[i]).tipo = MEDIAS; 
    }
}

//Pre: -
//Post: Carga todos los necesarios en lo que respecta a los obstaculos.
void cargar_obstaculos( nivel_t* nivel, int nivel_actual, char personaje_tp1){
    
    int cantidad_fuegos = 0;
    int cantidad_medias = 0; 
    
    definir_cantidad_obstaculos( &cantidad_fuegos, &cantidad_medias, &(*nivel).tope_obstaculos, nivel_actual, personaje_tp1);
    definir_tipo_obstaculos( (*nivel).obstaculos, (*nivel).tope_obstaculos, cantidad_fuegos);
    asignar_posiciones_objetos( nivel, (*nivel).obstaculos, (*nivel).tope_obstaculos, nivel_actual);
}

//Pre: -
//Post: Procedimiento que recibe el juego e imprime toda su información por pantalla.
void imprimir_terreno(juego_t juego){
    
    char primer_terreno[MAX_FILAS_MATRIZ][MAX_COLUMNAS_MATRIZ];
    int tope_matriz = determinar_tope_matriz(juego.nivel_actual);

    for(int i = 0; i < tope_matriz; i++){
        for(int j = 0; j < tope_matriz; j++){
            primer_terreno[i][j] = ESPACIO; } }

    posicionar_coordenadas( juego, juego.nivel_actual, primer_terreno);
    
    for(int i = 0; i < tope_matriz; i++){
        for(int j = 0; j < tope_matriz; j++){
            printf(" %c", primer_terreno[i][j]);
        }
        printf(" \n");
    }   
}

// Pre: -
// Post: Asigna todos los valores correspondientes a cada elemento.
void cargar_valores( nivel_t* nivel, int nivel_actual, char personaje_tp1){

    cargar_obstaculos( nivel, nivel_actual, personaje_tp1);
    
    cargar_herramientas( nivel, nivel_actual);

    cargar_papeleos( nivel, nivel_actual, personaje_tp1);
}

//Pre: -
/* Post: Inicializará un nivel, cargando toda la información inicial, las paredes,los objetos, 
         los papeleos y la posición inicial del jugador en dicho nivel. */
void inicializar_nivel(nivel_t* nivel, int nivel_actual, char personaje_tp1){

    (*nivel).tope_paredes = 0;
   
    (*nivel).tope_obstaculos = 0;

    (*nivel).tope_herramientas = 0;

    (*nivel).tope_papeleos = 0;
    
    obtener_paredes(nivel_actual, (*nivel).paredes, &(*nivel).tope_paredes, &(*nivel).pos_inicial_jugador);

    cargar_valores( nivel, nivel_actual, personaje_tp1);
}

// Pre: -
/* Post: Inicializará el juego, cargando toda la información inicial, los datos del jugador, 
    el personaje resultado del tp anterior, y los 3 niveles. El campo "nivel_actual"
    comienza en 1. */
void inicializar_juego(juego_t* juego, char personaje_tp1){

    (*juego).nivel_actual = 1;
    (*juego).personaje_tp1 = personaje_tp1;

    for(int i = 0; i < 3; i++){

        int numero_nivel = i + 1;
        
        inicializar_nivel( &(*juego).niveles[i], numero_nivel , personaje_tp1);
    }

    cargar_jugador( &(*juego).jugador, (*juego).niveles[(*juego).nivel_actual - 1], (*juego).nivel_actual, personaje_tp1);
}

