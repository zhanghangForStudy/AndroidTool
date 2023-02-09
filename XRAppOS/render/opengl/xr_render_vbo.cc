#include "xr_render_gl_program.h"
//#include <gpu/gl_base.h>
//#include "glog/logging.h"
#include "XRAppOS/XRAppOSCommon.h"

namespace XRAppOS {
    VBO::VBO(GLenum type) : m_bo_type(type) {}

    VBO::~VBO() { Destroy(); }

    void VBO::Init(int attrib, int buffer_size, int element_count,
                   GLenum element_format, bool dynamic) {

        Init(buffer_size, dynamic);

        AddAttrLayoutIfNeed(attrib, element_count, element_format);

    }

    void VBO::Init(int buffer_size, bool dynamic) {
        this->m_buffer_size = buffer_size;
        this->m_dynamic = dynamic;
        this->m_inited = true;
    }

    XRAPPOS_PUBLIC void
    VBO::AddAttrLayoutIfNeed(int attrib, int element_count, GLenum element_format, size_t stride,
                             long pointer) {
        if (m_bo_type != GL_ARRAY_BUFFER) {
            XRCErrorLog("the type[%d] is no support attrib laytou", m_bo_type);
            abort();
        }

        // 更新已有的
        if (!m_attrib_layout.empty()) {
            auto it = m_attrib_layout.begin();
            while (it != m_attrib_layout.end()) {
                if (it->m_attrib == attrib) {
                    it->m_element_count = element_count;
                    it->m_element_format = element_format;
                    it->stride = stride;
                    it->pointer = pointer;
                    return;
                }
                it++;
            }
        }

        AttribLayout attribLayout;
        attribLayout.m_attrib = attrib;
        attribLayout.m_element_count = element_count;
        attribLayout.m_element_format = element_format;
        attribLayout.pointer = pointer;
        attribLayout.stride = stride;

        m_attrib_layout.push_back(attribLayout);
    }

    void VBO::Destroy() {
        if (this->m_id > 0) {
            glDeleteBuffers(1, &m_id);
            m_inited = false;
        }
    }

    void VBO::Activate(const void *data, bool updateAlways) {
        if (!this->m_inited) {
            XRCErrorLog("%s", "VBO haven't been inited yet~");
            return;
        }

        // 是否需要更新?
        if (this->m_current_buffer != data || updateAlways) {
            this->m_current_buffer = (void *) data;

            if (this->m_id == 0) {
                glGenBuffers(1, &m_id);
                glBindBuffer(m_bo_type, m_id);
                glBufferData(m_bo_type, m_buffer_size, m_current_buffer,
                             this->m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            } else {
                if (!m_dynamic) {
                    // give a warning
                    XRCErrorLog("%s", "VBO buffer is static, but it's being updated dynamic!");
                }

                glBindBuffer(m_bo_type, m_id);
                glBufferSubData(m_bo_type, 0, m_buffer_size, m_current_buffer);
            }
        }

        glBindBuffer(m_bo_type, m_id);

        if (!m_attrib_layout.empty()) {
            auto it = m_attrib_layout.begin();
            while (it != m_attrib_layout.end()) {
                glEnableVertexAttribArray(it->m_attrib);
                glVertexAttribPointer(it->m_attrib, it->m_element_count,
                                      it->m_element_format, false, it->stride,
                                      (void *) it->pointer);
                it++;
            }
        }
        bool anyError = false;
        for (GLint error = glGetError(); error; error = glGetError()) {
            XRCErrorLog("after VBO::Activate, glError (0x%x)\n", error);
            anyError = true;
        }
        if (anyError) {
            abort();
        }
//        glBindBuffer(m_bo_type, 0);
    }

    void VBO::Deactivate() {
        if (!m_attrib_layout.empty()) {
            auto it = m_attrib_layout.begin();
            while (it != m_attrib_layout.end()) {
                glDisableVertexAttribArray(it->m_attrib);
                it++;
            }
        }
        glBindBuffer(m_bo_type, 0);
    }
}
