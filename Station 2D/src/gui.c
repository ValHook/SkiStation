#include "gui.h"

/* Reference counting variables, initialized at 0 at compile time */
static int nb_bitmaps;



/* RESOURCE CHECKER */
void sha1_bitmaps_check(void)
{
        char filename[20];
        unsigned char sha1_digest[20], sha1_digest_tmp[20];
        int i;
        for (i = 0; i < 20; i++)
                sha1_digest[i] = 0;

        for (i = 1; i <= 254; i++) {
                sprintf(filename, "images/%d.bmp", i);
                sha1_file(filename, sha1_digest_tmp);
                sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        }

        sha1_file("images/background.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/button-cancel.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/button-credits.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/button-customize.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/button-easiest.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/button-fastest.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/button-nicest.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/button-quit.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/button-resume.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/logo-ece.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/minus.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/mouse.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/pinpoint.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/pinpoint-hovered.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/plus.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);
        sha1_file("images/popover.bmp", sha1_digest_tmp);
        sum_sha1_digests(sha1_digest, sha1_digest_tmp);



        unsigned char checksum[20] =
        {0x01, 0xC1, 0x03, 0x77, 0x88, 0xFC, 0x26, 0x81, 0x19, 0x8F,
        0x88, 0x5A, 0x49, 0x63, 0x69, 0x5B, 0xF4, 0x7A, 0x9F, 0x07};
        if (memcmp(checksum, sha1_digest, sizeof(sha1_digest)) != 0)
                die("Checksum failed");
}

/* ALLEGRO LAUNCHER */
void startAllegroFullScreen(int min_width, int min_height)
{
        /* Get desktop resolution */
        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);

        /* Bind minimum screen dimensions */
        if (desktop.right < min_width || desktop.bottom < min_height)
                die("You must have at least a %dx%d screen resolution", min_width, min_height);

        /* Continue launching allegro */
        allegro_init();
        set_color_depth(32);
        if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, desktop.right,desktop.bottom,0,0) != 0) {
                allegro_exit();
                die("Cannot initialize allegro");
        }
        install_keyboard();
        install_mouse();
}






/* SAFE FUNCTIONS */
/* 2 because the ex one already existed */
BITMAP *create_bitmap_ex2(int width, int height)
{
        BITMAP *bmp = create_bitmap(width, height);
        if (!bmp)
                die("Cannot create bitmap");

        nb_bitmaps++;
        return bmp;
}

BITMAP *load_bitmap_ex(const char *filename)
{
        BITMAP *bmp = load_bitmap(filename, NULL);
        if (!bmp)
                die("Cannot load image %s", filename);

        nb_bitmaps++;
        return bmp;
}

void destroy_bitmap_ex(BITMAP *bmp)
{
        if (!bmp)
                die("Cannot destroy NULL bitmap");

        destroy_bitmap(bmp);
        nb_bitmaps--;
}

void show_bitmapcounter(void)
{
        printf("Video RAM bitmap result: %d", nb_bitmaps);
}













/* SOFTWARE LOADER */
sw_ctx *start_software_context(domain_t *domain)
{
        int i;
        srand(time(NULL));

        sw_ctx *ctx = calloc_ex(1, sizeof(sw_ctx));
        ctx->domain = domain;
        for (i = 0; i < NUMBER_OF_EDGE_TYPES - 1; i++)
                ctx->route_times[i] = &(get_domain_edge_types(domain)[i].route_time);

        ctx->images = load_context_images(get_domain_nodes(domain)->length, get_domain_edges(domain)->length);

        ctx->viewport.right = ctx->images->buffer->w;
        ctx->viewport.bottom = ctx->images->buffer->h;
        ctx->old_mouse_x = -1;
        ctx->old_mouse_y = -1;
        ctx->hovered = -1;
        ctx->selected = -1;
        ctx->selected2 = -1;
        ctx->path_option = -1;
        ctx->current_edge = -1;
        ctx->current_pos_in_edge = -1;
        ctx->pause_option = DEFAULT;
        ctx->mode = STAND_BY;

        return ctx;
}

void enable_mouse(sw_ctx *ctx)
{
        ctx->mouse_enabled = 1;
}

