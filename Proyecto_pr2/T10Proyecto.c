#include <stdio.h>
#include <string.h>
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

void dibujarMapa(char mapa[MAX_FILAS][MAX_COLUMNAS], ALLEGRO_BITMAP* sprite_barrera, ALLEGRO_BITMAP* sprite_tesoro, ALLEGRO_BITMAP* sprite_trampa, ALLEGRO_BITMAP* sprite_trampa2);
bool colisionBarrera(char mapa[MAX_FILAS][MAX_COLUMNAS], int pos_x, int pos_y, int ancho_sprite, int alto_sprite);
bool colisionTrampa(char mapa[MAX_FILAS][MAX_COLUMNAS], Personaje personaje);
void moverEnemigo(char mapa[MAX_FILAS][MAX_COLUMNAS], Enemigo* enemigo, ALLEGRO_BITMAP* sprite_enemigo);
void MenuPrinc(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* eventQueue2);

int main() {
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* eventQueue = NULL;
    ALLEGRO_EVENT_QUEUE* eventQueue2 = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_BITMAP* sprite = NULL;
    ALLEGRO_BITMAP* sprite_barrera = NULL;
    ALLEGRO_BITMAP* sprite_tesoro = NULL;
    ALLEGRO_BITMAP* sprite_trampa = NULL;
    ALLEGRO_BITMAP* sprite_trampa2 = NULL;
    ALLEGRO_BITMAP* sprite_enemigo = NULL;
    ALLEGRO_BITMAP* frames_enemigo[6];

    int i, fila;
    int frame_actual = 0;

    Personaje personaje;
    personaje.pos_x = 80;
    personaje.pos_y = 40;
    personaje.ancho_sprite = 13;
    personaje.alto_sprite = 21;

    Enemigo enemigo;
    enemigo.ene_posx = 1 * TAMANO_CELDA;
    enemigo.ene_posy = 1 * TAMANO_CELDA;

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

    eventQueue2 = al_create_event_queue();
    eventQueue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60);

    al_register_event_source(eventQueue2, al_get_display_event_source(display));
    al_register_event_source(eventQueue2, al_get_keyboard_event_source());

    MenuPrinc(display, eventQueue2);

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
        if (fgets(mapa[fila], MAX_COLUMNAS + 3, archivo) == NULL) {
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

    sprite_trampa = al_load_bitmap("trampa1.png");
    if (!sprite_trampa) {
        fprintf(stderr, "Error al cargar el sprite de trampa.\n");
        return -1;
    }

    sprite_trampa2 = al_load_bitmap("trampa2.png");
    if (!sprite_trampa2) {
        fprintf(stderr, "Error al cargar el sprite de la trampa 2.\n");
        return -1;
    }

    sprite_enemigo = al_load_bitmap("enemigo.png");
    if (!sprite_enemigo) {
        fprintf(stderr, "Error al cargar el sprite del enemigo.\n");
        return -1;
    }

    int enemigo_ancho_imagen = al_get_bitmap_width(sprite_enemigo);
    int enemigo_alto_imagen = al_get_bitmap_height(sprite_enemigo);

    enemigo.ancho_enesprite = 30;
    enemigo.alto_enesprite = 30;
    enemigo.desplazamiento = 10;

    for (i = 0; i < 6; i++) {
        frames_enemigo[i] = al_create_sub_bitmap(sprite_enemigo, i * (enemigo_ancho_imagen / 6), 0, enemigo_ancho_imagen / 6, enemigo_alto_imagen);
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

            moverEnemigo(mapa, &enemigo, sprite_enemigo);
            frame_actual = (frame_actual + 1) % 6;

            al_clear_to_color(al_map_rgb(255, 255, 255));
            dibujarMapa(mapa, sprite_barrera, sprite_tesoro, sprite_trampa, sprite_trampa2);

            al_draw_bitmap(sprite, personaje.pos_x, personaje.pos_y, 0);
            al_draw_bitmap(frames_enemigo[frame_actual], enemigo.ene_posx, enemigo.ene_posy, 0);
            al_flip_display();
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            salir = true;
        }
    }

    for (i = 0; i < 6; i++) {
        al_destroy_bitmap(frames_enemigo[i]);
    }

    al_destroy_bitmap(sprite_enemigo);
    al_destroy_bitmap(sprite);
    al_destroy_bitmap(sprite_barrera);
    al_destroy_bitmap(sprite_tesoro);
    al_destroy_bitmap(sprite_trampa);
    al_destroy_bitmap(sprite_trampa2);
    al_destroy_display(display);
    al_destroy_event_queue(eventQueue);
    al_destroy_timer(timer);

    return 0;
}

