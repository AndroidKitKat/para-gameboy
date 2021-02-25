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

// Bullets
struct Bullet {
  UINT8 x;
  UINT8 y;
  UINT8 sprite;
  UINT8 dx;
  UINT8 dy;
  UINT8 state; // 0 for off screen, 1 for on screen
  UINT8 pos;
} bullets[20] = {
  {0, 0, 20, 0, 0, 0},
  {0, 0, 21, 0, 0, 0},
  {0, 0, 22, 0, 0, 0},
  {0, 0, 23, 0, 0, 0},
  {0, 0, 24, 0, 0, 0},
  {0, 0, 25, 0, 0, 0},
  {0, 0, 26, 0, 0, 0},
  {0, 0, 27, 0, 0, 0},
  {0, 0, 28, 0, 0, 0},
  {0, 0, 29, 0, 0, 0},
  {0, 0, 30, 0, 0, 0},
  {0, 0, 31, 0, 0, 0},
  {0, 0, 32, 0, 0, 0},
  {0, 0, 33, 0, 0, 0},
  {0, 0, 34, 0, 0, 0},
  {0, 0, 35, 0, 0, 0},
  {0, 0, 36, 0, 0, 0},
  {0, 0, 37, 0, 0, 0},
  {0, 0, 38, 0, 0, 0},
  {0, 0, 39, 0, 0, 0},
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
UINT8 score = 0;
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

  set_sprite_data(74, 2, bullet); // 28 total
  set_sprite_tile(20, 74); // 1
  set_sprite_tile(21, 74); // 2
  set_sprite_tile(22, 74); // 3
  set_sprite_tile(23, 74); // 4
  set_sprite_tile(24, 74); // 5
  set_sprite_tile(25, 74); // 6
  set_sprite_tile(26, 74); // 7
  set_sprite_tile(27, 74); // 8
  set_sprite_tile(28, 74); // 9
  set_sprite_tile(29, 74); // 10
  set_sprite_tile(30, 74);
  set_sprite_tile(31, 74);
  set_sprite_tile(32, 74);
  set_sprite_tile(33, 74);
  set_sprite_tile(34, 74);
  set_sprite_tile(35, 74);
  set_sprite_tile(36, 74);
  set_sprite_tile(37, 74);
  set_sprite_tile(38, 74);
  set_sprite_tile(39, 74);


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
    gotoxy(0, 0); printf(HUD, score, lives);
    // if(lives <= 0)
    // {
    //   // end the game
    //   // figure out how to do this. 
    // }
    // gotoxy(0,0); printf(" %d ", cooldown);
    if (lives == 0)
    {
      // end the game
      HIDE_SPRITES;
      break;
    }
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
          if (dudes[j].state == 0 && cooldown2 < 50)
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


    if (joypad() == J_A)
    {
      i = 0;
      while (i < 20) {
      // Right now hardcoded. Somehow, have to know the angle and x/y of end of gun
      // We can choose angle and starting x/y based on "pos" var
      // e.g. when pos == 60, angle is straight up (90), starting x/y is 
      // Also this might just be a shite way to do it. But I'm kinda brute forcing the initial
      // x/y for bullets from each position in the gun. From there we can mess around to find angle,
      // then animate, then collision.
      if (bullets[i].state == 0) {

        switch (pos) {
          case 0:  
            bullets[i].x = 95;
            bullets[i].y = 140; 
            bullets[i].dx = 1; 
            bullets[i].dy = 0; 
            break; 
          case 4:  
            bullets[i].x = 95;
            bullets[i].y = 137; 
            bullets[i].dx = 5; 
            bullets[i].dy = 1; 
            break; 
          case 8:  
            bullets[i].x = 95;
            bullets[i].y = 135; 
            bullets[i].dx = 4; 
            bullets[i].dy = 1; 
            break; 
          case 12:  
            bullets[i].x = 95;
            bullets[i].y = 133; 
            bullets[i].dx = 3; 
            bullets[i].dy = 1; 
            break; 
          case 16:  
            bullets[i].x = 95;
            bullets[i].y = 131; 
            bullets[i].dx = 2; 
            bullets[i].dy = 1; 
            break; 
          case 20:  
            bullets[i].x = 95;
            bullets[i].y = 129; 
            bullets[i].dx = 4; 
            bullets[i].dy = 3; 
            break; 
          case 24:  
            bullets[i].x = 95;
            bullets[i].y = 127; 
            bullets[i].dx = 4; 
            bullets[i].dy = 4; 
            break; 
          case 28:  
            bullets[i].x = 95;
            bullets[i].y = 125; 
            bullets[i].dx = 4; 
            bullets[i].dy = 4; 
            break; 
          case 32:  
            bullets[i].x = 93;
            bullets[i].y = 125;
            bullets[i].dx = 4; 
            bullets[i].dy = 4; 
            break; 
          case 36:  
            bullets[i].x = 92;
            bullets[i].y = 125;
            bullets[i].dx = 3; 
            bullets[i].dy = 4; 
            break; 
          case 40:
            bullets[i].x = 89;
            bullets[i].y = 125;
            bullets[i].dx = 1; 
            bullets[i].dy = 2; 
            break; 
          case 44:
            bullets[i].x = 87;
            bullets[i].y = 125;
            bullets[i].dx = 1; 
            bullets[i].dy = 3; 
            break; 
          case 48:
            bullets[i].x = 85;
            bullets[i].y = 125;
            bullets[i].dx = 1; 
            bullets[i].dy = 4; 
            break; 
          case 56:
            bullets[i].x = 80;
            bullets[i].y = 125;
            bullets[i].dx = 5; 
            bullets[i].dy = 1; 
            break; 
          case 60:
            bullets[i].x = 80;
            bullets[i].y = 125;
            bullets[i].dx = 0; 
            bullets[i].dy = 1; 
            break; 
          case 64:
            bullets[i].x = 73;
            bullets[i].y = 125;
            bullets[i].dx = 0; 
            bullets[i].dy = 1; 
            break; 
          case 68:
            bullets[i].x = 71;
            bullets[i].y = 125;
            bullets[i].dx = 1; 
            bullets[i].dy = 2; 
            break; 
          case 72:
            bullets[i].x = 69;
            bullets[i].y = 125;
            bullets[i].dx = 1; 
            bullets[i].dy = 4; 
            break; 
          case 76:
            bullets[i].x = 67;
            bullets[i].y = 125;
            bullets[i].dx = 1; 
            bullets[i].dy = 3; 
            break; 
          case 80:
            bullets[i].x = 65;
            bullets[i].y = 125;
            bullets[i].dx = 1; 
            bullets[i].dy = 2; 
            break; 
          case 84:
            bullets[i].x = 63;
            bullets[i].y = 125;
            bullets[i].dx = 3; 
            bullets[i].dy = 4; 
            break; 
          case 88:
            bullets[i].x = 63;
            bullets[i].y = 126;
            bullets[i].dx = 4; 
            bullets[i].dy = 4; 
            break; 
          case 92:
            bullets[i].x = 62;
            bullets[i].y = 128;
            bullets[i].dx = 4; 
            bullets[i].dy = 4; 
            break; 
          case 96:
            bullets[i].x = 62;
            bullets[i].y = 130;
            bullets[i].dx = 4; 
            bullets[i].dy = 4; 
            break; 
          case 100:
            bullets[i].x = 62;
            bullets[i].y = 132;
            bullets[i].dx = 4; 
            bullets[i].dy = 3; 
            break; 
          case 104:
            bullets[i].x = 63;
            bullets[i].y = 134;
            bullets[i].dx = 2; 
            bullets[i].dy = 1; 
            break; 
          case 108:
            bullets[i].x = 63;
            bullets[i].y = 136;
            bullets[i].dx = 3; 
            bullets[i].dy = 1; 
            break; 
          case 112:
            bullets[i].x = 62;
            bullets[i].y = 140;
            bullets[i].dx = 4; 
            bullets[i].dy = 1; 
            break; 
          default:
            break;
        }
        bullets[i].state = 1;
        bullets[i].pos = pos;
        break;
      }

      //printf("POS: %d", pos); // debug
      move_sprite(bullets[i].sprite, bullets[i].x, bullets[i].y);
      i += 1;
    }
   }


   i = 0;
   while(i < 20) {
     if (bullets[i].state == 1) {
       if (bullets[i].pos >= 64) {
        bullets[i].x -= bullets[i].dx;
       } else {
        bullets[i].x += bullets[i].dx;
       }
       bullets[i].y -= bullets[i].dy;
       move_sprite(bullets[i].sprite, bullets[i].x, bullets[i].y);

       // collision here & score
       while (j < 12) // loop through dudes
       {
         if (j < 3) // do choppers
         {
           if (bullets[i].x <= copters[j].x && bullets[i].y < copters[j].height && bullets[i].x > copters[j].x - 16 && bullets[i].y > copters[j].height - 16)
           {
             copters[j].state = 0;
             move_sprite(copters[j].sprite, 255, 255);
             move_sprite(copters[j].sprite + 1, 255, 255);
             copters[j].x = 255;
             copters[j].height = 255;
             copters[j].direction = 0;
             set_sprite_prop(copters[j].sprite, CLEAR_PROP);
             set_sprite_prop(copters[j].sprite + 1, CLEAR_PROP);
             c_alive -= 1;
             bullets[i].state = 0;
             move_sprite(bullets[i].sprite, 255, 255);
             score += 3;
           }
         }
         // check if the bullet is inside the dude
         if (bullets[i].x <= dudes[j].x && bullets[i].y < dudes[j].y && bullets[i].x > dudes[j].x - 8 && bullets[i].y > dudes[j].y - 16 && dudes[j].state != 2)
         {
           dudes[j].state = 0;
           move_sprite(dudes[j].sprite, 255, 255);
           dudes[j].x = 255;
           dudes[j].y = 255;
           d_alive -= 1;
           bullets[i].state = 0;
           move_sprite(bullets[i].sprite, 255, 255);
           score += 1;
         }
         j += 1;
       }
       j = 0;
     }
     // 0 - 160 x
     // 0 - 240 y
     if (bullets[i].x <= 0 || bullets[i].x >= 155) {
       bullets[i].x = 0;
       bullets[i].y = 0;
       move_sprite(bullets[i].sprite, 0,0);
       bullets[i].state = 0;
     }

     if (bullets[i].y <= 5 || bullets[i].y >= 240) {
       bullets[i].x = 0;
       bullets[i].y = 0;
      move_sprite(bullets[i].sprite, 0,0);
       bullets[i].state = 0;
     }


     i += 1;
   }

   i = 0;

    // REFERNCE
    // pos / 4 = ...
    // 0 - 14 normal
    // 14 - 28 // flip y


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
  // say game over & show score
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); 

  gotoxy(0,0); printf("GAME OVER!\n");
  printf("Score: %d ", score);
}