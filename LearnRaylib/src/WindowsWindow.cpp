#include "WindowsWindow.h"

namespace casic706 {
	WindowsWindow::WindowsWindow(const WindowData& data)
		: m_Window(nullptr)
	{
		m_Data = data;
	}

	WindowsWindow::~WindowsWindow()
	{

	}

	bool WindowsWindow::Init()
	{
		glfwSetErrorCallback(WindowsWindow::ErrorCallback);

		int result = glfwInit();
		if (result == GLFW_FALSE) {
			//TODO: ��������Ӧ��ֱ��ֹͣ����
			return false;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// Enable opengl debug mode
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

		m_Window = glfwCreateWindow(m_Data.width, m_Data.height, m_Data.title, NULL, NULL);

		if (m_Window == nullptr) {
			//TODO: ��������Ӧ��ֱ��ֹͣ����
			glfwTerminate();
			return false;
		}

		//TODO: Ӧ�����һ��glfwMakeContextCurrent�Ƿ��з��ش���
		glfwMakeContextCurrent(m_Window);
		glfwSwapInterval(0);        // No V-Sync by default

		//TODO: ���ø����¼��ص�����
		return true;
	}

	bool WindowsWindow::WindowShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}

	void WindowsWindow::SwapBufferAndPollEvent()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void WindowsWindow::SwapBuffer()
	{
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::PollEvent()
	{
		glfwPollEvents();
		
		glfwSetWindowShouldClose(m_Window, GLFW_FALSE);
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	double WindowsWindow::GetTime()
	{
		double time = glfwGetTime();
		return time;
	}


}
