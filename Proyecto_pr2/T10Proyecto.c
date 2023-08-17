#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>


#define MAX_FILAS 30
#define MAX_COLUMNAS 42
#define TAMANO_CELDA 30
#define ANCHO_VENTANA 1260
#define ALTO_VENTANA 900

typedef struct
{
    int pos_x;
    int pos_y;
    int ancho_sprite;
    int alto_sprite;
    int corazones;
    bool invulnerable;
    double invulnerableTimer;
    bool estaVivo;
    int spawnX;
    int spawnY;
} Personaje;

typedef struct
{
    int ene_posx;
    int ene_posy;
    int ancho_enesprite;
    int alto_enesprite;
    int desplazamiento;
    int spawnX;
    int spawnY;
} Enemigo;

typedef struct
{
    int eneV_posx;
    int eneV_posy;
    int anchoV_enesprite;
    int altoV_enesprite;
    int desplazamientoV;
    int spawnX;
    int spawnY;
    ALLEGRO_BITMAP *spriteV;
} EnemigoV;

typedef struct
{
    int eneP_posx;
    int eneP_posy;
    int anchoP_enesprite;
    int altoP_enesprite;
    int desplazamientoP;
    bool persigue;
    int spawnX;
    int spawnY;
    ALLEGRO_BITMAP *spriteP;
} EnemigoP;

typedef struct
{
    int tes_posx;
    int tes_posy;
    int anchoT;
    int altoT;
    ALLEGRO_BITMAP *sprite;
} Tesoros;

void ejecutarJuego(int mapaSeleccionado, ALLEGRO_DISPLAY* displayM, int nivelActual, ALLEGRO_EVENT_QUEUE *eventqueue2);
void MenuPrinc(ALLEGRO_BITMAP *fondoMenu, ALLEGRO_DISPLAY *displayM, ALLEGRO_EVENT_QUEUE *eventqueue2, int *mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS], int nivelActual);
void mostrarMenu(ALLEGRO_DISPLAY *displayM, ALLEGRO_EVENT_QUEUE *eventqueue2, char mapa[MAX_FILAS][MAX_COLUMNAS], int *mapaSeleccionado, int nivelActual);
void Juego(int* mapaSeleccionado, ALLEGRO_DISPLAY *displayM, int nivelActual, ALLEGRO_EVENT_QUEUE *eventqueue2);
void cargarRecursos();
void liberarRecursos(ALLEGRO_DISPLAY* displayM);
void recibeDaño(Personaje *personaje);
void renderLlave(Personaje personaje, ALLEGRO_BITMAP* sprite_llave);
void reiniciarPersonaje(Personaje personaje);
void cargarMapa(int mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS]);
void seleccionarMapa(int mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS]);
void dibujarMapa(char mapa[MAX_FILAS][MAX_COLUMNAS], ALLEGRO_BITMAP *sprite_barrera, ALLEGRO_BITMAP *sprite_trampa, ALLEGRO_BITMAP *sprite_trampa2, ALLEGRO_BITMAP *sprite_trampa3, ALLEGRO_BITMAP *sprite_trampa4, ALLEGRO_BITMAP *sprite_llave, ALLEGRO_BITMAP *sprite_puertaL);
void moverEnemigo(char mapa[MAX_FILAS][MAX_COLUMNAS], Enemigo enemigos[], int contE);
void moverEnemigoV(char mapa[MAX_FILAS][MAX_COLUMNAS], EnemigoV enemigosV[], int contV);
void perseguirPersonaje(Personaje personaje, EnemigoP enemigosP[], int contP);
void moverEnemigoP(char mapa[MAX_FILAS][MAX_COLUMNAS], EnemigoP enemigosP[], Personaje personaje, int contP);
void actualizarPuntaje(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje, int *puntaje);
void dibujarPuntaje(int puntaje);
void renderTesoros(Tesoros tesoro[], Personaje personaje, ALLEGRO_BITMAP *sprite_tesoro);
void leerEnemigoE(int contE);
void leerEnemigoV(int contV);
void leerEnemigoP(int contP);
void leerTesoros(int contT);
bool colisionBarrera(char mapa[MAX_FILAS][MAX_COLUMNAS], int pos_x, int pos_y, int ancho_sprite, int alto_sprite);
bool colisionTrampa(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje);
bool colisionConLlave(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje);
bool colisionPuertaB(char mapa[MAX_FILAS][MAX_COLUMNAS], int pos_x, int pos_y, int ancho_sprite, int alto_sprite);
bool colisionConEnemigoE(Enemigo enemigos[], Personaje personaje, int contE);
bool colisionConEnemigoV(EnemigoV enemigosV[], Personaje personaje, int contV);
bool colisionConEnemigoP(EnemigoP enemigosP[], Personaje personaje, int contP);
bool colisionconTesoro(Tesoros tesoro[], Personaje personaje, int contT);
bool colisionPuertaU(char mapa[MAX_FILAS][MAX_COLUMNAS], int pos_x, int pos_y, int ancho_sprite, int alto_sprite);
int inicializarPersonaje(char mapa[MAX_FILAS][MAX_COLUMNAS]);
int inicializarEnemigosE(char mapa[MAX_FILAS][MAX_COLUMNAS]);
int inicializarEnemigosV(char mapa[MAX_FILAS][MAX_COLUMNAS]);
int inicializarEnemigosP(char mapa[MAX_FILAS][MAX_COLUMNAS]);
int inicializarTesoros(char mapa[MAX_FILAS][MAX_COLUMNAS], ALLEGRO_BITMAP *sprite_tesoro);

Enemigo enemigos[10];
EnemigoV enemigosV[10];
EnemigoP enemigosP[10];
Tesoros tesoro[20];
Personaje personaje;

int puertaPosx, puertaPosy, llaveposX, llaveposY;

bool puertaBloqueada = true;
bool llaveRecogida = false;

int contE = 0;
int contV = 0;
int contP = 0;
int contC = 0;
int contT = 0;
int puntaje = 0;
int total_tesoros;
int tesoro_actual;
int frame_actual = 0;
int frame_actualV = 0;
int frame_actualP = 0;

double tiempo_anterior;
double refresh_animacion; 
double refresh_animacionP; 
double tiempo_actual;
double tiempo_gen;

