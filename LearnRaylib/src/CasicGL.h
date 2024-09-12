#pragma once

#include <GLFW/glfw3.h>

namespace casic706 {

	typedef enum {
        CASIC_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1,     // 8 bit per pixel (no alpha) �Ҷ�ͼ��
        CASIC_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,        // 8*2 bpp (2 channels) 8λ�ҶȺ�8λ͸����
        CASIC_PIXELFORMAT_UNCOMPRESSED_R5G6B5,            // 16 bpp 5λ��ɫ 6λ��ɫ 5λ��ɫ
        CASIC_PIXELFORMAT_UNCOMPRESSED_R8G8B8,            // 24 bpp 8λ��ɫ��8λ��ɫ��8λ��ɫ
        CASIC_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,          // 16 bpp (1 bit alpha) 5λ��ɫ��5λ��ɫ��5λ��ɫ��1λ alpha��͸���ȣ�
        CASIC_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,          // 16 bpp (4 bit alpha) 4λ��ɫ��4λ��ɫ��4λ��ɫ��4λ alpha��͸���ȣ�
        CASIC_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,          // 32 bpp 8λ��ɫ��8λ��ɫ��8λ��ɫ��8λ alpha
        CASIC_PIXELFORMAT_UNCOMPRESSED_R32,               // 32 bpp (1 channel - float) ֻ��һ��������ͨ��
        CASIC_PIXELFORMAT_UNCOMPRESSED_R32G32B32,         // 32*3 bpp (3 channels - float) ÿ��ͨ����R, G, B����Ϊ 32 λ������
        CASIC_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,      // 32*4 bpp (4 channels - float) ÿ��ͨ����R, G, B, A����Ϊ32λ������
        CASIC_PIXELFORMAT_UNCOMPRESSED_R16,               // 16 bpp (1 channel - half float) һ���뾫�ȸ�����ͨ��
        CASIC_PIXELFORMAT_UNCOMPRESSED_R16G16B16,         // 16*3 bpp (3 channels - half float) 3���뾫�ȸ�����ͨ����
        CASIC_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,      // 16*4 bpp (4 channels - half float) 4���뾫�ȸ�����ͨ��
        CASIC_PIXELFORMAT_COMPRESSED_DXT1_RGB,            // 4 bpp (no alpha) ʹ�� DXT1 ѹ������֧�� alpha ͨ��
        CASIC_PIXELFORMAT_COMPRESSED_DXT1_RGBA,           // 4 bpp (1 bit alpha) DXT1 ѹ����֧�� 1-bit alpha
        CASIC_PIXELFORMAT_COMPRESSED_DXT3_RGBA,           // 8 bpp DXT3 ѹ����֧�� 4-bit alpha
        CASIC_PIXELFORMAT_COMPRESSED_DXT5_RGBA,           // 8 bpp DXT5 ѹ����֧�ֽ��� alpha ͨ��
        CASIC_PIXELFORMAT_COMPRESSED_ETC1_RGB,            // 4 bpp ETC1 ѹ������֧�� alpha ͨ����
        CASIC_PIXELFORMAT_COMPRESSED_ETC2_RGB,            // 4 bpp ETC2 ѹ������֧�� alpha ͨ��
        CASIC_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,       // 8 bpp ETC2 ѹ����֧�� 8-bit alpha������ζ�� alpha ͨ����ԭʼ���� �� 8 λ �ģ�����ѹ�������У���� 8 λ alpha ���ݻᱻ��һ��ѹ����ͨ�� EAC��Enhanced Alpha Compression���㷨��alpha ͨ�������ڴ洢ʱ��ѹ�������ٵ�λ����ͨ���� 4 λ�����Ӷ�ʹ������������ѹ����ÿ������ֻռ�� 1 �ֽڣ����� RGB �� alpha����
        CASIC_PIXELFORMAT_COMPRESSED_PVRT_RGB,            // 4 bpp PVRTC ѹ������֧�� alpha ͨ����
        CASIC_PIXELFORMAT_COMPRESSED_PVRT_RGBA,           // 4 bpp PVRTC ѹ����֧�� alpha ͨ����
        CASIC_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,       // 8 bpp ASTC 4x4 ѹ����֧�� RGBA��
        CASIC_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA        // 2 bpp ASTC 8x8 ѹ����֧�� RGBA��
	} CasicPixelFormat;

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
	private:
		unsigned int m_DefaultTextureID;
		unsigned int m_CurrentShaderID;
		int* m_CurrentShaderLocs;
	};

}