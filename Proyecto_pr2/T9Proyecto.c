#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define MAX_FILAS 30
#define MAX_COLUMNAS 42
#define TAMANO_CELDA 30
#define ANCHO_VENTANA 1260
#define ALTO_VENTANA 900

void dibujarMapa(char mapa[MAX_FILAS][MAX_COLUMNAS], ALLEGRO_BITMAP* sprite_barrera, ALLEGRO_BITMAP* sprite_tesoro) {
	int fila, columna, x, y;
	char c;
    for (fila = 0; fila < MAX_FILAS; fila++) {
        for (columna = 0; columna < MAX_COLUMNAS; columna++) {
        	c = mapa[fila][columna];
			x = columna * (TAMANO_CELDA);
			y = fila * (TAMANO_CELDA);
            if (c == '#') {
                al_draw_bitmap(sprite_barrera, x, y, 0);
            } else if (c == '$') {
                al_draw_bitmap(sprite_tesoro, x, y, 0);
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

int main() {
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* eventQueue = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_BITMAP* sprite = NULL;
    ALLEGRO_BITMAP* sprite_barrera = NULL;
    ALLEGRO_BITMAP* sprite_tesoro = NULL;
	int fila;
    if (!al_init()) {
        fprintf(stderr, "Error al inicializar Allegro.\n");
        return -1;
    }

    display = al_create_display(ANCHO_VENTANA, ALTO_VENTANA);
    if (!display) {
        fprintf(stderr, "Error al crear la ventana.\n");
        return -1;
    }

    al_init_primitives_addon();
    al_install_keyboard();
    al_init_image_addon();

    eventQueue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60);

    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    ALLEGRO_KEYBOARD_STATE keyboardState;
    bool salir = false;

    char mapa[MAX_FILAS][MAX_COLUMNAS];
	
    FILE* archivo = fopen("mapa.txt", "r");
    if (!archivo) {
        fprintf(stderr, "Error al abrir el archivo de mapa.\n");
        return -1;
    }

    for (fila = 0; fila < MAX_FILAS; fila++) {
        if (fgets(mapa[fila], MAX_COLUMNAS + 5, archivo) == NULL) {
            fprintf(stderr, "Error al leer el archivo de mapa.\n");
            fclose(archivo);
            return -1;
        }
    }

    fclose(archivo);

    sprite = al_load_bitmap("Man.png");
    if (!sprite) {
        fprintf(stderr, "Error al cargar el sprite.\n");
        return -1;
    }

    sprite_barrera = al_load_bitmap("barrera.png");
    if (!sprite_barrera) {
        fprintf(stderr, "Error al cargar el sprite de las barreras.\n");
        return -1;
    }

    sprite_tesoro = al_load_bitmap("Tesoro1.png");
    if (!sprite_tesoro) {
        fprintf(stderr, "Error al cargar el sprite del tesoro.\n");
        return -1;
    }

    int pos_x = 0;
    int pos_y = 0;
    int ancho_sprite = 13;
    int alto_sprite = 22;

    al_start_timer(timer);

    while (!salir) {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventQueue, &event);
        al_clear_to_color(al_map_rgb(0, 0, 0));

        if (event.type == ALLEGRO_EVENT_TIMER) {
            al_get_keyboard_state(&keyboardState);

            if (al_key_down(&keyboardState, ALLEGRO_KEY_UP)) {
                if (pos_y > 0 && !colisionBarrera(mapa, pos_x, pos_y - TAMANO_CELDA / 4, ancho_sprite, alto_sprite)) {
                    pos_y -= TAMANO_CELDA / 4;
                }
            } else if (al_key_down(&keyboardState, ALLEGRO_KEY_DOWN)) {
                if (pos_y < (MAX_FILAS - 1) * TAMANO_CELDA - alto_sprite && !colisionBarrera(mapa, pos_x, pos_y + TAMANO_CELDA / 4, ancho_sprite, alto_sprite)) {
                    pos_y += TAMANO_CELDA / 4;
                }
            } else if (al_key_down(&keyboardState, ALLEGRO_KEY_LEFT)) {
                if (pos_x > 0 && !colisionBarrera(mapa, pos_x - TAMANO_CELDA / 4, pos_y, ancho_sprite, alto_sprite)) {
                    pos_x -= TAMANO_CELDA / 4;
                }
            } else if (al_key_down(&keyboardState, ALLEGRO_KEY_RIGHT)) {
                if (pos_x < (MAX_COLUMNAS - 1) * TAMANO_CELDA - ancho_sprite && !colisionBarrera(mapa, pos_x + TAMANO_CELDA / 4, pos_y, ancho_sprite, alto_sprite)) {
                    pos_x += TAMANO_CELDA / 4;
                }
            }

            al_clear_to_color(al_map_rgb(255, 255, 255));
            dibujarMapa(mapa, sprite_barrera, sprite_tesoro);
            al_draw_bitmap(sprite, pos_x, pos_y, 0);
            al_flip_display();
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            salir = true;
        }
    }

    al_destroy_bitmap(sprite);
    al_destroy_bitmap(sprite_barrera);
    al_destroy_bitmap(sprite_tesoro);
    al_destroy_display(display);
    al_destroy_event_queue(eventQueue);
    al_destroy_timer(timer);

    return 0;
}

