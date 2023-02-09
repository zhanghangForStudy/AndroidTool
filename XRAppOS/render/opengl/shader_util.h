#ifndef CAMERALINK_GPU_SHADER_UTIL_H_
#define CAMERALINK_GPU_SHADER_UTIL_H_

#include <string>
#include <unordered_map>
#include "xr_render_gl_base.h"

namespace XRAppOS {

// TODO: Remove the C-style helpers.
// Compiles a GLSL shader, logs errors, returns the compile status
// (GL_TRUE for success, GL_FALSE for failure).
    GLint GlhCompileShader(GLenum target, const GLchar* source, GLuint* shader);

// Links a GLSL program, logs errors, returns the link status
// (GL_TRUE for success, GL_FALSE for failure).
    GLint GlhLinkProgram(GLuint program);

// Validates a GLSL program, logs errors, returns the validate status
// (GL_TRUE for success, GL_FALSE for failure).
    GLint GlhValidateProgram(GLuint program);

    // todo[架构统一] 分平台实现
    // 需要在GL线程中创建
    GLuint createTexture2D(int width = 0, int height = 0);

// Creates a GLSL program by compiling and linking the provided shaders.
// Also obtains the locations of the requested attributes.
// Return GL_TRUE for success, GL_FALSE for failure.
    GLint GlhCreateProgram(const GLchar* vert_src, const GLchar* frag_src,
                           GLsizei attr_count, const GLchar* const* attr_names,
                           const GLint* attr_locations, GLuint* program);

// Compiles a shader specified by shader_source. Returns true on success.
    bool CompileShader(GLenum shader_type, const std::string& shader_source,
                       GLuint* shader);

// Creates a shader program using the supplied vertex shader, fragment shader
// and attributes and stores in program. Returns true on success.
    bool CreateShaderProgram(
            GLuint vertex_shader, GLuint fragment_shader,
            const std::unordered_map<GLuint, std::string>& attributes, GLuint* program);
}  // namespace cameralink

#endif  // CAMERALINK_GPU_SHADER_UTIL_H_