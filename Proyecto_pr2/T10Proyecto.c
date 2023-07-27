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
#define ajuste_ene -20


typedef struct {
    int pos_x;
    int pos_y;
    int ancho_sprite;
    int alto_sprite;
} Personaje;

typedef struct {
    int ene_posx;
    int ene_posy;
    int ancho_enesprite;
    int alto_enesprite;
    int desplazamiento;
} Enemigo;

typedef struct {
    int eneV_posx;
    int eneV_posy;
    int anchoV_enesprite;
    int altoV_enesprite;
    int desplazamientoV;
} EnemigoV;

typedef struct {
    int pos_x;
    int pos_y;
    int ancho_enesprite;
    int alto_enesprite;
    int desplazamiento;
    bool persigue;
} EnemigoP;

void MenuPrinc(ALLEGRO_DISPLAY* displayM, ALLEGRO_EVENT_QUEUE* eventqueue2, int* mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS]);
void Juego(int mapaSeleccionado, ALLEGRO_DISPLAY* displayM);
void seleccionarMapa(int mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS]);
void dibujarMapa(char mapa[MAX_FILAS][MAX_COLUMNAS], ALLEGRO_BITMAP* sprite_barrera, ALLEGRO_BITMAP* sprite_tesoro, ALLEGRO_BITMAP* sprite_trampa, ALLEGRO_BITMAP* sprite_trampa2);
bool colisionBarrera(char mapa[MAX_FILAS][MAX_COLUMNAS], int pos_x, int pos_y, int ancho_sprite, int alto_sprite);
bool colisionTrampa(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje);
void moverEnemigo(char mapa[MAX_FILAS][MAX_COLUMNAS], Enemigo* enemigo, ALLEGRO_BITMAP* sprite_enemigo);
void moverEnemigoV(char mapa[MAX_FILAS][MAX_COLUMNAS], EnemigoV* enemigoV, ALLEGRO_BITMAP* sprite_enemigo);
void perseguirPersonaje(Personaje personaje, EnemigoP* perseguidor);
void moverEnemigoP(char mapa[MAX_FILAS][MAX_COLUMNAS], EnemigoP* perseguidor, Personaje personaje, ALLEGRO_BITMAP* sprite_enemigo);
bool colisionConEnemigoE(Enemigo enemigo, Personaje personaje);
bool colisionConEnemigoV(EnemigoV enemigoV, Personaje personaje);
bool colisionConEnemigoP(EnemigoP perseguidor, Personaje personaje);
void actualizarPuntaje(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje, int* puntaje);
void dibujarPuntaje(int puntaje);

int main() {
    al_init();
    al_init_font_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_image_addon();
    ALLEGRO_DISPLAY* displayM = al_create_display(ANCHO_VENTANA, ALTO_VENTANA);
    ALLEGRO_EVENT_QUEUE* eventQueue2 = al_create_event_queue();
    al_register_event_source(eventQueue2, al_get_display_event_source(displayM));
    al_register_event_source(eventQueue2, al_get_keyboard_event_source());
    char mapa[MAX_FILAS][MAX_COLUMNAS];
    int mapaSeleccionado = 1;
    
    MenuPrinc(displayM, eventQueue2, &mapaSeleccionado, mapa);
    
    al_destroy_event_queue(eventQueue2);
    
    if (mapaSeleccionado > 0) {
        Juego(mapaSeleccionado, displayM);
    }
    
    al_destroy_display(displayM);

    return 0;
}

