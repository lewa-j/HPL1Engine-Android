
#ifndef HPL_GL_H
#define HPL_GL_H

#ifdef ANDROID
#include <GLES2/gl2.h>

#define GL_TEXTURE_1D 				0x0DE0
#define GL_TEXTURE_3D 				0x806F
#define GL_TEXTURE_RECTANGLE 	0x84F5
#define GL_BGR					0x80E0
#define GL_BGRA					0x80E1

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
