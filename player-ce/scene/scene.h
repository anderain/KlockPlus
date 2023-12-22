#ifndef _SCENE_H_
#define _SCENE_H_

// Scene

typedef struct tagSCENE {
	void (* fnEnter)();
	void (* fnLeave)();
	void (* fnRedraw)(int delta);
	void (* fnUpdate)(int delta);
} SCENE;

extern SCENE* CurrentScene;

void Scene_To(SCENE *next);

// Scene List

extern SCENE Scene_Menu;
extern SCENE Scene_App;

#define SCENE_DEFAULT Scene_Menu

#endif