void Juego(int mapaSeleccionado, ALLEGRO_DISPLAY* displayM) {
    ALLEGRO_EVENT_QUEUE* eventQueue = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_BITMAP* sprite = NULL;
    ALLEGRO_BITMAP* sprite_barrera = NULL;
    ALLEGRO_BITMAP* sprite_tesoro = NULL;
    ALLEGRO_BITMAP* sprite_trampa = NULL;
    ALLEGRO_BITMAP* sprite_trampa2 = NULL;
    ALLEGRO_BITMAP* sprite_enemigo = NULL;
    ALLEGRO_BITMAP* frames_enemigo[4];

    int i, fila, puntaje = 0;
    int frame_actual = 0;

    Personaje personaje;
    personaje.pos_x = 80;
    personaje.pos_y = 40;
    personaje.ancho_sprite = 13;
    personaje.alto_sprite = 21;

    Enemigo enemigo;
    enemigo.ene_posx = 90;
    enemigo.ene_posy = 390;

    EnemigoV enemigoV;
    enemigoV.eneV_posx = 1020;
    enemigoV.eneV_posy = 330;
    enemigoV.anchoV_enesprite = 21;
    enemigoV.altoV_enesprite = 24;
    enemigoV.desplazamientoV = -1;

    EnemigoP perseguidor;
    perseguidor.pos_x = 750;
    perseguidor.pos_y = 120;
    perseguidor.ancho_enesprite = 30;
    perseguidor.alto_enesprite = 30;
    perseguidor.desplazamiento = 2;
    perseguidor.persigue = false;


    eventQueue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60);

    al_register_event_source(eventQueue, al_get_display_event_source(displayM));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));
    ALLEGRO_KEYBOARD_STATE keyboardState;
    bool salir = false;

    char mapa[MAX_FILAS][MAX_COLUMNAS];
    seleccionarMapa(mapaSeleccionado, mapa);
    
    sprite = al_load_bitmap("sprites/Man.png");
    sprite_barrera = al_load_bitmap("sprites/barrera.png");
    sprite_tesoro = al_load_bitmap("sprites/Tesoro1.png");
    sprite_trampa = al_load_bitmap("sprites/trampa1.png");
    sprite_trampa2 = al_load_bitmap("sprites/trampa2.png");
    sprite_enemigo = al_load_bitmap("sprites/enemigo.png");

    int enemigo_ancho_imagen = al_get_bitmap_width(sprite_enemigo);
    int enemigo_alto_imagen = al_get_bitmap_height(sprite_enemigo);

    enemigo.ancho_enesprite = 30;
    enemigo.alto_enesprite = 30;
    enemigo.desplazamiento = -1;

    for (i = 0; i < 4; i++) {
        frames_enemigo[i] = al_create_sub_bitmap(sprite_enemigo, i * (enemigo_ancho_imagen / 4), 0, enemigo_ancho_imagen / 4, enemigo_alto_imagen);
    }

    al_start_timer(timer);

    while (!salir) {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventQueue, &event);
        al_clear_to_color(al_map_rgb(0, 0, 0));

        if (event.type == ALLEGRO_EVENT_TIMER) {
            al_get_keyboard_state(&keyboardState);

            if (al_key_down(&keyboardState, ALLEGRO_KEY_UP)) {
                if (personaje.pos_y > 0 && !colisionBarrera(mapa, personaje.pos_x, personaje.pos_y - TAMANO_CELDA / 8, personaje.ancho_sprite, personaje.alto_sprite)) {
                    personaje.pos_y -= TAMANO_CELDA / 8;
                }
            } else if (al_key_down(&keyboardState, ALLEGRO_KEY_DOWN)) {
                if (personaje.pos_y < (MAX_FILAS - 1) * TAMANO_CELDA - personaje.alto_sprite && !colisionBarrera(mapa, personaje.pos_x, personaje.pos_y + TAMANO_CELDA / 8, personaje.ancho_sprite, personaje.alto_sprite)) {
                    personaje.pos_y += TAMANO_CELDA / 8;
                }
            } else if (al_key_down(&keyboardState, ALLEGRO_KEY_LEFT)) {
                if (personaje.pos_x > 0 && !colisionBarrera(mapa, personaje.pos_x - TAMANO_CELDA / 8, personaje.pos_y, personaje.ancho_sprite, personaje.alto_sprite)) {
                    personaje.pos_x -= TAMANO_CELDA / 8;
                }
            } else if (al_key_down(&keyboardState, ALLEGRO_KEY_RIGHT)) {
                if (personaje.pos_x < (MAX_COLUMNAS - 1) * TAMANO_CELDA - personaje.ancho_sprite && !colisionBarrera(mapa, personaje.pos_x + TAMANO_CELDA / 8, personaje.pos_y, personaje.ancho_sprite, personaje.alto_sprite)) {
                    personaje.pos_x += TAMANO_CELDA / 8;
                }
            }

            if (colisionTrampa(mapa, personaje)) {
                personaje.pos_x = 80;
                personaje.pos_y = 40;
            }
            frame_actual = (frame_actual + 1) % 4;

            moverEnemigo(mapa, &enemigo, sprite_enemigo);
            moverEnemigoV(mapa, &enemigoV, sprite_enemigo);
            moverEnemigoP(mapa, &perseguidor, personaje, sprite_enemigo);
            perseguirPersonaje(personaje, &perseguidor);
            

            if (colisionConEnemigoE(enemigo, personaje)){
                personaje.pos_x = 80;
                personaje.pos_y = 40;
            }

            if (colisionConEnemigoV(enemigoV, personaje)){
                personaje.pos_x = 80;
                personaje.pos_y = 40;                
            }  

            if (colisionConEnemigoP(perseguidor, personaje)) {
            personaje.pos_x = 80;
            personaje.pos_y = 40;
            }

            al_clear_to_color(al_map_rgb(255, 255, 255));
            dibujarMapa(mapa, sprite_barrera, sprite_tesoro, sprite_trampa, sprite_trampa2);

            al_draw_bitmap(sprite, personaje.pos_x, personaje.pos_y, 0);
            al_draw_bitmap(frames_enemigo[frame_actual], enemigo.ene_posx - enemigo.ancho_enesprite / 2, enemigo.ene_posy - enemigo.alto_enesprite / 2, 0);
            al_draw_bitmap(frames_enemigo[frame_actual], enemigoV.eneV_posx - enemigoV.anchoV_enesprite / 2, enemigoV.eneV_posy - enemigoV.altoV_enesprite / 2, 0);
            al_draw_bitmap(frames_enemigo[frame_actual], perseguidor.pos_x - perseguidor.ancho_enesprite / 2, perseguidor.pos_y - perseguidor.alto_enesprite / 2, 0);
            dibujarPuntaje(puntaje);
            actualizarPuntaje(mapa, personaje, &puntaje);            
            al_flip_display();
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            salir = true;
        }
    }

    for (i = 0; i < 4; i++) {
        al_destroy_bitmap(frames_enemigo[i]);
    }

    al_destroy_bitmap(sprite_enemigo);
    al_destroy_bitmap(sprite);
    al_destroy_bitmap(sprite_barrera);
    al_destroy_bitmap(sprite_tesoro);
    al_destroy_bitmap(sprite_trampa);
    al_destroy_bitmap(sprite_trampa2);
    al_destroy_display(displayM);
    al_destroy_event_queue(eventQueue);
    al_destroy_timer(timer);

}

