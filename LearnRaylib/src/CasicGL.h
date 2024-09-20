#pragma once

#include <GLFW/glfw3.h>

#include "Window.h"

#include "CasicGLConfig.h"
#include "CasicMath.h"

namespace casic706 {

	class CasicGL {
	public:
		typedef struct {
			int elementCount;
			float* vertices;	//positions��3������������һ��position��ÿһ��vertex element��4��positions
			float* texCoord;	//texture coordinates��2������������һ��texture coordinate��ÿһ��vertex element��4��texture coordiantes
			float* normals;		//normals��3������������һ��normal��ÿһ��vertex element��4��normals
			unsigned char* colors;	//colors, 4���޷����ֽڴ���һ��color��ÿһ��vertex element��4��colors
			unsigned int* indices;	//indices���ڵ�������ÿһ��vertex element��6��indices
			unsigned int vaoId;
			unsigned int vboId[5];	//positions, texture coordinates, normals, colors, indices
		}VertexBuffer;
		typedef struct {
			int mode;			// LINES, TRIANGLES or QUADS
			int vertexCount;	// ��Ҫ����vertex elements�ĸ��������ɣ�
			int vertexAlignment;// ���뷽ʽ�����LINES����TRIANGLES
			unsigned int textureId;
		}DrawCall;
		typedef struct {
			int bufferCount;            // Number of vertex buffers (multi-buffering support)
			int currentBuffer;          // Current buffer tracking in case of multi-buffering
			VertexBuffer* vertexBuffer;	// Dynamic buffer(s) for vertex data
			DrawCall* draws;			// Draw calls array, depends on textureId
			int drawCounter;
			float currentDepth;			// Current depth value for next draw
		}RenderBatch;
	

		CasicGL() = default;
		~CasicGL() = default;

		void LoadExtensions(void* loader = glfwGetProcAddress);
		void Init(int width, int height);
		void Shutdown();

		// OpenGL init functions
		unsigned int LoadTextures(const void* data, int width, int height, int format, int mipmapcount);
        int GetPixelDataSize(int width, int height, int format);
        void GetTextureFormats(int format, unsigned int* glInternalFormat, unsigned int* glFormat, unsigned int* glType);
        const char* GetPixelFormatName(unsigned int format);
        void LoadShaderDefault();
        unsigned int CompileShader(const char* shaderCode, int type);
        unsigned int LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId);
        void UnloadShaderDefault();
		RenderBatch LoadRenderBatch(int numBuffers, int bufferElements);
		void UnloadRenderBatch(RenderBatch batch);

		// Drawing-releated functions
		void ClearBackgound(Color color);
		void BeginDrawing(Window* window);
		void EndDrawing(Window* window);
		void LoadIdentity();
		void DrawRenderBatchActive();
		void DrawRenderBatch(RenderBatch* batch);
		void BeginDrawMode(int mode);
		void EndDrawMode();
		bool CheckRenderBatchLimit(int count);

		// OpenGL function wrappers
		void CasicClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		void CasicClearScreenBuffers();

		// Vertex level operations
		void Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		void Vertex3f(float x, float y, float z);

		// Basic geometric 3D shapes drawing functions
		void DrawTriangle3D(Vector3 v1, Vector3 v2, Vector3 v3, Color color);
	private:
		unsigned int m_DefaultTextureId;
		unsigned int m_ActiveTextureId[CASIC_DEFAULT_BATCH_MAX_TEXTURE_UNITS] = { 0 };
		unsigned int m_CurrentShaderId;
        unsigned int m_DefaultShaderId;
        unsigned int m_DefaultVShaderId;
        unsigned int m_DefaultFShaderId;
        int* m_DefaultShaderLocs;
		int* m_CurrentShaderLocs;

		Color m_Color;
		float m_Texcoordx, m_Texcoordy;
		float m_Normalx, m_Normaly, m_Normalz;
		int m_VertexCounter;                  // Current active render batch vertex counter (generic, used for all batches)
		RenderBatch		m_DefaultBatch;
		RenderBatch*	m_CurrentBatch;

		Matrix* m_CurrentMatrix;	// Current matrix pointer
		Matrix  m_Transform;		// Transform matrix to be used with rlTranslate, rlRotate, rlScale
		Matrix  m_ModelView;		// Default modelview matrix
		Matrix  m_Projection;		// Default projection matrix
	};

}