ALLEGRO_EVENT_QUEUE *eventQueue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *sprite = NULL;
ALLEGRO_BITMAP *sprite_izq = NULL;
ALLEGRO_BITMAP *sprite_der = NULL;
ALLEGRO_BITMAP *sprite_muerte = NULL;
ALLEGRO_BITMAP *sprite_barrera = NULL;
ALLEGRO_BITMAP *sprite_tesoro = NULL;
ALLEGRO_BITMAP *sprite_trampa = NULL;
ALLEGRO_BITMAP *sprite_trampa2 = NULL;
ALLEGRO_BITMAP *sprite_trampa3 = NULL;
ALLEGRO_BITMAP *sprite_trampa4 = NULL;
ALLEGRO_BITMAP *sprite_enemigo = NULL;
ALLEGRO_BITMAP *sprite_enemigoV = NULL;
ALLEGRO_BITMAP *sprite_enemigoP = NULL;
ALLEGRO_BITMAP *sprite_llave = NULL;
ALLEGRO_BITMAP *sprite_puertaL = NULL;
ALLEGRO_BITMAP *sprite_puertaU = NULL;
ALLEGRO_BITMAP *sprite_llaveRecogida = NULL;
ALLEGRO_BITMAP *corazon_lleno = NULL;
ALLEGRO_BITMAP *corazon_vacio = NULL;
ALLEGRO_BITMAP *frames_enemigo[4];
ALLEGRO_BITMAP *frames_enemigoV[3];
ALLEGRO_BITMAP *frames_enemigoP[4];
ALLEGRO_BITMAP *fondoMenu = NULL;

int main()
{
    al_init();
    al_init_font_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_image_addon();

    ALLEGRO_DISPLAY *displayM = al_create_display(ANCHO_VENTANA, ALTO_VENTANA);
    ALLEGRO_EVENT_QUEUE *eventQueue2 = al_create_event_queue();
    
    al_register_event_source(eventQueue2, al_get_display_event_source(displayM));
    al_register_event_source(eventQueue2, al_get_keyboard_event_source());

    char mapa[MAX_FILAS][MAX_COLUMNAS];
    int mapaSeleccionado = 1;
    int nivelActual = mapaSeleccionado;

    cargarRecursos();
    MenuPrinc(fondoMenu, displayM, eventQueue2, &mapaSeleccionado, mapa, nivelActual);

    al_destroy_event_queue(eventQueue2);

    if (nivelActual > 0)
    {
        ejecutarJuego(mapaSeleccionado, displayM, nivelActual, eventQueue2);
    }

    al_destroy_display(displayM);

    return 0;
}

int inicializarPersonaje(char mapa[MAX_FILAS][MAX_COLUMNAS])
{
    int fila, columna, contC = 0;
    for (fila = 0; fila < MAX_FILAS; fila++)
    {
        for (columna = 0; columna < MAX_COLUMNAS; columna++)
        {
            if (mapa[fila][columna] == 'C')
            {
                personaje.pos_x = columna * TAMANO_CELDA;
                personaje.pos_y = fila * TAMANO_CELDA;
                personaje.ancho_sprite = 20;
                personaje.alto_sprite = 22;
                personaje.corazones = 4;
                personaje.spawnX = columna * TAMANO_CELDA;
                personaje.spawnY = fila * TAMANO_CELDA;
                personaje.invulnerable = false;
                personaje.estaVivo = true;
                contC++;
            }
        }
    }
    return contC;
}


int inicializarTesoros(char mapa[MAX_FILAS][MAX_COLUMNAS], ALLEGRO_BITMAP *sprite_tesoro)
{
    int fila, columna, contT = 0;
    for (fila = 0; fila < MAX_FILAS; fila++)
    {
        for (columna = 0; columna < MAX_COLUMNAS; columna++)
        {
            if (mapa[fila][columna] == '$')
            {
                tesoro[contT].tes_posx = (columna * TAMANO_CELDA) + 20;
                tesoro[contT].tes_posy = (fila * TAMANO_CELDA) + 20;
                tesoro[contT].anchoT = 20;
                tesoro[contT].altoT = 20;
                tesoro[contT].sprite = sprite_tesoro;
                contT++;
            }
        }
    }
    return contT;
}

int inicializarEnemigosE(char mapa[MAX_FILAS][MAX_COLUMNAS])
{
    int fila, columna, contE = 0;
    for (fila = 0; fila < MAX_FILAS; fila++)
    {
        for (columna = 0; columna < MAX_COLUMNAS; columna++)
        {
            if (mapa[fila][columna] == 'E')
            {
                enemigos[contE].ene_posx = columna * TAMANO_CELDA;
                enemigos[contE].ene_posy = (fila * TAMANO_CELDA) + 20;
                enemigos[contE].ancho_enesprite = 15;
                enemigos[contE].alto_enesprite = 20;
                enemigos[contE].desplazamiento = -1;
                enemigos[contE].spawnX = columna * TAMANO_CELDA;
                enemigos[contE].spawnY = (fila * TAMANO_CELDA) + 20;
                contE++;
            }
        }
    }
    return contE;
}

int inicializarEnemigosV(char mapa[MAX_FILAS][MAX_COLUMNAS])
{
    int fila, columna, contV = 0;
    for (fila = 0; fila < MAX_FILAS; fila++)
    {
        for (columna = 0; columna < MAX_COLUMNAS; columna++)
        {
            if (mapa[fila][columna] == 'V')
            {
                enemigosV[contV].eneV_posx = columna * TAMANO_CELDA;
                enemigosV[contV].eneV_posy = fila * TAMANO_CELDA;
                enemigosV[contV].anchoV_enesprite = 30;
                enemigosV[contV].altoV_enesprite = 30;
                enemigosV[contV].desplazamientoV = -1;
                enemigosV[contV].spawnX = columna * TAMANO_CELDA;
                enemigosV[contV].spawnY = fila * TAMANO_CELDA;
                enemigosV[contV].spriteV = sprite_enemigoV;
                contV++;
            }
        }
    }
    return contV;
}

int inicializarEnemigosP(char mapa[MAX_FILAS][MAX_COLUMNAS])
{
    int fila, columna, contP = 0;
    for (fila = 0; fila < MAX_FILAS; fila++)
    {
        for (columna = 0; columna < MAX_COLUMNAS; columna++)
        {
            if (mapa[fila][columna] == 'P')
            {
                enemigosP[contP].eneP_posx = columna * TAMANO_CELDA;
                enemigosP[contP].eneP_posy = fila * TAMANO_CELDA;
                enemigosP[contP].anchoP_enesprite = 30;
                enemigosP[contP].altoP_enesprite = 30;
                enemigosP[contP].desplazamientoP = -1;
                enemigosP[contP].persigue = false;
                enemigosP[contP].spawnX = columna * TAMANO_CELDA;
                enemigosP[contP].spawnY = fila * TAMANO_CELDA;
                enemigosP[contP].spriteP = sprite_enemigoP;
                contP++;
            }
        }
    }
    return contP;
}

