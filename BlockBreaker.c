

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

//#link "famitone2.s"
void __fastcall__ famitone_update(void);
//#link "music_aftertherain.s"
extern char after_the_rain_music_data[];
//#link "demosounds.s"
extern char demo_sounds[];

// create easy to track tile/attr for metasprites 
#define TILE 0xd8
#define ATTR 0x00

#define TILE2 0xdc
#define ATTR2 0x02
///// METASPRITES


// define a 2x2 metasprites
const unsigned char paddle[]={
        0,      0,      TILE+0,   ATTR, 
        0,      8,      TILE+1,   ATTR, 
        8,      0,      TILE+2,   ATTR, 
        8,      8,      TILE+3,   ATTR, 
        128};

const unsigned char ball[]={
        0,      0,      TILE2+0,   ATTR2, 
        0,      8,      TILE2+1,   ATTR2, 
        8,      0,      TILE2+2,   ATTR2, 
        8,      8,      TILE2+3,   ATTR2, 
        128};



/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x03,			// screen color

  0x11,0x30,0x27,0x0,	// background palette 0
  0x1c,0x20,0x2c,0x0,	// background palette 1
  0x00,0x10,0x20,0x0,	// background palette 2
  0x06,0x16,0x26,0x0,	// background palette 3

  0x16,0x35,0x24,0x0,	// sprite palette 0
  0x00,0x37,0x25,0x0,	// sprite palette 1
  0x0d,0x2d,0x3a,0x0,	// sprite palette 2
  0x0d,0x27,0x2a	// sprite palette 3
};



// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_hide_rest(0);
  // set palette colors
  pal_all(PALETTE);
  // turn on PPU
  ppu_on_all();
}




// number of actors (4 h/w sprites each)
#define NUM_ACTORS 1

// actor x/y positions
byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
// actor x/y deltas per frame (signed)
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];
//our ball and sprite stuff
byte ball_x[1];
byte ball_y[1];

sbyte ball_dx[1];
sbyte ball_dy[1]; 


byte sprite = 0x02;



byte sprite_y1 = 100;

byte sprite_y2 = 108;
// game bool value
bool game = true;
//score value
int score;
// lives value
int lives; 
//struct for bricks 
typedef struct Brick{
  byte _x;		// fruit x/y position
  byte _y;		
  byte sprite; 
  
};
// make our brick structures
struct Brick bricks[20];
//This is our starting bricks, what the starting area looks like. 
void starting_bricks() {
  char oam_id;
  char i; 
  oam_id = 0;
    for (i = 10; i < 20; i++) {
      bricks[i-10]._x = i*8;
      bricks[i-10]._y = sprite_y1;
      bricks[i-10].sprite = sprite;
      bricks[i]._x = i*8;
      bricks[i]._y = sprite_y2;
      bricks[i].sprite = sprite;
      
      oam_id = oam_spr(bricks[i-10]._x, bricks[i-10]._y, bricks[i-10].sprite, 0x02, oam_id);
      oam_id = oam_spr(bricks[i]._x, bricks[i]._y, bricks[i].sprite, 0x00, oam_id);
 
    }
  
}
//This checks if ball has collided with a brick and returns true if so.
bool brick_collision(int i){
  if(((bricks[i]._x >= ball_x[0]-4 && bricks[i]._x <= ball_x[0]+8)&& (bricks[i]._y >= ball_y[0]-2 && bricks[i]._y <= ball_y[0]+4))) //hits floor or collision detected
      {
        score +=1;
        return true;     // erase brick that was hit. 
        
      }
  
}
//stops music and displays game over message
void game_over()
{
  music_stop();
  setup_graphics();
  while(1) 
  { 
   
  }
  
}