void MenuPrinc(ALLEGRO_DISPLAY* displayM, ALLEGRO_EVENT_QUEUE* eventqueue2, int* mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS]) {
    ALLEGRO_FONT* fuente = al_create_builtin_font();
    ALLEGRO_COLOR textColor = al_map_rgb(255, 255, 255);
    int opcion = 0;
    int totalOpciones = 3;
    bool salirMenu = false;
    bool seleccionandoMapa = false;
    const char* opciones[] = {
        "Jugar",
        "Seleccionar mapa",
        "Salir"
    };
    while (!salirMenu) {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventqueue2, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            salirMenu = true;
            break;
        }

        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (!seleccionandoMapa) {
                if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                    opcion = (opcion + totalOpciones - 1) % totalOpciones;
                } else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    opcion = (opcion + 1) % totalOpciones;
                } else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    if (opcion == 0) {
                        salirMenu = true;
                        break;
                    } else if (opcion == 1) {
                        seleccionandoMapa = true;
                        opcion = 0;
                        
                    } else if (opcion == 2) {
                        salirMenu = true;
                        al_destroy_display(displayM);
                        exit(0);
                    }
                }
            } else {
                if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                    opcion = (opcion + totalOpciones - 1) % totalOpciones;
                } else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    opcion = (opcion + 1) % totalOpciones;
                } else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    seleccionandoMapa = false;
                    *mapaSeleccionado = opcion + 1;
                    seleccionarMapa(*mapaSeleccionado, mapa);
                    al_destroy_event_queue(eventqueue2);
                    Juego(*mapaSeleccionado, displayM);               
                }
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));

        al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 3 - 40, ALLEGRO_ALIGN_CENTER, "Menu Principal");

        for (int i = 0; i < totalOpciones; i++) {
            int posY = ALTO_VENTANA / 2 + (i + 1) * 40;
            if (opcion == i) {
                al_draw_textf(fuente, textColor, ANCHO_VENTANA / 2, posY, ALLEGRO_ALIGN_CENTER, "-> %s", opciones[i]);
            } else {
                al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, posY, ALLEGRO_ALIGN_CENTER, opciones[i]);
            }
        }

        if (seleccionandoMapa) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 2 - 40, ALLEGRO_ALIGN_CENTER, "Selecciona un mapa:");
            for (int i = 0; i < totalOpciones; i++) {
                int posY = ALTO_VENTANA / 2 + (i + 1) * 40;
                if (opcion == i) {
                    al_draw_textf(fuente, textColor, ANCHO_VENTANA / 2, posY, ALLEGRO_ALIGN_CENTER, "-> Mapa %d", i + 1);
                } else {
                    al_draw_textf(fuente, textColor, ANCHO_VENTANA / 2, posY, ALLEGRO_ALIGN_CENTER, "Mapa %d", i + 1);
                }
            }
        }

        al_flip_display();
    }

    al_destroy_font(fuente);
}

