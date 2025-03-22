#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"
#include "gen/assets/gt_fighters.h"

unsigned char p1_gx, p1_gy, p1_x, p1_y, p2_gx, p2_gy, p2_x, p2_y;
unsigned char counter, p1_step, p2_step;

unsigned char p1_kick_gx[] = {0, 42, 84, 0, 42, 84, 0};
unsigned char p1_kick_gy[] = {0, 0, 0, 40, 40, 40, 80};

int main () {
 
    SpriteSlot dojo_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__dojo_bmp_load_list);
    SpriteSlot kick1_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__kick1_bmp_load_list);
    SpriteSlot walk1_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__walk1_bmp_load_list);
    SpriteSlot kick2_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__kick2_bmp_load_list);
    SpriteSlot punch2_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__punch2_bmp_load_list);

    counter = 0;
    p1_step = 0;
    p2_step = 0;
    p1_gx = p1_kick_gx[p1_step];
    p1_gy = p1_kick_gy[p1_step];
    p1_x = 20;
    p1_y = 60;
    p2_gx = p1_kick_gx[0];
    p2_gy = p1_kick_gy[0];
    p2_x = 80;
    p2_y = 60;

    while (1) {                                     //  Run forever
//        queue_clear_screen(3);
        queue_draw_sprite(0,0,127,127,0,0,dojo_sprite);
        queue_draw_sprite(p1_x, p1_y, 42, 40, p1_gx,p1_gy, walk1_sprite);
        queue_draw_sprite(p2_x, p2_y, 42, 40, p2_gx,p2_gy, punch2_sprite);
        queue_clear_border(0);

        counter++;
        if (!(counter & 7)) {
            p1_step++;
            if (p1_step == 5) {
                p1_step = 0;
            } else if (p1_step == 4 || p1_step == 3) {
                p1_x++;
            }
            p1_gx = p1_kick_gx[p1_step];
            p1_gy = p1_kick_gy[p1_step];

            p2_step++;
            if (p2_step == 6) {
                p2_step = 0;
            }
            p2_gx = p1_kick_gx[p2_step];
            p2_gy = p1_kick_gy[p2_step];
        }

        await_draw_queue();
        await_vsync(1);
        flip_pages();
 
    }
 
  return (0);                                     //  We should never get here!
}