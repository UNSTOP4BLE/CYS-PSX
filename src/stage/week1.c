/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week1.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"

int bgx = 69;
int buildx = 1199;
int build2x = 1199 * 1.5;

#include "../pad.h"

//Week 1 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Stage and back
	Gfx_Tex tex_back1; //Stage and back
	Gfx_Tex tex_grass0; //Stage and back
	Gfx_Tex tex_grass1; //Stage and back

	Gfx_Tex tex_build; //Stage and back

} Back_Week1;

//Week 1 background functions
void Back_Week1_DrawBG(StageBack *back)
{
	Back_Week1 *this = (Back_Week1*)back;
	
	fixed_t fx, fy;

	//Draw bg
	fx = stage.camera.x;
	fy = stage.camera.y;

	RECT bg_src = {0, 0, 256, 256};

	RECT_FIXED bg0_dst = {
		FIXED_DEC(bgx,1) - fx,
		FIXED_DEC(-229,1) - fy,
		FIXED_DEC(522,1),
		FIXED_DEC(285,1)
	};
	RECT_FIXED bg1_dst = {
		FIXED_DEC(bgx - 519,1) - fx,
		FIXED_DEC(-229,1) - fy,
		FIXED_DEC(522,1),
		FIXED_DEC(285,1)
	};
	//draw buildings
	RECT b_src = {0, 0, 124, 117};
	RECT_FIXED b_dst = {
		FIXED_DEC(buildx,1) - fx,
		FIXED_DEC(-160,1) - fy,
		FIXED_DEC(124,1),
		FIXED_DEC(117,1)
	};

	RECT b2_src = {124, 0, 52, 162};
	RECT_FIXED b2_dst = {
		FIXED_DEC(build2x,1) - fx,
		FIXED_DEC(-182,1) - fy,
		FIXED_DEC(52,1),
		FIXED_DEC(162,1)
	};

	Stage_DrawTex(&this->tex_grass1, &bg_src, &bg0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_grass0, &bg_src, &bg1_dst, stage.camera.bzoom);

	Stage_DrawTex(&this->tex_build, &b_src, &b_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_build, &b2_src, &b2_dst, stage.camera.bzoom);

	Stage_DrawTex(&this->tex_back1, &bg_src, &bg0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &bg_src, &bg1_dst, stage.camera.bzoom);
	

	//move da funni
	bgx -= 10;
	buildx -= 10;
	build2x -= 10;

	if (bgx <= -50)
		bgx = 69;
	if (buildx <= -570)
		buildx = 1199;
	if (build2x <= -570 * 1.5)
		build2x = 1199 * 1.5;
}

void Back_Week1_Free(StageBack *back)
{
	Back_Week1 *this = (Back_Week1*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week1_New(void)
{
	//Allocate background structure
	Back_Week1 *this = (Back_Week1*)Mem_Alloc(sizeof(Back_Week1));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week1_DrawBG;
	this->back.free = Back_Week1_Free;
	
	Gfx_SetClear(134, 108, 16);
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK1\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_grass0, Archive_Find(arc_back, "grass0.tim"), 0);
	Gfx_LoadTex(&this->tex_grass1, Archive_Find(arc_back, "grass1.tim"), 0);
	Gfx_LoadTex(&this->tex_build, Archive_Find(arc_back, "build.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