void leerEnemigoE(int contE)
{
    int i;
    for (i = 0; i < contE; i++)
    {
        enemigos[i].ene_posx;
        enemigos[i].ene_posy;
    }
    printf("\n");
}

void leerEnemigoV(int contV)
{
    int i;
    for (i = 0; i < contV; i++)
    {
        enemigosV[i].eneV_posx;
        enemigosV[i].eneV_posy;
    }
    printf("\n");
}

void leerEnemigoP(int contP)
{
    int i;
    for (i = 0; i < contP; i++)
    {
        enemigosP[i].eneP_posx;
        enemigosP[i].eneP_posy;
    }
    printf("\n");
}

void leerTesoros(int contT)
{
    int i;
    for (i = 0; i < contT; i++)
    {
        tesoro[i].tes_posx;
        tesoro[i].tes_posy;
    }
    printf("\n");
}

void ejecutarJuego(int mapaSeleccionado, ALLEGRO_DISPLAY* displayM, int nivelActual, ALLEGRO_EVENT_QUEUE *eventqueue2) {
    cargarRecursos();
    Juego(&mapaSeleccionado, displayM, nivelActual, eventqueue2);
    liberarRecursos(displayM);
}

void Juego(int* mapaSeleccionado, ALLEGRO_DISPLAY* displayM, int nivelActual, ALLEGRO_EVENT_QUEUE *eventqueue2)
{
    int i, fila;

    int cora_x1 = 1000;
    int cora_x2 = 1030;
    int cora_x3 = 1060;
    int cora_x4 = 1090;
    int y_corazones = 0;

    Enemigo enemigo;
    EnemigoV enemigoV;
    EnemigoP enemigoP;

    eventQueue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60);

    ALLEGRO_FONT *fuente = al_create_builtin_font();

    al_register_event_source(eventQueue, al_get_display_event_source(displayM));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    

    ALLEGRO_KEYBOARD_STATE keyboardState;

    bool salir = false;
    bool nivelCompletado = false;
    bool izqPresionada = false;
    bool derPresionada = false;

    char mapa[MAX_FILAS][MAX_COLUMNAS];
    seleccionarMapa(*mapaSeleccionado, mapa);

    int enemigo_ancho_imagen = al_get_bitmap_width(sprite_enemigo);
    int enemigo_alto_imagen = al_get_bitmap_height(sprite_enemigo);
    int enemigoV_ancho_imagen = al_get_bitmap_width(sprite_enemigoV);
    int enemigoV_alto_imagen = al_get_bitmap_height(sprite_enemigoV);    
    int enemigoP_ancho_imagen = al_get_bitmap_width(sprite_enemigoP);
    int enemigoP_alto_imagen = al_get_bitmap_height(sprite_enemigoP); 


    for (i = 0; i < 4; i++)
    {
        frames_enemigo[i] = al_create_sub_bitmap(sprite_enemigo, i * (enemigo_ancho_imagen / 4), 0, enemigo_ancho_imagen / 4, enemigo_alto_imagen);
    }

    for (i = 0; i < 3; i++)
    {
        frames_enemigoV[i] = al_create_sub_bitmap(sprite_enemigoV, i * (enemigoV_ancho_imagen / 3), 0, enemigoV_ancho_imagen / 3, enemigoV_alto_imagen);
    }    

    for (i = 0; i < 4; i++)
    {
        frames_enemigoP[i] = al_create_sub_bitmap(sprite_enemigoP, i * (enemigoP_ancho_imagen / 4), 0, enemigoP_ancho_imagen / 4, enemigoP_alto_imagen);
    }

    tiempo_anterior = al_get_time();
    refresh_animacion = 0.02;
    refresh_animacionP = 1;    
    contE = inicializarEnemigosE(mapa);
    contV = inicializarEnemigosV(mapa);
    contP = inicializarEnemigosP(mapa);
    contC = inicializarPersonaje(mapa);
    contT = inicializarTesoros(mapa, sprite_tesoro);
    total_tesoros = contT;
    tesoro_actual = 0;
    al_start_timer(timer);
    
    while (!nivelCompletado && !salir)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventQueue, &event);
        al_clear_to_color(al_map_rgb(0, 0, 0));
        
        tiempo_actual = al_get_time();
        tiempo_gen = tiempo_actual - tiempo_anterior;
        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            al_get_keyboard_state(&keyboardState);

            if (al_key_down(&keyboardState, ALLEGRO_KEY_UP))
            {
                if (personaje.pos_y > 0 && !colisionBarrera(mapa, personaje.pos_x, personaje.pos_y - TAMANO_CELDA / 8, personaje.ancho_sprite, personaje.alto_sprite) && !colisionPuertaB(mapa, personaje.pos_x, personaje.pos_y - TAMANO_CELDA / 8, personaje.ancho_sprite, personaje.alto_sprite))
                {
                    personaje.pos_y -= TAMANO_CELDA / 8;
                }
            }
            else if (al_key_down(&keyboardState, ALLEGRO_KEY_DOWN))
            {
                if (personaje.pos_y < (MAX_FILAS - 1) * TAMANO_CELDA - personaje.alto_sprite && !colisionBarrera(mapa, personaje.pos_x, personaje.pos_y + TAMANO_CELDA / 8, personaje.ancho_sprite, personaje.alto_sprite) && !colisionPuertaB(mapa, personaje.pos_x, personaje.pos_y + TAMANO_CELDA / 8, personaje.ancho_sprite, personaje.alto_sprite))
                {
                    personaje.pos_y += TAMANO_CELDA / 8;
                }
            }
            else if (al_key_down(&keyboardState, ALLEGRO_KEY_LEFT))
            {
                derPresionada = false;
                izqPresionada = true;
                if (personaje.pos_x > 0 && !colisionBarrera(mapa, personaje.pos_x - TAMANO_CELDA / 8, personaje.pos_y, personaje.ancho_sprite, personaje.alto_sprite) && !colisionPuertaB(mapa, personaje.pos_x - TAMANO_CELDA / 8, personaje.pos_y, personaje.ancho_sprite, personaje.alto_sprite))
                {
                    personaje.pos_x -= TAMANO_CELDA / 8;
                }
            }
            else if (al_key_down(&keyboardState, ALLEGRO_KEY_RIGHT))
            {
                izqPresionada = false;
                derPresionada = true;
                if (personaje.pos_x < (MAX_COLUMNAS - 1) * TAMANO_CELDA - personaje.ancho_sprite && !colisionBarrera(mapa, personaje.pos_x + TAMANO_CELDA / 8, personaje.pos_y, personaje.ancho_sprite, personaje.alto_sprite) && !colisionPuertaB(mapa, personaje.pos_x + TAMANO_CELDA / 8, personaje.pos_y, personaje.ancho_sprite, personaje.alto_sprite))
                {
                    personaje.pos_x += TAMANO_CELDA / 8;
                    
                }
            }

            if(al_key_down(&keyboardState, ALLEGRO_KEY_ESCAPE))
            {
                mostrarMenu(displayM, eventqueue2, mapa, mapaSeleccionado, nivelActual);
            }

            if (colisionTrampa(mapa, personaje))
            {
                recibeDaño(&personaje);
            }

            frame_actual = (frame_actual + 1) % 4;
            frame_actualV = (frame_actualV + 1) % 3;
            frame_actualP = (frame_actualP + 1) % 4;

            moverEnemigo(mapa, enemigos, contE);
            moverEnemigoV(mapa, enemigosV, contV);
            moverEnemigoP(mapa, enemigosP, personaje, contP);
            perseguirPersonaje(personaje, enemigosP, contP);
            leerEnemigoE(contE);
            leerEnemigoV(contV);
            leerEnemigoP(contP);
            leerTesoros(contT);
            if (colisionConEnemigoE(enemigos, personaje, contE))
            {
                recibeDaño(&personaje);
            }

            if (colisionConEnemigoV(enemigosV, personaje, contV))
            {
                recibeDaño(&personaje);
            }

            if (colisionConEnemigoP(enemigosP, personaje, contP))
            {
                recibeDaño(&personaje);
            }

            if (personaje.invulnerable && al_get_time() > personaje.invulnerableTimer)
            {
                personaje.invulnerable = false;
            }

            al_clear_to_color(al_map_rgb(0, 0, 0));

            dibujarMapa(mapa, sprite_barrera, sprite_trampa, sprite_trampa2, sprite_trampa3, sprite_trampa4, sprite_llave, sprite_puertaL);
            renderTesoros(tesoro, personaje, sprite_tesoro);
            renderLlave(personaje, sprite_llave);

            if(tiempo_gen >= refresh_animacion)
            {
                for(i = 0; i < contV; i++)
                {
                    frame_actualV = (frame_actualV + 1) % 3;
                    //frame_actualP = (frame_actualP + 1) % 4;
                }
                tiempo_anterior = tiempo_actual;
            }
            if(tiempo_gen >= refresh_animacionP)
            {
                for(i = 0; i < contP; i++)
                {
                    frame_actualP = (frame_actualP + 1) % 4;
                }
                tiempo_anterior = tiempo_actual;
            }
            for (i = 0; i < contE; i++)
            {
                al_draw_bitmap(frames_enemigo[frame_actual], enemigos[i].ene_posx - enemigos[i].ancho_enesprite / 2, enemigos[i].ene_posy - enemigos[i].alto_enesprite, 0);
            }
            for (i = 0; i < contV; i++)
            {
                al_draw_bitmap(frames_enemigoV[frame_actualV], enemigosV[i].eneV_posx - enemigosV[i].anchoV_enesprite / 2, enemigosV[i].eneV_posy - enemigosV[i].altoV_enesprite / 2, 0);
            }
            for (i = 0; i < contP; i++)
            {
                al_draw_bitmap(frames_enemigoP[frame_actualP], enemigosP[i].eneP_posx - enemigosP[i].anchoP_enesprite / 2, enemigosP[i].eneP_posy - enemigosP[i].altoP_enesprite / 2, 0);
            }

            if (colisionconTesoro(tesoro, personaje, contT))
            {
                tesoro[i] = tesoro[contT - 1];
                contT--;
                tesoro_actual++;
            }

            if (colisionConLlave(mapa, personaje) && !llaveRecogida)
            {
                llaveRecogida = true;
            }

            if (llaveRecogida)
            {
                al_draw_bitmap(sprite_llaveRecogida, 960, 5, 0);
                al_draw_bitmap(sprite_puertaU, puertaPosx, puertaPosy, 0);
                puertaBloqueada = false;
            }

            else
            {
                al_draw_bitmap(sprite_puertaL, puertaPosx, puertaPosy, 0);
                puertaBloqueada = true;
            }
            
            al_draw_bitmap(sprite, personaje.pos_x, personaje.pos_y, 0);

            if (izqPresionada)
            {
                al_draw_bitmap(sprite_izq, personaje.pos_x, personaje.pos_y, 0);
            }

            else if (derPresionada)
            {
                al_draw_bitmap(sprite_der, personaje.pos_x, personaje.pos_y, 0);
            }   

            if (colisionPuertaU(mapa, personaje.pos_x, personaje.pos_y, personaje.ancho_sprite, personaje.alto_sprite))
            {
                nivelCompletado = true;
                cargarMapa(*mapaSeleccionado, mapa);
            }

            if (nivelCompletado)
            {
                *mapaSeleccionado = nivelActual + 1;
                nivelCompletado = false;
            }

            al_draw_textf(fuente, al_map_rgb(255, 255, 255), 820, 10, 0, "Tesoros = %d / %d", tesoro_actual, total_tesoros);

            if (personaje.corazones == 4)
            {
                al_draw_bitmap(corazon_lleno, cora_x1, y_corazones, 0);
                al_draw_bitmap(corazon_lleno, cora_x2, y_corazones, 0);
                al_draw_bitmap(corazon_lleno, cora_x3, y_corazones, 0);
                al_draw_bitmap(corazon_lleno, cora_x4, y_corazones, 0);
            }

            else if (personaje.corazones == 3)
            {
                al_draw_bitmap(corazon_lleno, cora_x1, y_corazones, 0);
                al_draw_bitmap(corazon_lleno, cora_x2, y_corazones, 0);
                al_draw_bitmap(corazon_lleno, cora_x3, y_corazones, 0);
                al_draw_bitmap(corazon_vacio, cora_x4, y_corazones, 0);
            }

            else if (personaje.corazones == 2)
            {
                al_draw_bitmap(corazon_lleno, cora_x1, y_corazones, 0);
                al_draw_bitmap(corazon_lleno, cora_x2, y_corazones, 0);
                al_draw_bitmap(corazon_vacio, cora_x3, y_corazones, 0);
                al_draw_bitmap(corazon_vacio, cora_x4, y_corazones, 0);
            }

            else if (personaje.corazones == 1)
            {
                al_draw_bitmap(corazon_lleno, cora_x1, y_corazones, 0);
                al_draw_bitmap(corazon_vacio, cora_x2, y_corazones, 0);
                al_draw_bitmap(corazon_vacio, cora_x3, y_corazones, 0);
                al_draw_bitmap(corazon_vacio, cora_x4, y_corazones, 0);
            }

            if (personaje.corazones == 0)
            {
                al_draw_bitmap(corazon_vacio, cora_x1, y_corazones, 0);
                al_draw_bitmap(corazon_vacio, cora_x2, y_corazones, 0);
                al_draw_bitmap(corazon_vacio, cora_x3, y_corazones, 0);
                al_draw_bitmap(corazon_vacio, cora_x4, y_corazones, 0);
            }

            if (personaje.corazones <= 0) 
            {
                al_draw_bitmap(sprite_muerte, personaje.pos_x, personaje.pos_y, 0);
                al_flip_display();
                al_rest(1.0);

                personaje.pos_x = personaje.spawnX;
                personaje.pos_y = personaje.spawnY;
                personaje.corazones = 4; 
                personaje.invulnerable = true;
                personaje.invulnerableTimer = al_get_time() + 2.0;
                personaje.estaVivo = false;
                puntaje = 0;
                tesoro_actual = 0;
                inicializarEnemigosE(mapa);
                inicializarEnemigosV(mapa);
                inicializarEnemigosP(mapa);

                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_text(fuente, al_map_rgb(255, 255, 255), ANCHO_VENTANA / 2, ALTO_VENTANA / 2, ALLEGRO_ALIGN_CENTER, "Game Over :(");
                al_flip_display();    
                al_rest(2.0);

                al_clear_to_color(al_map_rgb(0, 0, 0));
                dibujarMapa(mapa, sprite_barrera, sprite_trampa, sprite_trampa2, sprite_trampa3, sprite_trampa4, sprite_llave, sprite_puertaL);
                renderTesoros(tesoro, personaje, sprite_tesoro);
                personaje.invulnerable = false;
            }
            dibujarPuntaje(puntaje);
            actualizarPuntaje(mapa, personaje, &puntaje);
            al_flip_display();
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            salir = true;
        }
    }
}


