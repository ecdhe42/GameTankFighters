#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"
#include "gt/input.h"
#include "gt/audio/music.h"
#include "gt/feature/random/random.h"
#include "gen/assets/gt_fighters.h"
#include "gen/assets/player1.h"
#include "gen/assets/player2.h"
#include "gen/assets/sfx.h"

#define STATE_IDLE  0
#define STATE_MOVE_RIGHT    1
#define STATE_MOVE_LEFT     2
#define STATE_KICK_HIGH     3
#define STATE_KICK_LOW      4
#define STATE_PUNCH_HIGH    5
#define STATE_BLOCK_HIGH    6
#define STATE_BLOCK_LOW     7
#define STATE_HIT           8
#define STATE_KO            9

#define AI_NONE             0
#define AI_ATTACK_PUNCH     1
#define AI_ATTACK_KICK_HIGH 2
#define AI_ATTACK_KICK_LOW  3
#define AI_BLOCK_HIGH       4
#define AI_BLOCK_LOW        5
#define AI_RETREAT          6

#define P2_HUMAN            0
#define P2_CPU1             1
#define P2_CPU2             2

unsigned char p1_gx, p1_gy, p1_x, p1_y, p2_gx, p2_gy, p2_x, p2_y, p3_y, p1_state, p2_state;
unsigned char p1_block_high, p1_block_low, p2_block_high, p2_block_low, p1_blocking, p2_blocking;
unsigned char counter, p1_step, p2_step, p1_max_step, p2_max_step, p1_strike, p2_strike, p1_health, p2_health, p1_win, p2_win;
SpriteSlot p1_sprite, p2_sprite, bg_sprite;
unsigned char *p1_gtx, *p2_gtx, *p1_frame, *p2_frame;
unsigned char tmp, button_down, game_over, animation_over;
unsigned char p2_controls, ai_strategy;
SpriteSlot loading_sprite, elts_sprite, splash_sprite, bg1_sprite, bg2_sprite;
SpriteSlot idle1_sprite, walk1_sprite, kickhigh1_sprite, kicklow1_sprite, punch1_sprite, block1_sprite, hit1_sprite, ko1_sprite;
SpriteSlot idle2_sprite, walk2_sprite, kickhigh2_sprite, kicklow2_sprite, punch2_sprite, block2_sprite, hit2_sprite, ko2_sprite;

unsigned char max_step[10] = {
    5,      // Idle
    5, 5,   // Move
    9, 9,   // Kick
    6,      // Punch
    4, 8,   // Block
    4,      // Hit
    9       // KO
};

unsigned char frame_gx[] = {0, 42, 84,  0, 42, 84,  0, 42, 84};
unsigned char frame_gy[] = {0,  0,  0, 40, 40, 40, 80, 80, 80};

#define SET_P1_STATE(STATE, SPRITE) p1_state = STATE; p1_step = 0; p1_gx = 0; p1_gy = 0; p1_sprite = SPRITE; p1_max_step = max_step[STATE]; p1_block_high = 0; p1_block_low = 0;
#define SET_P2_STATE(STATE, SPRITE) p2_state = STATE; p2_step = 0; p2_gx = 0; p2_gy = 0; p2_sprite = SPRITE; p2_max_step = max_step[STATE]; p2_block_high = 0; p2_block_low = 0;

void breakpoint() {}

void reset_players() {
    counter = 0;
    p1_x = 14;
    p1_y = 60;
    p2_x = 72;
    p2_y = 60;
    p1_health = 58;
    p2_health = 58;
    p1_blocking = 0;
    p2_blocking = 0;
}

void wait_release_buttons() {
    while (1) {
        update_inputs();
        if (!(player1_buttons & INPUT_MASK_ALL_KEYS)) {
            return;
        }
    }
}

void draw_progress(int nb) {
    queue_clear_screen(0);
    queue_draw_sprite(34,58,64,12,0,0,elts_sprite);  // Empty health bars
    queue_draw_sprite(38,65,nb,4,4,14,elts_sprite);
    queue_clear_border(0);
    await_draw_queue();
    await_vsync(1);
    flip_pages();
}

