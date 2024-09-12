#pragma once

#include <GLFW/glfw3.h>

namespace casic706 {

	typedef enum {
        CASIC_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1,     // 8 bit per pixel (no alpha) 灰度图像
        CASIC_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,        // 8*2 bpp (2 channels) 8位灰度和8位透明度
        CASIC_PIXELFORMAT_UNCOMPRESSED_R5G6B5,            // 16 bpp 5位红色 6位绿色 5位蓝色
        CASIC_PIXELFORMAT_UNCOMPRESSED_R8G8B8,            // 24 bpp 8位红色，8位绿色，8位蓝色
        CASIC_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,          // 16 bpp (1 bit alpha) 5位红色，5位绿色，5位蓝色，1位 alpha（透明度）
        CASIC_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,          // 16 bpp (4 bit alpha) 4位红色，4位绿色，4位蓝色，4位 alpha（透明度）
        CASIC_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,          // 32 bpp 8位红色，8位绿色，8位蓝色，8位 alpha
        CASIC_PIXELFORMAT_UNCOMPRESSED_R32,               // 32 bpp (1 channel - float) 只有一个浮点数通道
        CASIC_PIXELFORMAT_UNCOMPRESSED_R32G32B32,         // 32*3 bpp (3 channels - float) 每个通道（R, G, B）都为 32 位浮点数
        CASIC_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,      // 32*4 bpp (4 channels - float) 每个通道（R, G, B, A）都为32位浮点数
        CASIC_PIXELFORMAT_UNCOMPRESSED_R16,               // 16 bpp (1 channel - half float) 一个半精度浮点数通道
        CASIC_PIXELFORMAT_UNCOMPRESSED_R16G16B16,         // 16*3 bpp (3 channels - half float) 3个半精度浮点数通道。
        CASIC_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,      // 16*4 bpp (4 channels - half float) 4个半精度浮点数通道
        CASIC_PIXELFORMAT_COMPRESSED_DXT1_RGB,            // 4 bpp (no alpha) 使用 DXT1 压缩，不支持 alpha 通道
        CASIC_PIXELFORMAT_COMPRESSED_DXT1_RGBA,           // 4 bpp (1 bit alpha) DXT1 压缩，支持 1-bit alpha
        CASIC_PIXELFORMAT_COMPRESSED_DXT3_RGBA,           // 8 bpp DXT3 压缩，支持 4-bit alpha
        CASIC_PIXELFORMAT_COMPRESSED_DXT5_RGBA,           // 8 bpp DXT5 压缩，支持渐变 alpha 通道
        CASIC_PIXELFORMAT_COMPRESSED_ETC1_RGB,            // 4 bpp ETC1 压缩，不支持 alpha 通道。
        CASIC_PIXELFORMAT_COMPRESSED_ETC2_RGB,            // 4 bpp ETC2 压缩，不支持 alpha 通道
        CASIC_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,       // 8 bpp ETC2 压缩，支持 8-bit alpha（这意味着 alpha 通道的原始数据 是 8 位 的，但在压缩过程中，这个 8 位 alpha 数据会被进一步压缩。通过 EAC（Enhanced Alpha Compression）算法，alpha 通道数据在存储时被压缩到更少的位数（通常是 4 位），从而使得整个像素在压缩后每个像素只占用 1 字节（包括 RGB 和 alpha））
        CASIC_PIXELFORMAT_COMPRESSED_PVRT_RGB,            // 4 bpp PVRTC 压缩，不支持 alpha 通道。
        CASIC_PIXELFORMAT_COMPRESSED_PVRT_RGBA,           // 4 bpp PVRTC 压缩，支持 alpha 通道。
        CASIC_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,       // 8 bpp ASTC 4x4 压缩，支持 RGBA。
        CASIC_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA        // 2 bpp ASTC 8x8 压缩，支持 RGBA。
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