void MenuPrinc(ALLEGRO_BITMAP *fondoMenu, ALLEGRO_DISPLAY *displayM, ALLEGRO_EVENT_QUEUE *eventqueue2, int *mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS], int nivelActual)
{
    ALLEGRO_FONT *fuente = al_create_builtin_font();
    ALLEGRO_COLOR textColor = al_map_rgb(255, 255, 255);

    int opcion = 0;
    int totalOpciones = 3;
    int totalOpcionesMapa = 5;
    bool salirMenu = false;
    bool seleccionandoMapa = false;
    const char *opciones[] = {
        "Jugar",
        "Seleccionar mapa",
        "Salir"};

    while (!salirMenu)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventqueue2, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            salirMenu = true;
            exit(0);
            break;
        }

        if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (!seleccionandoMapa)
            {
                if (event.keyboard.keycode == ALLEGRO_KEY_UP)
                {
                    opcion = (opcion + totalOpciones - 1) % totalOpciones;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
                {
                    opcion = (opcion + 1) % totalOpciones;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
                {
                    if (opcion == 0)
                    {
                        salirMenu = true;
                        break;
                    }
                    else if (opcion == 1)
                    {
                        seleccionandoMapa = true;
                        opcion = 0;
                    }
                    else if (opcion == 2)
                    {
                        salirMenu = true;
                        al_destroy_display(displayM);
                        exit(0);
                    }
                }
            }

            else
            {
                if (event.keyboard.keycode == ALLEGRO_KEY_UP)
                {
                    opcion = (opcion + totalOpcionesMapa - 1) % totalOpcionesMapa;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
                {
                    opcion = (opcion + 1) % totalOpcionesMapa;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
                {
                    seleccionandoMapa = false;
                    *mapaSeleccionado = opcion + 1;
                    seleccionarMapa(*mapaSeleccionado, mapa);
                    al_destroy_event_queue(eventqueue2);
                    ejecutarJuego(*mapaSeleccionado, displayM, nivelActual, eventqueue2);
                }
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));

        al_draw_bitmap(fondoMenu, 0, 0, 0);
        al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 2 - 50, ALLEGRO_ALIGN_CENTER, "Menu Principal");

        for (int i = 0; i < totalOpciones; i++)
        {
            int posY = ALTO_VENTANA / 2 + (i + 1) * 40;
            if (opcion == i)
            {
                al_draw_textf(fuente, textColor, ANCHO_VENTANA / 2, posY, ALLEGRO_ALIGN_CENTER, "-> %s", opciones[i]);
            }
            else
            {
                al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, posY, ALLEGRO_ALIGN_CENTER, opciones[i]);
            }
        }

        if (seleccionandoMapa)
        {
            al_draw_bitmap(fondoMenu, 0, 0, 0);
            al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 2 - 40, ALLEGRO_ALIGN_CENTER, "Selecciona un mapa:");
            for (int i = 0; i < totalOpcionesMapa; i++)
            {
                int posY = ALTO_VENTANA / 2 + (i + 1) * 40;
                if (opcion == i)
                {
                    al_draw_textf(fuente, textColor, ANCHO_VENTANA / 2, posY, ALLEGRO_ALIGN_CENTER, "-> Mapa %d", i + 1);
                }
                else
                {
                    al_draw_textf(fuente, textColor, ANCHO_VENTANA / 2, posY, ALLEGRO_ALIGN_CENTER, "Mapa %d", i + 1);
                }
            }
        }

        al_flip_display();
    }

    al_destroy_font(fuente);
    al_destroy_bitmap(fondoMenu);
}