context_images *load_context_images(int nb_nodes, int nb_edges)
{
        char filename[20];
        int i;
        context_images *images;

        images = malloc_ex(sizeof(context_images));
        images->pre_screen = create_bitmap_ex2(SCREEN_W, SCREEN_H);
        images->mouse = load_bitmap_ex("images/mouse.bmp");
        images->background = load_bitmap_ex("images/background.bmp");
        images->buffer = create_bitmap_ex2(images->background->w, images->background->h);
        images->pinpoint = load_bitmap_ex("images/pinpoint.bmp");
        images->pinpoint_hovered = load_bitmap_ex("images/pinpoint-hovered.bmp");
        images->pinpoint_selected = create_bitmap_ex2(images->pinpoint->w, images->pinpoint->h);
        blit(images->pinpoint, images->pinpoint_selected, 0, 0, 0, 0, images->pinpoint->w, images->pinpoint->h);
        images->popover = load_bitmap_ex("images/popover.bmp");

        images->fastest_button = load_bitmap_ex("images/button-fastest.bmp");
        images->nicest_button = load_bitmap_ex("images/button-nicest.bmp");
        images->easiest_button = load_bitmap_ex("images/button-easiest.bmp");
        images->cancel_button = load_bitmap_ex("images/button-cancel.bmp");

        images->resume_button = load_bitmap_ex("images/button-resume.bmp");
        images->customize_button = load_bitmap_ex("images/button-customize.bmp");
        images->credits_button = load_bitmap_ex("images/button-credits.bmp");
        images->quit_button = load_bitmap_ex("images/button-quit.bmp");

        images->plus = load_bitmap_ex("images/plus.bmp");
        images->minus = load_bitmap_ex("images/minus.bmp");

        images->splashscreen = load_bitmap_ex("images/logo-ece.bmp");

        images->pinpoints_y = calloc_ex(nb_nodes, sizeof(int));
        images->edges = calloc_ex(nb_edges, sizeof(BITMAP *));


        for (i = 1; i <= nb_edges; i++){
                sprintf(filename, "images/%d.bmp", i);
                images->edges[i - 1] = load_bitmap_ex(filename);
        }
        return images;
}

/* SOFTWARE RESET */
void reset_context(sw_ctx *ctx, enum EResetAnimation animate)
{
        /* Reset animation */
        switch (animate) {
        case DO_ANIMATE:
        {
                memset(ctx->images->pinpoints_y, 0, get_domain_nodes(ctx->domain)->length * sizeof(int));
                break;
        }
        case RANDOM_ANIMATE:
        {
                if (rand()%5 == 0)
                        memset(ctx->images->pinpoints_y, 0, get_domain_nodes(ctx->domain)->length * sizeof(int));
                break;
        }
        case DONT_ANIMATE: ;
        }

        /* Reset other variables */
        ctx->path_option = -1;
        ctx->current_edge = -1;
        ctx->current_pos_in_edge = -1;
        if (ctx->mode != PAUSED) {
                ctx->hovered = -1;
                ctx->selected = -1;
                ctx->selected2 = -1;
        }
        if (ctx->ev) {
                remove_edge_vector(ctx->ev);
                ctx->ev = NULL;
        }
        if (ctx->pause_option == RESUME)
                ctx->pause_option = DEFAULT;

        ctx->mode = STAND_BY;
}

/* SOFTWARE DESTRUCTOR */
void delete_software_context(sw_ctx *ctx)
{
        delete_context_images(ctx->images, get_domain_edges(ctx->domain)->length);
        if (ctx->ev) {
                remove_edge_vector(ctx->ev);
                ctx->ev = NULL;
        }
        free_ex(ctx);

}

void delete_context_images(context_images *images, int nb_edges)
{
        destroy_bitmap_ex(images->pre_screen);
        destroy_bitmap_ex(images->mouse);
        destroy_bitmap_ex(images->background);
        destroy_bitmap_ex(images->buffer);
        destroy_bitmap_ex(images->pinpoint);
        destroy_bitmap_ex(images->pinpoint_hovered);
        destroy_bitmap_ex(images->pinpoint_selected);
        destroy_bitmap_ex(images->popover);

        destroy_bitmap_ex(images->fastest_button);
        destroy_bitmap_ex(images->nicest_button);
        destroy_bitmap_ex(images->easiest_button);
        destroy_bitmap_ex(images->cancel_button);

        destroy_bitmap_ex(images->resume_button);
        destroy_bitmap_ex(images->customize_button);
        destroy_bitmap_ex(images->credits_button);
        destroy_bitmap_ex(images->quit_button);

        destroy_bitmap_ex(images->plus);
        destroy_bitmap_ex(images->minus);

        destroy_bitmap_ex(images->splashscreen);

        int i;
        for (i = 0; i < nb_edges; i++)
                destroy_bitmap_ex(images->edges[i]);

        free_ex(images->pinpoints_y);
        free_ex(images->edges);
        free_ex(images);
}






