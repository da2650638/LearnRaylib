#include "WindowsWindow.h"
#include "CasicGL.h"
#include "Utils.h"

#include <iostream>

using namespace casic706;

int main() {
	Window* window = Window::GetWindow();
	if (!window->Init()) {
		std::cout << "���ڳ�ʼ��ʧ��" << std::endl;
		return 1;
	}
	CasicGL casicGL;
	casicGL.LoadExtensions();
	casicGL.Init(Window::Data.width, Window::Data.height);

	while (!window->WindowShouldClose()) {
		//Swap buffers and poll events
		window->SwapBufferAndPollEvent();
	}

	window->Shutdown();
}