int main () {
    elts_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__sprites_bmp_load_list);
    draw_progress(3);
    splash_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__splash_bmp_load_list);
    draw_progress(6);
    bg1_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__dojo_bmp_load_list);
    draw_progress(9);
    bg2_sprite = allocate_sprite((SpritePage *)&ASSET__gt_fighters__background2_bmp_load_list);
    draw_progress(12);

    idle1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__idle1_bmp_load_list);
    draw_progress(15);
    walk1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__walk1_bmp_load_list);
    draw_progress(18);
    kickhigh1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__kickhigh1_bmp_load_list);
    draw_progress(21);
    kicklow1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__kicklow1_bmp_load_list);
    draw_progress(24);
    punch1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__punch1_bmp_load_list);
    draw_progress(27);
    block1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__block1_bmp_load_list);
    draw_progress(30);
    hit1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__hit1_bmp_load_list);
    draw_progress(33);
    ko1_sprite = allocate_sprite((SpritePage *)&ASSET__player1__ko1_bmp_load_list);
    draw_progress(36);

    idle2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__idle2_bmp_load_list);
    draw_progress(39);
    walk2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__walk2_bmp_load_list);
    draw_progress(42);
    kickhigh2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__kickhigh2_bmp_load_list);
    draw_progress(45);
    kicklow2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__kicklow2_bmp_load_list);
    draw_progress(48);
    punch2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__punch2_bmp_load_list);
    draw_progress(51);
    block2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__block2_bmp_load_list);
    draw_progress(54);
    hit2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__hit2_bmp_load_list);
    draw_progress(56);
    ko2_sprite = allocate_sprite((SpritePage *)&ASSET__player2__ko2_bmp_load_list);
    draw_progress(58);

    init_music();
    p2_controls = P2_HUMAN;

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
    wait_release_buttons();

    // Game options
    button_down = 0;
    animation_over = 0;
    p2_controls = P2_HUMAN;
    p1_y = 49;
    p2_y = 134;
    p3_y = 219;

    while (1) {
        queue_clear_screen(0);
        queue_draw_box(0, 0, 127, 127, 32);
        queue_draw_sprite(2,49,50,30,0,60,elts_sprite);     // Player 1
        queue_draw_sprite(54,54,20,20,50,60,elts_sprite);     // vs
        queue_draw_sprite(42, 84, 44, 33, 0, 90, elts_sprite);  // Instructions

        queue_draw_sprite(77,p1_y,50,30,0,60,elts_sprite);  // Player 2
        queue_draw_sprite(81,p2_y,40,30,70,50,elts_sprite); // CPU 1
        queue_draw_sprite(81,p3_y,40,30,70,80,elts_sprite); // CPU 2

        queue_clear_border(0);

        if (animation_over == 1) {
            p1_y -= 5;
            p2_y -= 5;
            p3_y -= 5;
            counter++;
            if (counter == 17) {
                if (p1_y == 50) {
                    p1_y--;
                    p2_y--;
                    p3_y--;
                }
                animation_over = 0;
            }
        } else if (animation_over == 2) {
            p1_y += 5;
            p2_y += 5;
            p3_y += 5;
            counter++;
            if (counter == 17) {
                if (p1_y == 48) {
                    p1_y++;
                    p2_y++;
                    p3_y++;
                }
                animation_over = 0;
            }
        } else {
            update_inputs();
            if (!(player1_buttons & INPUT_MASK_ANY_DIRECTION)) {
                button_down = 0;
            } else if (!button_down) {
                button_down = 1;
                if (player1_buttons & INPUT_MASK_UP) {
                    counter = 0;
                    animation_over = 1;
                    if (p2_controls == P2_CPU2) {
                        p2_controls = P2_HUMAN;
                    } else {
                        p2_controls++;
                    }
                } else if (player1_buttons & INPUT_MASK_DOWN) {
                    counter = 0;
                    animation_over = 2;
                    if (p2_controls) {
                        p2_controls--;
                    } else {
                        p2_controls = P2_CPU2;
                    }
                }
            }
            if (player1_buttons & INPUT_MASK_A) {
                break;
            }
        }

        await_draw_queue();
        await_vsync(1);
        flip_pages();
    }
    
    // Wait until the player releases any button
    wait_release_buttons();
    
    reset_players();
    p1_win = 0;
    p2_win = 0;
    game_over = 0;
    animation_over = 0;
    ai_strategy = AI_NONE;
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
        queue_draw_sprite(0,15,127,6,0,6,elts_sprite);  // Empty health bars
        if (p1_health) {
            queue_draw_sprite(4,16,p1_health,4,4,14,elts_sprite);  // Player 1 health bar
        }
        if (p2_health) {
            tmp = 70+58-p2_health;
            queue_draw_sprite(tmp,16,p2_health,4,tmp,14,elts_sprite);    // Player 2 health bar
        }
        queue_draw_sprite(p1_x, 60, 42, 40, p1_gx, p1_gy, p1_sprite);   // Player 1
        queue_draw_sprite(p2_x, 60, 42, 40, p2_gx, p2_gy, p2_sprite);   // Player 2

        if (p1_state == STATE_KO) {
            queue_draw_sprite(14,48,  81,17, 0,20,elts_sprite);      // Player
            queue_draw_sprite(100,42, 21,26, 109,20,elts_sprite);    // 2
        } else if (p2_state == STATE_KO) {
            queue_draw_sprite(14,48,  81,17, 0,20,elts_sprite);      // Player
            queue_draw_sprite(100,42, 12,26, 90,20,elts_sprite);    // 1
        }
        if (game_over) {
            queue_draw_sprite(37,70,  53,20, 0,40,elts_sprite);    // Wins
        }
        queue_clear_border(0);

        // Update the player sprites
        if (!animation_over) {
            counter++;
            if (!(counter & 7)) {
                p1_step++;
                if (p1_step == 8) {
                    breakpoint();
                }
                if (p1_state == STATE_KO) {
                    if (p1_step < p1_max_step) {
                        p1_gx = frame_gx[p1_step];
                        p1_gy = frame_gy[p1_step];
                    }
                } else {
                    if (p1_step == p1_max_step) {
                        SET_P1_STATE(STATE_IDLE, idle1_sprite);
                        p1_block_high = 0; p1_block_low = 0;
                        p1_blocking = 0;
                    } else {
                        p1_gx = frame_gx[p1_step];
                        p1_gy = frame_gy[p1_step];
                    }
                }

                p2_step++;
                if (p2_state == STATE_KO) {
                    if (p2_step < p2_max_step) {
                        p2_gx = frame_gx[p2_step];
                        p2_gy = frame_gy[p2_step];
                    }
                } else {
                    if (p2_step == p2_max_step) {
                        SET_P2_STATE(STATE_IDLE, idle2_sprite);
                        p2_blocking = 0;
                    } else {
                        p2_gx = frame_gx[p2_step];
                        p2_gy = frame_gy[p2_step];
                    }
                }
            }
        }

        update_inputs();
        if (player1_buttons & INPUT_MASK_START) {
            break;
        }

        // Player 1 controls
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
                    SET_P1_STATE(STATE_KICK_HIGH, kickhigh1_sprite);
                }
            } else if (player1_buttons & INPUT_MASK_A) {
                SET_P1_STATE(STATE_PUNCH_HIGH, punch1_sprite);
            } else if (player1_buttons & INPUT_MASK_C) {
                if (player1_buttons & INPUT_MASK_UP) {
                    SET_P1_STATE(STATE_BLOCK_HIGH, block1_sprite);
                } else if (player1_buttons & INPUT_MASK_DOWN) {
                    SET_P1_STATE(STATE_BLOCK_LOW, block1_sprite);
                    p1_step = 4;
                    p1_gx = 42;
                    p1_gy = 40;
                }
            }
            break;
        case STATE_MOVE_RIGHT:
            if (!(counter & 7) && ((p1_x & 0x80) || ((p1_x < 70) && (p1_x < p2_x - 10)))) {
                p1_x++;
                p1_x++;
            }
            break;
        case STATE_MOVE_LEFT:
            if (!(counter & 7) && (p1_x > 246 || !(p1_x & 0x80))) {
                p1_x--;
                p1_x--;
            }
            break;
        case STATE_HIT:
            if (!(counter & 7)) {
                p1_x-=2;
            }
            break;
        case STATE_PUNCH_HIGH:
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
                p1_strike = p1_x + 41;
            } else if (p1_step == 4) {
                p1_strike = p1_x + 40;
            }
            break;
        case STATE_BLOCK_HIGH:
            if (p1_step != 0) p1_block_high = 1;
            break;
        case STATE_BLOCK_LOW:
            if (p1_step != 4) p1_block_low = 1;
            break;
        case STATE_KO:
            if (p1_step < 9) {
                if (!(counter & 7)) {
                    p1_x -= 4;
                }
            } else if (p1_step == 32) {
                if (!game_over) {
                    reset_players();
                    SET_P1_STATE(STATE_IDLE, idle1_sprite);
                    SET_P2_STATE(STATE_IDLE, idle2_sprite);            
                } else {
                    animation_over = 1;
                }
            }
            break;
        }

        // Player2 controls
        p2_strike = 0;
        switch(p2_state) {
        case STATE_IDLE:
            if (p2_controls == P2_CPU1) {
                if (p1_state == STATE_KICK_HIGH || p1_state == STATE_PUNCH_HIGH) {
                    SET_P2_STATE(STATE_BLOCK_HIGH, block2_sprite);
                } else if (p1_state == STATE_KICK_LOW) {
                    SET_P2_STATE(STATE_BLOCK_LOW, block2_sprite);
                } else {
                    if (!ai_strategy) {
                        ai_strategy = (rnd() & 3) + 1;
                        if (ai_strategy == 4) {
                            ai_strategy = AI_ATTACK_PUNCH;
                        }
                    }
                    if (ai_strategy == AI_ATTACK_PUNCH) {
                        if (p1_x & 0x80 || (p2_x-p1_x >= 16)) {
                            SET_P2_STATE(STATE_MOVE_LEFT, walk2_sprite);
                        } else {
                            ai_strategy = AI_NONE;
                            SET_P2_STATE(STATE_PUNCH_HIGH, punch2_sprite);
                        }
                    } else if (ai_strategy == AI_ATTACK_KICK_HIGH) {
                        if (p1_x & 0x80 || (p2_x-p1_x >= 20)) {
                            SET_P2_STATE(STATE_MOVE_LEFT, walk2_sprite);
                        } else {
                            ai_strategy = AI_NONE;
                            SET_P2_STATE(STATE_KICK_HIGH, kickhigh2_sprite);
                        }
                    } else if (ai_strategy == AI_ATTACK_KICK_LOW) {
                        if (p1_x & 0x80 || (p2_x-p1_x >= 22)) {
                            SET_P2_STATE(STATE_MOVE_LEFT, walk2_sprite);
                        } else {
                            ai_strategy = AI_NONE;
                            SET_P2_STATE(STATE_KICK_LOW, kicklow2_sprite);
                        }
                    }
                }
            } else if (p2_controls == P2_CPU2) {
                if (!ai_strategy) {
                    ai_strategy = (rnd() & 3) + 1;
                    if (ai_strategy == 4) {
                        ai_strategy = AI_ATTACK_PUNCH;
                    }
                    ai_strategy = AI_ATTACK_KICK_LOW;
                }
                if (ai_strategy == AI_ATTACK_PUNCH) {
                    if (p1_x & 0x80 || (p2_x-p1_x >= 16)) {
                        SET_P2_STATE(STATE_MOVE_LEFT, walk2_sprite);
                    } else {
                        ai_strategy = AI_NONE;
                        SET_P2_STATE(STATE_PUNCH_HIGH, punch2_sprite);
                    }
                } else if (ai_strategy == AI_ATTACK_KICK_HIGH) {
                    if (p1_x & 0x80 || (p2_x-p1_x >= 20)) {
                        SET_P2_STATE(STATE_MOVE_LEFT, walk2_sprite);
                    } else {
                        ai_strategy = AI_NONE;
                        SET_P2_STATE(STATE_KICK_HIGH, kickhigh2_sprite);
                    }
                } else if (ai_strategy == AI_ATTACK_KICK_LOW) {
                    if (p1_x & 0x80 || (p2_x-p1_x >= 23)) {
                        SET_P2_STATE(STATE_MOVE_LEFT, walk2_sprite);
                    } else {
                        ai_strategy = AI_NONE;
                        SET_P2_STATE(STATE_KICK_LOW, kicklow2_sprite);
                    }
                }
            // Human control
            } else {
            
            if (player2_buttons & INPUT_MASK_RIGHT) {
                SET_P2_STATE(STATE_MOVE_RIGHT, walk2_sprite);
            } else if (player2_buttons & INPUT_MASK_LEFT) {
                SET_P2_STATE(STATE_MOVE_LEFT, walk2_sprite);
            }

            if (player2_buttons & INPUT_MASK_B) {
                if (player2_buttons & INPUT_MASK_DOWN) {
                    SET_P2_STATE(STATE_KICK_LOW, kicklow2_sprite);
                } else {
                    SET_P2_STATE(STATE_KICK_HIGH, kickhigh2_sprite);
                }
            } else if (player2_buttons & INPUT_MASK_A) {
                SET_P2_STATE(STATE_PUNCH_HIGH, punch2_sprite);
            } else if (player2_buttons & INPUT_MASK_C) {
                if (player2_buttons & INPUT_MASK_UP) {
                    SET_P2_STATE(STATE_BLOCK_HIGH, block2_sprite);
                } else if (player1_buttons & INPUT_MASK_DOWN) {
                    SET_P2_STATE(STATE_BLOCK_LOW, block2_sprite);
                    p2_step = 4;
                    p2_gx = 42;
                    p2_gy = 40;
                }
            }            
            }
            break;
        case STATE_MOVE_RIGHT:
            if (!(counter & 7) && (p2_x < 96)) {
                p2_x++;
                p2_x++;
            }
            break;
        case STATE_MOVE_LEFT:
            if (!(counter & 7) && (p2_x > 16) && ((p2_x > p1_x + 10) || (p1_x & 0x80))) {
                p2_x--;
                p2_x--;
            }
            break;
        case STATE_HIT:
            if (!(counter & 7)) {
                p2_x+=2;
            }
            break;
        case STATE_PUNCH_HIGH:
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
                p2_strike = p2_x + 1;
            } else if (p2_step == 4) {
                p2_strike = p2_x + 2;
            }
            break;
        case STATE_BLOCK_HIGH:
            if (p2_step != 0) p2_block_high = 1;
            break;
        case STATE_BLOCK_LOW:
            if (p2_step != 4) p2_block_low = 1;
            break;
        case STATE_KO:
            if (p2_step < 9) {
                if (!(counter & 7)) {
                    p2_x += 4;
                }
            } else if (p2_step == 32) {
                if (!game_over) {
                    reset_players();
                    SET_P1_STATE(STATE_IDLE, idle1_sprite);
                    SET_P2_STATE(STATE_IDLE, idle2_sprite);            
                } else {
                    animation_over = 1;
                }
            }
            break;
        }

        // Player 1 hits
        if (p1_strike && p2_state != STATE_HIT && p2_state != STATE_KO) {
            // Player 2 is in range
            if (p1_strike >= p2_x + 20) {
                // Player 2 blocks
                if (((p1_state == STATE_PUNCH_HIGH || p1_state == STATE_KICK_HIGH) && p2_state == STATE_BLOCK_HIGH) ||
                    (p1_state == STATE_KICK_LOW && p2_state == STATE_BLOCK_LOW)) {
                    if (!p2_blocking) {
                        play_sound_effect(ASSET__sfx__blocked_sfx_ID, 0);
                        p2_blocking = 1;
                    }
                // Player 2 is hit
                } else {                    
                    // Knocked out!
                    if (p2_health <= 10) {
                        p2_health = 0;
                        p1_win++;
                        if (p1_win == 2) {
                            game_over = 1;
                        }
                        SET_P2_STATE(STATE_KO, ko2_sprite);
                        play_sound_effect(ASSET__sfx__win_sfx_ID, 0);
                    } else {
                        p2_x++;
                        p2_health -= 10;
                        SET_P2_STATE(STATE_HIT, hit2_sprite);
                        play_sound_effect(ASSET__sfx__hit_sfx_ID, 0);
                    }
                }
            }
        // Player 2 hits
        } else if (p2_strike && p1_state != STATE_HIT && p1_state != STATE_KO) {
            // Player 1 is in range
            if ((p2_strike <= p1_x + 22) && !(p1_x & 0x80)) {
                if (((p2_state == STATE_PUNCH_HIGH || p2_state == STATE_KICK_HIGH) && p1_state == STATE_BLOCK_HIGH) ||
                    (p2_state == STATE_KICK_LOW && p1_state == STATE_BLOCK_LOW)) {
                    if (!p1_blocking) {
                        play_sound_effect(ASSET__sfx__blocked_sfx_ID, 0);
                        p1_blocking = 1;
                    }
                // Player 1 is hit
                } else {
                    // Knocked out!
                    if (p1_health <= 10) {
                        p1_health = 0;
                        p2_win++;
                        if (p2_win == 2) {
                            game_over = 1;
                        }
                        SET_P1_STATE(STATE_KO, ko1_sprite);
                        play_sound_effect(ASSET__sfx__win_sfx_ID, 0);
                    } else {
                        p1_x--;
                        p1_health -= 10;
                        SET_P1_STATE(STATE_HIT, hit1_sprite);
                        play_sound_effect(ASSET__sfx__hit_sfx_ID, 0);
                    }
                }
            }
        }

        await_draw_queue();
        await_vsync(1);
        tick_music();
        flip_pages();
 
    } // Game loop

    // Wait until the player releases any button
    wait_release_buttons();
 
  } // Forever loop
 
  return (0);                                     //  We should never get here!
}