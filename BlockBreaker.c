

#include <stdlib.h>
#include <string.h>

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

byte ball_x[1];
byte ball_y[1];

sbyte ball_dx[1];
sbyte ball_dy[1]; 


byte sprite = 0x02;



byte sprite_y1 = 100;

byte sprite_y2 = 108;
// main program
void main() {
  
  char i;	// actor index
  char oam_id;	// sprite ID
  char pad;	// controller flags
  bool falling; 
  falling = true; 
 
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
  //famitone_init(after_the_rain_music_data);
   // set music callback function for NMI
  nmi_set_callback(famitone_update);
  // play music
  music_play(0);

  // loop forever
  // Will need to take out bricks from game once we work out collision 
  
  while (1) {
    // start with OAMid/sprite 0
    oam_id = 0;
    for (i = 10; i < 20; i++) {
      oam_id = oam_spr(i*8, sprite_y1, sprite, 0x02, oam_id);
      oam_id = oam_spr(i*8,sprite_y2, sprite,0x00, oam_id);
 
    }
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
    
    // make ball fall
    if (falling) 
    {	//if ball is touching floor or paddle, falling is now false and a collission has taken place
        if(ball_y[0] >= 210 || ((ball_x[0] >= actor_x[0]-4 && ball_x[0] <= actor_x[0]+8)&& (ball_y[0] >= actor_y[0]-2 && ball_y[0] <= actor_y[0]+4)))
      {
        falling = false;
           
      }
      // if no collission, then continue falling. 
    	for (i = 0; i<1; i++)
    	{
      
     	 ball_y[i] += 2;
    	}
    }
    else 
    {
      // if ball is touching the ceiling then start falling again
      if(ball_y[0] <= 10 )
      {
        falling = true;
          
      }
      
      // otherwise continue rising 
      for (i = 0; i<1; i++)
    	{
      
     	 ball_y[i] -= 2;
    	}
      
    }
    
    
    // hide rest of sprites
    // if we haven't wrapped oam_id around to 0
    if (oam_id!=0) oam_hide_rest(oam_id);
    // wait for next frame
    ppu_wait_frame();
  }
}