void dibujarMapa(char mapa[MAX_FILAS][MAX_COLUMNAS], ALLEGRO_BITMAP* sprite_barrera, ALLEGRO_BITMAP* sprite_tesoro, ALLEGRO_BITMAP* sprite_trampa, ALLEGRO_BITMAP* sprite_trampa2) {
    int fila, columna, x, y;
    char c;
    for (fila = 0; fila < MAX_FILAS; fila++) {
        for (columna = 0; columna < MAX_COLUMNAS; columna++) {
            c = mapa[fila][columna];
            x = columna * TAMANO_CELDA;
            y = fila * TAMANO_CELDA;
            if (c == '#') {
                al_draw_bitmap(sprite_barrera, x, y, 0);
            } else if (c == '$') {
                al_draw_bitmap(sprite_tesoro, x, y, 0);
            }
            if (c == 'd') {
                al_draw_bitmap(sprite_trampa, x, y, 0);
            } else if (c == 'a') {
                al_draw_bitmap(sprite_trampa2, x, y, 0);
            }
        }
    }
}

bool colisionBarrera(char mapa[MAX_FILAS][MAX_COLUMNAS], int pos_x, int pos_y, int ancho_sprite, int alto_sprite) {
    int fila, columna;
    int inicio_x = pos_x / TAMANO_CELDA;
    int inicio_y = pos_y / TAMANO_CELDA;
    int fin_x = (pos_x + ancho_sprite - 1) / TAMANO_CELDA;
    int fin_y = (pos_y + alto_sprite - 1) / TAMANO_CELDA;

    for (fila = inicio_y; fila <= fin_y; fila++) {
        for (columna = inicio_x; columna <= fin_x; columna++) {
            if (mapa[fila][columna] == '#') {
                return true;
            }
        }
    }

    return false;
}

