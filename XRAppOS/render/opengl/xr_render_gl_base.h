#ifndef XR_RENDER_GL_BASE_H_
#define XR_RENDER_GL_BASE_H_

#if defined(__EMSCRIPTEN__)
#include <emscripten/html5.h>
#endif  // defined(__EMSCRIPTEN__)

#if defined(__APPLE__)

#include <TargetConditionals.h>

#if TARGET_OS_OSX

#define HAS_NSGL 1

#include <OpenGL/OpenGL.h>

#if CGL_VERSION_1_3
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif  // CGL_VERSION_1_3

#else

#define HAS_EAGL 1

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#endif  // TARGET_OS_OSX

#else

#define HAS_EGL 1

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifdef __ANDROID__
// Weak-link all GL APIs included from this point on.
// TODO: Annotate these with availability attributes for the
// appropriate versions of Android, by including gl{3,31,31}.h and resetting
// GL_APICALL for each.
#undef GL_APICALL
#define GL_APICALL __attribute__((weak_import)) KHRONOS_APICALL
#endif  // __ANDROID__

#include <GLES3/gl32.h>

// When using the Linux EGL headers, we may end up pulling a
// "#define Status int" from Xlib.h, which interferes with cameralink::Status.
#undef Status

// More crud from X
#undef None
#undef Bool
#undef Success

#endif  // defined(__APPLE__)

namespace cameralink {

// Doing this as an inline function allows us to avoid unwanted "pointer will
// never be null" errors on certain platforms and compilers.
    template <typename T>
    inline bool SymbolAvailable(T* symbol) {
        return symbol != nullptr;
    }

}  // namespace cameralink

#endif  // XR_RENDER_GL_BASE_H_