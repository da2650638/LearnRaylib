#pragma once

#include <mutex>
#include <thread>

namespace casic706 {
	class Window {
	public:
		struct WindowData {
			const char* title;
			int width;
			int height;
		};
		static WindowData Data;

		virtual bool Init() = 0;
		virtual bool WindowShouldClose() = 0;
		virtual void SwapBufferAndPollEvent() = 0;;
		virtual void Shutdown() = 0;

		static Window* GetWindow(const WindowData& data = {"Test", 1280, 720});
	};
}