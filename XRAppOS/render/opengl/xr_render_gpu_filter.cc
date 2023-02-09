#include "xr_render_gpu_filter.h"
//#include "xr_render_kit.h"
#include "xr_render_gl_base.h"
#include "XRAppOS/XRAppOSCommon.h"
//#include "gpu/gl_calculator_helper.h"

namespace XRAppOS {

    bool CheckGLContext() {
//        return GlContext::GetCurrent() != nullptr;
        return true;
    }

//    void XRRenderGpuFilter::createRenderTargetIfNeed(GlCalculatorHelper &helper, int width, int height,
//                                                 GpuBufferFormat gpuBufferFormat) {
//
////        bool need_create_render_target = true;
////
////        auto output = render_target.template GetFrame<GpuBuffer>();
////        if (output->width() != width
////            || output->height() != height
////            || output->format() != gpuBufferFormat) {
////            render_target.Release();
////            need_create_render_target = true;
////        }
////        if (need_create_render_target) {
//        // CreateDestinationTexture方法会自动复用尺寸、格式相同的纹理，故，此处无需进行复用判断
//        render_target.Release();
//        render_target = helper.CreateDestinationTexture(width, height, gpuBufferFormat);
////        }
//    }

//    void XRRenderGpuFilter::fillRenderTargetToGpuBufferT(GpuBuffer &gpu_buffer) {
//        auto output = render_target.GetFrame<GpuBuffer>();
//        gpu_buffer = *output;
//    }
//
//    GLuint XRRenderGpuFilter::GetRenderTargetTextureId() {
//        auto output = render_target.GetFrame<GpuBuffer>();
//        return output ? output->GetGlTextureBufferSharedPtr()->name() : 0;
//    }

    bool XRRenderGpuFilter::setUniform1f(const std::string &name, float value) {
        if (!program || !program->isValidProgram() || !CheckGLContext()) {
            return false;
        }
        GLint uniform_handler = program->getUniformHandleByName(name);
        glUniform1f(uniform_handler, value);
        return true;
    }

    bool XRRenderGpuFilter::setUniform1i(const std::string &name, int value) {
        if (!program || !program->isValidProgram() || !CheckGLContext()) {
            return false;
        }
        GLint uniform_handler = program->getUniformHandleByName(name);
        glUniform1i(uniform_handler, value);
        return true;
    }

    bool XRRenderGpuFilter::setUniformMatrix4fv(const std::string &name, float *value) {
        if (!program || !program->isValidProgram() || !CheckGLContext()) {
            return false;
        }
        GLint uniform_handler = program->getUniformHandleByName(name);
        glUniformMatrix4fv(uniform_handler, 1, GL_FALSE, value);
        return true;
    }

    bool XRRenderGpuFilter::setUniform3f(const std::string &name, float *value) {
        if (!program || !program->isValidProgram() || !CheckGLContext()) {
            return false;
        }
        GLint uniform_handler = program->getUniformHandleByName(name);
        glUniform3f(uniform_handler, value[0], value[1], value[2]);
        return true;
    }

    bool XRRenderGpuFilter::updateTexture(GLuint *textureId, GLenum format,
                                          int width, int height,
                                          const void *data, int stride,
                                          GLenum target,
                                          GLenum magMinFilter,
                                          GLenum wrapMode) {
        if (data == nullptr || !CheckGLContext()) {
            XRCErrorLog("%s", "xyk TryUpdateGLTexture error: null data!");
            return false;
        }

        //ip7测试情况下,unpack_align_value默认为4,当遇到一些特别的分辨率如750x1000,并且内存没有做对齐的话,会有问题,所以需要把GL_UNPACK_ALIGNMENT设置为1
        int unpack_align_value = 0;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_align_value);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // LOG(INFO) << "xyk start updating gl texture.." << *textureId << "," <<
        // width << "," << height << ", " << (unsigned long)data << std::endl;
        if (stride != 0) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
        }

        if (*textureId == 0) {
            glGenTextures(1, textureId);
            glBindTexture(target, *textureId);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
            glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
                         data);
            cached_texture_id.push_back(*textureId);
        } else {
            glBindTexture(target, *textureId);
            glTexImage2D(target, 0, format, width, height, 0, format,
                         GL_UNSIGNED_BYTE, data);
        }

        if (stride != 0) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_align_value);

        return true;
    }

    void
    XRRenderGpuFilter::ActivateGLTexture(GLuint textureId, GLuint textureHandle, int textureStage,
                                         GLenum target) {
        if (textureId > 0 && CheckGLContext()) {
            glActiveTexture(GL_TEXTURE0 + textureStage);
            glBindTexture(target, textureId);
            glUniform1i(textureHandle, textureStage);
        } else {
            XRCErrorLog("%s", "xyk ActivateGLTexture error: invalid texture id!");
        }
    }

    void XRRenderGpuFilter::DeactivationGLTexture(int textureStage, GLenum target) {
        if (CheckGLContext()) {
//            glActiveTexture(GL_TEXTURE0 + textureStage);
            glBindTexture(target, 0);
        } else {
            XRCErrorLog("%s", "xyk DeactivationGLTexture error: invalid texture id!");
        }
    }

    bool XRRenderGpuFilter::destroy() {
        if (!CheckGLContext()) {
            return false;
        }
        bool result = true;
        if (program) {
            if (program.use_count() == 1) {
                result = program->destroy();
            }
            program.reset();
            program = nullptr;
        }

//        render_target.Release();

        _SafeDeleteTexture_(render_target)

        _SafeDeleteFrameBuffer_(render_frame_buffer);

        // 释放创建的纹理
        auto iterator = cached_texture_id.begin();
        while (iterator != cached_texture_id.end()) {
            GLuint texture_id = *iterator;
            if (texture_id > 0) {
                _SafeDeleteTexture_(texture_id);
            }
            iterator++;
        }

        return result;
    }
}