/* HELPER VIEWS */
void splashscreen(sw_ctx *ctx)
{
        int i, j, step, pixel;
        int originx, originy;
        BITMAP *splsh = ctx->images->splashscreen;
        BITMAP *pre_screen = ctx->images->pre_screen;
        clear_to_color(pre_screen, makecol(255, 255, 255));

        originx = (SCREEN_W - splsh->w)/2;
        originy = (SCREEN_H - splsh->h)/2;
        step = 0;

        /* Fade Out */
        while (step == 0) {
                step = 1;
                for (i = 0; i < splsh->w; i++) {
                        for (j = 0; j < splsh->h; j++) {
                                pixel = getpixel(pre_screen, originx + i, originy + j);

                                if (getr(pixel) > getr(getpixel(splsh, i, j)) +1) {
                                        pixel = makecol(getr(pixel) - 2, getg(pixel), getb(pixel));
                                        step = 0;
                                }

                                if (getg(pixel) > getg(getpixel(splsh, i, j)) +1) {
                                        pixel = makecol(getr(pixel), getg(pixel) - 2, getb(pixel));
                                        step = 0;
                                }

                                if (getb(pixel) > getb(getpixel(splsh, i, j)) +1) {
                                        pixel = makecol(getr(pixel), getg(pixel), getb(pixel) - 2);
                                        step = 0;
                                }

                                putpixel(pre_screen, originx + i, originy + j, pixel);
                        }
                }
                swap_buffer(ctx);
        }

        /* Fade In */
        while (step == 1) {
                step = 2;
                for (i = 0; i < splsh->w; i++) {
                        for (j = 0; j < splsh->h; j++) {
                                pixel = _getpixel32(pre_screen, originx + i, originy + j);

                                if (getr(pixel) < 254) {
                                        pixel = makecol(getr(pixel) + 1, getg(pixel), getb(pixel));
                                        step = 1;
                                }

                                if (getg(pixel) < 254) {
                                        pixel = makecol(getr(pixel), getg(pixel) + 1, getb(pixel));
                                        step = 1;
                                }

                                if (getb(pixel) < 254) {
                                        pixel = makecol(getr(pixel), getg(pixel), getb(pixel) + 2);
                                        step = 1;
                                }

                                _putpixel32(pre_screen, originx + i, originy + j, pixel);
                        }
                }
                swap_buffer(ctx);
        }
}

void pinpoint(sw_ctx *ctx, int node_id, enum EPinpointMode mode)
{
        int i, j;
        int *pinpoints_y = ctx->images->pinpoints_y;
        node_vector *nv = get_domain_nodes(ctx->domain);
        BITMAP *pinpoint_default = ctx->images->pinpoint;
        BITMAP *pinpoint_hovered = ctx->images->pinpoint_hovered;
        BITMAP *pinpoint_selected = ctx->images->pinpoint_selected;
        BITMAP *buffer = ctx->images->buffer;

        /* Halo Effetc */
        int period = (ctx->mode == STAND_BY || ctx->mode == SHOWING_PATH) ? 600 : 20;
        ctx->halo_phase++;
        for (i = 0; i < pinpoint_selected->w; i++)
                for (j = 0; j < pinpoint_selected->h; j++)
                        if (ctx->selected != -1)
                                if (_getpixel32(pinpoint_selected, i, j) != makecol(0, 0, 0) && getpixel(pinpoint_selected, i, j) != makecol(255, 0, 255))
                                        _putpixel32(pinpoint_selected, i, j, makecol(0, 128 + 127*sin(ctx->halo_phase/period), 255));

        /* Draw Pinpoint */
        switch(mode) {
        case PINPOINT_DEFAULT: draw_sprite(buffer, pinpoint_default, nv->items[node_id]->pos_x - 13, pinpoints_y[node_id] - 40); /* Hovered */
                                break;
        case PINPOINT_HOVERED: draw_sprite(buffer, pinpoint_hovered, nv->items[node_id]->pos_x - 13, pinpoints_y[node_id] - 40); /* Selected */
                                break;
        case PINPOINT_SELECTED: draw_sprite(buffer, pinpoint_selected, nv->items[node_id]->pos_x - 13, pinpoints_y[node_id] - 40); /* Default */
        }
}