bool colisionTrampa(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje) {
    int fila, columna;
    int inicio_x = (personaje.pos_x + personaje.ancho_sprite / 2) / TAMANO_CELDA;
    int inicio_y = (personaje.pos_y + personaje.alto_sprite / 2) / TAMANO_CELDA;
    int fin_x = (personaje.pos_x + personaje.ancho_sprite - 1 - personaje.ancho_sprite / 2) / TAMANO_CELDA;					
    int fin_y = (personaje.pos_y + personaje.alto_sprite - 1 - personaje.alto_sprite / 2) / TAMANO_CELDA;

    for (fila = inicio_y; fila <= fin_y; fila++) {
        for (columna = inicio_x; columna <= fin_x; columna++) {
            if (mapa[fila][columna] == 'd' || mapa[fila][columna] == 'a') {
                return true;
            }
        }
    }
    return false;
}

void actualizarPuntaje(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje, int* puntaje) {
    int fila = personaje.pos_y / TAMANO_CELDA;
    int columna = personaje.pos_x / TAMANO_CELDA;


    if (mapa[fila][columna] == '$') {
        *puntaje += 1000;
        mapa[fila][columna] = ' ';
    }
}

void dibujarPuntaje(int puntaje) {
    int tamano_fuente = 30;
    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);

    int pos_x_texto = ANCHO_VENTANA - 10;
    int pos_y_texto = 10;

    al_draw_textf(font, color, pos_x_texto, pos_y_texto, ALLEGRO_ALIGN_RIGHT, "Puntaje: %d", puntaje);
    al_destroy_font(font);
}

void moverEnemigo(char mapa[MAX_FILAS][MAX_COLUMNAS], Enemigo* enemigo, ALLEGRO_BITMAP* sprite_enemigo) {
    int pos_x = enemigo->ene_posx / TAMANO_CELDA;
    int pos_y = enemigo->ene_posy / TAMANO_CELDA;
    int nueva_pos_x = (enemigo->ene_posx + enemigo->desplazamiento) / TAMANO_CELDA;

    if (mapa[pos_y][nueva_pos_x] != '#') {
        enemigo->ene_posx += enemigo->desplazamiento;
    } else {
        enemigo->desplazamiento = -enemigo->desplazamiento;
    }

    al_draw_bitmap(sprite_enemigo, enemigo->ene_posx, enemigo->ene_posy, 0);
}

void moverEnemigoV(char mapa[MAX_FILAS][MAX_COLUMNAS], EnemigoV* enemigoV, ALLEGRO_BITMAP* sprite_enemigo) {
    int columna = enemigoV->eneV_posx / TAMANO_CELDA;
    int fila = enemigoV->eneV_posy / TAMANO_CELDA;
    int nueva_pos_y = (enemigoV->eneV_posy + enemigoV->desplazamientoV) / TAMANO_CELDA;

    if (mapa[nueva_pos_y][columna] != '#') {
        enemigoV->eneV_posy += enemigoV->desplazamientoV;
    } else {
        enemigoV->desplazamientoV = -enemigoV->desplazamientoV;
    }

    al_draw_bitmap(sprite_enemigo, enemigoV->eneV_posx - enemigoV->anchoV_enesprite / 2, enemigoV->eneV_posy - enemigoV->altoV_enesprite / 2, 0);
}

void perseguirPersonaje(Personaje personaje, EnemigoP* perseguidor) {
    int distancia_x = personaje.pos_x - perseguidor->pos_x;
    int distancia_y = personaje.pos_y - perseguidor->pos_y;

    int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;

    int distancia_radio = 50000; 
    perseguidor->persigue = (distancia_cuadrada < distancia_radio);
}