void dibujarMapa(char mapa[MAX_FILAS][MAX_COLUMNAS], ALLEGRO_BITMAP *sprite_barrera, ALLEGRO_BITMAP *sprite_trampa, ALLEGRO_BITMAP *sprite_trampa2, ALLEGRO_BITMAP *sprite_trampa3, ALLEGRO_BITMAP *sprite_trampa4, ALLEGRO_BITMAP *sprite_llave, ALLEGRO_BITMAP *sprite_puertaL)
{
    int fila, columna, x, y;
    char c;
    for (fila = 0; fila < MAX_FILAS; fila++)
    {
        for (columna = 0; columna < MAX_COLUMNAS; columna++)
        {
            c = mapa[fila][columna];
            x = columna * TAMANO_CELDA;
            y = fila * TAMANO_CELDA;

            if (c == '#')
            {
                al_draw_bitmap(sprite_barrera, x, y, 0);
            }
            else if (c == 'd')
            {
                al_draw_bitmap(sprite_trampa, x, y, 0);
            }
            if (c == 'a')
            {
                al_draw_bitmap(sprite_trampa2, x, y, 0);
            }
            else if (c == 'l')
            {
                al_draw_bitmap(sprite_trampa3, x, y, 0);
            }
            if (c == 'b')
            {
                al_draw_bitmap(sprite_trampa4, x, y, 0);
            }
            else if (c == 'u')
            {
                llaveposX = columna * TAMANO_CELDA;
                llaveposY = fila * TAMANO_CELDA;
            }
            if (c == 'p')
            {
                puertaPosx = x;
                puertaPosy = y;
            }
        }
    }
}

