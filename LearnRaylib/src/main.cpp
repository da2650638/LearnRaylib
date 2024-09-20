#include "WindowsWindow.h"
#include "CasicGL.h"
#include "Utils.h"

#include <iostream>

using namespace casic706;

int main() {
	Window* window = Window::GetWindow();
	if (!window->Init()) {
		std::cout << "´°¿Ú³õÊ¼»¯Ê§°Ü" << std::endl;
		return 1;
	}
	CasicGL casicGL;
	casicGL.LoadExtensions();
	casicGL.Init(Window::Data.width, Window::Data.height);

	const int screenWidth = Window::Data.width;
	const int screenHeight = Window::Data.height;
	while (!window->WindowShouldClose()) {
		casicGL.BeginDrawing(window);
		casicGL.ClearBackgound(CASIC_WHITE);

		casicGL.DrawTriangle3D( 
			{ screenWidth / 4.0f * 3.0f, 80.0f, 0.0f }, 
			{ screenWidth / 4.0f * 3.0f - 60.0f, 150.0f, 0.0f }, 
			{ screenWidth / 4.0f * 3.0f + 60.0f, 150.0f, 0.0f }, 
			CASIC_VIOLET);

		casicGL.EndDrawing(window);
	}

	casicGL.Shutdown();
	window->Shutdown();
}