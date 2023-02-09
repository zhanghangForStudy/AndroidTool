#ifndef AR_SCENE_RENDER_LIGHT_H
#define AR_SCENE_RENDER_LIGHT_H

#include <map>
// 此头文件，定义光源的概念
namespace XRAppOS {
    enum XRRenderLightType {
        DIRECT_TYPE, //  平行光，直射光源
        IBL_TYPE // 基于图片的光源，例如环境光
    };

    // AR渲染场景中的光源
    class XRRenderLight {
    public:
        XRRenderLight(XRRenderLightType type = IBL_TYPE) : light_type(type) {}

        ~XRRenderLight() {}

        XRRenderLightType getLightType() {
            return light_type;
        }

        inline bool
        UpdateLightProperty(const std::string &property_name, const void *property_value) {
            if (property_name.empty() || !property_value) {
                return false;
            }
            if (UpdateLightPropertyInRenderer(property_name, property_value)) {
                light_property_map[property_name] = property_value;
            }
            return true;
        }

        virtual inline const void *GetLightProperty(const std::string &property_name) {
            if (property_name.empty()) {
                return nullptr;
            }

            return light_property_map[property_name];
        }

    protected:
        friend XRRenderNode;

        XRRenderLightType light_type;

        virtual inline bool
        UpdateLightPropertyInRenderer(const std::string &property_name,
                                      const void *property_value) {
            return true;
        }

        virtual bool onAttachedToNode() {
            return true;
        }

        virtual void onDetachedFromNode() {

        }

    private:

        // 光源相关参数
        std::map<const std::string, const void *> light_property_map;
    };
}

#endif// end AR_SCENE_RENDER_LIGHT_H