#include "Window.h"

#include "WindowsWindow.h"

namespace casic706 {

	Window::WindowData Window::Data{};

	Window* Window::GetWindow(const WindowData& data)
	{
		static std::once_flag m_CreateWindowFlag;
		static std::unique_ptr<Window> window;
#if defined(_WIN32)
		std::call_once(m_CreateWindowFlag, [data]() { Data = data; window = std::make_unique<WindowsWindow>(data); });
#elif defined(_linux_)
		std::call_once(m_CreateWindowFlag, [data]() { Data = data; window = std::make_unique<LinuxWindow>(data); });
#endif
		return window.get();
	}
}
