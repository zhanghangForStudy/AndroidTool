//
//
//  Created by soulghost on 2022/6/30.
//

#ifndef XRValueBox_hpp
#define XRValueBox_hpp

#include <string>
#include <map>
#include <vector>
#include "XRAppOS/XRAppOSCommon.h"
#include "glm/ext.hpp"

//#include "XRCameraFrame.h"

enum XRValueBoxType {
    // 基本类型
    XRValueBoxTypeString = 0,
    XRValueBoxTypeUnsignedInt16,
    XRValueBoxTypeUnsignedInt64,
    XRValueBoxTypeInt,
    XRValueBoxTypeUnsignedInt,
    XRValueBoxTypeLong,
    XRValueBoxTypeFloat,
    XRValueBoxTypeDouble,
    XRValueBoxTypeAddr,
    XRValueBoxTypeNull,
    // 容器类型
    XRValueBoxTypeGlmVec2,
    XRValueBoxTypeGlmVec3,
    XRValueBoxTypeGlmVec4,
    XRValueBoxTypeGlmMat4,
    XRValueBoxTypeOpacity,
    XRValueBoxTypeFloatVector,
    XRValueBoxTypeMap,
    XRValueBoxTypeVector,
    XRValueBoxTypeOpacityObject,
    // 复合类型
    XRValueBoxTypeImage,
    XRValueBoxTypeModelInfo,
    XRValueBoxTypeXRFrame,
    XRValueBoxTypePlaneInfo,
    XRValueBoxTypeJNIObject,
    XRValueBoxTypeHitTestResult,
    XRValueBoxTypeMesh,
    XRValueBoxTypeHitResultInfo,
    XRValueBoxTypeAnchor,
    XRValueBoxTypePlaneAnchor,
    XRValueBoxTypeRay,
    XRValueBoxTypeAABB,
    // 未知
    XRValueBoxTypeUnknown,

};

namespace XRAppOS {

    class XRAPPOS_PUBLIC XRTensorValue {
    public:
        std::vector<size_t> shape;

        // 智能指针管控，避免深度copy
        std::shared_ptr<std::vector<uint8_t>> rawData;

        inline size_t width() const {
            return shape.size() <= 0 ? 0 : shape[0];
        }

        inline size_t height() const {
            return shape.size() <= 1 ? 0 : shape[1];
        }

        inline const uint8_t *data() const {
            return rawData ? rawData->data() : nullptr;
        }
    };

    class XRAPPOS_PUBLIC XRImage : public XRTensorValue {
    public:
        // 与MNN::CV::ImageFormat一致
        int format = 0;

        static float getChannelByFormat(const XRImage &image);
    };

    class XROpacityValue {
    public:
        std::string realType;
        void *data;

        virtual ~XROpacityValue() {};
    };

    class XRAPPOS_PUBLIC XRValueBox {
    public:
        const XRValueBoxType getType() const {
            return type;
        }

        static std::shared_ptr<XRValueBox> createNull() {
            auto result = std::make_shared<XRValueBox>();
            result->type = XRValueBoxTypeNull;
            return result;
        }

    protected:
        XRValueBoxType type = XRValueBoxTypeUnknown;
    };

    typedef std::map<std::string, std::shared_ptr<XRValueBox>> XRValueMap;

    typedef std::vector<std::shared_ptr<XRValueBox>> XRValueVector;

    typedef std::shared_ptr<XROpacityValue> XRValueOpacityObject;

    template<typename T>
    class XRValueBoxInner : public XRValueBox {
    public:
        XRValueBoxInner(T &originValue) {

        }

        // 该方法只能在本头文件之中使用
        void setType(XRValueBoxType _type) {
            type = _type;
        }

        std::shared_ptr<T> toOriginValue() const {
            return nullptr;
        }

    private:
        std::shared_ptr<T> value;
    };

#define DEFINE_XRVALUEBOX_TYPE(T, TYPE)                     \
    template<>                                              \
    class XRValueBoxInner<T> : public XRValueBox {          \
    public:                                                 \
        XRValueBoxInner(T& originValue) : XRValueBox(){     \
             value = std::make_shared<T>(originValue);      \
             type = TYPE;                                   \
        };                                                  \
                                                            \
        void setType(XRValueBoxType _type) {                \
            type = _type;                                   \
        }                                                   \
                                                            \
        std::shared_ptr<T> toOriginValue() const {          \
            return value;                                   \
        }                                                   \
                                                            \
    private:                                                \
        std::shared_ptr<T> value;                           \
    }                                                       \


    DEFINE_XRVALUEBOX_TYPE(std::string, XRValueBoxTypeString);

    DEFINE_XRVALUEBOX_TYPE(uint16_t, XRValueBoxTypeUnsignedInt16);

    DEFINE_XRVALUEBOX_TYPE(int, XRValueBoxTypeInt);

