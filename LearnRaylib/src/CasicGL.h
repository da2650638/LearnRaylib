#pragma once

#include <GLFW/glfw3.h>

#include "CasicGLConfig.h"

namespace casic706 {

	class CasicGL {
	public:
		CasicGL() = default;
		~CasicGL() = default;

		void LoadExtensions(void* loader = glfwGetProcAddress);
		void Init(int width, int height);

		// OpenGL init functions
		unsigned int LoadTextures(const void* data, int width, int height, int format, int mipmapcount);
        int GetPixelDataSize(int width, int height, int format);
        void GetTextureFormats(int format, unsigned int* glInternalFormat, unsigned int* glFormat, unsigned int* glType);
        const char* GetPixelFormatName(unsigned int format);
        void LoadShaderDefault();
        unsigned int CompileShader(const char* shaderCode, int type);
        unsigned int LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId);
        void UnloadShaderDefault();
	private:
		unsigned int m_DefaultTextureId;
		unsigned int m_CurrentShaderId;
        unsigned int m_DefaultShaderId;
        unsigned int m_DefaultVShaderId;
        unsigned int m_DefaultFShaderId;
        int* m_DefaultShaderLocs;
		int* m_CurrentShaderLocs;
	};

}