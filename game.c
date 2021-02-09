#include <gb/gb.h> // include this file for Game Boy functions
#include <gb/console.h>
#include <gb/sgb.h>
#include <stdio.h>
#include "big_gun.c"

const unsigned char HUD[] = " %d : %d  ? %d ";
const unsigned char HUD2[] = " %d ";

void main()
{

  UINT8 x = 80; // Our beginning x coord
  UINT8 y = 140; // Our beginning y coord

  SPRITES_8x16;
#define GUN_POS 0
  set_sprite_data(0, 64, big_gun);
  set_sprite_tile(0, 0);
  // set_sprite_tile(0, 0);
  move_sprite(0, x, y); // Move sprite to our predefined x and y coords
  set_sprite_tile(1, 2);
  move_sprite(1, x + 8, y);
  UINT8 pos = 0;
  SHOW_SPRITES;
  while (1)
  {
    // REFERNCE
    // pos / 4 = ...
    // 0 - 7 == no transformations
    // 8 - 15 == rotate 90 clockwise && mirror y
    // 16 - 23 == rotate 90 clockwise1
    // 24 - 31 == mirror y
    // if (check_out_of_bounds(x,y)) {
    //   continue;
    //
    gotoxy(0, 0); printf(HUD2, pos);
    if (pos <= 28) // 0 - 7 
    {
      if (joypad() == J_RIGHT)
      {
        pos = pos - 4;
        set_sprite_tile(0, pos % 32);
        set_sprite_tile(1, (pos % 32) + 2);
        delay(75);
      }

      if (joypad() == J_LEFT)
      {
        pos = pos + 4;
        set_sprite_tile(0, (pos % 32));
        set_sprite_tile(1, (pos % 32) + 2);
        delay(75);
      }
    }
    // need to modify controls for every other case
    else if (pos >= 32 && pos <= 60) { // 8 - 15
      if (joypad() == J_RIGHT)
      {
        pos = pos - 4;
        set_sprite_tile(0, pos % 32);
        set_sprite_tile(1, (pos % 32) + 2);
        delay(75);
      }
      if (joypad() == J_LEFT)
      {
        pos = pos + 4;
        set_sprite_tile(0, (pos % 32));
        set_sprite_tile(1, (pos % 32) + 2);
        delay(75);
      }
    } 
    else if (pos >= 64 && pos <= 92) // 16 - 23
    {
            if (joypad() == J_RIGHT)
      {
        pos = pos - 4;
        set_sprite_tile(0, pos % 32);
        set_sprite_tile(1, (pos % 32) + 2);
        delay(75);
      }
      if (joypad() == J_LEFT)
      {
        pos = pos + 4;
        set_sprite_tile(0, (pos % 32));
        set_sprite_tile(1, (pos % 32) + 2);
        delay(75);
      }
    }
    else if (pos >= 96) {
            if (joypad() == J_RIGHT)
      {
        pos = pos - 4;
        set_sprite_tile(0, pos % 32);
        set_sprite_tile(1, (pos % 32) + 2);
        delay(75);
      }
      if (joypad() == J_LEFT)
      {
        pos = pos + 4;
        set_sprite_tile(0, (pos % 32));
        set_sprite_tile(1, (pos % 32) + 2);
        delay(75);
      }
    }
    // if (joypad() == J_START)
    // {
    //   move_sprite(0, x - 8, y);
    //   set_sprite_prop(0, S_FLIPX);
    //   move_sprite(1, x - 16, y);
    //   set_sprite_prop(1, S_FLIPX);
    //   delay(75);
    // }
    wait_vbl_done();
  }
}