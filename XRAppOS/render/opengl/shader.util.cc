#include "shader_util.h"
#include "XRAppOS/XRAppOSCommon.h"

#include <stdlib.h>

#if DEBUG
#define GL_DEBUG_LOG(type, object, action)                        \
  do {                                                            \
    GLint log_length = 0;                                         \
    glGet##type##iv(object, GL_INFO_LOG_LENGTH, &log_length);     \
    if (log_length > 0) {                                         \
      GLchar* log = static_cast<GLchar*>(malloc(log_length));     \
      glGet##type##InfoLog(object, log_length, &log_length, log); \
      LOG(INFO) << #type " " action " log:\n" << log;             \
      free(log);                                                  \
    }                                                             \
  } while (0)
#else
#define GL_DEBUG_LOG(type, object, action)
#endif

#define GL_ERROR_LOG(type, object, action)                        \
  do {                                                            \
    GLint log_length = 0;                                         \
    glGet##type##iv(object, GL_INFO_LOG_LENGTH, &log_length);     \
    if (log_length > 0) {                                         \
      GLchar* log = static_cast<GLchar*>(malloc(log_length));     \
      glGet##type##InfoLog(object, log_length, &log_length, log); \
      LOG(ERROR) << #type " " action " log:\n" << log;            \
      free(log);                                                  \
    }                                                             \
  } while (0)

namespace XRAppOS {

    constexpr int kMaxShaderInfoLength = 1024;

    GLint GlhCompileShader(GLenum target, const GLchar* source, GLuint* shader) {
        GLint status;

        *shader = glCreateShader(target);
        if (*shader == 0) {
            return GL_FALSE;
        }
        glShaderSource(*shader, 1, &source, NULL);
        glCompileShader(*shader);

//  GL_DEBUG_LOG(Shader, *shader, "compile");

        glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
        if(status == GL_FALSE) {
            XRCErrorLog("Failed to compile shader:%s", source);

            int length = 0;
            GLchar cmessage[kMaxShaderInfoLength];
            glGetShaderInfoLog(*shader, kMaxShaderInfoLength, &length, cmessage);
            XRCErrorLog("Create GL Program Error message: %s", cmessage);
        }
        return status;
    }

    GLint GlhLinkProgram(GLuint program) {
        GLint status;

        glLinkProgram(program);

//  GL_DEBUG_LOG(Program, program, "link");

        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if(status == GL_FALSE) {
            XRCErrorLog("Failed to link program : %s", program);
        }

        return status;
    }

    GLint GlhValidateProgram(GLuint program) {
        GLint status;

        glValidateProgram(program);

//  GL_DEBUG_LOG(Program, program, "validate");

        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
        if(status == GL_FALSE) {
            XRCErrorLog("Failed to validate program : %s", program);
        }

        return status;
    }

    GLint GlhCreateProgram(const GLchar* vert_src, const GLchar* frag_src,
                           GLsizei attr_count, const GLchar* const* attr_names,
                           const GLint* attr_locations, GLuint* program) {
        GLuint vert_shader = 0;
        GLuint frag_shader = 0;
        GLint ok = GL_TRUE;

        *program = glCreateProgram();
        if (*program == 0) {
            return GL_FALSE;
        }

        ok = ok && GlhCompileShader(GL_VERTEX_SHADER, vert_src, &vert_shader);
        ok = ok && GlhCompileShader(GL_FRAGMENT_SHADER, frag_src, &frag_shader);

        if (ok) {
            glAttachShader(*program, vert_shader);
            glAttachShader(*program, frag_shader);

            // Attribute location binding must be set before linking.
            for (int i = 0; i < attr_count; i++) {
                glBindAttribLocation(*program, attr_locations[i], attr_names[i]);
            }

            ok = GlhLinkProgram(*program);
        }

        if (vert_shader) glDeleteShader(vert_shader);
        if (frag_shader) glDeleteShader(frag_shader);

        if (!ok) {
            glDeleteProgram(*program);
            *program = 0;
        }

        return ok;
    }

    bool CompileShader(GLenum shader_type, const std::string& shader_source,
                       GLuint* shader) {
        *shader = glCreateShader(shader_type);
        if (*shader == 0) {
            XRCErrorLog("Unable to create shader of type:  : %s", shader_type);
            return false;
        }
        const char* shader_source_cstr = shader_source.c_str();
        glShaderSource(*shader, 1, &shader_source_cstr, NULL);
        glCompileShader(*shader);

        GLint compiled;
        glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {

            XRCErrorLog("Unable to create shader of type:  : %s", shader_source_cstr);
//    GL_ERROR_LOG(Shader, *shader, "compile");
            glDeleteShader(*shader);
            *shader = 0;
            return false;
        }
        return true;
    }

    bool CreateShaderProgram(
            GLuint vertex_shader, GLuint fragment_shader,
            const std::unordered_map<GLuint, std::string>& attributes,
            GLuint* shader_program) {
        *shader_program = glCreateProgram();
        if (*shader_program == 0) {
            XRCErrorLog("%s", "Unable to create shader program");
            return false;
        }
        glAttachShader(*shader_program, vertex_shader);
        glAttachShader(*shader_program, fragment_shader);

        for (const auto& it : attributes) {
            glBindAttribLocation(*shader_program, it.first, it.second.c_str());
        }
        glLinkProgram(*shader_program);

        GLint is_linked = 0;
        glGetProgramiv(*shader_program, GL_LINK_STATUS, &is_linked);
        if (!is_linked) {
            glDeleteProgram(*shader_program);
            *shader_program = 0;
            return false;
        }
        return true;
    }

    GLuint createTexture2D(int width, int height) {
        GLuint result = 0;
        glGenTextures(1, &result);
        glBindTexture(GL_TEXTURE_2D, result);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if(width > 0 && height > 0){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         nullptr);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        return result;
    }

}  // namespace cameralink