    DEFINE_XRVALUEBOX_TYPE(uint32_t, XRValueBoxTypeUnsignedInt);

    DEFINE_XRVALUEBOX_TYPE(uint64_t, XRValueBoxTypeUnsignedInt64);

    DEFINE_XRVALUEBOX_TYPE(long, XRValueBoxTypeLong);

    DEFINE_XRVALUEBOX_TYPE(double, XRValueBoxTypeDouble);

    DEFINE_XRVALUEBOX_TYPE(float, XRValueBoxTypeFloat);

    DEFINE_XRVALUEBOX_TYPE(void*, XRValueBoxTypeAddr);

    DEFINE_XRVALUEBOX_TYPE(glm::vec2, XRValueBoxTypeGlmVec2);

    DEFINE_XRVALUEBOX_TYPE(glm::vec3, XRValueBoxTypeGlmVec3);

    DEFINE_XRVALUEBOX_TYPE(glm::vec4, XRValueBoxTypeGlmVec4);

    DEFINE_XRVALUEBOX_TYPE(glm::mat4, XRValueBoxTypeGlmMat4);

    DEFINE_XRVALUEBOX_TYPE(XROpacityValue, XRValueBoxTypeOpacity);

    DEFINE_XRVALUEBOX_TYPE(std::vector<float>, XRValueBoxTypeFloatVector);

    DEFINE_XRVALUEBOX_TYPE(XRImage, XRValueBoxTypeImage);

//    DEFINE_XRVALUEBOX_TYPE(ModelInfo, XRValueBoxTypeModelInfo);

    DEFINE_XRVALUEBOX_TYPE(XRValueMap, XRValueBoxTypeMap);

    DEFINE_XRVALUEBOX_TYPE(XRValueVector, XRValueBoxTypeVector);

//    DEFINE_XRVALUEBOX_TYPE(PlaneInfo, XRValueBoxTypePlaneInfo);
//
//    DEFINE_XRVALUEBOX_TYPE(HitResultInfo, XRValueBoxTypeHitResultInfo);


    DEFINE_XRVALUEBOX_TYPE(XRValueOpacityObject, XRValueBoxTypeOpacityObject);


//    DEFINE_XRVALUEBOX_TYPE(XRAnchor, XRValueBoxTypeAnchor);
//
//    DEFINE_XRVALUEBOX_TYPE(XRPlaneAnchor, XRValueBoxTypePlaneAnchor);
//
//    DEFINE_XRVALUEBOX_TYPE(XRRay, XRValueBoxTypeRay);
//
//    DEFINE_XRVALUEBOX_TYPE(XRAABB, XRValueBoxTypeAABB);
//
//
//    DEFINE_XRVALUEBOX_TYPE(XRMesh, XRValueBoxTypeMesh);

//    DEFINE_XRVALUEBOX_TYPE(XRAppOS::XRCameraFrame, XRValueBoxTypeXRFrame);

    template<typename T>
    std::shared_ptr<T> XRValueBox_As(const XRValueBox *box, XRValueBoxType type) {
        if (!box) {
            return std::make_shared<T>();
        }
        // 如果类型不等，说明该API使用错误
        // 较严重的错误，所以直接通过asset暴露出来
        assert(box->getType() == type);
        auto boxInner = static_cast<const XRValueBoxInner<T> *>(box);
        auto result = boxInner->toOriginValue();
        return std::static_pointer_cast<T>(result);
    };

    template<typename T>
    T &XRValueBox_AsOriginValue(const XRValueBox *box, XRValueBoxType type) {
        auto result = XRValueBox_As<T>(box, type);
        return *result;
    }

    template<typename T>
    T &XRValueBox_AsOriginValue(std::shared_ptr<XRValueBox> box, XRValueBoxType type) {
        return XRValueBox_AsOriginValue < T > (box.get(), type);
    }

    // 使用前，确保模板类型T通过DEFINE_XRVALUEBOX_TYPE注册过
    template<typename T>
    std::shared_ptr<XRValueBox> XRValueBox_create(T originValue) {
        auto result = new XRValueBoxInner<T>(originValue);
        return std::shared_ptr<XRValueBox>(result);
    };

    // 使用前，确保模板类型T通过DEFINE_XRVALUEBOX_TYPE注册过
    // 此API用以解决originValue类型一致，但是具体的语义类型(XRValueBoxType)不一致的情况
    template<typename T>
    std::shared_ptr<XRValueBox> XRValueBox_create(T originValue, XRValueBoxType realType) {
        auto result = XRValueBox_create < T > (originValue);
        if (realType == XRValueBoxTypeJNIObject) {
            static_assert(std::is_same<long, T>::value, "jni object must be a long");
            auto resultInner = std::static_pointer_cast<XRValueBoxInner<T>>(result);
            resultInner->setType(realType);
        }
        return result;
    };
};

#endif /* XRValueBox_hpp */
