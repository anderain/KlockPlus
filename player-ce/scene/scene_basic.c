#include "scene.h"

SCENE* CurrentScene = 0;

void Scene_To(SCENE *next) {
	if (CurrentScene) {
		CurrentScene->fnLeave();
	}

	CurrentScene = next;

	if (CurrentScene) {
		CurrentScene->fnEnter();
	}
}

