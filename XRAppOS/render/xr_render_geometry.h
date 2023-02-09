#ifndef AR_SCENE_RENDER_GEOMETRY_H
#define AR_SCENE_RENDER_GEOMETRY_H

//#include "ar_scene_render_material_property.h"
#include <string>
#include <map>
#include <XRAppOS/XRAppOSCommon.h>
// 此头文件，定义3D形状的概念
namespace XRAppOS {
    class XRRenderer;

    class XRRenderMaterial;

    class XRRenderNode;

    // 可渲染的3D几何体(形状)
    class XRAPPOS_PUBLIC  XRRenderGeometry {
        friend XRRenderer;
        friend XRRenderNode;
    public:
        ~XRRenderGeometry() {}

        const size_t GetMaterialCount() {
            return material_map.size();
        }

        bool SetMaterial(const int &index, const std::shared_ptr<XRRenderMaterial> material);

        bool
        GetMaterialByName(const int &index, std::shared_ptr<XRRenderMaterial> output_material) {
            return false;
        };

        bool
        UpdateGeometryProperty(const std::string &property_name, const void *property_value);

        inline const void *GetGeometryProperty(const std::string &property_name) {
            if (property_name.empty()) {
                return nullptr;
            }

            return geometry_property_map[property_name];
        }

    protected:
        // 只能通过ARSRRenderer或该类子类创建
        // 确保ARSR统一渲染API在创建时，ARSRRenderer或该类子类，
        // 能够有机会创建特属于渲染引擎相关的部分。
        XRRenderGeometry()
                : material_map(),
                  geometry_property_map() {
        }

        inline virtual bool UpdateGeometryPropertyInRenderer(const std::string &property_name,
                                                             const void *property_value) {
            return true;
        }

        inline virtual bool SetMaterialInRenderer(const int &index,
                                                  const std::shared_ptr<XRRenderMaterial> material) {
            return true;
        }

        virtual bool onAttachedToNode() {
            return true;
        }

        virtual void onDetachedFromNode() {

        }

        std::map<const int, std::shared_ptr<XRRenderMaterial>> material_map;

    private:

        // 3D形状相关参数
        // 例如，filament引擎中的cast shadows属性
        std::map<const std::string, const void *> geometry_property_map;
    };
}

#endif// end AR_SCENE_RENDER_MATERIAL_PROPERTY_H