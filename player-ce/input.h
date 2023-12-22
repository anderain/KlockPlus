#ifndef _INPUT_H_
#define _INPUT_H_

#define INPUT_BUTTON_UP			0
#define INPUT_BUTTON_DOWN		1
#define INPUT_BUTTON_LEFT		2
#define INPUT_BUTTON_RIGHT		3
#define INPUT_BUTTON_A			4
#define INPUT_BUTTON_B			5
#define INPUT_BUTTON_EXIT		6

// On screen Pad
void	Input_ClearAllState			();
int 	Input_OnScreen_GetVisible	();
void	Input_OnScreen_SetVisible	();
void	Input_OnScreen_Draw			();
void	Input_OnScreen_Detect		(int x, int y);

#define Input_IsKeyDown(kv)			(Input_KeyState[(kv)])
#define Input_KeyStateClear(kv)		(Input_KeyState[(kv)] = 0)
#define Input_SetKeyDown(kv)		(Input_KeyState[(kv)] = 1)

extern int Input_KeyState[];

#endif