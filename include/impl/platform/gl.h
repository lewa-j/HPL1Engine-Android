
#ifndef HPL_GL_H
#define HPL_GL_H

#ifdef _WIN32
#define APIENTRY __stdcall
#endif

#include <impl/glad/glad.h>

#if defined(WIN32)
#undef APIENTRY
#include <impl/glad/glad_wgl.h>
#elif defined(__linux__) && !defined(ANDROID)
#include <impl/glad/glad_glx.h>
#endif

#endif //HPL_GL_H
