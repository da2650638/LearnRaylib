#pragma once

#include "Window.h"

#include <GLFW/glfw3.h>

#include <iostream>

namespace casic706 {
	class WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowData& data);
		~WindowsWindow();

		virtual bool Init() override;
		virtual bool WindowShouldClose() override;
		virtual void SwapBufferAndPollEvent() override;
		virtual void Shutdown() override;

		static void ErrorCallback(int error, const char* description)
		{
			std::cout << "GLFW: Error: " << error << " Description: " << description << std::endl;
		}
	private:
		GLFWwindow* m_Window;
		WindowData m_Data;
	};
}