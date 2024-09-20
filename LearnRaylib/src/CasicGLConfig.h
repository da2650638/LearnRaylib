#pragma once

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

#define CASIC_MAX_SHADER_LOCATIONS 32

#define CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION	0
#define CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD	1
#define CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL     2
#define CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR      3
#define CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT    4
#define CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2  5
#define CASIC_DEFAULT_SHADER_ATTRIB_LOCATION_INDICE     6

#define CASIC_DEFAULT_SHADER_ATTRIB_NAME_POSITION	"vertexPostion"
#define CASIC_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD	"vertexTexCoord"
#define CASIC_DEFAULT_SHADER_ATTRIB_NAME_NORMAL		"vertexNormal"
#define CASIC_DEFAULT_SHADER_ATTRIB_NAME_COLOR		"vertexColor"
#define CASIC_DEFAULT_SHADER_ATTRIB_NAME_TANGENT	"vertexTangent"
#define CASIC_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2	"vertexTexCoord2"

#define CASIC_DEFAULT_SHADER_UNIFORM_NAME_MVP			"mvp"
#define CASIC_DEFAULT_SHADER_UNIFORM_NAME_VIEW			"matView"
#define CASIC_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION	"matProjection"
#define CASIC_DEFAULT_SHADER_UNIFORM_NAME_MODEL			"matModel"
#define CASIC_DEFAULT_SHADER_UNIFORM_NAME_NORMAL		"matNormal"
#define CASIC_DEFAULT_SHADER_UNIFORM_NAME_COLOR			"colDiffuse"
#define CASIC_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0	"texture0"
#define CASIC_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1	"texture1"
#define CASIC_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2	"texture2"

// Shader location point type
typedef enum {
    CASIC_SHADER_LOC_VERTEX_POSITION = 0,  // Shader location: vertex attribute: position
    CASIC_SHADER_LOC_VERTEX_TEXCOORD01,    // Shader location: vertex attribute: texcoord01
    CASIC_SHADER_LOC_VERTEX_TEXCOORD02,    // Shader location: vertex attribute: texcoord02
    CASIC_SHADER_LOC_VERTEX_NORMAL,        // Shader location: vertex attribute: normal
    CASIC_SHADER_LOC_VERTEX_TANGENT,       // Shader location: vertex attribute: tangent
    CASIC_SHADER_LOC_VERTEX_COLOR,         // Shader location: vertex attribute: color
    CASIC_SHADER_LOC_MATRIX_MVP,           // Shader location: matrix uniform: model-view-projection
    CASIC_SHADER_LOC_MATRIX_VIEW,          // Shader location: matrix uniform: view (camera transform)
    CASIC_SHADER_LOC_MATRIX_PROJECTION,    // Shader location: matrix uniform: projection
    CASIC_SHADER_LOC_MATRIX_MODEL,         // Shader location: matrix uniform: model (transform)
    CASIC_SHADER_LOC_MATRIX_NORMAL,        // Shader location: matrix uniform: normal
    CASIC_SHADER_LOC_VECTOR_VIEW,          // Shader location: vector uniform: view
    CASIC_SHADER_LOC_COLOR_DIFFUSE,        // Shader location: vector uniform: diffuse color
    CASIC_SHADER_LOC_COLOR_SPECULAR,       // Shader location: vector uniform: specular color
    CASIC_SHADER_LOC_COLOR_AMBIENT,        // Shader location: vector uniform: ambient color
    CASIC_SHADER_LOC_MAP_ALBEDO,           // Shader location: sampler2d texture: albedo (same as: RL_SHADER_LOC_MAP_DIFFUSE)
    CASIC_SHADER_LOC_MAP_METALNESS,        // Shader location: sampler2d texture: metalness (same as: RL_SHADER_LOC_MAP_SPECULAR)
    CASIC_SHADER_LOC_MAP_NORMAL,           // Shader location: sampler2d texture: normal
    CASIC_SHADER_LOC_MAP_ROUGHNESS,        // Shader location: sampler2d texture: roughness
    CASIC_SHADER_LOC_MAP_OCCLUSION,        // Shader location: sampler2d texture: occlusion
    CASIC_SHADER_LOC_MAP_EMISSION,         // Shader location: sampler2d texture: emission
    CASIC_SHADER_LOC_MAP_HEIGHT,           // Shader location: sampler2d texture: height
    CASIC_SHADER_LOC_MAP_CUBEMAP,          // Shader location: samplerCube texture: cubemap
    CASIC_SHADER_LOC_MAP_IRRADIANCE,       // Shader location: samplerCube texture: irradiance
    CASIC_SHADER_LOC_MAP_PREFILTER,        // Shader location: samplerCube texture: prefilter
    CASIC_SHADER_LOC_MAP_BRDF              // Shader location: sampler2d texture: brdf
} rlShaderLocationIndex;

#define CASIC_SHADER_LOC_MAP_DIFFUSE    CASIC_SHADER_LOC_MAP_ALBEDO
#define CASIC_SHADER_LOC_MAP_SPECULAR   CASIC_SHADER_LOC_MAP_METALNESS

// Primitive assembly draw modes
#define CASIC_LINES                                0x0001      // GL_LINES
#define CASIC_TRIANGLES                            0x0004      // GL_TRIANGLES
#define CASIC_QUADS                                0x0007      // GL_QUADS

#define CASIC_DEFAULT_BATCH_DRAWCALLS           256      // Default number of batch draw calls (by state changes: mode, texture)
#define CASIC_DEFAULT_BATCH_BUFFERS               1      // Default number of batch buffers (multi-buffering)
#define CASIC_DEFAULT_BATCH_BUFFER_ELEMENTS    8192
#define CASIC_DEFAULT_BATCH_MAX_TEXTURE_UNITS     4  

#define CASIC_WHITE      {255, 255, 255, 255}
#define CASIC_RED        {255, 0, 0, 255}
#define CASIC_VIOLET     { 135, 60, 190, 255 }    // Violet
