#include "raylib.h"

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1920, 1080, "Hello Raylib");
	SetExitKey(KEY_NULL);
	
	// main loop
	while (!WindowShouldClose())
	{
		// drawing
		BeginDrawing();
			ClearBackground(BLACK);

			DrawText("Hello Raylib", 200,200,20,WHITE);

		EndDrawing();
	}
	
	CloseWindow();
	return 0;
}
