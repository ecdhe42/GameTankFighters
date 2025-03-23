#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"
#include "gt/input.h"
#include "gt/feature/random/random.h"
#include "gen/assets/gt_fighters.h"
#include "gen/assets/player1.h"
#include "gen/assets/player2.h"

#define STATE_IDLE  0
#define STATE_MOVE_RIGHT    1
#define STATE_MOVE_LEFT     2
#define STATE_KICK_HIGH     3
#define STATE_KICK_LOW      4
#define STATE_PUNCH         5
#define STATE_HIT           6
#define STATE_DIE           7

unsigned char p1_gx, p1_gy, p1_x, p1_y, p2_gx, p2_gy, p2_x, p2_y, p1_state, p2_state;
unsigned char counter, p1_step, p2_step, p1_max_step, p2_max_step, p1_strike, p2_strike, p1_health, p2_health;
SpriteSlot p1_sprite, p2_sprite, bg_sprite;
unsigned char *p1_gtx, *p2_gtx, *p1_frame, *p2_frame;
unsigned char tmp;

unsigned char max_step[] = {5, 5, 5, 9, 9, 6, 3, 9};

unsigned char frame_gx[] = {0, 42, 84, 0, 42, 84, 0, 84, 42 };
unsigned char frame_gy[] = {0, 0, 0, 40, 40, 40, 80, 80, 80};

#define SET_P1_STATE(STATE, SPRITE) p1_state = STATE; p1_step = 0; p1_gx = 0; p1_gy = 0; p1_sprite = SPRITE; p1_max_step = max_step[STATE];
#define SET_P2_STATE(STATE, SPRITE) p2_state = STATE; p2_step = 0; p2_gx = 0; p2_gy = 0; p2_sprite = SPRITE; p2_max_step = max_step[STATE];

void breakpoint() {}

void reset_players() {
    counter = 0;
    p1_x = 50;
    p1_y = 60;
    p2_x = 80;
    p2_y = 60;
    p1_health = 58;
    p2_health = 58;
}

