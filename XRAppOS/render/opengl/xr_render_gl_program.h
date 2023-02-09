#ifndef AR_SCENE_RENDER_GL_PROGRAM_H
#define AR_SCENE_RENDER_GL_PROGRAM_H
//#include <gpu/gl_base.h>
#include <string>
#include <vector>
#include "XRAppOS/XRAppOSCommon.h"
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#import <OpenGLES/ES2/gl.h>
#endif

// 此头文件，定义自定义的GL程序(GLSL)
namespace XRAppOS {

    class VertexDescribeList;

    class VertexDescribe : public std::enable_shared_from_this<VertexDescribe> {

    public:
        ~VertexDescribe(){
//            DBD("shuyi test: %s", "delete the VertexDescribe");
        };

        XRAPPOS_PUBLIC VertexDescribe(size_t len, std::shared_ptr<VertexDescribeList> list);

        XRAPPOS_PUBLIC void Init();

    protected:

        void XRAPPOS_PUBLIC setValue(int index, float value);

        const size_t start_index_in_vertex_describe_list;

    private:
        friend VertexDescribeList;

        const size_t vertex_describe_len;

        // 使用虚引用，防止循环引用
        std::weak_ptr<VertexDescribeList> vertex_describe_list;
    };

    class VertexDescribeList {
    public:
        ~VertexDescribeList(){
//            DBD("shuyi test: %s", "delete the VertexDescribeList");
        };

        inline void clear() {
            raw_vertex_buffer.clear();
            vertex_describe_vector.clear();
        }

        inline size_t GetVertexCount() {
            return vertex_describe_vector.size();
        }

        inline size_t GetRawVertexBufferCount() {
            return raw_vertex_buffer.size();
        }

        inline const void *GetRawVertexBuffer() {
            return raw_vertex_buffer.data();
        }

        inline std::shared_ptr<VertexDescribe> GetVertexDescription(int index) {
            if (index < 0 || index >= vertex_describe_vector.size()) {
                return nullptr;
            }

            return vertex_describe_vector[index];
        }

    private:
        friend VertexDescribe;

        // 原始的vertex buffer
        std::vector<GLfloat> raw_vertex_buffer;

        std::vector<std::shared_ptr<VertexDescribe>> vertex_describe_vector;
    };

    // simplified version of vbo management
    class VBO {
    public:
        XRAPPOS_PUBLIC VBO(GLenum type = GL_ARRAY_BUFFER);

        XRAPPOS_PUBLIC ~VBO();

        XRAPPOS_PUBLIC void Init(int attrib, int buffer_size, int element_count,
                              GLenum element_format = GL_FLOAT, bool dynamic = true);

        XRAPPOS_PUBLIC void Init(int buffer_size, bool dynamic = true);

        XRAPPOS_PUBLIC void AddAttrLayoutIfNeed(int attrib, int element_count, GLenum element_format = GL_FLOAT, size_t stride = 0, long pointer = 0);

        XRAPPOS_PUBLIC void Activate(const void *data, bool updateAlways = false);

        XRAPPOS_PUBLIC void Deactivate();

        int GetBufferSize(){
            return m_buffer_size;
        }

    private:
        void Destroy();

    private:
        struct AttribLayout{
            int m_attrib = 0;
            int m_element_count = 0;
            GLenum m_element_format = GL_FLOAT;

            // 每一个vertex attrib的数据结果的长度
            size_t stride = 0;

            long pointer = 0;
        };

        GLuint m_id = 0;
        // 记录最近的buffer.
        void *m_current_buffer = nullptr;
        // 简化: 只处理尺寸一样的buffer. 在Init时即确定
        int m_buffer_size = 0;

        std::vector<AttribLayout> m_attrib_layout;

        bool m_dynamic = false;
        bool m_inited = false;
        // bo的类型，默认是VBO
        GLenum m_bo_type = GL_ARRAY_BUFFER;
    };

    class XRRenderGpuFilter;

    // GLSL程序
    XRAPPOS_PUBLIC
    class XRRenderGpuProgram {
    public:
        XRAPPOS_PUBLIC XRRenderGpuProgram(const std::string &vertex_shader,
                                          const std::string &fragment_shader,
                                          const std::vector<std::string> &attr_names);

        ~XRRenderGpuProgram() {
        }

        bool isValidProgram();

        inline void UseGlProgram(){
            if(m_program){
                glUseProgram(m_program);
            }
        }

        inline void UnUseGlProgram(){
            if(m_program){
                glUseProgram(0);
            }
        }

        XRAPPOS_PUBLIC GLint getUniformHandleByName(const std::string &name);

        XRAPPOS_PUBLIC GLint getAttribHandleByName(const std::string &name);

        XRAPPOS_PUBLIC bool render();

        XRAPPOS_PUBLIC bool destroy();

        GLuint m_program = 0;

    private:

        const std::vector<std::string> attr_name_vector;
    };
}

#endif// end AR_SCENE_RENDER_GL_PROGRAM_H
