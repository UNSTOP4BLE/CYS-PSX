/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ben.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//ben character structure
enum
{
	ben_ArcMain_Idle0,
	ben_ArcMain_Idle1,
	ben_ArcMain_Idle2,
	ben_ArcMain_Left0,
	ben_ArcMain_Left1,
	ben_ArcMain_Down0,
	ben_ArcMain_Down1,
	ben_ArcMain_Up0,
	ben_ArcMain_Up1,
	ben_ArcMain_Right0,
	ben_ArcMain_Right1,
	
	ben_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[ben_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_ben;

//ben character definitions
static const CharFrame char_ben_frame[] = {
	{ben_ArcMain_Idle0, {  0,   0, 142, 123}, { 74 - 42, 78 + 45}}, //0 idle 1
	{ben_ArcMain_Idle1, {  0, 123, 141, 124}, { 73 - 42, 78 + 45}}, //1 idle 2
	{ben_ArcMain_Idle2, {  0,   0, 141, 125}, { 73 - 42, 78 + 45}}, //2 idle 3

	{ben_ArcMain_Left0, {  0,   0, 120, 135}, { 79, 135}}, //4 left 1
	{ben_ArcMain_Left1, {113,   0, 113, 133}, { 75, 133}}, //5 left 2
	
	{ben_ArcMain_Down0, {  0,   0, 144,  96}, { 18, 90}}, //6 down 1
	{ben_ArcMain_Down1, {  0, 100, 147, 101}, { 21, 100}}, //7 down 2
	
	{ben_ArcMain_Up0, {  0,   0, 111, 146}, { 38, 146}}, //8 up 1
	{ben_ArcMain_Up1, {115,   0, 115, 138}, { 36, 138}}, //9 up 2
	
	{ben_ArcMain_Right0, {  0,   0, 144, 113}, { 29, 113}}, //10 right 1
	{ben_ArcMain_Right1, {  0, 109, 139, 109}, { 26, 109}}, //11 right 2
};

static const Animation char_ben_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){ 6,  7, 8, 9, ASCR_BACK, 0}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 10,  11, 12, 13, ASCR_BACK, 0}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 14,  15, 16, 17, ASCR_BACK, 0}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 18, 19, 20, 21, ASCR_BACK, 0}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//ben character functions
void Char_ben_SetFrame(void *user, u8 frame)
{
	Char_ben *this = (Char_ben*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_ben_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_ben_Tick(Character *character)
{
	Char_ben *this = (Char_ben*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_ben_SetFrame);
	Character_Draw(character, &this->tex, &char_ben_frame[this->frame]);
}

void Char_ben_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_ben_Free(Character *character)
{
	Char_ben *this = (Char_ben*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_ben_New(fixed_t x, fixed_t y)
{
	//Allocate ben object
	Char_ben *this = Mem_Alloc(sizeof(Char_ben));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_ben_New] Failed to allocate ben object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_ben_Tick;
	this->character.set_anim = Char_ben_SetAnim;
	this->character.free = Char_ben_Free;
	
	Animatable_Init(&this->character.animatable, char_ben_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;
	
	this->character.focus_x = FIXED_DEC(-40 - -120,1);
	this->character.focus_y = FIXED_DEC(-87 - -20,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SPEED.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //ben_ArcMain_Idle0
		"idle1.tim", //ben_ArcMain_Idle1
		"idle2.tim", //ben_ArcMain_Idle1
		"left0.tim",  //ben_ArcMain_Left
		"left1.tim",  //ben_ArcMain_Left
		"down0.tim",  //ben_ArcMain_Down
		"down1.tim",  //ben_ArcMain_Down
		"up0.tim",    //ben_ArcMain_Up
		"up1.tim",    //ben_ArcMain_Up
		"right0.tim", //ben_ArcMain_Right
		"right1.tim", //ben_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