void MenuPrinc(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* eventqueue2){
    ALLEGRO_FONT* fuente = al_create_builtin_font();
    ALLEGRO_COLOR textColor = al_map_rgb(255, 255, 255);
    int opcion = 0;
    int totalOpciones = 3;
    bool salirMenu = false;

    while(!salirMenu){
        ALLEGRO_EVENT event;
        al_wait_for_event(eventqueue2, &event);
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            salirMenu = true;
            break;
        }
        if (event.type == ALLEGRO_EVENT_KEY_DOWN){
            if(event.keyboard.keycode == ALLEGRO_KEY_UP){
                opcion = (opcion + totalOpciones - 1) % totalOpciones;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN){
                opcion = (opcion + 1) % totalOpciones;
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));

        al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 3 - 40, ALLEGRO_ALIGN_CENTER, "Menu Principal");
        if (opcion == 0)
        {
            al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 1 * 40, ALLEGRO_ALIGN_CENTER, "-> Jugar");
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER){
                salirMenu = true;
                break;
            }
        } else{
            al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 1 * 40, ALLEGRO_ALIGN_CENTER, "Jugar");
        }
        if (opcion == 1){
            al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 2 * 40, ALLEGRO_ALIGN_CENTER, "-> Seleccion de mapa");
        } else{
            al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 2 * 40, ALLEGRO_ALIGN_CENTER, "Seleccion de mapa");
        }
        if (opcion == 2){
            al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 3 * 40, ALLEGRO_ALIGN_CENTER, "-> Salir");
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER){
                salirMenu = true;
                al_destroy_display(display);
                exit(0);
            }       
        } else{
            al_draw_text(fuente, textColor, ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 3 * 40, ALLEGRO_ALIGN_CENTER, "Salir");
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

void moverEnemigo(char mapa[MAX_FILAS][MAX_COLUMNAS], Enemigo* enemigo, ALLEGRO_BITMAP* sprite_enemigo) {
    int i, j, pos, cont = 0;
    for (j = 0; j < MAX_COLUMNAS; j++) {
        for (i = 0; i < MAX_FILAS; i++) {
            if (mapa[i][j] == 'E') {
                cont++;
                pos = i * TAMANO_CELDA;

                if (mapa[(pos - enemigo->desplazamiento) / TAMANO_CELDA][j] != '#') {
                    enemigo->ene_posx = (j * TAMANO_CELDA) - enemigo->desplazamiento;
                    enemigo->ene_posy = (i * TAMANO_CELDA) + ajuste_ene;
                    al_draw_bitmap(sprite_enemigo, enemigo->ene_posx, enemigo->ene_posy, 0);
                    enemigo->desplazamiento++;
                } else {
                    enemigo->desplazamiento = 0;
                }

                if (mapa[(pos - enemigo->desplazamiento) / TAMANO_CELDA][j] != '#') {
                    enemigo->ene_posx = (j * TAMANO_CELDA) - enemigo->desplazamiento;
                    enemigo->ene_posy = (i * TAMANO_CELDA) + ajuste_ene;
                    al_draw_bitmap(sprite_enemigo, enemigo->ene_posx, enemigo->ene_posy, 0);
                    enemigo->desplazamiento++;
                } else {
                    enemigo->desplazamiento = 0;
                }
            }
        }
    }
}


