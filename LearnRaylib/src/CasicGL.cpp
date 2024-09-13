#include "CasicGL.h"

#include "Utils.h"

#include <glad/glad.h>

namespace casic706 {
	void CasicGL::LoadExtensions(void* loader)
	{
		if (gladLoadGLLoader(GLADloadproc(loader)) == 0) TRACELOG(LOG_WARNING, "GLAD: Cannot load OpenGL extensions.");
		else TRACELOG(LOG_INFO, "GLAD: OpenGL extensions load succsssfully.");
		// Check OpenGL information and capabilities
		// Show current OpenGL and GLSL version
		TRACELOG(LOG_INFO, "GL: OpenGL Device Information");
		TRACELOG(LOG_INFO, "	> Vendor: %s", glGetString(GL_VENDOR));
		TRACELOG(LOG_INFO, "	> Render: %s", glGetString(GL_RENDERER));
		TRACELOG(LOG_INFO, "	> Version: %s", glGetString(GL_VERSION));
		TRACELOG(LOG_INFO, "	> GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	void CasicGL::Init(int width, int height)
	{
		// 1.设置默认纹理
		static const char pixels[4] = { 255,255,255,255 };
		m_DefaultTextureId = LoadTextures(pixels, 1, 1, CASIC_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

		if (m_DefaultTextureId != 0)TRACELOG(LOG_INFO, "TEXTURE: [ID %i] Default texture loaded successfully.", m_DefaultTextureId);
		else TRACELOG(LOG_WARNING, "TEXTURE: Failed to load default texture.");
		
		LoadShaderDefault();
		m_CurrentShaderId = m_DefaultShaderId;
		m_CurrentShaderLocs = m_DefaultShaderLocs;

		// 3.设置默认纹理位置
		// 
		// 4.设置默认defaultBatch

	}

	unsigned int CasicGL::LoadTextures(const void* data, int width, int height, int format, int mipmapcount)
	{
		// TODO: 现在系统只关注R8G8B8A8格式的图像，其他格式的图像一概不关心，后边再深度了解。
		GLint openglMaxTextureUnit;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &openglMaxTextureUnit);
		TRACELOG(LOG_INFO, " Current OpenGL support %d texture units", openglMaxTextureUnit);

		unsigned int id = 0;
		glBindTexture(GL_TEXTURE_2D, 0);
		//将像素存储的对齐方式设置为 1 字节对齐，确保每一行图像数据在内存中紧密排列，没有任何填充字节。
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		int mipWidth = width;
		int mipHeight = height;
		int mipOffset = 0;	// Mipmap data offset, only used for tracelog（没什么实际意义，纯粹为了打印输出）

		unsigned char* dataPtr = NULL;
		if (data != NULL) dataPtr = (unsigned char*)data;

		for (int i = 0; i < mipmapcount; i++) {
			unsigned int mipSize = GetPixelDataSize(mipWidth, mipHeight, format);

			unsigned int glInternalFormat, glFormat, glType;
			GetTextureFormats(format, &glInternalFormat, &glFormat, &glType);

			TRACELOGD("TEXTURE: Load mipmap level %i (%i x %i), size: %i, offset: %i", i, mipWidth, mipHeight, mipSize, mipOffset);

			if (glInternalFormat != 0) {
				glTexImage2D(GL_TEXTURE_2D, i, glInternalFormat, mipWidth, mipHeight, 0, glFormat, glType, dataPtr);
			}

			mipWidth /= 2;
			mipHeight /= 2;
			mipOffset += mipSize;
			if (data != NULL) dataPtr += mipSize;

			if (mipWidth < 1) mipWidth = 1;
			if (mipHeight < 1) mipHeight = 1;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		if (mipmapcount > 1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		if (id > 0) TRACELOG(LOG_INFO, "TEXTURE: [ID %i] Texture loaded successfully (%ix%i | %s | %i mipmaps)", id, width, height, GetPixelFormatName(format), mipmapcount);
		else TRACELOG(LOG_WARNING, "TEXTURE: Failed to load texture");

		return id;
	}

	int CasicGL::GetPixelDataSize(int width, int height, int format)
	{
		int dataSize = 0;
		int bpp = 0;
		switch (format) {
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: bpp = 32; break;
		default: break;
		}

		double bytesPerPixel = (double)bpp / 8.0;
		dataSize = (int)(bytesPerPixel * width * height);

		return dataSize;
	}

	void CasicGL::GetTextureFormats(int format, unsigned int* glInternalFormat, unsigned int* glFormat, unsigned int* glType)
	{
		*glInternalFormat = 0;
		*glFormat = 0;
		*glType = 0;

		switch (format) {
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: *glInternalFormat = GL_RGBA8; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
		default: break;
		}
	}

	const char* CasicGL::GetPixelFormatName(unsigned int format)
	{
		switch (format)
		{
		case CASIC_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: return "GRAYSCALE"; break;         // 8 bit per pixel (no alpha)
		case CASIC_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: return "GRAY_ALPHA"; break;       // 8*2 bpp (2 channels)
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R5G6B5: return "R5G6B5"; break;               // 16 bpp
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R8G8B8: return "R8G8B8"; break;               // 24 bpp
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1: return "R5G5B5A1"; break;           // 16 bpp (1 bit alpha)
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: return "R4G4B4A4"; break;           // 16 bpp (4 bit alpha)
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: return "R8G8B8A8"; break;           // 32 bpp
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R32: return "R32"; break;                     // 32 bpp (1 channel - float)
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R32G32B32: return "R32G32B32"; break;         // 32*3 bpp (3 channels - float)
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: return "R32G32B32A32"; break;   // 32*4 bpp (4 channels - float)
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R16: return "R16"; break;                     // 16 bpp (1 channel - half float)
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R16G16B16: return "R16G16B16"; break;         // 16*3 bpp (3 channels - half float)
		case CASIC_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16: return "R16G16B16A16"; break;   // 16*4 bpp (4 channels - half float)
		case CASIC_PIXELFORMAT_COMPRESSED_DXT1_RGB: return "DXT1_RGB"; break;             // 4 bpp (no alpha)
		case CASIC_PIXELFORMAT_COMPRESSED_DXT1_RGBA: return "DXT1_RGBA"; break;           // 4 bpp (1 bit alpha)
		case CASIC_PIXELFORMAT_COMPRESSED_DXT3_RGBA: return "DXT3_RGBA"; break;           // 8 bpp
		case CASIC_PIXELFORMAT_COMPRESSED_DXT5_RGBA: return "DXT5_RGBA"; break;           // 8 bpp
		case CASIC_PIXELFORMAT_COMPRESSED_ETC1_RGB: return "ETC1_RGB"; break;             // 4 bpp
		case CASIC_PIXELFORMAT_COMPRESSED_ETC2_RGB: return "ETC2_RGB"; break;             // 4 bpp
		case CASIC_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA: return "ETC2_RGBA"; break;       // 8 bpp
		case CASIC_PIXELFORMAT_COMPRESSED_PVRT_RGB: return "PVRT_RGB"; break;             // 4 bpp
		case CASIC_PIXELFORMAT_COMPRESSED_PVRT_RGBA: return "PVRT_RGBA"; break;           // 4 bpp
		case CASIC_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA: return "ASTC_4x4_RGBA"; break;   // 8 bpp
		case CASIC_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA: return "ASTC_8x8_RGBA"; break;   // 2 bpp
		default: return "UNKNOWN"; break;
		}
	}

	void CasicGL::LoadShaderDefault()
	{
		m_DefaultShaderLocs = (int*)calloc(CASIC_MAX_SHADER_LOCATIONS, sizeof(int));
		for (int i = 0; i < CASIC_MAX_SHADER_LOCATIONS; i++) {
			m_DefaultShaderLocs[i] = -1;
		}

		const char* defaultVShaderCode =
			"#version 460										\n"
			"in vec3 vertexPostion;								\n"
			"in vec2 vertexTexCoord;							\n"
			"in vec4 vertexColor;								\n"
			"out vec2 fragTexCoord;								\n"
			"out vec4 fragColor;								\n"
			"uniform mat4 mvp;									\n"
			"void main()										\n"
			"{													\n"
			"	fragTexCoord = vertexTexCoord;					\n"
			"	fragColor = vertexColor;						\n"
			"	gl_Position = mvp * vec4(vertexPostion, 1.0);	\n"
			"}													\n";
		const char* defaultFShaderCode =
			"#version 460											\n"
			"in vec2 fragTexCoord;									\n"
			"in vec4 fragColor;										\n"
			"out vec4 finalColor;									\n"
			"uniform sampler2D texture0;							\n"
			"uniform vec4 colDiffuse;								\n"
			"void main()											\n"
			"{														\n"
			"	vec4 texelColor = texture(texture0, fragTexCoord);	\n"
			"	finalColor = texelColor * colDiffuse * fragColor;	\n"
			"}														\n";

		m_DefaultVShaderId = CompileShader(defaultVShaderCode, GL_VERTEX_SHADER);
		m_DefaultFShaderId = CompileShader(defaultFShaderCode, GL_FRAGMENT_SHADER);
		m_DefaultShaderId = LoadShaderProgram(m_DefaultVShaderId, m_DefaultFShaderId);

		if (m_DefaultShaderId > 0) {
			TRACELOG(LOG_INFO, "SHADER: [ID %i] Default shader loaded successfully", m_DefaultShaderId);
			m_DefaultShaderLocs[CASIC_SHADER_LOC_VERTEX_POSITION] = glGetAttribLocation(m_DefaultShaderId, CASIC_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
			m_DefaultShaderLocs[CASIC_SHADER_LOC_VERTEX_TEXCOORD01] = glGetAttribLocation(m_DefaultShaderId, CASIC_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
			m_DefaultShaderLocs[CASIC_SHADER_LOC_VERTEX_COLOR] = glGetAttribLocation(m_DefaultShaderId, CASIC_DEFAULT_SHADER_ATTRIB_NAME_COLOR);

			// Set default shader locations: uniform locations
			m_DefaultShaderLocs[CASIC_SHADER_LOC_MATRIX_MVP] = glGetUniformLocation(m_DefaultShaderId, CASIC_DEFAULT_SHADER_UNIFORM_NAME_MVP);
			m_DefaultShaderLocs[CASIC_SHADER_LOC_COLOR_DIFFUSE] = glGetUniformLocation(m_DefaultShaderId, CASIC_DEFAULT_SHADER_UNIFORM_NAME_COLOR);
			m_DefaultShaderLocs[CASIC_SHADER_LOC_MAP_DIFFUSE] = glGetUniformLocation(m_DefaultShaderId, CASIC_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0);
		}
		else {
			TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to load default shader", m_DefaultShaderId);
		}
	}

	unsigned int CasicGL::CompileShader(const char* shaderCode, int type)
	{
		unsigned int shader;
		shader = glCreateShader(type);
		glShaderSource(shader, 1, &shaderCode, NULL);

		GLint success = 0;
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE) {
			switch (type) {
				case GL_VERTEX_SHADER: TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to compile vertex shader code", shader); break;
				case GL_FRAGMENT_SHADER: TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to compile fragment shader code", shader); break;
				case GL_GEOMETRY_SHADER: TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to compile geometry shader code", shader); break;
				case GL_COMPUTE_SHADER: TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to compile compute shader code", shader); break;
				default: break;
			}

			int maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			if (maxLength > 0) {
				int length;
				char* log = (char*)calloc(maxLength + 1, sizeof(char));
				glGetShaderInfoLog(shader, maxLength + 1, &length, log);
				TRACELOG(LOG_WARNING, "SHADER: [ID %i] Compile error: %s", shader, log);
				free(log);
			}

			shader = 0;
		}
		else {
			switch (type) {
				case GL_VERTEX_SHADER: TRACELOG(LOG_INFO, "SHADER: [ID %i] vertex shader code compiled successfully", shader); break;
				case GL_FRAGMENT_SHADER: TRACELOG(LOG_INFO, "SHADER: [ID %i] fragment shader code successfully", shader); break;
				case GL_GEOMETRY_SHADER: TRACELOG(LOG_INFO, "SHADER: [ID %i] geometry shader code successfully", shader); break;
				case GL_COMPUTE_SHADER: TRACELOG(LOG_INFO, "SHADER: [ID %i] compute shader code successfully", shader); break;
				default: break;
			}
		}

		return shader;
	}

	unsigned int CasicGL::LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId)
	{
		unsigned int program;
		program = glCreateProgram();

		glAttachShader(program, vShaderId);
		glAttachShader(program, fShaderId);

		glBindAttribLocation(program, CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION, CASIC_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
		glBindAttribLocation(program, CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD, CASIC_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
		glBindAttribLocation(program, CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL, CASIC_DEFAULT_SHADER_ATTRIB_NAME_NORMAL);
		glBindAttribLocation(program, CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR, CASIC_DEFAULT_SHADER_ATTRIB_NAME_COLOR);
		glBindAttribLocation(program, CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD, CASIC_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
		glBindAttribLocation(program, CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2, CASIC_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2);

		glLinkProgram(program);

		GLint success = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (success == GL_FALSE) {
			TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to link shader program", program);

			int maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
			if (maxLength > 0) {
				char* log = (char*)calloc(maxLength + 1, sizeof(char));
				int length;
				glGetProgramInfoLog(program, maxLength + 1, &length, log);
				TRACELOG(LOG_WARNING, "SHADER: [ID %i] Link error: %s", program, log);
				free(log);
			}

			glDeleteProgram(program);
			program = 0;
		}
		else {
			TRACELOG(LOG_INFO, "SHADER: [ID %i] Program shader loaded successfully", program);
		}

		return program;
	}

	void CasicGL::UnloadShaderDefault()
	{
		glUseProgram(0);

		glDetachShader(m_DefaultShaderId, m_DefaultVShaderId);
		glDetachShader(m_DefaultShaderId, m_DefaultFShaderId);
		glDeleteShader(m_DefaultVShaderId);
		glDeleteShader(m_DefaultFShaderId);

		glDeleteProgram(m_DefaultShaderId);

		free(m_DefaultShaderLocs);

		TRACELOG(LOG_INFO, "SHADER: [ID %i] Default shader unloaded successfully", m_DefaultShaderId);
	}
}



