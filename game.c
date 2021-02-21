#include <gb/gb.h> // include this file for Game Boy functions
#include <gb/console.h>
#include <gb/sgb.h>
#include <rand.h>
#include <stdio.h>
// my sprites :)
#include "big_gun.c"
#include "copter.c"
#include "bullet.c"
#include "dude.c"
#include "dude_landed.c"

// const unsigned char HUD2[] = " %d %c : %d %d ";

#define CLEAR_PROP  0x0U

#define GX 80 // gun y
#define GY 140 // gun x

fixed seed;

// struct Copter {
//   UINT8 height; // what the copter's copter's height is
//   UINT8 direction; // -1 or 1 depending on left / right
//   UINT8 sprite; // sprite # of copter
// };

// struct Copter new_copter;

// struct Copters {
//   struct Copter copters[3];
//   UINT8 active;
// };

// struct copters Copters;

UBYTE new_height, new_direction, copters_active;
void main()
{

  SPRITES_8x16;

  set_sprite_data(0, 64, big_gun);
  set_sprite_tile(0, 0);
  
  move_sprite(0, GX, GY); // Move sprite to our predefined x and y coords
  set_sprite_tile(1, 2);
  move_sprite(1, GX + 8, GY);
  UINT8 pos = 0;
  unsigned char switcheroo = 'r';
  UINT8 new_pos = 0;


  UINT8 copters = 0;
  UINT8 copX = 0;
  set_sprite_data(64, 4, copter);
  set_sprite_tile(2, 64); // copter 1
  set_sprite_tile(3, 66);

  set_sprite_tile(4, 64); // copter 2
  set_sprite_tile(5, 66);

  set_sprite_tile(6, 64); // copter 3
  set_sprite_tile(7, 66);


UINT8 dudeX = 0;
UINT8 dudeY = 0;
UINT8 dudes = 0;
  set_sprite_data(68, 2, dude);
  set_sprite_tile(8, 68);  //  1
  set_sprite_tile(9, 68);  //  2
  set_sprite_tile(10, 68); //  3
  set_sprite_tile(11, 68); //  4
  set_sprite_tile(12, 68); //  5
  set_sprite_tile(13, 68); //  6
  set_sprite_tile(14, 68); //  7
  set_sprite_tile(15, 68); //  8
  set_sprite_tile(16, 68); //  9
  set_sprite_tile(17, 68); // 10
  set_sprite_tile(18, 68); // 11
  set_sprite_tile(19, 68); // 12
  
  // we're also loading data for dudes when then land
  set_sprite_data(70, 2, dude_landed);

  // UINT8 bul_x = 0; 
  // UINT8 bul_y = 0;
  // // set_sprite_data(2, 1, bullet);
  // // set_sprite_tile(2, 60);
  // // move_sprite(2, bul_x, bul_y);
  // unsigned char active_shooter = 'f';



  // generate random data
  printf("Press any button!");
  waitpad(0xFF);
  waitpadup();
  seed.b.l = DIV_REG;
  gotoxy(0,0); printf("Now loading SABOTAGE");
  delay(1000);
  printf(".");
  delay(1000);
  printf(".");
  delay(1000);
  printf(".\n");
  delay(1000);
  printf("Press any button!");
  waitpad(0xFF);
  waitpadup();
  seed.b.h = DIV_REG;
  initarand(seed.w);
  // clear screen
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  

  SHOW_SPRITES;



  while (1)
  {
    if (!copters) { // make a new copter
      new_height = rand();
      if (new_height > 15 && new_height < 48) { // rng has blessed us with a chopper
        copters = 1;
        // move_sprite(2, 80, new_height);
        // move_sprite(3, 88, new_height);
      }
    } else {
      // animate the copter
      copX = copX + 1;
      move_sprite(2, copX, new_height);
      move_sprite(3, copX+8, new_height);
      // animate the dude
      // needs some randomness, however.
      if (!dudes) {
        dudeX = rand();
        if (dudeX <= copX && dudeX > copX - 15 && dudeX < 150 && dudeX > 10) {
          dudeY = new_height;
          dudes = 1;
        }
      } else {
        // animate the dude falling
        // add some randomness into their falling?
        if (rand() % 2){
          dudeY = dudeY + 1;
          move_sprite(8, dudeX, dudeY);
        }
      }
    }
    gotoxy(0,0); printf(" %d ", dudes);
    // gotoxy(0,0); printf(" %d : %d ",  cop_2_x, new_height);

    // REFERNCE
    // pos / 4 = ...
    // 0 - 14 normal
    // 14 - 28 // flip y
    // gotoxy(0, 0); printf(HUD2, pos / 4, switcheroo, bul_x, bul_y);
    // if (active_shooter == 't')
    // {
    //   bul_y--;
    //   move_sprite(2, bul_x, bul_y);
    // }
    // // // shoot bullet!
    // if (joypad() == J_A) {
    //   active_shooter = 't';
    //   bul_x = 80;
    //   bul_y = 120;
    //   move_sprite(2, bul_x, bul_y);
    // }
    // right half
    if (pos / 4 < 14)
    {
      // only need to do this if and only if we're returning from the left side
      if (switcheroo == 'l') 
      {
        move_sprite(0, GX, GY);
        set_sprite_prop(0, CLEAR_PROP);
        move_sprite(1, GX+8, GY);
        set_sprite_prop(1, CLEAR_PROP);
        switcheroo = 'r';
      }

      if (joypad() == J_RIGHT)
      {
        // make sure pos - 4 doesn't break something. 
        if ((pos - 4) / 4 < 0) {
          delay(75);
          continue;
        }
        pos = pos - 4;
        set_sprite_tile(0, pos);
        set_sprite_tile(1, pos + 2);
        // delay(75);
      }

      else if (joypad() == J_LEFT)
      {
        pos = pos + 4;
        set_sprite_tile(0, (pos));
        set_sprite_tile(1, pos + 2);
        // delay(75);
      }
    }
    // special case for 14 & 15 ( the middle )
    else if (pos / 4 == 14 || pos / 4 == 15) 
    {
      // move_sprite(0, GX, GY);
      // set_sprite_prop(0, CLEAR_PROP);
      // move_sprite(1, GX+8, GY);
      // set_sprite_prop(1, CLEAR_PROP);
      // still need to update pos, but we always will use the middle, neutral position
      set_sprite_tile(0, 56);
      set_sprite_tile(1, 58);
      if(switcheroo == 'r') 
      {
        
        if (joypad() == J_LEFT)
        {
          pos = pos + 8;
        }
        else if (joypad() == J_RIGHT)
        {
          pos = pos - 4;
        }
      }
       else if(switcheroo == 'l')
      {
        if (joypad() == J_LEFT)
        {
          pos = pos + 4;
        }
        else if (joypad() == J_RIGHT)
        {
          pos = pos - 8;
        }
      }
      // delay(75);
    } 
    // left half
    else if (pos / 4 > 15 && pos / 4 <= 28) {
      if (switcheroo == 'r') {
        move_sprite(0, GX-8, GY);
        set_sprite_prop(0, S_FLIPX);
        move_sprite(1, GX-16, GY);
        set_sprite_prop(1, S_FLIPX);
        switcheroo = 'l';
      }
      if (joypad() == J_LEFT)
      {
       if ((pos + 4) / 4 > 28) {
         delay(75);
          continue;
        }
        pos = pos + 4;
        new_pos = -1 * pos + (2 * 56);
        set_sprite_tile(0, new_pos);
        set_sprite_tile(1, new_pos + 2);
        // delay(75);
      }
      else if (joypad() == J_RIGHT)
      {
        pos = pos - 4;
        new_pos = -1 * pos + (2 * 56);
        set_sprite_tile(0, new_pos);
        set_sprite_tile(1, new_pos + 2);
        // delay(75);
      }
    }
    delay(50);
    wait_vbl_done();
  }
}