void draw_pinpoints(sw_ctx *ctx)
{
        int i;
        node_vector *nv = get_domain_nodes(ctx->domain);
        int *pinpoints_y = ctx->images->pinpoints_y;


        /* Usual pinpoints */
        for (i = 0; i < nv->length; i++) {
                pinpoint(ctx, i, (i == ctx->hovered) ? PINPOINT_HOVERED : PINPOINT_DEFAULT);
                /* Pinpoint falldown */
                if (pinpoints_y[i] < nv->items[i]->pos_y)
                         pinpoints_y[i]+=nv->items[i]->pos_y/100;
                else if (pinpoints_y[i] > nv->items[i]->pos_y)
                        pinpoints_y[i]--;
        }


        /* Selected pinpoints */
        if (ctx->selected != -1) {
                /* Pinpoint for first selected node */
                pinpoint(ctx, ctx->selected, PINPOINT_SELECTED);
        }
        if (ctx->selected2 != -1) {
                /* Pinpoint for second selected node */
                pinpoint(ctx, ctx->selected2, PINPOINT_SELECTED);
        }

        if (ctx->ev) {
                /* Pinpoint for path nodes */
                for (i = 0; i < ctx->current_edge; i++)
                        if (i < ctx->ev->length - 1)
                                pinpoint(ctx, ctx->ev->items[i]->child, PINPOINT_HOVERED);
        }
}



void popover(sw_ctx *ctx, int node_id)
{
        /* Resources */
        node_vector *nv = get_domain_nodes(ctx->domain);
        char node_location[255];
        int popover_width;
        int *pinpoints_y = ctx->images->pinpoints_y;
        BITMAP *buffer = ctx->images->buffer;
        BITMAP *popover = ctx->images->popover;
        BITMAP *pinpoint = ctx->images->pinpoint;

        /* Popover */
        get_node_location(node_location, ctx->domain, node_id);
        popover_width = 10*strlen(node_location);
        masked_stretch_blit(popover, buffer, 0, 0, popover->w, popover->h,
                     nv->items[node_id]->pos_x - popover_width/2,pinpoints_y[node_id] - 68 - pinpoint->h, popover_width, popover->h);
        textprintf_centre_ex(buffer, font, nv->items[node_id]->pos_x, pinpoints_y[node_id] - 45 - pinpoint->h,
                      makecol(0,0,0), -1, "%s", node_location);
}

void draw_popovers(sw_ctx *ctx)
{
        if (ctx->selected != -1) {
                /* Popover for first selected node */
                popover(ctx, ctx->selected);
        }
        if (ctx->selected2 != -1) {
                /* Popover for second selected node */
                popover(ctx, ctx->selected2);
        }
        if (ctx->hovered != -1) {
                /* Popover for hovered node */
                popover(ctx, ctx->hovered);
        }

        if (ctx->ev) {
                /* Popover for path nodes */
                int i;
                for (i = 0; i < ctx->current_edge; i++)
                        popover(ctx, ctx->ev->items[i]->child);
        }
}

void edit_box(sw_ctx *ctx, enum EEditBoxOrientation orientation, int posx, int posy, const char *title, int *var)
{
        int i, j;
        BITMAP *pre_screen = ctx->images->pre_screen;
        BITMAP *plus = ctx->images->plus;
        BITMAP *minus = ctx->images->minus;

        int o[2]; /* Mouse buttons offsets */
        o[0] = (orientation == LABEL_ON_THE_LEFT) ? posx + 148 : posx - 172; /* Plus button */
        o[1] = (orientation == LABEL_ON_THE_LEFT) ? posx + 174 : posx - 198; /* Minus button */

        /* Lefty UI */
        if (orientation == LABEL_ON_THE_LEFT) {
                textprintf_right_ex(pre_screen, font, posx - 10, posy + 7, makecol(255, 255, 255), -1, "%s :", title);
                rect(pre_screen, posx, posy, posx + 140, posy + 20, makecol(255, 255, 255));
                textprintf_ex(pre_screen, font, posx + 10, posy + 7, makecol(255, 255, 255), -1, "%d", *var);
        }

        /* Righty UI */
        if (orientation == LABEL_ON_THE_RIGHT) {
                textprintf_ex(pre_screen, font, posx + 10, posy + 7, makecol(255, 255, 255), -1, ": %s", title);
                rect(pre_screen, posx - 140, posy, posx, posy + 20, makecol(255, 255, 255));
                textprintf_right_ex(pre_screen, font, posx - 10, posy + 7, makecol(255, 255, 255), -1, "%d", *var);
        }

        /* Buttons */
        stretch_blit(plus, pre_screen, 0, 0, plus->w, plus->h, o[0], posy - 2, 24, 24);
        stretch_blit(minus, pre_screen, 0, 0, minus->w, minus->h, o[1], posy - 2, 24, 24);

        /* On Hover, On Click */
        if (mouse_y > posy - 2 && mouse_y < posy + 22) {
                if (mouse_x > o[0] && mouse_x < o[0] + 24) {
                        for (i = o[0]; i <= o[0] + 24; i++)
                                for (j = posy - 2; j <= posy + 22; j++)
                                        if (_getpixel32(pre_screen, i, j) >= makecol(0xf0, 0xf0, 0xf0))
                                                _putpixel32(pre_screen, i, j, makecol(0, 200 - (j - posy +2)* 8, 255));

                        if (*var < INT_MAX - 10 && ctx->can_click && (mouse_b & 1)) {
                                *var = *var + 10;
                                update_domain_route_times(ctx->domain);
                        }

                } else if (mouse_x > o[1] && mouse_x < o[1] + 24) {
                        for (i = o[1]; i <= o[1] + 24; i++)
                                for (j = posy - 2; j <= posy + 22; j++)
                                        if (_getpixel32(pre_screen, i, j) >= makecol(0xf0, 0xf0, 0xf0))
                                                _putpixel32(pre_screen, i, j, makecol(0, 200 - (j - posy +2)* 9, 255));

                        if (*var > 10 && ctx->can_click && (mouse_b & 1)) {
                                *var = *var - 10;
                                update_domain_route_times(ctx->domain);
                        }
                }
        }
}


