#ifndef AR_SCENE_RENDER_CAERMA_H
#define AR_SCENE_RENDER_CAERMA_H

#include <glm/mat4x4.hpp>

// 此头文件，定义相机的概念
namespace XRAppOS {

    // AR渲染场景中的相机
    // 包含渲染相机的内参
    class XRRenderCamera {
    public:

        ~XRRenderCamera() {}

        inline bool
        UpdateCameraProperty(const std::string &property_name, const void *property_value) {
            if (property_name.empty() || !property_value) {
                return false;
            }
            bool  result = UpdateCameraPropertyInRenderer(property_name, property_value);
            if (result) {
                camera_property_map[property_name] = property_value;
            }
            return result;
        }

        inline const void *GetCameraProperty(const std::string &property_name) {
            if (property_name.empty()) {
                return nullptr;
            }

            return camera_property_map[property_name];
        }

        // 返回引擎层的camera实例
        // 完全由子类，根据其所处的引擎环境来决定相应的内部实例；
        // 只会由引擎环境内部调用；对上层无实意，禁止上层调用；
        virtual inline const void *GetCameraInstanceInRenderer() {
            return nullptr;
        }

    protected:
        friend XRRenderNode;

        virtual inline bool
        UpdateCameraPropertyInRenderer(const std::string &property_name,
                                       const void *property_value) {
            return true;
        }

        virtual bool onAttachedToNode(std::shared_ptr<XRRenderNode> attached_arsr_node) {
            return true;
        }

        virtual void onDetachedFromNode() {

        }

        XRRenderCamera() : projected_matrix() {}

        // 相机内参计算得出的投影矩阵
        glm::mat4 projected_matrix;

    private:

        // 相机相关参数
        std::map<const std::string, const void *> camera_property_map;
    };
}

#endif// end AR_SCENE_RENDER_CAERMA_H