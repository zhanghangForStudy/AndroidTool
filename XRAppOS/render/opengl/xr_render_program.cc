

#include "xr_render_gl_program.h"
#include "shader_util.h"

#include "XRAppOS/XRAppOSCommon.h"

namespace XRAppOS {

    VertexDescribe::VertexDescribe(const size_t vertex_describe_len_,
                                   std::shared_ptr<VertexDescribeList> list)
            : vertex_describe_list(list),
              start_index_in_vertex_describe_list(list->raw_vertex_buffer.size()),
              vertex_describe_len(vertex_describe_len_) {
    }

    void VertexDescribe::Init() {
        if(vertex_describe_list.expired()){
            return;
        }
        auto list = vertex_describe_list.lock();
        for (int i = 0; i < vertex_describe_len; i++) {
            list->raw_vertex_buffer.push_back(0.0f);
        }
        list->vertex_describe_vector.push_back(shared_from_this());
    }

    void VertexDescribe::setValue(int index, float value) {
        if(vertex_describe_list.expired()){
            return;
        }
        auto list = vertex_describe_list.lock();
        if (list->raw_vertex_buffer.empty()
            || index < 0
            || index >= list->raw_vertex_buffer.size()) {
            return;
        }

        list->raw_vertex_buffer[index] = value;
    }

    XRRenderGpuProgram::XRRenderGpuProgram(const std::string &vertex_shader,
                                           const std::string &fragment_shader,
                                           const std::vector<std::string> &attr_names) :
            attr_name_vector(attr_names) {
        size_t attr_count = attr_name_vector.size();
        const GLchar **attr_name_array = new const GLchar *[attr_count];
        GLint *attr_locations = new GLint[attr_count];
        for (size_t i = 0; i < attr_count; i++) {
            attr_name_array[i] = attr_name_vector[i].c_str();
            attr_locations[i] = i;
        }

        GlhCreateProgram(vertex_shader.c_str(), fragment_shader.c_str(), attr_count,
                         attr_name_array, attr_locations, &m_program);

        delete[] attr_name_array;
        delete[] attr_locations;
    }

    bool XRRenderGpuProgram::isValidProgram() {
        return m_program > 0;
    }

    GLint XRRenderGpuProgram::getUniformHandleByName(const std::string &name) {
        return glGetUniformLocation(m_program, name.c_str());
    }

    GLint XRRenderGpuProgram::getAttribHandleByName(const std::string &name) {
        return glGetAttribLocation(m_program, name.c_str());
    }

    bool XRRenderGpuProgram::render() {

        // Run core program.
//        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//        glUseProgram(m_program);

        // draw
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        return true;
    }

    bool XRRenderGpuProgram::destroy() {
        if (m_program > 0) {
            glDeleteProgram(m_program);
            m_program = 0;
        }

        return true;
    }
}
