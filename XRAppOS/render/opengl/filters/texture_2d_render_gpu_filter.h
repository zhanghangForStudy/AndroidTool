#ifndef TEXTURE_2D_RENDER_GPU_FILTER_H_
#define TEXTURE_2D_RENDER_GPU_FILTER_H_

#include "xr_render_gl_base.h"
#include "xr_render_gpu_filter.h"

namespace XRAppOS {
    // 简单的渲染2D纹理
    class XRAPPOS_PUBLIC ARSRTexture2DGpuFilter : public XRRenderGpuFilter {
    public:
        ARSRTexture2DGpuFilter(GLenum texture_format = GL_TEXTURE_2D);

//        ARSRTexture2DGpuFilter(std::shared_ptr<XRRenderGpuProgram> program_ptr,
//                               GLenum texture_format = GL_TEXTURE_2D) : XRRenderGpuFilter(program_ptr),
//                                                                        need_render_texture_format(
//                                                                                texture_format) {}

        ~ARSRTexture2DGpuFilter() {}


        bool render();

        inline void UpdateNeedRenderTextureId(GLuint texture_id) {
            need_render_texture_id = texture_id;
        }

        inline GLuint CreateNeedRenderTextureId() {
            if (!need_render_texture_id) {
                glGenTextures(1, &need_render_texture_id);
                glBindTexture(need_render_texture_format, need_render_texture_id);
                glTexParameteri(need_render_texture_format, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(need_render_texture_format, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            return need_render_texture_id;
        }

        GLfloat *GetTextureCoordinationPtr() {
            return need_render_texture_coordination;
        }

        GLfloat *GetVerCoordination();

        inline bool destroy() override {

            _SafeDeleteTexture_(need_render_texture_id);

            return XRRenderGpuFilter::destroy();
        }

    private:
        GLfloat *need_render_texture_coordination;

        GLenum need_render_texture_format = GL_TEXTURE_2D;
        // 需要渲染的纹理ID
        GLuint need_render_texture_id = 0;
        // 表示4个顶点的VBO
        VBO m_vertex_vbo;
        // 表示纹理4个顶点的VBO
        VBO m_tex0_vbo;

        bool initialized_ = false;
    };
}
#endif // end TEXTURE_2D_RENDER_GPU_FILTER_H_