void moverEnemigoP(char mapa[MAX_FILAS][MAX_COLUMNAS], EnemigoP* perseguidor, Personaje personaje, ALLEGRO_BITMAP* sprite_enemigo) {
    int columna = perseguidor->pos_x / TAMANO_CELDA;
    int fila = perseguidor->pos_y / TAMANO_CELDA;
    
    int tiempo_maximo_persecucion = 180; 
    static int contador_persecucion = 0;

    if (!perseguidor->persigue) {
        contador_persecucion = 0;
    }

    int distancia_x = personaje.pos_x - perseguidor->pos_x;
    int distancia_y = personaje.pos_y - perseguidor->pos_y;

    if (distancia_x * distancia_x + distancia_y * distancia_y < 50000) {
        perseguidor->persigue = true;
    } else {
        perseguidor->persigue = false;
    }

    if (perseguidor->persigue) {
        float velocidad = 1;

        int desplazamiento_x = 0;
        int desplazamiento_y = 0;

        if (distancia_x != 0) {
            if (distancia_x > 0) {
                desplazamiento_x = velocidad;  
            } else {
                desplazamiento_x = -velocidad;
            }
        }

        if (distancia_y != 0) {
            if (distancia_y > 0) {
                desplazamiento_y = velocidad;  
            } else {
                desplazamiento_y = -velocidad;
            }
        }

        int nueva_pos_x = perseguidor->pos_x + desplazamiento_x;
        int nueva_pos_y = perseguidor->pos_y + desplazamiento_y;

        if (!colisionBarrera(mapa, nueva_pos_x, nueva_pos_y, perseguidor->ancho_enesprite, perseguidor->alto_enesprite)) {
            perseguidor->pos_x = nueva_pos_x;
            perseguidor->pos_y = nueva_pos_y;
            
            contador_persecucion++;
            
            if (contador_persecucion >= tiempo_maximo_persecucion) {
                perseguidor->persigue = false;
                contador_persecucion = 0;
            }
        }
    }

    al_draw_bitmap(sprite_enemigo, perseguidor->pos_x - perseguidor->ancho_enesprite / 2, perseguidor->pos_y - perseguidor->alto_enesprite / 2, 0);
}

bool colisionConEnemigoE(Enemigo enemigo, Personaje personaje) {
    int distancia_x = personaje.pos_x - enemigo.ene_posx;
    int distancia_y = personaje.pos_y - enemigo.ene_posy;

    int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;


    int distancia_colision = 21 * 24; 

    if (distancia_cuadrada < distancia_colision) {
        return true; 
    } else {
        return false;
    }
}

bool colisionConEnemigoV(EnemigoV enemigoV, Personaje personaje) {
    int distancia_x = personaje.pos_x - enemigoV.eneV_posx;
    int distancia_y = personaje.pos_y - enemigoV.eneV_posy;

    int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;


    int distancia_colision = 22 * 22; 

    if (distancia_cuadrada < distancia_colision) {
        return true; 
    } else {
        return false;
    }
}

bool colisionConEnemigoP(EnemigoP perseguidor, Personaje personaje) {
    int distancia_x = personaje.pos_x - perseguidor.pos_x;
    int distancia_y = personaje.pos_y - perseguidor.pos_y;

    int distancia_cuadrada = distancia_x * distancia_x + distancia_y * distancia_y;


    int distancia_colision = 21 * 24; 

    if (distancia_cuadrada < distancia_colision) {
        return true; 
    } else {
        return false;
    }
}

void seleccionarMapa(int mapaSeleccionado, char mapa[MAX_FILAS][MAX_COLUMNAS]) {
    char nombreArchivo[20];
    sprintf(nombreArchivo, "mapa/mapa%d.txt", mapaSeleccionado);
    FILE* archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        fprintf(stderr, "Error al abrir el archivo de mapa.\n");
        exit(EXIT_FAILURE);
    }

    int fila;
    for (fila = 0; fila < MAX_FILAS; fila++) {
        if (fgets(mapa[fila], MAX_COLUMNAS + 3, archivo) == NULL) {
            fclose(archivo);
            fprintf(stderr, "Error al leer el archivo de mapa.\n");
            exit(EXIT_FAILURE);
        }
    }

    fclose(archivo);
}