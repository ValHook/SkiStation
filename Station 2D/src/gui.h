#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <allegro.h>
#include <winalleg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "domain.h"
#include "libsaferesource.h"
#include "sha1.h"

/* ENUMERATIONS */
/* Enumeration for pinpoint modes */
enum EPinpointMode
{
        PINPOINT_DEFAULT,
        PINPOINT_HOVERED,
        PINPOINT_SELECTED
};

/* Enumeration for edit box orientation */
enum EEditBoxOrientation
{
        LABEL_ON_THE_LEFT,
        LABEL_ON_THE_RIGHT
};

/* Enumeration for pause menu modes */
enum EPauseMenuMode
{
        DEFAULT,
        RESUME,
        CUSTOMIZE,
        CREDITS,
        QUIT
};

/* Enumeration for reset animation mode */
enum EResetAnimation
{
        DONT_ANIMATE,
        RANDOM_ANIMATE,
        DO_ANIMATE
};

/* Enumeration for the different modes */
enum ESoftwareMode
{
        STAND_BY,
        PROMPT_PATH_FILTER,
        SHOWING_PATH,
        PAUSED,
        ENDED
};



/* DATA STRUCTURES */
/* Struct that holds every images */
typedef struct SContextImages
{
        BITMAP *pre_screen, *buffer, *mouse;
        BITMAP *splashscreen;
        BITMAP *background;
        BITMAP *popover, *pinpoint, *pinpoint_hovered, *pinpoint_selected;
        BITMAP **edges;
        BITMAP *fastest_button, *nicest_button, *easiest_button, *cancel_button;
        BITMAP *resume_button, *customize_button, *credits_button, *quit_button;
        BITMAP *plus, *minus;
        int *pinpoints_y;
} context_images;

/* Software context structure */
typedef struct SSoftwareContext
{
        /* Main variables */
        domain_t *domain;
        context_images *images;
        enum ESoftwareMode mode;
        int can_press_key, can_click;

        /* Usual view variables */
        RECT viewport; /* Visible part of the buffer */
        int mouse_enabled;
        int old_mouse_z, old_mouse_x, old_mouse_y, zoom;
        int hovered, selected, selected2;
        int halo_phase;

        /* Prompt path filter variables */
        int path_option;

        /* Draw path variables */
        edge_vector *ev;
        int current_edge;
        int current_pos_in_edge;

        /* Pause menu variables */
        int *route_times[NUMBER_OF_EDGE_TYPES - 1];
        enum EPauseMenuMode pause_option;
} sw_ctx;





/* FUNCTIONS */
/* Function that performs a sha1 checksum verification
 * for every images of the project */
void sha1_bitmaps_check(void);

/* Allegro toolset */
void startAllegroFullScreen(int min_width, int min_height);
BITMAP *create_bitmap_ex2(int width, int height);
BITMAP *load_bitmap_ex(const char *filename);
void destroy_bitmap_ex(BITMAP *bmp);
void show_bitmapcounter(void);


/* Software context loader */
sw_ctx *start_software_context(domain_t *domain);
void reset_context(sw_ctx *ctx, enum EResetAnimation animate);
void enable_mouse(sw_ctx *ctx);
void delete_software_context(sw_ctx *ctx);

/* Images loader */
context_images *load_context_images(int nb_nodes, int nb_edges);
void delete_context_images(context_images *images, int nb_edges);

/* Helper views */
void splashscreen(sw_ctx *ctx);
void pinpoint(sw_ctx *ctx, int node_id, enum EPinpointMode);
void draw_pinpoints(sw_ctx *ctx);
void popover(sw_ctx *ctx, int node_id);
void draw_popovers(sw_ctx *ctx);
void edit_box(sw_ctx *ctx, enum EEditBoxOrientation orientation, int posx, int posy, const char *title, int *var);
void grayscale(sw_ctx *ctx);
void blur(sw_ctx *ctx);

/* Main view controlers */
void draw_usual_view(sw_ctx *ctx);
void prompt_path_filter(sw_ctx *ctx);
void draw_path(sw_ctx *ctx);
void draw_pause_view(sw_ctx *ctx);

/* Video buffer management functions */
void prepare_viewport(sw_ctx *ctx);
void swap_buffer(sw_ctx *ctx);

/* Event receiver */
void get_context_events(sw_ctx *ctx);
void prepare_path(sw_ctx *ctx, enum EPathType filter);

#endif // GUI_H_INCLUDED
