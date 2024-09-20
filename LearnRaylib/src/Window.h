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
		typedef struct {
			double current;                     // Current time measure
			double previous;                    // Previous time measure
			double update;                      // Time measure for frame update
			double draw;                        // Time measure for frame draw
			double frame;                       // Time measure for one frame
			double target;                      // Desired time for one frame, if 0 not applied
			unsigned long long int base;        // Base time measure for hi-res timer (PLATFORM_ANDROID, PLATFORM_DRM)
			unsigned int frameCounter;          // Frame counter
		}Time;
		static WindowData Data;
		static Time time;

		virtual bool Init() = 0;
		virtual bool WindowShouldClose() = 0;
		virtual void SwapBufferAndPollEvent() = 0;
		virtual void SwapBuffer() = 0;
		virtual void PollEvent() = 0;
		virtual void Shutdown() = 0;
		virtual double GetTime() = 0;

		static Window* GetWindow(const WindowData& data = {"Test", 1280, 720});
	};
}