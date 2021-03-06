/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend skull fragments
static SkullFragment char_bf_skull[15] = {
	{ 1 * 8, -87 * 8, -13, -13},
	{ 9 * 8, -88 * 8,   5, -22},
	{18 * 8, -87 * 8,   9, -22},
	{26 * 8, -85 * 8,  13, -13},
	
	{-3 * 8, -82 * 8, -13, -11},
	{ 8 * 8, -85 * 8,  -9, -15},
	{20 * 8, -82 * 8,   9, -15},
	{30 * 8, -79 * 8,  13, -11},
	
	{-1 * 8, -74 * 8, -13, -5},
	{ 8 * 8, -77 * 8,  -9, -9},
	{19 * 8, -75 * 8,   9, -9},
	{26 * 8, -74 * 8,  13, -5},
	
	{ 5 * 8, -73 * 8, -5, -3},
	{14 * 8, -76 * 8,  9, -6},
	{26 * 8, -67 * 8, 15, -3},
};

//Boyfriend player types
enum
{
	BF_ArcMain_Idle,
	BF_ArcMain_Left,
	BF_ArcMain_Down,
	BF_ArcMain_Up,
	BF_ArcMain_Right,
	BF_ArcMain_Idleb,
	BF_ArcMain_Dead0, //BREAK
	BF_ArcDead_Dead1, //Mic Drop
	BF_ArcDead_Dead2, //Twitch
	BF_ArcDead_Dead3, //Twitch
	BF_ArcDead_Retry, //Retry prompt
	
	BF_ArcMain_Max,
};

#define BF_Arc_Max BF_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BF_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
	
	u8 retry_bump;
	
	SkullFragment skull[COUNT_OF(char_bf_skull)];
	u8 skull_scale;
} Char_BF;

//Boyfriend player definitions
static const CharFrame char_bf_frame[] = {
	{BF_ArcMain_Idle, {  0,   0, 100,  115}, { 66,  79 - 5}}, //0 idle 1
	{BF_ArcMain_Idle, {100,   0,  90,  120}, { 65,  79 - 5}}, //1 idle 1
	{BF_ArcMain_Idle, {  0, 115,  83,  118}, { 55,  78 - 5}}, //2 idle 1
	{BF_ArcMain_Idle, { 83, 120,  81,  122}, { 56,  79 - 5}}, //3 idle 1
	
	{BF_ArcMain_Left, {  0,   0, 108,  115}, { 65,  74}}, //4 idle 1
	{BF_ArcMain_Left, {108,   0, 107,  120}, { 64,  74}}, //5 idle 1
	{BF_ArcMain_Left, {  0, 115,  97,  119}, { 54,  73}}, //6 idle 1
	{BF_ArcMain_Left, { 97, 120, 102,  118}, { 60,  74}}, //7 idle 1
	
	{BF_ArcMain_Down, {  0,   0, 121,  108}, { 76,  69}}, //8 idle 1
	{BF_ArcMain_Down, {121,   0, 121,  115}, { 76,  69}}, //9 idle 1
	{BF_ArcMain_Down, {  0, 108, 121,  114}, { 77,  69}}, //10 idle 1
	{BF_ArcMain_Down, {121, 115, 122,  119}, { 78,  71}}, //11 idle 1
	
	{BF_ArcMain_Up, {  0,   0, 106,  114}, { 70,  72}}, //12 idle 1
	{BF_ArcMain_Up, {106,   0,  96,  120}, { 70,  73}}, //13 idle 1
	{BF_ArcMain_Up, {  0, 114,  98,  120}, { 69,  74}}, //14 idle 1
	{BF_ArcMain_Up, { 98, 120,  94,  123}, { 70,  74}}, //15 idle 1
	
	{BF_ArcMain_Right, {  0,   0, 110,  113}, { 77,  72}}, //16 idle 1
	{BF_ArcMain_Right, {110,   0, 103,  119}, { 77,  73}}, //17 idle 1
	{BF_ArcMain_Right, {  0, 113, 103,  118}, { 76,  73}}, //18 idle 1
	{BF_ArcMain_Right, {103, 119, 102,  121}, { 76,  73}}, //19 idle 1

	{BF_ArcMain_Dead0, {  0,   0, 181, 140}, { 63,  94}}, //21 dead0 0
	{BF_ArcDead_Dead1, {  0,   0, 182, 138}, { 64,  93}}, //22 dead1 1
	{BF_ArcDead_Dead2, {  0,   0, 183, 137}, { 65,  91}}, //23 dead1 1
	{BF_ArcDead_Dead3, {  0,   0, 184, 137}, { 65,  92}}, //24 dead1 1
	{BF_ArcDead_Dead3, {184,   0,  42,  21}, { 57, 111}}, //25 dead1 1
	{BF_ArcDead_Dead3, {184,  21,  44,  23}, { 57, 111}},  //26 dead1 1
	{BF_ArcDead_Dead3, {184,  44,  48,  26}, { 57, 111}},  //27 dead1 1
	{BF_ArcDead_Dead3, {184,  70,  49,  27}, { 57, 111}},  //28 dead1 1
	{BF_ArcDead_Dead3, {184,  97,  47,  26}, { 57, 111}},  //29 dead1 1

	{BF_ArcMain_Idleb, {  0,   0, 105,  101}, { 75,  68}}, //30 idle 1
	{BF_ArcMain_Idleb, {105,   0, 102,  100}, { 71,  68}}, // idle 1
	{BF_ArcMain_Idleb, {  0, 101, 104,   98}, { 72,  68}}, //2 idle 1
	{BF_ArcMain_Idleb, {104, 100, 104,   99}, { 72,  68}}, //3 idle 1
};

