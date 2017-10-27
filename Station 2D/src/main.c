#include "domain.h"
#include "gui.h"


int main(void)
{
        /* PERFORM SOFTWARE RESOURCES VERIFICATIONS */
        /* Text file */
        char data_arcs[] = "data_arcs.txt";
        unsigned char checksum_data_arcs[20] =
        {0xF6, 0x2F, 0xCB, 0xC6, 0xD7, 0x64, 0x73, 0xF8, 0xE6, 0xAB,
        0x02, 0x1A, 0xF7, 0x4A, 0xBD, 0xAE, 0x25, 0x0E, 0x2C, 0xDC};
        sha1_file_check(data_arcs, checksum_data_arcs);
        /* Images */
        sha1_bitmaps_check();


        /* BEGIN SOFTWARE */
        startAllegroFullScreen(/*Minimum Screen Size */980, 590);
        domain_t *domain = create_domain(data_arcs);
        sw_ctx *ctx = start_software_context(domain);

        /* Splashscreen */
        splashscreen(ctx);
        enable_mouse(ctx);

        /* Run Loop */
        while (ctx->mode != ENDED) {
                /* Get events */
                get_context_events(ctx);

                /* Call view controllers */
                switch (ctx->mode) {
                case STAND_BY: draw_usual_view(ctx);
                                break;

                case PROMPT_PATH_FILTER: draw_usual_view(ctx);
                                grayscale(ctx);
                                blur(ctx);
                                prompt_path_filter(ctx);
                                break;

                case SHOWING_PATH: draw_usual_view(ctx);
                                draw_path(ctx);
                                break;

                case PAUSED: draw_usual_view(ctx);
                                grayscale(ctx);
                                blur(ctx);
                                draw_pause_view(ctx);
                                break;

                case ENDED: ;
                }

                /* Refresh screen */
                swap_buffer(ctx);
        }


        /* TERMINATE SOFTWARE */
        delete_software_context(ctx);
        unload_domain(domain);
        allegro_exit();

        /* Allocation/Deallocation feedback */
        /* show_refcounter();
        show_bitmapcounter(); */

        return 0;
}

END_OF_MAIN();