int main () {
 
    SpriteSlot splash_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__splash_bmp_load_list);
    SpriteSlot bg1_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__dojo_bmp_load_list);
    SpriteSlot bg2_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__background2_bmp_load_list);
    SpriteSlot elts_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__sprites_bmp_load_list);

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

    // Forever loop
    while (1) {

    // Splash screen
    while (1) {
        rnd();
        queue_clear_screen(0);
        queue_draw_sprite(0,0,127,127,0,0,splash_sprite);
        queue_clear_border(0);

        update_inputs();
        if (player1_buttons & INPUT_MASK_ALL_KEYS) {
            break;
        }
        await_draw_queue();
        await_vsync(1);
        flip_pages();
    }

    // Wait until the player releases any button
    while (1) {
        update_inputs();
        if (!(player1_buttons & INPUT_MASK_ALL_KEYS)) {
            break;
        }
    }

    reset_players();
    SET_P1_STATE(STATE_IDLE, idle1_sprite);
    SET_P2_STATE(STATE_IDLE, idle2_sprite);

    if (rnd() & 1) {
        bg_sprite = bg1_sprite;
    } else {
        bg_sprite = bg2_sprite;
    }

    // Game loop
    while (1) {
//        queue_clear_screen(3);
        queue_draw_sprite(0,0,127,127,0,0,bg_sprite); // Background
        queue_draw_sprite(0,15,127,6,0,5,elts_sprite);  // Empty health bars
        if (p1_health) {
            queue_draw_sprite(4,16,p1_health,4,4,13,elts_sprite);  // Player 1 health bar
        }
        if (p2_health) {
            tmp = 70+58-p2_health;
            queue_draw_sprite(tmp,16,p2_health,4,tmp,13,elts_sprite);    // Player 2 health bar
        }
        queue_draw_sprite(p1_x, 60, 42, 40, p1_gx, p1_gy, p1_sprite);   // Player 1
        queue_draw_sprite(p2_x, 60, 42, 40, p2_gx, p2_gy, p2_sprite);   // Player 2

        if (p1_state == STATE_DIE) {
            queue_draw_sprite(14,48,  81,17, 0,20,elts_sprite);      // Player
            queue_draw_sprite(100,42, 21,46, 109,20,elts_sprite);    // 2
            queue_draw_sprite(37,70,  53,20, 0,40,elts_sprite);    // Wins
        } else if (p2_state == STATE_DIE) {
            queue_draw_sprite(14,48,  81,17, 0,20,elts_sprite);      // Player
            queue_draw_sprite(100,42, 12,46, 90,20,elts_sprite);    // 1
            queue_draw_sprite(37,70,  53,20, 0,40,elts_sprite);    // Wins
        }
        queue_clear_border(0);

        // Update the player sprites
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
        if (player1_buttons & INPUT_MASK_START) {
            break;
        }

        p1_strike = 0;
        switch(p1_state) {
        case STATE_IDLE:
            if (player1_buttons & INPUT_MASK_RIGHT) {
                SET_P1_STATE(STATE_MOVE_RIGHT, walk1_sprite);
            } else if (player1_buttons & INPUT_MASK_LEFT) {
                SET_P1_STATE(STATE_MOVE_LEFT, walk1_sprite);
            }

            if (player1_buttons & INPUT_MASK_B) {
                if (player1_buttons & INPUT_MASK_DOWN) {
                    SET_P1_STATE(STATE_KICK_LOW, kicklow1_sprite);
                } else {
                    SET_P1_STATE(STATE_KICK_HIGH, kick1_sprite);
                }
            } else if (player1_buttons & INPUT_MASK_A) {
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
        case STATE_HIT:
            if (!(counter & 7)) {
                p1_x-=2;
            }
            break;
        case STATE_PUNCH:
            if (p1_step == 5) {
                p1_strike = p1_x + 35;
            }
            break;
        case STATE_KICK_HIGH:
            if (p1_step == 3) {
                p1_strike = p1_x + 36;
            } else if (p1_step == 4) {
                p1_strike = p1_x + 39;
            }
            break;
        case STATE_KICK_LOW:
            if (p1_step == 3) {
                p1_strike = p1_x + 36;
            } else if (p1_step == 4) {
                p1_strike = p1_x + 39;
            }
            break;
        case STATE_DIE:
            if (p1_step < 9) {
                if (!(counter & 7)) {
                    p1_x -= 4;
                }
            } else if (p1_step == 32) {
                reset_players();
                SET_P1_STATE(STATE_IDLE, idle1_sprite);
                SET_P2_STATE(STATE_IDLE, idle2_sprite);            
            }
            break;
        }

        p2_strike = 0;
        switch(p2_state) {
        case STATE_IDLE:
            if (player2_buttons & INPUT_MASK_RIGHT) {
                SET_P2_STATE(STATE_MOVE_RIGHT, walk2_sprite);
            } else if (player2_buttons & INPUT_MASK_LEFT) {
                SET_P2_STATE(STATE_MOVE_LEFT, walk2_sprite);
            }

            if (player2_buttons & INPUT_MASK_B) {
                if (player2_buttons & INPUT_MASK_DOWN) {
                    SET_P2_STATE(STATE_KICK_LOW, kicklow2_sprite);
                } else {
                    SET_P2_STATE(STATE_KICK_HIGH, kick2_sprite);
                }
            } else if (player2_buttons & INPUT_MASK_A) {
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
        case STATE_HIT:
            if (!(counter & 7)) {
                p2_x+=2;
            }
            break;
        case STATE_PUNCH:
            if (p2_step == 5) {
                p2_strike = p2_x + 7;
            }
            break;
        case STATE_KICK_HIGH:
            if (p2_step == 3) {
                p2_strike = p2_x + 6;
            } else if (p2_step == 4) {
                p2_strike = p2_x + 3;
            }
            break;
        case STATE_KICK_LOW:
            if (p2_step == 3) {
                p2_strike = p2_x + 6;
            } else if (p2_step == 4) {
                p2_strike = p2_x + 3;
            }
            break;
        case STATE_DIE:
            if (p2_step < 9) {
                if (!(counter & 7)) {
                    p2_x += 4;
                }
            } else if (p2_step == 32) {
                reset_players();
                SET_P1_STATE(STATE_IDLE, idle1_sprite);
                SET_P2_STATE(STATE_IDLE, idle2_sprite);            
            }
            break;
        }

        if (p1_strike && p2_state != STATE_HIT) {
            if (p1_strike >= p2_x + 20) {
                if (p2_health <= 10) {
                    p2_health = 0;
                    SET_P2_STATE(STATE_DIE, die2_sprite);
                } else {
                    p2_x++;
                    p2_health -= 10;
                    SET_P2_STATE(STATE_HIT, walk2_sprite);
                }
            }
        }

        if (p2_strike && p1_state != STATE_HIT) {
            if (p2_strike <= p1_x + 22) {
                if (p1_health <= 10) {
                    p1_health = 0;
                    SET_P1_STATE(STATE_DIE, die1_sprite);
                } else {
                    p1_x--;
                    p1_health -= 10;
                    SET_P1_STATE(STATE_HIT, walk1_sprite);
                }
            }
        }

        await_draw_queue();
        await_vsync(1);
        flip_pages();
 
    } // Game loop

  } // Forever loop
 
  return (0);                                     //  We should never get here!
}