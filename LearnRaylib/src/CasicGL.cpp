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
		// 2.设置默认着色器
		LoadShaderDefault();
		m_CurrentShaderId = m_DefaultShaderId;
		m_CurrentShaderLocs = m_DefaultShaderLocs;
		// 3.设置默认render batch
		m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_NORMAL] = CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL;
		m_DefaultBatch = LoadRenderBatch(CASIC_DEFAULT_BATCH_BUFFERS, CASIC_DEFAULT_BATCH_BUFFER_ELEMENTS);
		m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_NORMAL] = -1;
		
		m_Transform = CasicMath::MatrixIdentity();
		m_Projection = CasicMath::MatrixIdentity();
		m_ModelView = CasicMath::MatrixIdentity();
		m_CurrentMatrix = &m_ModelView;

		// Initialize OpenGL default states
		//----------------------------------------------------------
		// Init state: Depth test
		glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
		glDisable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

		// Init state: Blending mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
		glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)
		// Init state: Culling
		// NOTE: All shapes/models triangles are drawn CCW
		glCullFace(GL_BACK);                                    // Cull the back face (default)
		glFrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
		glEnable(GL_CULL_FACE);                                 // Enable backface culling

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);                 // Seamless cubemaps (not supported on OpenGL ES 2.0)

		// Init state: Color/Depth buffers clear
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
		glClearDepth(1.0f);                                     // Set clear depth value (default)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)
	}

	void CasicGL::Shutdown()
	{
		UnloadRenderBatch(m_DefaultBatch);
		UnloadShaderDefault();

		glDeleteTextures(1, &m_DefaultTextureId);
		TRACELOG(LOG_INFO, "TEXTURE: [ID %i] Default texture unloaded successfully", m_DefaultTextureId);
		TRACELOG(LOG_INFO, "OPENGL: Opengl environment shutdown.");
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

	CasicGL::RenderBatch CasicGL::LoadRenderBatch(int numBuffers, int bufferElements)
	{
		RenderBatch batch{ 0 };
		// Initialize CPU (RAM) vertex buffers (position, texcoord, color data and indexes)
		//--------------------------------------------------------------------------------------------
		batch.vertexBuffer = (VertexBuffer*)malloc(numBuffers * sizeof(VertexBuffer));
		for (int i = 0; i < numBuffers; i++){
			batch.vertexBuffer[i].vertices = (float*)malloc(bufferElements * 4 * 3 * sizeof(float));
			batch.vertexBuffer[i].texCoord = (float*)malloc(bufferElements * 4 * 2 * sizeof(float));
			batch.vertexBuffer[i].normals = (float*)malloc(bufferElements * 4 * 3 * sizeof(float));
			batch.vertexBuffer[i].colors = (unsigned char*)malloc(bufferElements * 4 * 4 * sizeof(unsigned char));
			batch.vertexBuffer[i].indices = (unsigned int*)malloc(bufferElements * 6 * sizeof(unsigned int));
			
			for (int j = 0; j < bufferElements * 4 * 3; j++) {
				batch.vertexBuffer[i].vertices[j] = 0.0f;
			}
			for (int j = 0; j < bufferElements * 4 * 2; j++) {
				batch.vertexBuffer[i].texCoord[j] = 0.0f;
			}
			for (int j = 0; j < bufferElements * 4 * 3; j++) {
				batch.vertexBuffer[i].normals[j] = 0.0f;
			}
			for (int j = 0; j < bufferElements * 4 * 4; j++) {
				batch.vertexBuffer[i].colors[j] = 0;
			}
			int k = 0;
			for (int j = 0; j < bufferElements; j++) {
				batch.vertexBuffer[i].indices[j    ] = 4 * k;
				batch.vertexBuffer[i].indices[j + 1] = 4 * k + 1;
				batch.vertexBuffer[i].indices[j + 2] = 4 * k + 2;
				batch.vertexBuffer[i].indices[j + 3] = 4 * k;
				batch.vertexBuffer[i].indices[j + 4] = 4 * k + 2;
				batch.vertexBuffer[i].indices[j + 5] = 4 * k + 3;
				k++;
			}
			vertexCounter = 0;
		}

		TRACELOG(LOG_INFO, "RLGL: Render batch vertex buffers loaded successfully in RAM (CPU)");
		//--------------------------------------------------------------------------------------------

		// Upload to GPU (VRAM) vertex data and initialize VAOs/VBOs
		//--------------------------------------------------------------------------------------------
		for (int i = 0; i < numBuffers; i++) {
			glGenVertexArrays(1, &batch.vertexBuffer[i].vaoId);
			glBindVertexArray(batch.vertexBuffer[i].vaoId);

			// 方形四个顶点的位置
			glGenBuffers(1, &batch.vertexBuffer[i].vboId[0]);
			glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[0]);
			glBufferData(GL_ARRAY_BUFFER, bufferElements * 4 * 3 * sizeof(float), batch.vertexBuffer[i].vertices, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_POSITION]);
			glVertexAttribPointer(m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);

			glGenBuffers(1, &batch.vertexBuffer[i].vboId[1]);
			glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[1]);
			glBufferData(GL_ARRAY_BUFFER, bufferElements * 4 * 2 * sizeof(float), batch.vertexBuffer[i].texCoord, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_TEXCOORD01]);
			glVertexAttribPointer(m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);

			glGenBuffers(1, &batch.vertexBuffer[i].vboId[2]);
			glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[2]);
			glBufferData(GL_ARRAY_BUFFER, bufferElements * 4 * 3 * sizeof(float), batch.vertexBuffer[i].normals, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_NORMAL]);
			glVertexAttribPointer(m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_NORMAL], 3, GL_FLOAT, 0, 0, 0);

			glGenBuffers(1, &batch.vertexBuffer[i].vboId[3]);
			glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[3]);
			glBufferData(GL_ARRAY_BUFFER, bufferElements * 4 * 4 * sizeof(unsigned char), batch.vertexBuffer[i].colors, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_COLOR]);
			glVertexAttribPointer(m_CurrentShaderLocs[CASIC_SHADER_LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

			glGenBuffers(1, &batch.vertexBuffer[i].vboId[4]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[4]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferElements * 6 * sizeof(unsigned int), batch.vertexBuffer[i].indices, GL_STATIC_DRAW);
		}

		TRACELOG(LOG_INFO, "RLGL: Render batch vertex buffers loaded successfully in VRAM (GPU)");

		glBindVertexArray(0);

		// Init draw calls tracking system
		//--------------------------------------------------------------------------------------------
		batch.draws = (DrawCall*)malloc(CASIC_DEFAULT_BATCH_DRAWCALLS * sizeof(DrawCall));

		for (int i = 0; i < CASIC_DEFAULT_BATCH_DRAWCALLS; i++) {
			batch.draws[i].mode = CASIC_LINES;
			batch.draws[i].vertexCount = 0;
			batch.draws[i].vertexAlignment = 0;
			batch.draws[i].textureId = m_DefaultTextureId;
		}

		batch.bufferCount = numBuffers;
		batch.drawCounter = 1;
		batch.currentDepth = -1.0f;

		return batch;
	}

	void CasicGL::UnloadRenderBatch(RenderBatch batch)
	{
		// Unbind everything
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		for (int i = 0; i < batch.bufferCount; i++) {
			
			glBindVertexArray(batch.vertexBuffer[i].vaoId);
			glEnableVertexAttribArray(CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION);
			glEnableVertexAttribArray(CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD);
			glEnableVertexAttribArray(CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL);
			glEnableVertexAttribArray(CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR);
			glBindVertexArray(0);

			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[0]);
			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[1]);
			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[2]);
			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[3]);
			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[4]);

			glDeleteVertexArrays(1, &batch.vertexBuffer[i].vaoId);

			free(batch.vertexBuffer[i].vertices);
			free(batch.vertexBuffer[i].texCoord);
			free(batch.vertexBuffer[i].normals);
			free(batch.vertexBuffer[i].colors);
			free(batch.vertexBuffer[i].indices);

		}

		free(batch.vertexBuffer);
		free(batch.draws);
	}

	void CasicGL::ClearBackgound(Color color)
	{
		CasicClearColor(color.r, color.g, color.b, color.a);
		CasicClearScreenBuffers();
	}

	void CasicGL::BeginDrawing(Window* window)
	{
		window->time.current = window->GetTime();
		window->time.update = window->time.current - window->time.previous;
		window->time.previous = window->time.current;

		LoadIdentity();
	}

	void CasicGL::EndDrawing()
	{

	}

	void CasicGL::LoadIdentity()
	{
		*m_CurrentMatrix = CasicMath::MatrixIdentity();
	}

	void CasicGL::DrawRenderBatchActive()
	{
		DrawRenderBatch(m_CurrentBatch);
	}

	void CasicGL::DrawRenderBatch(RenderBatch* batch)
	{
		if (m_VertexCounter > 0) {
			glBindVertexArray(batch->vertexBuffer[batch->currentBuffer].vaoId);

			glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_VertexCounter * 3 * sizeof(float), batch->vertexBuffer[batch->currentBuffer].vertices);

			glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_VertexCounter * 2 * sizeof(float), batch->vertexBuffer[batch->currentBuffer].texCoord);

			glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[2]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_VertexCounter * 3 * sizeof(float), batch->vertexBuffer[batch->currentBuffer].normals);

			glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[3]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_VertexCounter * 4 * sizeof(unsigned char), batch->vertexBuffer[batch->currentBuffer].colors);

			glBindVertexArray(0);
		}

		// Draw batch vertex buffers
		//------------------------------------------------------------------------------------------------------------
		Matrix matProjection = m_Projection;
		Matrix matModelView = m_ModelView;

		int eyeCount = 1;
		for (int eye = 0; eye < eyeCount; eye++) {
			
			if (m_VertexCounter > 0) {
				// Set current shader and upload current MVP matrix
				glUseProgram(m_CurrentShaderId);

				Matrix matMVP = CasicMath::MatrixMultiply(m_ModelView, m_Projection);
				glUniformMatrix4fv(m_CurrentShaderLocs[CASIC_SHADER_LOC_MATRIX_MVP], 1, false, CasicMath::MatrixToFloatV(matMVP));
				
				if (m_CurrentShaderLocs[CASIC_SHADER_LOC_MATRIX_PROJECTION] != -1) {
					glUniformMatrix4fv(m_CurrentShaderLocs[CASIC_SHADER_LOC_MATRIX_PROJECTION], 1, false, CasicMath::MatrixToFloatV(m_Projection));
				}

				if (m_CurrentShaderLocs[CASIC_SHADER_LOC_MATRIX_VIEW] != -1) {
					glUniformMatrix4fv(m_CurrentShaderLocs[CASIC_SHADER_LOC_MATRIX_VIEW], 1, false, CasicMath::MatrixToFloatV(m_ModelView));
				}

				if (m_CurrentShaderLocs[CASIC_SHADER_LOC_MATRIX_MODEL] != -1) {
					glUniformMatrix4fv(m_CurrentShaderLocs[CASIC_SHADER_LOC_MATRIX_MODEL], 1, false, CasicMath::MatrixToFloatV(m_Transform));
				}

				if (m_CurrentShaderLocs[CASIC_SHADER_LOC_MATRIX_NORMAL] != -1) {
					glUniformMatrix4fv(m_CurrentShaderLocs[CASIC_SHADER_LOC_MATRIX_NORMAL], 1, false, CasicMath::MatrixToFloatV(CasicMath::MatrixTranspose(CasicMath::MatrixInvert(m_Transform))));
				}

				glBindVertexArray(batch->vertexBuffer[batch->currentBuffer].vaoId);

				glUniform4f(m_CurrentShaderLocs[CASIC_SHADER_LOC_COLOR_DIFFUSE], 1.0f, 1.0f, 1.0f, 1.0f);
				glUniform1i(m_CurrentShaderLocs[CASIC_SHADER_LOC_MAP_DIFFUSE], 0);

				for (int i = 0; i < CASIC_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++) {
					if (m_ActiveTextureId[i] > 0) {
						glActiveTexture(GL_TEXTURE0 + 1 + i);
						glBindTexture(GL_TEXTURE_2D, m_ActiveTextureId[i]);
					}
				}

				glActiveTexture(GL_TEXTURE0);

				for (int i = 0, vertexOffset = 0; i < batch->drawCounter; i++) {
					// Bind current draw call texture, activated as GL_TEXTURE0 and Bound to sampler2D texture0 by default
					glBindTexture(GL_TEXTURE_2D, batch->draws[i].textureId);
					if ((batch->draws[i].mode == CASIC_LINES) || (batch->draws[i].mode == CASIC_TRIANGLES)) glDrawArrays(batch->draws[i].mode, vertexOffset, batch->draws[i].vertexCount);
					else {
						// We need to define the number of indices to be processed: elementCount*6
						// NOTE: The final parameter tells the GPU the offset in bytes from the
						// start of the index buffer to the location of the first index to process
						glDrawElements(GL_TRIANGLES, batch->draws[i].vertexCount / 4 * 6, GL_UNSIGNED_INT, (GLvoid*)(vertexOffset / 4 * 6 * sizeof(GLuint)));
					}

					vertexOffset += (batch->draws[i].vertexCount + batch->draws[i].vertexAlignment);
				}

				glBindTexture(GL_TEXTURE_2D, 0);    // Unbind textures
			}
		
			glBindVertexArray(0);
			
			glUseProgram(0);
		}

		m_VertexCounter;
		batch->currentDepth = -1.0f;

		m_Projection = matProjection;
		m_ModelView = matModelView;

		// Reset RLGL.currentBatch->draws array
		for (int i = 0; i < CASIC_DEFAULT_BATCH_DRAWCALLS; i++){
			batch->draws[i].mode = CASIC_QUADS;
			batch->draws[i].vertexCount = 0;
			batch->draws[i].textureId = m_DefaultTextureId;
		}

		batch->drawCounter = 1;

		batch->currentBuffer++;
		if (batch->currentBuffer >= batch->bufferCount) batch->currentBuffer = 0;
	}

	void CasicGL::CasicClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		float cr = (float)r / 255;
		float cg = (float)g / 255;
		float cb = (float)b / 255;
		float ca = (float)a / 255;
		glClearColor(cr, cb, cg, ca);
	}

	void CasicGL::CasicClearScreenBuffers()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}