void grayscale(sw_ctx *ctx)
{
        int i, j, grey_level;

        for (i = 0; i < SCREEN_W; i++) {
                for (j = 0; j < SCREEN_H; j++) {
                        grey_level = _getpixel32(ctx->images->pre_screen, i, j);
                        grey_level = 0.299*getr(grey_level) + 0.587*getg(grey_level) + 0.114*getb(grey_level);
                        _putpixel32(ctx->images->pre_screen, i, j, makecol(grey_level, grey_level, grey_level));
                }
        }
}

void blur(sw_ctx *ctx)
{
        set_trans_blender(0, 0, 0, 127);
        draw_trans_sprite(ctx->images->pre_screen, ctx->images->pre_screen, 4, 0);
        set_trans_blender(0, 0, 0, 0);
}










/* VIEWCONTROLLERS */
void draw_usual_view(sw_ctx *ctx)
{
        int i;
        int *hovered = &ctx->hovered;
        int *selected = &ctx->selected, *selected2 = &ctx->selected2;
        BITMAP *buffer = ctx->images->buffer;
        BITMAP *background = ctx->images->background;
        BITMAP *pinpoint = ctx->images->pinpoint;
        RECT *viewport = &ctx->viewport;
        node_vector *nv = get_domain_nodes(ctx->domain);


        /* Absolute mouse position */
        int msx = viewport->left + mouse_x*viewport->right/SCREEN_W;
        int msy = viewport->top + mouse_y*viewport->bottom/SCREEN_H;


        /* HANDLE ZOOM */
        if (mouse_z != ctx->old_mouse_z) {
                /* get zoom factor */
                ctx->zoom += (mouse_z - ctx->old_mouse_z);
                ctx->old_mouse_z = mouse_z;
                if (ctx->zoom < 0) {
                        ctx->zoom = 0;
                } else if (ctx->zoom > 10) {
                        ctx->zoom = 10;
                } else {
                        /* Calculate viewport */
                        viewport->right = SCREEN_W + (buffer->w - SCREEN_W)*(10 - ctx->zoom)/10;
                        viewport->bottom = SCREEN_H + (buffer->h - SCREEN_H)*(10 - ctx->zoom)/10;
                        viewport->left = (mouse_x*ctx->images->buffer->w/SCREEN_W)*(1 - (float)viewport->right/(float)ctx->images->buffer->w);
                        viewport->top = (mouse_y*ctx->images->buffer->h/SCREEN_H)*(1 - (float)viewport->bottom/(float)ctx->images->buffer->h);
                }
        }

        /* HORIZONTAL SCROLLER */
        if (mouse_b == 4) {
                if (ctx->old_mouse_x > 0 && ctx->old_mouse_y > 0) {
                        viewport->left -= (mouse_x - ctx->old_mouse_x);
                        viewport->top -= (mouse_y - ctx->old_mouse_y);
                        if (viewport->left < 0)
                                viewport->left = 0;
                        if (viewport->left + viewport->right > buffer->w)
                                viewport->left = buffer->w - viewport->right;
                        if (viewport->top < 0)
                                viewport->top = 0;
                        if (viewport->top + viewport->bottom > buffer->h)
                                viewport->top = buffer->h - viewport->bottom;
                }
                ctx->old_mouse_x = mouse_x;
                ctx->old_mouse_y = mouse_y;
        } else {
                ctx->old_mouse_x = -1;
                ctx->old_mouse_y = -1;
        }



        /* FIND HOVERED AND SELECTED POINTS */
        if (ctx->mode == STAND_BY) {
                *hovered = -1;
                for (i = 0; i < nv->length; i++) {
                        /* Disc equation */
                        int radius = 30;
                        if (pow(msx - nv->items[i]->pos_x, 2) + pow(msy - (nv->items[i]->pos_y - pinpoint->h/2), 2) < pow(radius, 2)){
                                *hovered = i;
                                break;
                        }
                }
                if (mouse_b & 1) {
                        if (*selected == -1 || *hovered == -1) {
                                *selected = *hovered;
                         } else if (*selected != -1 && *selected != *hovered) {
                                *selected2 = *hovered;
                                ctx->mode = PROMPT_PATH_FILTER;
                        }
                }
        }





        /* DRAW VIEW */
        /* Background */
        draw_sprite(buffer, background, 0, 0);

        /* Pinpoints, we dont call them now if we're showing a path, they will be called
         * after the path paint */
        if (ctx->mode != SHOWING_PATH)
                draw_pinpoints(ctx);

        /* Popovers, we dont call them now if we're showing a path, they will be called
         * after the path paint */
        if (ctx->mode != SHOWING_PATH)
                draw_popovers(ctx);

        /* Swap buffer to viewport */
        prepare_viewport(ctx);
}









