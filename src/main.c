#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"
#include "gt/input.h"
#include "gen/assets/gt_fighters.h"
#include "gen/assets/player1.h"
#include "gen/assets/player2.h"

#define STATE_IDLE  0
#define STATE_MOVE_RIGHT    1
#define STATE_MOVE_LEFT     2
#define STATE_KICK          3
#define STATE_PUNCH         4
#define STATE_DIE           5

unsigned char p1_gx, p1_gy, p1_x, p1_y, p2_gx, p2_gy, p2_x, p2_y, p1_state, p2_state;
unsigned char counter, p1_step, p2_step, p1_max_step, p2_max_step;
SpriteSlot p1_sprite, p2_sprite;
unsigned char *p1_gtx, *p2_gtx, *p1_frame, *p2_frame;

unsigned char max_step[] = {5, 5, 5, 9, 6, 9};

unsigned char frame_gx[] = {0, 42, 84, 0, 42, 84, 0, 84, 42 };
unsigned char frame_gy[] = {0, 0, 0, 40, 40, 40, 80, 80, 80};

#define SET_P1_STATE(STATE, SPRITE) p1_state = STATE; p1_step = 0; p1_gx = 0; p1_gy = 0; p1_sprite = SPRITE; p1_max_step = max_step[STATE];
#define SET_P2_STATE(STATE, SPRITE) p2_state = STATE; p2_step = 0; p2_gx = 0; p2_gy = 0; p2_sprite = SPRITE; p2_max_step = max_step[STATE];

void breakpoint() {}

int main () {
 
    SpriteSlot dojo_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__dojo_bmp_load_list);

    SpriteSlot idle1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__idle1_bmp_load_list);
    SpriteSlot walk1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__walk1_bmp_load_list);
    SpriteSlot kick1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__kick1_bmp_load_list);
    SpriteSlot kicklow1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__kicklow1_bmp_load_list);
    SpriteSlot punch1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__punch1_bmp_load_list);
    SpriteSlot block1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__block1_bmp_load_list);
    SpriteSlot die1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__die1_bmp_load_list);

    SpriteSlot idle2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__idle2_bmp_load_list);
    SpriteSlot walk2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__walk2_bmp_load_list);
    SpriteSlot kick2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__kick2_bmp_load_list);
    SpriteSlot kicklow2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__kicklow2_bmp_load_list);
    SpriteSlot punch2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__punch2_bmp_load_list);
    SpriteSlot block2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__block2_bmp_load_list);
    SpriteSlot die2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__die2_bmp_load_list);

    counter = 0;
    p1_x = 50;
    p1_y = 60;
    p2_x = 80;
    p2_y = 60;
    SET_P1_STATE(STATE_IDLE, idle1_sprite);
    SET_P2_STATE(STATE_IDLE, idle2_sprite);

    while (1) {                                     //  Run forever
//        queue_clear_screen(3);
        queue_draw_sprite(0,0,127,127,0,0,dojo_sprite);
        queue_draw_sprite(p1_x, 60, 42, 40, p1_gx, p1_gy, p1_sprite);
        queue_draw_sprite(p2_x, 60, 42, 40, p2_gx, p2_gy, p2_sprite);
        queue_clear_border(0);

        counter++;
        if (!(counter & 7)) {
            p1_step++;
            if (p1_state == STATE_DIE) {
                if (p1_step < p1_max_step) {
                    p1_gx = frame_gx[p1_step];
                    p1_gy = frame_gy[p1_step];
                }
            } else {
                if (p1_step == p1_max_step) {
                    SET_P1_STATE(STATE_IDLE, idle1_sprite);
                } else {
                    p1_gx = frame_gx[p1_step];
                    p1_gy = frame_gy[p1_step];
                }
            }

            p2_step++;
            if (p2_state == STATE_DIE) {
                if (p2_step < p2_max_step) {
                    p2_gx = frame_gx[p2_step];
                    p2_gy = frame_gy[p2_step];
                }
            } else {
                if (p2_step == p2_max_step) {
                    SET_P2_STATE(STATE_IDLE, idle2_sprite);
                } else {
                    p2_gx = frame_gx[p2_step];
                    p2_gy = frame_gy[p2_step];
                }
            }
        }

        update_inputs();

        switch(p1_state) {
        case STATE_IDLE:
            if (player1_buttons & INPUT_MASK_RIGHT) {
                SET_P1_STATE(STATE_MOVE_RIGHT, walk1_sprite);
            } else if (player1_buttons & INPUT_MASK_LEFT) {
                SET_P1_STATE(STATE_MOVE_LEFT, walk1_sprite);
            }

            if (player1_buttons & INPUT_MASK_A) {
                if (player1_buttons & INPUT_MASK_DOWN) {
                    SET_P1_STATE(STATE_KICK, kicklow1_sprite);
                } else {
                    SET_P1_STATE(STATE_KICK, kick1_sprite);
                }
            } else if (player1_buttons & INPUT_MASK_B) {
                SET_P1_STATE(STATE_PUNCH, punch1_sprite);
            }
            break;
        case STATE_MOVE_RIGHT:
            if (!(counter & 7)) {
                p1_x++;
            }
            break;
        case STATE_MOVE_LEFT:
            if (!(counter & 7)) {
                p1_x--;
            }
            break;
        case STATE_DIE:
            if (p1_step < 9) {
                if (!(counter & 7)) {
                    p1_x -= 4;
                }
            } else if (p1_step == 32) {
                SET_P1_STATE(STATE_IDLE, idle1_sprite);
            }
            break;
        }

        switch(p2_state) {
        case STATE_IDLE:
            if (player2_buttons & INPUT_MASK_RIGHT) {
                SET_P2_STATE(STATE_MOVE_RIGHT, walk2_sprite);
            } else if (player2_buttons & INPUT_MASK_LEFT) {
                SET_P2_STATE(STATE_MOVE_LEFT, walk2_sprite);
            }

            if (player2_buttons & INPUT_MASK_A) {
                if (player2_buttons & INPUT_MASK_DOWN) {
                    SET_P2_STATE(STATE_KICK, kicklow2_sprite);
                } else {
                    SET_P2_STATE(STATE_KICK, kick2_sprite);
                }
            } else if (player2_buttons & INPUT_MASK_B) {
                SET_P2_STATE(STATE_PUNCH, punch2_sprite);
            }
            break;
        case STATE_MOVE_RIGHT:
            if (!(counter & 7)) {
                p2_x++;
            }
            break;
        case STATE_MOVE_LEFT:
            if (!(counter & 7)) {
                p2_x--;
            }
            break;
        case STATE_DIE:
            if (p2_step < 9) {
                if (!(counter & 7)) {
                    p2_x += 4;
                }
            } else if (p2_step == 32) {
                SET_P1_STATE(STATE_IDLE, idle2_sprite);
            }
            break;
        }


        if (p1_x == 62) {
            SET_P1_STATE(STATE_DIE, die1_sprite);
        }

        await_draw_queue();
        await_vsync(1);
        flip_pages();
 
    }
 
  return (0);                                     //  We should never get here!
}