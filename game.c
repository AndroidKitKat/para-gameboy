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
const unsigned char HUD[] = " Score: %d Lives: %d ";

#define CLEAR_PROP  0x0U

#define GX 80 // gun y
#define GY 140 // gun x

fixed seed;
struct Copter {
  UINT8 height;
  UINT8 direction;
  UINT8 sprite;
  UINT8 x;
  UINT8 state; // 0 = dead/not spawned
               // 1 = alive
} copters[3] = {
  {0, 0, 2, 0, 0},
  {0, 0, 4, 0, 0},
  {0, 0, 6, 0, 0}
};



struct Dude {
  UINT8 x;
  UINT8 y;
  UINT8 sprite;
  UINT8 state; // 0 = dead/not spawned
               // 1 = alive
               // 2 = on ground
} dudes[12] = {
  {0, 0,  8, 0},
  {0, 0,  9, 0},
  {0, 0, 10, 0},
  {0, 0, 11, 0},
  {0, 0, 12, 0},
  {0, 0, 13, 0},
  {0, 0, 14, 0},
  {0, 0, 15, 0},
  {0, 0, 16, 0},
  {0, 0, 17, 0},
  {0, 0, 18, 0},
  {0, 0, 19, 0}
};


// enemy vars
UINT8 c_alive = 0; // copters alive
UINT8 d_alive = 0; // dudes alive


// player vars
UINT8 pos = 0; // gun position
UINT8 new_pos = 0; // used to save calcuations for moving the gun

// misc vars
UINT8 i = 0;
UINT8 j = 0;
UINT8 temp_height = 0;
unsigned char switcheroo = 'r'; // used for logic in switching from the left to right
UINT8 cooldown = 250; // cooldown timer for between chopper spawns
UINT8 cooldown2 = 250;
UBYTE lives = 5;

void main()
{

  SPRITES_8x16;

  set_sprite_data(0, 64, big_gun);
  set_sprite_tile(0, 0);
  
  move_sprite(0, GX, GY); // Move sprite to our predefined x and y coords
  set_sprite_tile(1, 2);
  move_sprite(1, GX + 8, GY);

  set_sprite_data(64, 4, copter);
  set_sprite_tile(2, 64); // copter 1
  set_sprite_tile(3, 66);

  set_sprite_tile(4, 64); // copter 2
  set_sprite_tile(5, 66);

  set_sprite_tile(6, 64); // copter 3
  set_sprite_tile(7, 66);


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



  // generate random data
  printf("Press any button!");
  waitpad(0xFF);
  waitpadup();
  seed.b.l = DIV_REG;
  gotoxy(0,0); printf("Now loading SABOTAGE");
  // delay(1000);
  // printf(".");
  // delay(1000);
  // printf(".");
  // delay(1000);
  // printf(".\n");
  // delay(1000);
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
    gotoxy(0, 0); printf(HUD, 0, lives);
    // if(lives <= 0)
    // {
    //   // end the game
    //   // figure out how to do this. 
    // }
    // gotoxy(0,0); printf(" %d ", cooldown);
    if (c_alive < 3 && cooldown == 0)
    {
      cooldown = 250;
      temp_height = rand();

      // only need to enter this if we roll a valid number
      if (temp_height > 15 && temp_height < 48)
      {
        // choose a copter that is dead or not spawned (usually the first one) // DO NOT FORGET TO RESET i WHEN YOU ARE DONE
        while (copters[i].state != 0) {
          i = i + 1;
        } 

        copters[i].height = temp_height;
        if (rand() % 2 == 0)
        {
          copters[i].direction = 1;
          copters[i].x = 0;
          move_sprite(copters[i].sprite, 0, copters[i].height);
          move_sprite(copters[i].sprite + 1, copters[i].x + 8, copters[i].height);
        }
        else
        {
          copters[i].direction = 2;
          // mirror the sprite
          set_sprite_prop(copters[i].sprite, S_FLIPX);
          set_sprite_prop(copters[i].sprite + 1, S_FLIPX);
          
          copters[i].x = 150;
          move_sprite(copters[i].sprite, copters[i].x, copters[i].height);
          move_sprite(copters[i].sprite + 1, copters[i].x - 8, copters[i].height);
        }
        // if we get here, we need to mark the copter as alive and increment the number of choppers
        copters[i].state = 1;
        c_alive = c_alive + 1;
      }
      i = 0;
    }

    if (d_alive < 12)
    {

    }
    while(i < 3)
    {
      // gotoxy(0,0); printf(" %d ", i);
      if (copters[i].state == 1)
      {
        if (copters[i].direction % 2) {
          copters[i].x += 1;
          move_sprite(copters[i].sprite, copters[i].x, copters[i].height);
          move_sprite(copters[i].sprite + 1, copters[i].x + 8, copters[i].height);
        } else {
          copters[i].x -= 1;
          move_sprite(copters[i].sprite, copters[i].x, copters[i].height);
          move_sprite(copters[i].sprite + 1, copters[i].x - 8, copters[i].height);
        }
        // now do the dude
        while(j < 12) // again this is ass 
        {
          // gotoxy(0,0); printf(" %d ", d_alive);
          if (dudes[j].state == 0 && cooldown2 < 30)
          {
            temp_height = rand(); // please ignore this var being called temp_height, it is actually a temp X coord but i cba.
            // ass but might work
            if (temp_height > 0 && temp_height < 150)
            {
              if (copters[i].direction % 2) // this is for choppers that are moving left to right
              {
                if (temp_height > copters[i].x - 5 && temp_height <= copters[i].x)
                {
                  dudes[j].x = temp_height;
                  dudes[j].y = copters[i].height;
                  dudes[j].state = 1;
                  d_alive += 1;
                  break;
                }
              }
            }
          }
          else
          {
            if (dudes[j].state == 1) // dudes that are falling
            {
              if (rand() % 2)
              {
                if (dudes[j].y + 1 == 140)
                {
                  // player loses a life
                  lives -= 1;
                  set_sprite_tile(dudes[j].sprite, 70);
                  dudes[j].state = 2;
                }
                dudes[j].y += 1;
                move_sprite(dudes[j].sprite, dudes[j].x, dudes[j].y);
              }
            }
          }
          j += 1;
        }
        j = 0;
      }
      i = i + 1;
    }
    i = 0;
    //   // animate the copter
    //   copX = copX + 1;
    //   move_sprite(2, copX, new_height);
    //   move_sprite(3, copX+8, new_height);
    //   // animate the dude
    //   // needs some randomness, however.
    //   if (!dudes) {
    //     dudeX = rand();
    //     if (dudeX <= copX && dudeX > copX - 15 && dudeX < 150 && dudeX > 10) {
    //       dudeY = new_height;
    //       dudes = 1;
    //     }
    //   } else {
    //     // animate the dude falling
    //     // add some randomness into their falling?
    //     if (rand() % 2){
    //       dudeY = dudeY + 1;
    //       move_sprite(8, dudeX, dudeY);
    //     }
    //   }
    // }
    // gotoxy(0,0); printf(" %d ", dudes);
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
    cooldown = cooldown - 50;
    cooldown2 = cooldown2 - 5;
    wait_vbl_done();
  }
}