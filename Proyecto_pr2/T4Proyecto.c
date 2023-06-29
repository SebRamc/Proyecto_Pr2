#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define MAX_FILAS 20
#define MAX_COLUMNAS 20
#define TAMANO_CELDA 30
#define ANCHO_VENTANA 800
#define ALTO_VENTANA 600

void dibujarMapa(char mapa[MAX_FILAS][MAX_COLUMNAS], ALLEGRO_BITMAP* sprite, int pos_x, int pos_y) {
    for (int fila = 0; fila < MAX_FILAS; fila++) {
        for (int columna = 0; columna < MAX_COLUMNAS; columna++) {
            char c = mapa[fila][columna];
            int x = columna * TAMANO_CELDA;
            int y = fila * TAMANO_CELDA;

            if (c == '#') {
                al_draw_filled_rectangle(x, y, x + TAMANO_CELDA, y + TAMANO_CELDA, al_map_rgb(0, 0, 0));
            } else if (c == '.') {
                al_draw_filled_rectangle(x, y, x + TAMANO_CELDA, y + TAMANO_CELDA, al_map_rgb(255, 255, 255));
            }
        }
    }
    al_draw_bitmap(sprite, pos_x, pos_y, 0);
}

int main() {
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* eventQueue = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_BITMAP* sprite = NULL;

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
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());

    ALLEGRO_KEYBOARD_STATE keyboardState;
    bool salir = false;

    char mapa[MAX_FILAS][MAX_COLUMNAS];

    FILE* archivo = fopen("mapa.txt", "r");
    if (!archivo) {
        fprintf(stderr, "Error al abrir el archivo de mapa.\n");
        return -1;
    }

    for (int fila = 0; fila < MAX_FILAS; fila++) {
        if (fgets(mapa[fila], sizeof(mapa[fila]), archivo) == NULL) {
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

    int pos_x = 0;
    int pos_y = 0;

    al_start_timer(timer);

	while (!salir) {
	    ALLEGRO_EVENT event;
	    al_wait_for_event(eventQueue, &event);
	
	    if (event.type == ALLEGRO_EVENT_TIMER) {
	        al_get_keyboard_state(&keyboardState);
	
	        if (al_key_down(&keyboardState, ALLEGRO_KEY_UP)) {
	            if (pos_y > 0 && mapa[pos_y / TAMANO_CELDA - 1][pos_x / TAMANO_CELDA] != '#') {
	                pos_y -= TAMANO_CELDA / 2;
	            }
	        } else if (al_key_down(&keyboardState, ALLEGRO_KEY_DOWN)) {
	            if (pos_y < (MAX_FILAS - 1) * TAMANO_CELDA && mapa[pos_y / TAMANO_CELDA + 1][pos_x / TAMANO_CELDA] != '#') {
	                pos_y += TAMANO_CELDA / 2;
	            }
	        } else if (al_key_down(&keyboardState, ALLEGRO_KEY_LEFT)) {
	            if (pos_x > 0 && mapa[pos_y / TAMANO_CELDA][pos_x / TAMANO_CELDA - 1] != '#') {
	                pos_x -= TAMANO_CELDA / 2;
	            }
	        } else if (al_key_down(&keyboardState, ALLEGRO_KEY_RIGHT)) {
	            if (pos_x < (MAX_COLUMNAS - 1) * TAMANO_CELDA && mapa[pos_y / TAMANO_CELDA][pos_x / TAMANO_CELDA + 1] != '#') {
	                pos_x += TAMANO_CELDA / 2;
	            }
	        }
	
	        if (pos_x < 0) {
	            pos_x = 0;
	        } else if (pos_x > (MAX_COLUMNAS - 1) * TAMANO_CELDA) {
	            pos_x = (MAX_COLUMNAS - 1) * TAMANO_CELDA;
	        }
	
	        if (pos_y < 0) {
	            pos_y = 0;
	        } else if (pos_y > (MAX_FILAS - 1) * TAMANO_CELDA) {
	            pos_y = (MAX_FILAS - 1) * TAMANO_CELDA;
	        }
	
	        al_clear_to_color(al_map_rgb(0, 0, 0));
	        dibujarMapa(mapa, sprite, pos_x, pos_y);
	        al_flip_display();
	    } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
	        salir = true;
	    }
	}
	
	
	
	    al_destroy_bitmap(sprite);
	    al_destroy_display(display);
	    al_destroy_event_queue(eventQueue);
	    al_destroy_timer(timer);
	
	    return 0;
}