void prompt_path_filter(sw_ctx *ctx)
{
        int selected = -1, i, j;
        int *path_option = &ctx->path_option;
        BITMAP *pre_screen = ctx->images->pre_screen;
        BITMAP *buttons[4] = {
                ctx->images->fastest_button,
                ctx->images->nicest_button,
                ctx->images->easiest_button,
                ctx->images->cancel_button
        };

        /* Default UI */
        rectfill(pre_screen, 0, 100, SCREEN_W, 240, makecol(0, 0, 0));
        textprintf_centre_ex(pre_screen, font,SCREEN_W/2, 120,
                      makecol(255, 255, 255), -1, "Choisir un type de trajet pour parcourir ce chemin");
        for (i = 0; i < 4; i++)
                draw_sprite(pre_screen, buttons[i], SCREEN_W/2 - 490 + i*260, 160);


        /* On Hover UI */
        if (ctx->can_click && mouse_y < 210 && mouse_y > 160) {
                for (i = 0; i < 4; i++)
                        if (mouse_x > SCREEN_W/2 - 490 + i*260 && mouse_x < SCREEN_W/2 - 290 + i*260)
                                selected = i;

                if (selected != -1)
                        for (i = 0; i < buttons[selected]->w; i++)
                                for (j = 0; j < buttons[selected]->h; j++)
                                        if (_getpixel32(pre_screen,SCREEN_W/2 - 490 + selected*260 + i, 160 + j) <= makecol(170, 0, 0))
                                                _putpixel32(pre_screen,SCREEN_W/2 - 490 + selected*260 + i, 160 + j, makecol(0, 0xaa, 0xcc));
        }

        /* Detect buttons clicked */
        if (selected != -1 && ctx->can_click && (mouse_b & 1))
                *path_option = selected;
}