// main program
void main() {
  
  char i;	// actor index
  char oam_id;	// sprite ID
  char pad;	// controller flags
  bool falling; // falling flag 
  bool right;   // x direction of ball
  falling = true; 
  right = true;
  score = 0;
  lives = 3; 
  
 
  //set actor x and y
  actor_x[0] = 100;
  actor_y[0] = 200;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
  
  //set ball x and y and falling bool
  ball_x[0] = 100;
  ball_y[0] = 150;
  ball_dx[0] = 0;
  ball_dy[0] = 0; 
  
  // setup graphics
  setup_graphics();
  //PLAY MUSIC 
  famitone_init(after_the_rain_music_data);
  // Set demosounds
  sfx_init(demo_sounds); 
   // set music callback function for NMI
  nmi_set_callback(famitone_update);
  // play music
  music_play(0);

  // loop until game is over
  
  starting_bricks(); 
  while (game) {
    // random int for our ball's dx values
    int r = rand() % 2;
     
    // start with OAMid/sprite 0
    oam_id = 0;
    //draw our bricks on two levels with different colors (i and i-10)
    for (i = 10; i < 20; i++) {
      
      
      oam_id = oam_spr(bricks[i-10]._x, bricks[i-10]._y, bricks[i-10].sprite, 0x02, oam_id);
      oam_id = oam_spr(bricks[i]._x, bricks[i]._y, bricks[i].sprite, 0x00, oam_id);
      
      //if there is a collision with bricks play sound, and erase the brick and change 
      // our falling bool to the opposite of what it currently is 
      // do this for both i and i-10 for both levels
      if(brick_collision(i))
      {
        sfx_play(0,2); 
        bricks[i].sprite=0;
        bricks[i]._x = 0;
        bricks[i]._y = 0;
        if(falling)
        {
          falling = false; 
          break;
        }
        else
        {
          falling = true; 
          break;
        }
      }
      if(brick_collision(i-10))
      {
        sfx_play(0,2); 
        bricks[i-10].sprite=0;
        bricks[i-10]._x = 0;
        bricks[i-10]._y = 0;
        if(falling)
        {
          falling = false; 
          break;
        }
        else
        {
          falling = true; 
          break;
        }
      }
      
      
 
    }
   //draw our ball
    oam_id = oam_meta_spr(ball_x[0], ball_y[0], oam_id, ball);
    // set player 0/1 velocity based on controller
    for (i=0; i<2; i++) {
      // poll controller i (0-1)
      pad = pad_poll(i);
      // move actor[i] left/right
      if (pad&PAD_LEFT && actor_x[i]>10) actor_dx[i]=-2;
      else if (pad&PAD_RIGHT && actor_x[i]<230) actor_dx[i]=2;
      else actor_dx[i]=0;
      
    }
    // draw and move paddle
    for (i=0; i<1; i++) {
      
      
      oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, paddle);
      actor_x[i] += actor_dx[i];
      actor_y[i] += actor_dy[i];
    }
    
    
    
    // Set up walls around play area
    if (ball_x[0] >= 220)
      {
      	sfx_play(2,2);
      
        ball_dx[0] = -r;
      }
      
      if (ball_x[0] <= 10)
      {
        sfx_play(2,2);
        ball_dx[0] = r;
      }
    
    // make ball fall
    if (falling) 
    {//if ball is touching floor, game over end game loop if we get out of game over screen.
      if (ball_y[0] >= 220)
      {
        // lower our lives score, reset our ball and paddle and delay a bit so user
        // can register that they have lost a life. 
        lives -= 1; 
        ball_x[0] = 100;
        ball_y[0] = 150;
        ball_dx[0] = 0;
        actor_x[0] = 100;
        actor_y[0] = 200;
        delay(20); 
        if(lives < 0)
        {
          game_over();
        }
        
      }
      
      
      //if ball is touching paddle, falling is now false and a collision has taken place
      if(((ball_x[0] >= actor_x[0]-4 && ball_x[0] <= actor_x[0]+8)&& (ball_y[0] >= actor_y[0]-2 && ball_y[0] <= actor_y[0]+4)))
      {
        sfx_play(2,2); 
        falling = false;
        for (i = 0; i< 1; i++)
        {
          if(right)
          {
            ball_dx[i] = r;
            right = false; 
          }
          else
          {
            ball_dx[i] = -r;
            right = true;
          }
        }
           
      }
      
          
      
      
      
      
      
      
      // if no collission, then continue falling. 
    	for (i = 0; i<1; i++)
    	{
      
     	 ball_y[i] += 2;
         ball_x[i] += ball_dx[i];
    	}
    }
    else 
    {
      // if ball is touching the ceiling then start falling again
      if(ball_y[0] <= 10 )
      {
        falling = true;
        sfx_play(2,2);
        
          
      }
      
      // otherwise continue rising 
      for (i = 0; i<1; i++)
    	{
      
     	ball_y[i] -= 2;
        
        ball_x[i] += ball_dx[i];
    	}
      
    }
   
    //Draws and updates Scoreboard
    oam_id = oam_spr(184, 10, 83, 3, oam_id);
    oam_id = oam_spr(192, 10, 67, 3, oam_id);
    oam_id = oam_spr(200, 10, 79, 3, oam_id);
    oam_id = oam_spr(208, 10, 82, 3, oam_id);
    oam_id = oam_spr(216, 10, 69, 3, oam_id);
    oam_id = oam_spr(224, 10, 58, 3, oam_id);
    oam_id = oam_spr(232, 10, (score/10%10)+48, 3, oam_id);
    oam_id = oam_spr(240, 10, (score%10)+48, 3, oam_id);
    
    //Draws and updates Lives
    oam_id = oam_spr(8, 10, 76, 1, oam_id);
    oam_id = oam_spr(16, 10, 73, 1, oam_id);
    oam_id = oam_spr(24, 10, 86, 1, oam_id);
    oam_id = oam_spr(32, 10, 69, 1, oam_id);
    oam_id = oam_spr(40, 10, 83, 1, oam_id);
    oam_id = oam_spr(48, 10, 58, 1, oam_id);
    oam_id = oam_spr(56, 10, (lives%10)+48, 1, oam_id);
    
    // hide rest of sprites
    // if we haven't wrapped oam_id around to 0
    if (oam_id!=0) oam_hide_rest(oam_id);
    // wait for next frame
    ppu_wait_frame();
  }
}


