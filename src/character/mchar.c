/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "mchar.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//mchar character structure
enum
{
	mchar_ArcMain_Idle0,
	mchar_ArcMain_Idle1,
	mchar_ArcMain_Idle2,
	mchar_ArcMain_Idle3,
	mchar_ArcMain_Idle4,
	mchar_ArcMain_Idle5,
	
	mchar_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[mchar_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_mchar;

//mchar character definitions
static const CharFrame char_mchar_frame[] = {
	//normal sonic
	{mchar_ArcMain_Idle0, {  0,   0, 183, 158}, { 0, 0}}, //0 idle 1
	{mchar_ArcMain_Idle1, {  0,   0, 181, 162}, { 0, 0}}, //1 idle 2
	{mchar_ArcMain_Idle2, {  0,   0, 180, 164}, { 0, 0}}, //2 idle 3
	//static
	{mchar_ArcMain_Idle3, {  0,   0, 201, 201}, { 0, 0}}, //3 idle 1
	//exe sonic
	{mchar_ArcMain_Idle4, {  0,   0, 190, 116}, { 0, 0}}, //4 idle 2
	{mchar_ArcMain_Idle4, {  0, 116, 185, 119}, { 0, 0}}, //5 idle 2
	{mchar_ArcMain_Idle5, {  0,   0, 184, 122}, { 0, 0}}, //6 idle 3
	{mchar_ArcMain_Idle5, {  0, 122, 184, 122}, { 0, 0}}, //7 idle 3
};

static const Animation char_mchar_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){ 3,  4, ASCR_BACK, 0}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 5,  6, ASCR_BACK, 0}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 0}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 0}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//mchar character functions
void Char_mchar_SetFrame(void *user, u8 frame)
{
	Char_mchar *this = (Char_mchar*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_mchar_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_mchar_Tick(Character *character)
{
	Char_mchar *this = (Char_mchar*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_mchar_SetFrame);
	Character_Draw(character, &this->tex, &char_mchar_frame[this->frame]);
}

void Char_mchar_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_mchar_Free(Character *character)
{
	Char_mchar *this = (Char_mchar*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_mchar_New(fixed_t x, fixed_t y)
{
	//Allocate mchar object
	Char_mchar *this = Mem_Alloc(sizeof(Char_mchar));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_mchar_New] Failed to allocate mchar object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_mchar_Tick;
	this->character.set_anim = Char_mchar_SetAnim;
	this->character.free = Char_mchar_Free;
	
	Animatable_Init(&this->character.animatable, char_mchar_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;
	
	this->character.focus_x = FIXED_DEC(-77 - 60,1);
	this->character.focus_y = FIXED_DEC(-30 - 100,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\mchar.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //mchar_ArcMain_Idle0
		"idle1.tim", //mchar_ArcMain_Idle1
		"idle2.tim", //mchar_ArcMain_Idle1
		"idle3.tim", //mchar_ArcMain_Idle1
		"idle4.tim", //mchar_ArcMain_Idle1
		"idle5.tim", //mchar_ArcMain_Idle1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
