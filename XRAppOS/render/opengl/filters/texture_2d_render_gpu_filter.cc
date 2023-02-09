//#include <android/log.h>
#include "xr_render_gl_program.h"
#include "XRAppOS/XRAppOSCommon.h"
#include "texture_2d_render_gpu_filter.h"
#include "xr_render_simple_shaders.h"

namespace XRAppOS {
    GLfloat square_vertices[] = {
            -1.0f, -1.0f,  // bottom left
            1.0f, -1.0f,  // bottom right
            -1.0f, 1.0f,   // top left
            1.0f, 1.0f,   // top right
    };
    GLfloat texture_vertices[] = {
            0.0f, 0.0f,  // bottom left
            1.0f, 0.0f,  // bottom right
            0.0f, 1.0f,  // top left
            1.0f, 1.0f,  // top right
    };

    static const int SQUARE_VERTEX_BUFFER_SIZE = 8 * sizeof(float);
    static const int SQUARE_VERTEX_ELEMENT_COUNT = 2;
    static const int SQUARE_UV_BUFFER_SIZE = 8 * sizeof(float);
    static const int SQUARE_UV_ELEMENT_COUNT = 2;

    XRAPPOS_PUBLIC ARSRTexture2DGpuFilter::ARSRTexture2DGpuFilter(GLenum texture_format)
            : XRRenderGpuFilter(nullptr), need_render_texture_format(texture_format) {
        std::vector<std::string> attr_names;
        attr_names.emplace_back("position");
        attr_names.emplace_back("texture_coordinate");

        std::string fragment_shader = kBasicTexturedFlipXFragmentShader;
                
#ifdef __ANDROID__
        if(need_render_texture_format == GL_TEXTURE_EXTERNAL_OES){
            fragment_shader = kOESTexturedForAceTinyFragmentShader;
        }
#endif
        program = std::make_shared<XRRenderGpuProgram>(kBasicVertexShader, fragment_shader,
                                                   attr_names);

        need_render_texture_coordination = texture_vertices;
    }

    GLfloat *ARSRTexture2DGpuFilter::GetVerCoordination() {
        return square_vertices;
    }

    bool ARSRTexture2DGpuFilter::render() {
        if (!program
            || need_render_texture_id == 0) {
            return false;
        }

        program->UseGlProgram();
//        if (render_target.name() > 0) {
//            helper.BindFramebuffer(render_target);
//        }

        if (!initialized_) {
            initialized_ = true;

            GLuint position_attrib_ = program->getAttribHandleByName("position");
            GLuint tex_coord_attrib_ = program->getAttribHandleByName("texture_coordinate");
            m_vertex_vbo.Init(position_attrib_, SQUARE_VERTEX_BUFFER_SIZE, SQUARE_VERTEX_ELEMENT_COUNT, GL_FLOAT, false);
            m_tex0_vbo.Init(tex_coord_attrib_, SQUARE_UV_BUFFER_SIZE, SQUARE_UV_ELEMENT_COUNT);
        }

        GLint video_frame = program->getUniformHandleByName("video_frame");
        ActivateGLTexture(need_render_texture_id, video_frame, 0, need_render_texture_format);

//        // 激活vbo
        m_vertex_vbo.Activate(square_vertices);
        m_tex0_vbo.Activate(need_render_texture_coordination, true);

        // 渲染
        program->render();

        // 一些收尾工作
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        m_vertex_vbo.Deactivate();
        m_tex0_vbo.Deactivate();
        DeactivationGLTexture(0);
        program->UnUseGlProgram();

        bool anyError = false;
        for (GLint error = glGetError(); error; error = glGetError()) {
            XRCErrorLog("after %s() glError (0x%x)\n", "YUV2RGBrender::render", error);
            anyError = true;
        }
        if (anyError) {
            abort();
        }

        return true;
    }
}
