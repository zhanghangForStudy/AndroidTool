#ifndef AR_SCENE_RENDER_GPU_FILTER_H
#define AR_SCENE_RENDER_GPU_FILTER_H

//#include "gpu/gl_context.h"
//#include <gpu/gl_base.h>
//#include <gpu/gl_calculator_helper.h>
#include "glm/fwd.hpp"
#include "xr_render_gl_program.h"
#include "XRAppOS/XRAppOSCommon.h"
// 此文件定义，GPU实现的image filter base class
namespace XRAppOS {
    class XRRenderGpuProgram;

#define _SafeDeleteTexture_(texture) \
  {                                  \
    if (texture > 0) {               \
      glDeleteTextures(1, &texture); \
      texture = 0;                   \
    }                                \
  }

#define _SafeDeleteFrameBuffer_(frame_buffer) \
  {                                  \
    if (frame_buffer > 0) {               \
      glDeleteFramebuffers(1, &frame_buffer); \
      frame_buffer = 0;                   \
    }                                \
  }

    //  主要负责两个功能：1. 更新XRRenderGpuProgram中的参数；2. 定义gpu filter的渲染流程
    class XRAPPOS_PUBLIC XRRenderGpuFilter {
    public:
        XRRenderGpuFilter(std::shared_ptr<XRRenderGpuProgram> program_shared_ptr)
                : program(program_shared_ptr) {
        }

        ~XRRenderGpuFilter() {}

        void createOfflineFrameBufferIfNeed(GLuint renderTarget = 0) {
            render_target = renderTarget;

            if (!render_frame_buffer) {
//                // create a renderbuffer object to store depth info
//                GLuint rboId;
//                glGenRenderbuffers(1, &rboId);
//                glBindRenderbuffer(GL_RENDERBUFFER, rboId);
//                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
//                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                glGenFramebuffers(1, &render_frame_buffer);
                glBindFramebuffer(GL_FRAMEBUFFER, render_frame_buffer);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                       render_target, 0);

                // attach the renderbuffer to depth attachment point
//                glFramebufferRenderbuffer(GL_FRAMEBUFFER,      // 1. fbo target: GL_FRAMEBUFFER
//                                          GL_DEPTH_ATTACHMENT, // 2. attachment point
//                                          GL_RENDERBUFFER,     // 3. rbo target: GL_RENDERBUFFER
//                                          rboId);

                auto frame_buffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (frame_buffer_status != GL_FRAMEBUFFER_COMPLETE) {
//                    XRCErrorLog("glCheckFramebufferStatus failed 0x%08x", frame_buffer_status);
                    // todo 不要直接关闭进程
                    abort();
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }

        bool setUniform1f(const std::string &name, float value);

        bool setUniform1i(const std::string &name, int value);

        bool setUniformMatrix4fv(const std::string &name, float *value);

        bool setUniform3f(const std::string &name, float *value);

        // 更新texture, 如果texture不存在，只创建一个
        bool updateTexture(GLuint *textureId, GLenum format,
                           int width, int height,
                           const void *data, int stride,
                           GLenum target = GL_TEXTURE_2D,
                           GLenum magMinFilter = GL_LINEAR,
                           GLenum wrapMode = GL_CLAMP_TO_EDGE);

        // 激活纹理
        static void ActivateGLTexture(GLuint textureId, GLuint textureHandle, int textureStage,
                                      GLenum target = GL_TEXTURE_2D);

        // 去活活纹理
        static void DeactivationGLTexture(int textureStage, GLenum target = GL_TEXTURE_2D);

        virtual bool render() = 0;

        bool renderOffLine() {
            if (render_frame_buffer > 0) {
                glBindFramebuffer(GL_FRAMEBUFFER, render_frame_buffer);
            }

            render();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return true;
        }

        GLuint GetTextureIdAttachedFrameBuffer() {
            return render_target;
        }

        GLuint GetFrameBufferId(){
            return render_frame_buffer;
        }

//        void fillRenderTargetToGpuBufferT(GpuBuffer &gpu_buffer);

//        GLuint GetRenderTargetTextureId();

        virtual bool destroy();

    protected:

        // 使用共享指针，确保相同的program，可以被多个gpu filter复用
        // 这也是未将ARSRProgram与ARSRGpuProgram合并成一个类的原因
        std::shared_ptr<XRRenderGpuProgram> program;

//        GlTexture render_target;

    private:
        void createRenderTargetIfNeed(int width, int height) {
            if (render_target) {
                return;
            }

            glGenTextures(1, &render_target);
            if (!render_target) {
                return;
            }

            glBindTexture(GL_TEXTURE_2D, render_target);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // 缓存创建的纹理ID，以便统一回收
        std::vector<GLuint> cached_texture_id;

        GLuint render_frame_buffer = 0;

        GLuint render_target = 0;
    };
}

#endif// end AR_SCENE_RENDER_GPU_FILTER_H