void renderTesoros(Tesoros tesoro[], Personaje personaje, ALLEGRO_BITMAP *sprite_tesoro)
{
    int i;
    for (i = 0; i < contT; i++)
    {
        int distancia_x = personaje.pos_x - tesoro[i].tes_posx;
        int distancia_y = personaje.pos_y - tesoro[i].tes_posy;
        int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;

        int distancia_radio = 20000;

        if (distancia_cuadrada < distancia_radio)
        {
            al_draw_bitmap(sprite_tesoro, tesoro[i].tes_posx - tesoro[i].anchoT, tesoro[i].tes_posy - tesoro[i].altoT, 0);
        }
    }
}

void renderLlave(Personaje personaje, ALLEGRO_BITMAP* sprite_llave) {
    int i;
    int distancia_x = personaje.pos_x - llaveposX;
    int distancia_y = personaje.pos_y - llaveposY;
    int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;

    int distancia_radio = 5000;

    if(!llaveRecogida)
    {
        if (distancia_cuadrada < distancia_radio) {
            al_draw_bitmap(sprite_llave, llaveposX, llaveposY, 0);
        }
    }
}

void mostrarMenu(ALLEGRO_DISPLAY *displayM, ALLEGRO_EVENT_QUEUE *eventqueue2, char mapa[MAX_FILAS][MAX_COLUMNAS], int *mapaSeleccionado, int nivelActual)
{
    ALLEGRO_FONT *fuente = al_create_builtin_font();
    al_clear_to_color(al_map_rgb(0, 0, 0)); 
    
    al_draw_text(fuente, al_map_rgb(255, 255, 255), ANCHO_VENTANA / 2, ALTO_VENTANA / 2 - 100, ALLEGRO_ALIGN_CENTER, "Escoja la opcion presionando la tecla 1 o 2");
    al_draw_text(fuente, al_map_rgb(255, 255, 255), ANCHO_VENTANA / 2, ALTO_VENTANA / 2 - 50, ALLEGRO_ALIGN_CENTER, "1. Continuar");
    al_draw_text(fuente, al_map_rgb(255, 255, 255), ANCHO_VENTANA / 2, ALTO_VENTANA / 2, ALLEGRO_ALIGN_CENTER, "2. Salir");
    al_flip_display();

    bool enMenu = true;

    while (enMenu)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_1)
            {
                enMenu = false; 
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_2)
            {
                enMenu = false;
                exit(0); 
            }
        }
    }
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
}

void actualizarPuntaje(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje, int *puntaje)
{
    int fila = personaje.pos_y / TAMANO_CELDA;
    int columna = personaje.pos_x / TAMANO_CELDA;

    if (mapa[fila][columna] == '$')
    {
        *puntaje += 1000;
        mapa[fila][columna] = ' ';
    }
}

void dibujarPuntaje(int puntaje)
{
    int tamano_fuente = 30;
    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);

    int pos_x_texto = ANCHO_VENTANA - 10;
    int pos_y_texto = 10;

    al_draw_textf(font, color, pos_x_texto, pos_y_texto, ALLEGRO_ALIGN_RIGHT, "Puntaje: %d", puntaje);
    al_destroy_font(font);
}

void moverEnemigo(char mapa[MAX_FILAS][MAX_COLUMNAS], Enemigo enemigos[], int contE)
{
    int i;
    for (i = 0; i < contE; i++)
    {
        int pos_x = enemigos[i].ene_posx / TAMANO_CELDA;
        int pos_y = enemigos[i].ene_posy / TAMANO_CELDA;
        int nueva_pos_x = (enemigos[i].ene_posx + enemigos[i].desplazamiento) / TAMANO_CELDA;

        if (mapa[pos_y][nueva_pos_x] != '#')
        {
            enemigos[i].ene_posx += enemigos[i].desplazamiento;
        }
        else
        {
            enemigos[i].desplazamiento = -enemigos[i].desplazamiento;
        }
    }
}

void moverEnemigoV(char mapa[MAX_FILAS][MAX_COLUMNAS], EnemigoV enemigosV[], int contV)
{
    int i;
    for (i = 0; i < contV; i++)
    {
        int columna = enemigosV[i].eneV_posx / TAMANO_CELDA;
        int fila = enemigosV[i].eneV_posy / TAMANO_CELDA;
        int nueva_pos_y = (enemigosV[i].eneV_posy + enemigosV[i].desplazamientoV) / TAMANO_CELDA;

        if (mapa[nueva_pos_y][columna] != '#')
        {
            enemigosV[i].eneV_posy += enemigosV[i].desplazamientoV;
        }
        else
        {
            enemigosV[i].desplazamientoV = -enemigosV[i].desplazamientoV;
        }
    }
}

void moverEnemigoP(char mapa[MAX_FILAS][MAX_COLUMNAS], EnemigoP enemigosP[], Personaje personaje, int contP)
{
    int i;
    for (i = 0; i < contP; i++)
    {
        int columna = enemigosP[i].eneP_posx / TAMANO_CELDA;
        int fila = enemigosP[i].eneP_posy / TAMANO_CELDA;

        int tiempo_maximo_persecucion = 1800;
        static int contador_persecucion = 0;

        if (!enemigosP[i].persigue)
        {
            contador_persecucion = 0;
        }

        int distancia_x = personaje.pos_x - enemigosP[i].eneP_posx;
        int distancia_y = personaje.pos_y - enemigosP[i].eneP_posy;

        if (distancia_x * distancia_x + distancia_y * distancia_y < 50000)
        {
            enemigosP[i].persigue = true;
        }
        else
        {
            enemigosP[i].persigue = false;
        }

        if (enemigosP[i].persigue)
        {
            float velocidad = 1;

            int desplazamiento_x = 0;
            int desplazamiento_y = 0;

            if (distancia_x != 0)
            {
                if (distancia_x > 0)
                {
                    desplazamiento_x = velocidad;
                }
                else
                {
                    desplazamiento_x = -velocidad;
                }
            }

            if (distancia_y != 0)
            {
                if (distancia_y > 0)
                {
                    desplazamiento_y = velocidad;
                }
                else
                {
                    desplazamiento_y = -velocidad;
                }
            }

            int nueva_pos_x = enemigosP[i].eneP_posx + desplazamiento_x;
            int nueva_pos_y = enemigosP[i].eneP_posy + desplazamiento_y;

            if (!colisionBarrera(mapa, nueva_pos_x, nueva_pos_y, enemigosP[i].anchoP_enesprite, enemigosP[i].altoP_enesprite))
            {
                enemigosP[i].eneP_posx = nueva_pos_x;
                enemigosP[i].eneP_posy = nueva_pos_y;

                contador_persecucion++;

                if (contador_persecucion >= tiempo_maximo_persecucion)
                {
                    enemigosP[i].persigue = false;
                    contador_persecucion = 0;
                }
            }
        }
    }
}

