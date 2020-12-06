
#ifndef HPL_GL_H
#define HPL_GL_H

#ifdef ANDROID
#include <GLES2/gl2.h>

#define glGenBuffersARB glGenBuffers
#define glDeleteBuffersARB glDeleteBuffers
#define glBindBufferARB glBindBuffer
#define glBufferDataARB glBufferData
#define GL_ARRAY_BUFFER_ARB GL_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER_ARB GL_ELEMENT_ARRAY_BUFFER
#define GL_STATIC_DRAW_ARB GL_STATIC_DRAW
#define GL_DYNAMIC_DRAW_ARB GL_DYNAMIC_DRAW
#define GL_STREAM_DRAW_ARB GL_STREAM_DRAW

#define GL_TEXTURE_1D 				0x0DE0
#define GL_TEXTURE_3D 				0x806F
#define GL_TEXTURE_RECTANGLE_NV 	0x84F5
#define GL_BGR_EXT					0x80E0
#define GL_BGRA_EXT					0x80E1
#define GL_TEXTURE_CUBE_MAP_ARB 	GL_TEXTURE_CUBE_MAP
#define GL_INCR_WRAP_EXT 			GL_INCR_WRAP
#define GL_DECR_WRAP_EXT 			GL_DECR_WRAP

//hack
#define GL_CLAMP GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_BORDER GL_CLAMP_TO_EDGE

#else
#ifdef _WIN32
#define APIENTRY __stdcall
#endif
#include <impl/glad/glad.h>

#if defined(WIN32)
#undef APIENTRY
#include <impl/glad/glad_wgl.h>

#elif defined(__linux__)
#include <impl/glad/glad_glx.h>
#endif

#endif


#endif //HPL_GL_H