void draw_path(sw_ctx *ctx)
{
        if (!ctx->ev)
                return;

        /* Resources */
        int i, j, pixel;
        edge_vector *ev = ctx->ev;
        node_vector *nv = get_domain_nodes(ctx->domain);
        BITMAP **edges = ctx->images->edges;
        BITMAP *buffer = ctx->images->buffer;


        /* Path progression */
        if (ctx->current_edge < ev->length) {
                /* Draw current edge */
                for (i = 0; i < edges[ev->items[ctx->current_edge]->id]->w; i++) {
                        for (j = 0; j <= ctx->current_pos_in_edge; j++) {
                                if (nv->items[ev->items[ctx->current_edge]->parent]->pos_y < nv->items[ev->items[ctx->current_edge]->child]->pos_y) {
                                        pixel = getpixel(edges[ev->items[ctx->current_edge]->id], i, j);
                                        if (pixel != makecol(255, 0, 255))
                                                putpixel(buffer, ev->items[ctx->current_edge]->pos_x + i,
                                                         ev->items[ctx->current_edge]->pos_y + j, pixel);
                                } else {
                                        pixel = getpixel(edges[ev->items[ctx->current_edge]->id], i, edges[ev->items[ctx->current_edge]->id]->h - j - 1);
                                        if (pixel != makecol(255, 0, 255))
                                                putpixel(buffer, ev->items[ctx->current_edge]->pos_x + i,
                                                         ev->items[ctx->current_edge]->pos_y + edges[ev->items[ctx->current_edge]->id]->h - j - 1, pixel);
                                }
                        }

                }

                /* Edge transition */
                ctx->current_pos_in_edge++;
                if (ctx->current_pos_in_edge == edges[ev->items[ctx->current_edge]->id]->h) {
                        ctx->current_pos_in_edge = 0;
                        ctx->current_edge ++;
                }
        }



        /* Draw visited edges and nodes*/
        for (i = 0; i < ctx->current_edge; i++)
                draw_sprite(buffer, edges[ev->items[i]->id], ev->items[i]->pos_x, ev->items[i]->pos_y);

        /* Pinpoints */
        draw_pinpoints(ctx);

        /* Popovers */
        draw_popovers(ctx);

        /* Swap buffer to viewport */
        prepare_viewport(ctx);

        /* End animation notice */
        if (ctx->current_edge == ev->length) {
                grayscale(ctx);
                rectfill(ctx->images->pre_screen, 0, 0, SCREEN_W, 50, makecol(0, 0, 0));
                textprintf_centre_ex(ctx->images->pre_screen, font,SCREEN_W/2, 20,
                      makecol(255, 255, 255), -1, "Appuyer sur la touche entree pour continuer ...");
        }
}

void draw_pause_view(sw_ctx *ctx)
{
        int selected = -1, i, j;
        enum EPauseMenuMode tmp_option, *pause_option = &ctx->pause_option;
        BITMAP *pre_screen = ctx->images->pre_screen;
        BITMAP *buttons[4] = {
                ctx->images->resume_button,
                ctx->images->customize_button,
                ctx->images->credits_button,
                ctx->images->quit_button
        };

        /* Default UI */
        rectfill(pre_screen, 0, 100, SCREEN_W, 240, makecol(0, 0, 0));
        textprintf_centre_ex(pre_screen, font,SCREEN_W/2, 120,
                      makecol(255, 255, 255), -1, "Choisir une option");
        for (i = 0; i < 4; i++)
                draw_sprite(pre_screen, buttons[i], SCREEN_W/2 - 490 + i*260, 160);


        /* Credits UI */
        if (ctx->pause_option == CREDITS) {
                rectfill(pre_screen, 0, 240, SCREEN_W, 290, makecol(0, 0, 0));
                fastline(pre_screen, SCREEN_W/2 - 220, 245, SCREEN_W/2 + 220, 245, makecol(0x66, 0x66, 0x66));
                textprintf_centre_ex(pre_screen, font,SCREEN_W/2, 260,
                              makecol(255, 255, 255), -1, "Antoine Loret & Valentin Mercier - ECE Paris - 2014");
        }



        /* Customizer UI */
        char labels[NUMBER_OF_EDGE_TYPES - 1][26] =
        {
                "Pistes Vertes",
                "Pistes Bleues",
                "Pistes Rouges",
                "Pistes Noires",
                "Piste de Kiliometre Lance",
                "Snowpark",
                "Telepheriques",
                "Telecabines",
                "Telesieges Debrayables",
                "Telesieges",
                "Teleskis"
        };
        if (ctx->pause_option == CUSTOMIZE) {
                rectfill(pre_screen, 0, 240, SCREEN_W, 590, makecol(0, 0, 0));
                fastline(pre_screen, SCREEN_W/2 - 220, 245, SCREEN_W/2 + 220, 245, makecol(0x66, 0x66, 0x66));
                fastline(pre_screen, SCREEN_W/2, 310, SCREEN_W/2, 520, makecol(0x66, 0x66, 0x66));
                textprintf_centre_ex(pre_screen, font,SCREEN_W/2, 260,
                                makecol(255, 255, 255), -1, "Personnalisez votre rythme de ski, precisez en secondes "
                                "le temps pris pour 100m de denivele !");
                for (i = 0; i <= 5; i++)
                        edit_box(ctx, LABEL_ON_THE_LEFT, SCREEN_W/2 - 220, 330 + 30*i, labels[i], ctx->route_times[i]);
                for (i = 6; i < NUMBER_OF_EDGE_TYPES - 1; i++)
                        edit_box(ctx, LABEL_ON_THE_RIGHT, SCREEN_W/2 + 220, 330 + 30*(i - 6), labels[i], ctx->route_times[i]);
        }



        /* On Hover UI */
        if (ctx->can_click && mouse_y < 210 && mouse_y > 160) {
                for (i = 0; i < 4; i++)
                        if (mouse_x > SCREEN_W/2 - 490 + i*260 && mouse_x < SCREEN_W/2 - 290 + i*260)
                                selected = i;

                if (selected != -1)
                        for (i = 0; i < buttons[selected]->w; i++)
                                for (j = 0; j < buttons[selected]->h; j++)
                                        if (_getpixel32(pre_screen,SCREEN_W/2 - 490 + selected*260 + i, 160 + j) <= makecol(170, 0, 0))
                                                _putpixel32(pre_screen,SCREEN_W/2 - 490 + selected*260 + i, 160 + j, makecol(0, 0xaa, 0xcc));
        }

        /* Detect buttons clicked */
        if (selected != -1 && ctx->can_click && (mouse_b & 1)) {
                switch(selected) {
                case 0: tmp_option = RESUME;
                        break;
                case 1: tmp_option = CUSTOMIZE;
                        break;
                case 2: tmp_option = CREDITS;
                        break;
                case 3: tmp_option = QUIT;
                        break;
                default: tmp_option = DEFAULT;
                }
                *pause_option = (*pause_option != tmp_option) ? tmp_option : DEFAULT;
        }
}