static const Animation char_bf_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 4,  5, 6, 7, ASCR_BACK, 3}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{1, (const u8[]){ 8, 9, 10, 11, ASCR_BACK, 3}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{1, (const u8[]){ 12, 13, 14, 15, ASCR_BACK, 3}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 16, 17, 18, 19, ASCR_BACK, 3}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt

	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt

	{2, (const u8[]){20, 21, 22, 23, 23, 23, 23, 24, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{2, (const u8[]){24, 24, 24, 24, 24, ASCR_BACK, 1}},                                                      //PlayerAnim_Dead1
	{2, (const u8[]){25, 26, 27, 28, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{2, (const u8[]){28, 28, 28, 28, ASCR_CHGANI, PlayerAnim_Dead3}},                                                      //PlayerAnim_Dead3
	{2, (const u8[]){28, 28, 28, 28, ASCR_CHGANI, PlayerAnim_Dead3}},              //PlayerAnim_Dead4
	{2, (const u8[]){28, 28, 28, 28, ASCR_CHGANI, PlayerAnim_Dead3}},                   //PlayerAnim_Dead5
	
	{3, (const u8[]){28, 27, 26, 25, ASCR_CHGANI, PlayerAnim_Dead7}}, //PlayerAnim_Dead4
	{3, (const u8[]){25, 25, 25, 25, ASCR_CHGANI, PlayerAnim_Dead7}},  //PlayerAnim_Dead5
};

static const Animation char_bf_anim2[PlayerAnim_Max] = {
	{2, (const u8[]){ 29,  30,  31, 32, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 4,  5, 6, 7, ASCR_BACK, 3}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{1, (const u8[]){ 8, 9, 10, 11, ASCR_BACK, 3}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{1, (const u8[]){ 12, 13, 14, 15, ASCR_BACK, 3}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 16, 17, 18, 19, ASCR_BACK, 3}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt

	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt

	{2, (const u8[]){20, 21, 22, 23, 23, 23, 23, 24, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{2, (const u8[]){24, 24, 24, 24, 24, ASCR_BACK, 1}},                                                      //PlayerAnim_Dead1
	{2, (const u8[]){25, 26, 27, 28, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{2, (const u8[]){28, 28, 28, 28, ASCR_CHGANI, PlayerAnim_Dead3}},                                                      //PlayerAnim_Dead3
	{2, (const u8[]){28, 28, 28, 28, ASCR_CHGANI, PlayerAnim_Dead3}},              //PlayerAnim_Dead4
	{2, (const u8[]){28, 28, 28, 28, ASCR_CHGANI, PlayerAnim_Dead3}},                   //PlayerAnim_Dead5
	
	{3, (const u8[]){28, 27, 26, 25, ASCR_CHGANI, PlayerAnim_Dead7}}, //PlayerAnim_Dead4
	{3, (const u8[]){25, 25, 25, 25, ASCR_CHGANI, PlayerAnim_Dead7}},  //PlayerAnim_Dead5
};

//Boyfriend player functions
void Char_BF_SetFrame(void *user, u8 frame)
{
	Char_BF *this = (Char_BF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BF_Tick(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Retry screen
	if (character->animatable.anim >= PlayerAnim_Dead3)
	{
		/*
		//Tick skull fragments
		if (this->skull_scale)
		{
			SkullFragment *frag = this->skull;
			for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BF, skull); i++, frag++)
			{
				//Draw fragment
				RECT frag_src = {
					(i & 1) ? 112 : 96,
					(i >> 1) << 4,
					16,
					16
				};
				fixed_t skull_dim = (FIXED_DEC(16,1) * this->skull_scale) >> 6;
				fixed_t skull_rad = skull_dim >> 1;
				RECT_FIXED frag_dst = {
					character->x + (((fixed_t)frag->x << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.x,
					character->y + (((fixed_t)frag->y << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.y,
					skull_dim,
					skull_dim,
				};
				Stage_DrawTex(&this->tex_retry, &frag_src, &frag_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
				
				//Move fragment
				frag->x += frag->xsp;
				frag->y += ++frag->ysp;
			}
			
			//Decrease scale
			this->skull_scale--;
		}
		
		//Draw input options
		u8 input_scale = 16 - this->skull_scale;
		if (input_scale > 16)
			input_scale = 0;
		
		RECT button_src = {
			 0, 96,
			16, 16
		};
		RECT_FIXED button_dst = {
			character->x - FIXED_DEC(32,1) - stage.camera.x,
			character->y - FIXED_DEC(88,1) - stage.camera.y,
			(FIXED_DEC(16,1) * input_scale) >> 4,
			FIXED_DEC(16,1),
		};
		
		//Cross - Retry
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Circle - Blueball
		button_src.x = 16;
		button_dst.y += FIXED_DEC(56,1);
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Draw 'RETRY'
		u8 retry_frame;
		
		if (character->animatable.anim == PlayerAnim_Dead6)
		{
			//Selected retry
			retry_frame = 2 - (this->retry_bump >> 3);
			if (retry_frame >= 3)
				retry_frame = 0;
			if (this->retry_bump & 2)
				retry_frame += 3;
			
			if (++this->retry_bump == 0xFF)
				this->retry_bump = 0xFD;
		}
		else
		{
			//Idle
			retry_frame = 1 +  (this->retry_bump >> 2);
			if (retry_frame >= 3)
				retry_frame = 0;
			
			if (++this->retry_bump >= 55)
				this->retry_bump = 0;
		}
		
		RECT retry_src = {
			(retry_frame & 1) ? 48 : 0,
			(retry_frame >> 1) << 5,
			48,
			32
		};
		RECT_FIXED retry_dst = {
			character->x -  FIXED_DEC(7,1) - stage.camera.x,
			character->y - FIXED_DEC(92,1) - stage.camera.y,
			FIXED_DEC(48,1),
			FIXED_DEC(32,1),
		};
		Stage_DrawTex(&this->tex_retry, &retry_src, &retry_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		*/
	}
	
	if (stage.timercount >= 7733)
		this->character.health_i = 1;
	//Animate and draw character
	if (stage.timercount >= 7733)
		Animatable_Animate(&character->animatable2, (void*)this, Char_BF_SetFrame);
	else
		Animatable_Animate(&character->animatable, (void*)this, Char_BF_SetFrame);	

	Character_Draw(character, &this->tex, &char_bf_frame[this->frame]);
}

void Char_BF_SetAnim(Character *character, u8 anim)
{
	Char_BF *this = (Char_BF*)character;
	
	//Perform animation checks
	switch (anim)
	{
		case PlayerAnim_Dead0:
			//Begin reading dead.arc and adjust focus
			character->focus_x = FIXED_DEC(0,1);
			character->focus_y = FIXED_DEC(-40,1);
			character->focus_zoom = FIXED_DEC(125,100);
			break;
		case PlayerAnim_Dead2:
			//Load retry art
			//Gfx_LoadTex(&this->tex_retry, this->arc_ptr[BF_ArcDead_Retry], 0);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Animatable_SetAnim(&character->animatable2, anim);
	Character_CheckStartSing(character);
}

void Char_BF_Free(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BF_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BF *this = Mem_Alloc(sizeof(Char_BF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BF_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BF_Tick;
	this->character.set_anim = Char_BF_SetAnim;
	this->character.free = Char_BF_Free;
	
	Animatable_Init(&this->character.animatable, char_bf_anim);
	Animatable_Init(&this->character.animatable2, char_bf_anim2);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;

	this->character.health_i = 0;
	
	this->character.focus_x = FIXED_DEC(29 - 120,1);
	this->character.focus_y = FIXED_DEC(-74 - -30,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BF.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim",   //BF_ArcMain_BF0
		"left.tim",   //BF_ArcMain_BF1
		"down.tim",   //BF_ArcMain_BF2
		"up.tim",   //BF_ArcMain_BF3
		"right.tim",   //BF_ArcMain_BF4
		"idleb.tim",   //BF_ArcMain_BF0
		"dead0.tim", //BF_ArcMain_Dead0
		"dead1.tim", //BF_ArcDead_Dead1
		"dead2.tim", //BF_ArcDead_Dead2
		"dead3.tim", //BF_ArcDead_Dead3
		"retry.tim", //BF_ArcDead_Retry
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize player state
	this->retry_bump = 0;
	
	//Copy skull fragments
	memcpy(this->skull, char_bf_skull, sizeof(char_bf_skull));
	this->skull_scale = 64;
	
	SkullFragment *frag = this->skull;
	for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BF, skull); i++, frag++)
	{
		//Randomize trajectory
		frag->xsp += RandomRange(-4, 4);
		frag->ysp += RandomRange(-2, 2);
	}
	
	return (Character*)this;
}