void perseguirPersonaje(Personaje personaje, EnemigoP enemigosP[], int contP)
{
    int i;
    for (i = 0; i < contP; i++)
    {
        int distancia_x = personaje.pos_x - enemigosP[i].eneP_posx;
        int distancia_y = personaje.pos_y - enemigosP[i].eneP_posy;

        int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;

        int distancia_radio = 20000;
        enemigosP[i].persigue = (distancia_cuadrada < distancia_radio);
    }
}

bool colisionBarrera(char mapa[MAX_FILAS][MAX_COLUMNAS], int pos_x, int pos_y, int ancho_sprite, int alto_sprite)
{
    int fila, columna;
    int inicio_x = pos_x / TAMANO_CELDA;
    int inicio_y = pos_y / TAMANO_CELDA;
    int fin_x = (pos_x + ancho_sprite - 1) / TAMANO_CELDA;
    int fin_y = (pos_y + alto_sprite - 1) / TAMANO_CELDA;

    for (fila = inicio_y; fila <= fin_y; fila++)
    {
        for (columna = inicio_x; columna <= fin_x; columna++)
        {
            if (mapa[fila][columna] == '#')
            {
                return true;
            }
        }
    }

    return false;
}

bool colisionTrampa(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje)
{
    int fila, columna;
    int inicio_x = (personaje.pos_x + personaje.ancho_sprite / 2) / TAMANO_CELDA;
    int inicio_y = (personaje.pos_y + personaje.alto_sprite / 2) / TAMANO_CELDA;
    int fin_x = (personaje.pos_x + personaje.ancho_sprite - 1 - personaje.ancho_sprite / 2) / TAMANO_CELDA;
    int fin_y = (personaje.pos_y + personaje.alto_sprite - 1 - personaje.alto_sprite / 2) / TAMANO_CELDA;

    for (fila = inicio_y; fila <= fin_y; fila++)
    {
        for (columna = inicio_x; columna <= fin_x; columna++)
        {
            if (mapa[fila][columna] == 'd' || mapa[fila][columna] == 'a' || mapa[fila][columna] == 'b' || mapa[fila][columna] == 'l')
            {
                return true;
            }
        }
    }
    return false;
}

bool colisionPuertaU(char mapa[MAX_FILAS][MAX_COLUMNAS], int pos_x, int pos_y, int ancho_sprite, int alto_sprite)
{
    int fila, columna;
    int inicio_x = pos_x / TAMANO_CELDA;
    int inicio_y = pos_y / TAMANO_CELDA;
    int fin_x = (pos_x + ancho_sprite - 1) / TAMANO_CELDA;
    int fin_y = (pos_y + alto_sprite - 1) / TAMANO_CELDA;

    for (fila = inicio_y; fila <= fin_y; fila++)
    {
        for (columna = inicio_x; columna <= fin_x; columna++)
        {
            if (mapa[fila][columna] == 'p' && puertaBloqueada == false)
            {
                return true;
            }
        }
    }

    return false;
}

bool colisionConLlave(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje)
{
    int fila = personaje.pos_y / TAMANO_CELDA;
    int columna = personaje.pos_x / TAMANO_CELDA;

    if (mapa[fila][columna] == 'u')
    {
        mapa[fila][columna] = ' ';
        return true;
    }
    return false;
}

bool colisionconTesoro(Tesoros tesoro[], Personaje personaje, int contT)
{
    int i;
    for (i = 0; i < contT; i++)
    {
        int distancia_x = personaje.pos_x - tesoro[i].tes_posx;
        int distancia_y = personaje.pos_y - tesoro[i].tes_posy;

        int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;

        int distancia_radio = 500;

        if (distancia_cuadrada < distancia_radio)
        {
            tesoro[i] = tesoro[contT - 1];
            contT--;

            for (int j = i; j < contT; j++)
            {
                tesoro[j].tes_posx = tesoro[j + 1].tes_posx;
                tesoro[j].tes_posy = tesoro[j + 1].tes_posy;
            }

            return true;
        }
    }

    return false;
}

bool colisionPuertaB(char mapa[MAX_FILAS][MAX_COLUMNAS], int pos_x, int pos_y, int ancho_sprite, int alto_sprite)
{
    int fila, columna;
    int inicio_x = pos_x / TAMANO_CELDA;
    int inicio_y = pos_y / TAMANO_CELDA;
    int fin_x = (pos_x + ancho_sprite - 1) / TAMANO_CELDA;
    int fin_y = (pos_y + alto_sprite - 1) / TAMANO_CELDA;

    for (fila = inicio_y; fila <= fin_y; fila++)
    {
        for (columna = inicio_x; columna <= fin_x; columna++)
        {
            if (mapa[fila][columna] == 'p' && puertaBloqueada == true)
            {
                return true;
            }
        }
    }

    return false;
}

bool colisionConEnemigoE(Enemigo enemigos[], Personaje personaje, int contE)
{
    int i;
    for (i = 0; i < contE; i++)
    {
        int distancia_x = personaje.pos_x - enemigos[i].ene_posx;
        int distancia_y = personaje.pos_y - enemigos[i].ene_posy;

        int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;

        int distancia_colision = 21 * 24;

        if (distancia_cuadrada < distancia_colision)
        {
            return true;
        }
    }

    return false;
}