/* VIDEO BUFFER MANAGEMENT FUNCTIONS */
void prepare_viewport(sw_ctx *ctx)
{
        BITMAP *buffer = ctx->images->buffer;
        BITMAP *pre_screen = ctx->images->pre_screen;
        RECT *viewport = &ctx->viewport;

        stretch_blit(buffer, pre_screen, viewport->left, viewport->top, viewport->right, viewport->bottom, 0, 0, SCREEN_W, SCREEN_H);
}

void swap_buffer(sw_ctx *ctx)
{
        /* Mouse */
        if (ctx->mouse_enabled)
                draw_sprite(ctx->images->pre_screen, ctx->images->mouse, mouse_x, mouse_y);

        /* Refresh screen */
        draw_sprite(screen, ctx->images->pre_screen, 0, 0);
}







/* EVENT RECEIVER */
void get_context_events(sw_ctx *ctx)
{
        /* Mouse events (most likely packed inside viewcontrollers for saving memory and reducing scope complexity)*/
        /* Path filter menu */
        switch (ctx->path_option) {
                case 0: prepare_path(ctx, FASTEST);
                        break;
                case 1: prepare_path(ctx, NICEST);
                        break;
                case 2: prepare_path(ctx, EASIEST);
                        break;
                case 3: reset_context(ctx, DONT_ANIMATE);
                        break;
                default: ;
        }
        /* Pause menu */
        switch (ctx->pause_option) {
                case RESUME: reset_context(ctx, RANDOM_ANIMATE);
                        break;
                case QUIT: ctx->mode = ENDED;
                        break;
                default: ;
        }
        ctx->can_click = !(mouse_b & 1);



        /* Keyboard events */
        if (ctx->mode != SHOWING_PATH && key[KEY_ESC]) {
                /* Pause Menu and resume toggler*/
                if (ctx->can_press_key) {
                        if (ctx->mode == STAND_BY)
                                ctx->mode = PAUSED;
                        else
                                reset_context(ctx, RANDOM_ANIMATE);
                }
                ctx->can_press_key = 0;

        } else if (key[KEY_ENTER]) {
                /* End of path resume event */
                if (ctx->can_press_key && ctx->mode == SHOWING_PATH && ctx->current_edge == ctx->ev->length)
                        reset_context(ctx, DO_ANIMATE);
                ctx->can_press_key = 0;

        } else if (key[KEY_BACKSPACE]) {
                /* Funny reseter, because it's so fun to see the pinpoints drop */
                if (ctx->can_press_key)
                        reset_context(ctx, DO_ANIMATE);
                ctx->can_press_key = 0;

        } else
                ctx->can_press_key = 1;
}



/* Prepare Path (Called by event receiver) */
void prepare_path(sw_ctx *ctx, enum EPathType filter)
{
        ctx->ev = get_path(filter, ctx->domain, ctx->selected, ctx->selected2);
        ctx->current_edge = 0;
        ctx->current_pos_in_edge = 0;
        ctx->mode = SHOWING_PATH;
        ctx->path_option = -1;
}
