#include "xr_render_geometry.h"

namespace XRAppOS {
    bool
    XRRenderGeometry::UpdateGeometryProperty(const std::string &property_name, const void *property_value) {
        if (property_name.empty() || !property_value) {
            return false;
        }
        if (UpdateGeometryPropertyInRenderer(property_name, property_value)) {
            geometry_property_map[property_name] = property_value;
        }
        return true;
    }

    bool XRRenderGeometry::SetMaterial(const int &index,
                                       const std::shared_ptr <XRRenderMaterial> material) {
        if (!material) {
            return false;
        }

        if (SetMaterialInRenderer(index, material)) {
            // 生效成功
            material_map[index] = material;
            return true;
        }

        return false;
    }
}