bool colisionConEnemigoV(EnemigoV enemigosV[], Personaje personaje, int contV)
{
    int i;
    for (i = 0; i < contV; i++)
    {
        int distancia_x = personaje.pos_x - enemigosV[i].eneV_posx;
        int distancia_y = personaje.pos_y - enemigosV[i].eneV_posy;

        int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;

        int distancia_colision = 22 * 22;

        if (distancia_cuadrada < distancia_colision)
        {
            return true;
        }
    }

    return false;
}

bool colisionConEnemigoP(EnemigoP enemigosP[], Personaje personaje, int contP)
{
    int i;
    for (i = 0; i < contP; i++)
    {
        int distancia_x = personaje.pos_x - enemigosP[i].eneP_posx;
        int distancia_y = personaje.pos_y - enemigosP[i].eneP_posy;
        

        int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;

        int distancia_colision = 21 * 24;

        if (distancia_cuadrada < distancia_colision)
        {
            return true;
        }
    }

    return false;
}

void seleccionarMapa(int mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS])
{
    char nombreArchivo[20];
    sprintf(nombreArchivo, "mapa/mapa%d.txt", mapaSeleccionado);
    FILE *archivo = fopen(nombreArchivo, "r");
    if (!archivo)
    {
        fprintf(stderr, "Error al abrir el archivo de mapa.\n");
        exit(EXIT_FAILURE);
    }

    int fila;
    for (fila = 0; fila < MAX_FILAS; fila++)
    {
        if (fgets(mapa[fila], MAX_COLUMNAS + 3, archivo) == NULL)
        {
            fclose(archivo);
            fprintf(stderr, "Error al leer el archivo de mapa.\n");
            exit(EXIT_FAILURE);
        }
    }

    fclose(archivo);
}

void cargarMapa(int mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS])
{
    ALLEGRO_FONT *fuente = al_create_builtin_font();
    char nombreArchivo[20];
    puertaBloqueada = true;
    llaveRecogida = false;
    puntaje = 0;
    contT = 0;

    sprintf(nombreArchivo, "mapa/mapa%d.txt", mapaSeleccionado + 1);
    FILE *archivo = fopen(nombreArchivo, "r");

    int fila;
    for (fila = 0; fila < MAX_FILAS; fila++)
    {
        fgets(mapa[fila], MAX_COLUMNAS + 3, archivo);
    }

    //tiempo_anterior = 0;
    //refresh_animacion = 0.03;  
    frame_actualV = 0;
    contE = inicializarEnemigosE(mapa);
    contV = inicializarEnemigosV(mapa);
    contP = inicializarEnemigosP(mapa);
    contT = inicializarTesoros(mapa, sprite_tesoro);
    total_tesoros = contT;  
    tesoro_actual = 0;
    //tiempo_actual = 0;
    //tiempo_gen = 0;
    inicializarTesoros(mapa, sprite_tesoro);
    inicializarPersonaje(mapa);
    inicializarEnemigosE(mapa);
    inicializarEnemigosV(mapa);
    inicializarEnemigosP(mapa);    

    leerEnemigoE(contE);
    leerEnemigoV(contV);
    leerEnemigoP(contP);
    leerTesoros(contT);

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(fuente, al_map_rgb(255, 255, 255), ANCHO_VENTANA / 2, ALTO_VENTANA / 2, ALLEGRO_ALIGN_CENTER, "Pasaste de nivel! :D");
    al_flip_display();    
    al_rest(2.0);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    fclose(archivo);
}



void reiniciarPersonaje(Personaje personaje)
{
    personaje.pos_x = personaje.spawnX;
    personaje.pos_y = personaje.spawnY;
    personaje.corazones = 4;
}



void cargarRecursos(){
    fondoMenu = al_load_bitmap("sprites/fondoMenu.png");
    sprite = al_load_bitmap("sprites/personaje.png");
    sprite_izq = al_load_bitmap("sprites/personaje_izq.png");
    sprite_der = al_load_bitmap("sprites/personaje_der.png");
    sprite_muerte = al_load_bitmap("sprites/personaje_rip.png");
    sprite_barrera = al_load_bitmap("sprites/barrera.png");
    sprite_tesoro = al_load_bitmap("sprites/Tesoro1.png");
    sprite_trampa = al_load_bitmap("sprites/trampa1.png");
    sprite_trampa2 = al_load_bitmap("sprites/trampa2.png");
    sprite_trampa3 = al_load_bitmap("sprites/trampa3.png");
    sprite_trampa4 = al_load_bitmap("sprites/trampa4.png");
    sprite_enemigo = al_load_bitmap("sprites/enemigo.png");
    sprite_enemigoV = al_load_bitmap("sprites/enemigoV.png");
    sprite_enemigoP = al_load_bitmap("sprites/enemigoP.png");
    sprite_llave = al_load_bitmap("sprites/llave.png");
    sprite_llaveRecogida = al_load_bitmap("sprites/llaveReco.png");
    sprite_puertaL = al_load_bitmap("sprites/puertaL.png");
    sprite_puertaU = al_load_bitmap("sprites/puertaU.png");
    corazon_lleno = al_load_bitmap("sprites/corazonlleno.png");
    corazon_vacio = al_load_bitmap("sprites/corazonvacio.png");

}

void liberarRecursos(ALLEGRO_DISPLAY* displayM){
    int i;
    for (i = 0; i < 4; i++)
    {
        al_destroy_bitmap(frames_enemigo[i]);
    }
    for (i = 0; i < 3; i++)
    {
        al_destroy_bitmap(frames_enemigoV[i]);
    }
    for (i = 0; i < 4; i++)
    {
        al_destroy_bitmap(frames_enemigoP[i]);
    }    
    al_destroy_bitmap(sprite_enemigo);
    al_destroy_bitmap(sprite);
    al_destroy_bitmap(sprite_barrera);
    al_destroy_bitmap(sprite_tesoro);
    al_destroy_bitmap(sprite_trampa);
    al_destroy_bitmap(sprite_trampa2);
    al_destroy_bitmap(sprite_trampa3);
    al_destroy_bitmap(sprite_trampa4);
    al_destroy_bitmap(sprite_llave);
    al_destroy_bitmap(sprite_llaveRecogida);
    al_destroy_bitmap(sprite_puertaU);
    al_destroy_bitmap(sprite_puertaL);
    al_destroy_bitmap(corazon_lleno);
    al_destroy_bitmap(corazon_vacio);
    al_destroy_display(displayM);
    al_destroy_event_queue(eventQueue);
    al_destroy_timer(timer);

}

void recibeDaño(Personaje *personaje){
    if(!personaje->invulnerable)
    {
        personaje->corazones -=1;
        personaje->invulnerable = true;
        personaje->invulnerableTimer = al_get_time() + 2.0;
    }
}