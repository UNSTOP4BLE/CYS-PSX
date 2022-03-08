/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "dad.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Dad character structure
enum
{
	Dad_ArcMain_Idle0,
	Dad_ArcMain_Idle1,
	Dad_ArcMain_Idle2,
	Dad_ArcMain_Left0,
	Dad_ArcMain_Left1,
	Dad_ArcMain_Down0,
	Dad_ArcMain_Down1,
	Dad_ArcMain_Up0,
	Dad_ArcMain_Up1,
	Dad_ArcMain_Right0,
	Dad_ArcMain_Right1,
	Dad_ArcMain_Spec0,
	Dad_ArcMain_Spec1,
	
	Dad_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Dad_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Dad;

//Dad character definitions
static const CharFrame char_dad_frame[] = {
	{Dad_ArcMain_Idle0, {  0,   0, 142, 123}, { 74 - 42, 78 + 45}}, //0 idle 1
	{Dad_ArcMain_Idle0, {  0, 123, 141, 124}, { 73 - 42, 78 + 45}}, //1 idle 2
	{Dad_ArcMain_Idle1, {  0,   0, 141, 125}, { 73 - 42, 78 + 45}}, //2 idle 3
	{Dad_ArcMain_Idle1, {  0, 125, 141, 124}, { 73 - 42, 78 + 45}}, //3 idle 4
	{Dad_ArcMain_Idle2, {  0,   0, 142, 123}, { 74 - 42, 78 + 45}}, //2 idle 3
	{Dad_ArcMain_Idle2, {  0, 123, 142, 123}, { 74 - 42, 78 + 45}}, //3 idle 4	

	{Dad_ArcMain_Left0, {  0,   0, 120, 135}, { 79, 135}}, //4 left 1
	{Dad_ArcMain_Left0, {120,   0, 115, 133}, { 74, 133}}, //5 left 2
	{Dad_ArcMain_Left1, {  0,   0, 113, 133}, { 75, 133}}, //4 left 1
	{Dad_ArcMain_Left1, {113,   0, 113, 133}, { 75, 133}}, //5 left 2
	
	{Dad_ArcMain_Down0, {  0,   0, 144,  96}, { 18, 90}}, //6 down 1
	{Dad_ArcMain_Down0, {  0,  96, 146, 100}, { 21, 98}}, //7 down 2
	{Dad_ArcMain_Down1, {  0,   0, 146, 100}, { 20, 99}}, //6 down 1
	{Dad_ArcMain_Down1, {  0, 100, 147, 101}, { 21, 100}}, //7 down 2
	
	{Dad_ArcMain_Up0, {  0,   0, 111, 146}, { 38, 146}}, //8 up 1
	{Dad_ArcMain_Up0, {111,   0, 113, 142}, { 36, 142}}, //9 up 2
	{Dad_ArcMain_Up1, {  0,   0, 115, 138}, { 36, 138}}, //8 up 1
	{Dad_ArcMain_Up1, {115,   0, 115, 138}, { 36, 138}}, //9 up 2
	
	{Dad_ArcMain_Right0, {  0,   0, 144, 113}, { 29, 113}}, //10 right 1
	{Dad_ArcMain_Right0, {  0, 113, 140, 110}, { 28, 110}}, //11 right 2
	{Dad_ArcMain_Right1, {  0,   0, 139, 109}, { 26, 109}}, //10 right 1
	{Dad_ArcMain_Right1, {  0, 109, 139, 109}, { 26, 109}}, //11 right 2

	{Dad_ArcMain_Spec0, {  0,   0,  98, 152}, { 71, 152}}, //10 right 1
	{Dad_ArcMain_Spec0, { 98,   0,  99, 143}, { 69, 143}}, //11 right 2
	{Dad_ArcMain_Spec1, {  0,   0,  99, 139}, { 68, 139}}, //10 right 1
	{Dad_ArcMain_Spec1, { 99,   0,  99, 139}, { 68, 139}}, //11 right 2
};

static const Animation char_dad_anim[CharAnim_Max] = {
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

static const Animation char_dad_anim2[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 9, ASCR_BACK, 0}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 10, 11, 12, 13, ASCR_BACK, 0}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 22, 23, 24, 25, ASCR_BACK, 0}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 18, 19, 20, 21, ASCR_BACK, 0}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Dad character functions
void Char_Dad_SetFrame(void *user, u8 frame)
{
	Char_Dad *this = (Char_Dad*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_dad_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Dad_Tick(Character *character)
{
	Char_Dad *this = (Char_Dad*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	if (stage.song_step >= 224 && stage.song_step <= 253) 
		Animatable_Animate(&character->animatable2, (void*)this, Char_Dad_SetFrame);
	
	else if (stage.song_step >= 992 && stage.song_step <= 1020) 
		Animatable_Animate(&character->animatable2, (void*)this, Char_Dad_SetFrame);
	
	else if (stage.song_step >= 2015 && stage.song_step <= 2045) 
		Animatable_Animate(&character->animatable2, (void*)this, Char_Dad_SetFrame);
	
	else if (stage.song_step >= 2559 && stage.song_step <= 2588) 
		Animatable_Animate(&character->animatable2, (void*)this, Char_Dad_SetFrame);
	
	else if (stage.timercount >= 10722 && stage.timercount <= 10851) 
		Animatable_Animate(&character->animatable2, (void*)this, Char_Dad_SetFrame);
	
	else
		Animatable_Animate(&character->animatable, (void*)this, Char_Dad_SetFrame);
	
	Character_Draw(character, &this->tex, &char_dad_frame[this->frame]);
}

void Char_Dad_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Animatable_SetAnim(&character->animatable2, anim);
	Character_CheckStartSing(character);
}

void Char_Dad_Free(Character *character)
{
	Char_Dad *this = (Char_Dad*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Dad_New(fixed_t x, fixed_t y)
{
	//Allocate dad object
	Char_Dad *this = Mem_Alloc(sizeof(Char_Dad));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Dad_New] Failed to allocate dad object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Dad_Tick;
	this->character.set_anim = Char_Dad_SetAnim;
	this->character.free = Char_Dad_Free;
	
	Animatable_Init(&this->character.animatable, char_dad_anim);
	Animatable_Init(&this->character.animatable2, char_dad_anim2);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;
	
	this->character.focus_x = FIXED_DEC(-40 - -120,1);
	this->character.focus_y = FIXED_DEC(-87 - -20,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\DAD.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Dad_ArcMain_Idle0
		"idle1.tim", //Dad_ArcMain_Idle1
		"idle2.tim", //Dad_ArcMain_Idle1
		"left0.tim",  //Dad_ArcMain_Left
		"left1.tim",  //Dad_ArcMain_Left
		"down0.tim",  //Dad_ArcMain_Down
		"down1.tim",  //Dad_ArcMain_Down
		"up0.tim",    //Dad_ArcMain_Up
		"up1.tim",    //Dad_ArcMain_Up
		"right0.tim", //Dad_ArcMain_Right
		"right1.tim", //Dad_ArcMain_Right
		"spec0.tim", //Dad_ArcMain_Right
		"spec1.tim", //Dad_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
