/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week2.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"

//week 2 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;

} Back_week2;

void Back_week2_Free(StageBack *back)
{
	Back_week2 *this = (Back_week2*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_week2_New(void)
{
	//Allocate background structure
	Back_week2 *this = (Back_week2*)Mem_Alloc(sizeof(Back_week2));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = NULL;
	this->back.free = Back_week2_Free;
	
	Gfx_SetClear(0, 0, 0);
	
	return (StageBack*)this;
}
