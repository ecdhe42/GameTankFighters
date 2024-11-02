#include "gametank.h"
#include "drawing_funcs.h"
#include "sprites.h"
#include "input.h"
#include "dynawave.h"
#include "music.h"

//Specific to this example game code
#include "gen/assets/audio.h"
#include "./player.h"

int main () {
    
    //Setup the graphics and audio systems
    init_graphics();
    init_dynawave();
    init_music();

    /* See player.c for the player init code */
    init_player();

    /*  ASSET__audio__title_mid is an automatically-generated reference
        to assets/audio/gameloop.mid in the graphics folder, and provides
        both the memory location and ROM bank number to play_song */
    play_song(&ASSET__audio__gameloop_mid, REPEAT_LOOP);

    while (1) {                                     //  Run forever
        clear_screen(3);
        /* See player.c for the player draw code */
        draw_player();
        clear_border(0);

        /* See player.c for the player update code */
        update_player();

        /* Make sure the draw_queue is finished before flipping pages */
        await_draw_queue();

        /* sleep(1) will wait for the next start of frame
        instead of a whole frame, i.e. waiting for vsync*/
        sleep(1);
        /* there are two frame buffers. typically we draw on one while
         displaying the other, and then switch them during vsync */
        flip_pages();
        update_inputs();
        tick_music();
    }

  return (0);
}