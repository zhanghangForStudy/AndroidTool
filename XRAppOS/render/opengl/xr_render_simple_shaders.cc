#include "xr_render_simple_shaders.h"

namespace XRAppOS {

// This macro converts everything between its parentheses to a std::string.
// Using this instead of R"()" preserves C-like syntax coloring in most
// editors, which is desirable for shaders.


    const GLchar* const kMediaPipeVertexShaderPreamble = VERTEX_PREAMBLE;
    const GLchar* const kMediaPipeFragmentShaderPreamble = FRAGMENT_PREAMBLE;

    const GLchar* const kBasicVertexShader = VERTEX_PREAMBLE _STRINGIFY(
            // vertex position in clip space (-1..1)
            in vec4 position;

    // texture coordinate for each vertex in normalized texture space (0..1)
    in mediump vec4 texture_coordinate;

    // texture coordinate for fragment shader (will be interpolated)
    out mediump vec2 sample_coordinate;

    void main() {
        gl_Position = position;
        sample_coordinate = texture_coordinate.xy;
    });

    const GLchar* const kScaledVertexShader = VERTEX_PREAMBLE _STRINGIFY(
            in vec4 position; in mediump vec4 texture_coordinate;
    out mediump vec2 sample_coordinate; uniform vec4 scale;

    void main() {
        gl_Position = position * scale;
        sample_coordinate = texture_coordinate.xy;
    });

    const GLchar* const kBasicTexturedFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)

    in mediump vec2 sample_coordinate;  // texture coordinate (0..1)
    uniform sampler2D video_frame;

    void main() {
        gl_FragColor = texture2D(video_frame, sample_coordinate);
    });

    const GLchar* const kBasicTexturedFlipXFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)

    in mediump vec2 sample_coordinate;  // texture coordinate (0..1)
    uniform sampler2D video_frame;

    void main() {
        gl_FragColor = texture2D(video_frame, vec2(sample_coordinate.x, sample_coordinate.y));
    });

    const GLchar* const kOESTexturedForAceTinyFragmentShader = FRAGMENT_USE_OES_TEXTURE _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)
    in mediump vec2 sample_coordinate;  // texture coordinate (0..1)
    uniform samplerExternalOES video_frame;

    void main() {
//                gl_FragColor = texture2D(video_frame, vec2(1.0 - sample_coordinate.x, sample_coordinate.y));
        // acetiny引擎需要做以下线性转换，否则画面偏白
        vec4 cameraSample =  texture2D(video_frame, vec2(1.0 - sample_coordinate.x, sample_coordinate.y));
        vec3 c = cameraSample.rgb;
        vec3 linearRGBLo = c / 12.92;
        vec3 linearRGBHi = pow((c + 0.055) / 1.055, vec3(2.4, 2.4, 2.4));

        c.r = c.r <= 0.04045 ? linearRGBLo.r : linearRGBHi.r;
        c.g = c.g <= 0.04045 ? linearRGBLo.g : linearRGBHi.g;
        c.b = c.b <= 0.04045 ? linearRGBLo.b : linearRGBHi.b;
        gl_FragColor = vec4(c.rgb, 1.0);
    });

    const GLchar* const kBasicTexturedFlipYFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)

    in mediump vec2 sample_coordinate;  // texture coordinate (0..1)
    uniform sampler2D video_frame;
#if 0
    vec3 Inverse_Tonemap_Unreal(const vec3 x) {
              return (x * -0.155) / (x - 1.019);
         }

          vec3 inverseTonemapSRGB(vec3 color) {
              // sRGB input
              color = clamp(color, 0.0, 1.0);
              return Inverse_Tonemap_Unreal(color);
          }

          vec3 inverseTonemap(vec3 linear) {
              // Linear input
              linear = clamp(linear, 0.0, 1.0);
              return Inverse_Tonemap_Unreal(pow(linear, vec3(1.0 / 2.2)));
          }

         void main() {
     gl_FragColor = vec4(inverseTonemapSRGB(texture2D(video_frame, vec2(sample_coordinate.x, 1.0 - sample_coordinate.y)).rgb),1.0);
         });

//     float linearToSrgb(float x) {
//         return x <= 0.0031308 ? 12.92 * x : 1.055 * pow(x, 0.41666) - 0.055;
//     }
//
//
//     vec3 linearToSrgb(vec3 color) {
//         return vec3(linearToSrgb(color.r), linearToSrgb(color.g), linearToSrgb(color.b));
//     }
//
//     void main() {
//    vec4 color = texture2D(video_frame, vec2(sample_coordinate.x, 1.0 - sample_coordinate.y));
//    gl_FragColor = vec4(linearToSrgb(color.rgb),color.a);
//
//           });

#else

    void main() {
        gl_FragColor = texture2D(video_frame, vec2(sample_coordinate.x, 1.0 - sample_coordinate.y));
    });

#endif

// ENABLE_TRANSPARE_BACKGROUND宏 仅用于生产背景透明的PNG文件，正式线上环境不要打开
//#define ENABLE_TRANSPARE_BACKGROUND
#ifdef  ENABLE_TRANSPARE_BACKGROUND
    const GLchar* const kBasicTexturedMergeBackgroundAnd3DRenderFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
        DEFAULT_PRECISION(mediump, float)

        in mediump vec2 sample_coordinate;  // texture coordinate (0..1)
        uniform sampler2D backgroundTexture;
        uniform sampler2D filamentTexture;

        void main() {
             vec4 backGroundColor = texture2D(backgroundTexture, vec2(sample_coordinate.x, sample_coordinate.y));
             vec4 filamentColor = texture2D(filamentTexture, vec2(sample_coordinate.x, 1.0 - sample_coordinate.y));
            //根据输入的背景图，判断输出是否需要透明背景，目前暂时无局部透明背景功能
            if(backGroundColor.a < 1.0)
            {
                gl_FragColor  = filamentColor;
            }
            else
            {
                vec3 result = backGroundColor.rgb * (1.0 - filamentColor.a) + filamentColor.rgb;
                gl_FragColor  = vec4(result,backGroundColor.a);
            }

        });

#else
//用于3D纹理与背景混合
    const GLchar* const kBasicTexturedMergeBackgroundAnd3DRenderFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)

    in mediump vec2 sample_coordinate;  // texture coordinate (0..1)
    uniform sampler2D backgroundTexture;
    uniform sampler2D filamentTexture;


    void main() {
        vec4 backGroundColor = texture2D(backgroundTexture, vec2(sample_coordinate.x, sample_coordinate.y));
        vec4 filamentColor = texture2D(filamentTexture, vec2(sample_coordinate.x, 1.0 - sample_coordinate.y));
        vec3 result = backGroundColor.rgb * (1.0 - filamentColor.a) + filamentColor.rgb;
        gl_FragColor  = vec4(result,backGroundColor.a);//vec4(mix(backGroundColor.rgb,filamentColor.rgb,filamentColor.a),backGroundColor.a);
    });


#endif

    const GLchar* const kBasicTexturedFlippedFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)

    in mediump vec2 sample_coordinate;  // texture coordinate (0..1)
    uniform sampler2D video_frame;

    void main() {
        gl_FragColor = texture2D(video_frame, vec2(1.0 - sample_coordinate.x, 1.0 - sample_coordinate.y));
    });

    const GLchar* const kBasicTexturedFragmentShaderOES = FRAGMENT_PREAMBLE
    "#extension GL_OES_EGL_image_external : require\n" _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)

    in mediump vec2 sample_coordinate;  // texture coordinate (0..1)
    uniform samplerExternalOES video_frame;

    void main() {
        gl_FragColor = texture2D(video_frame, sample_coordinate);
    });

    const GLchar* const kFlatColorFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)

    uniform vec3 color;  // r,g,b color com.taobao.cameralink.components

    void main() { gl_FragColor = vec4(color.r, color.g, color.b, 1.0); });

    const GLchar* const kRgbWeightFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)

    in mediump vec2 sample_coordinate;  // texture coordinate (0..1)
    uniform sampler2D video_frame; uniform vec3 weights;  // r,g,b weights

    void main() {
        vec4 color = texture2D(video_frame, sample_coordinate);
        gl_FragColor.bgra = vec4(weights.z * color.b, weights.y * color.g,
                                 weights.x * color.r, color.a);
    });

    const GLchar* const kYUV2TexToRGBFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
            DEFAULT_PRECISION(mediump, float)

    in highp vec2 sample_coordinate;
    uniform sampler2D video_frame_y; uniform sampler2D video_frame_uv;

    void main() {
        mediump vec3 yuv;
        lowp vec3 rgb;
        yuv.r = texture2D(video_frame_y, sample_coordinate).r;
        // Subtract (0.5, 0.5) because conversion is done assuming UV color
        // midpoint of (128, 128).
        yuv.gb = texture2D(video_frame_uv, sample_coordinate).rg - vec2(0.5, 0.5);
        // Using BT.709 which is the standard for HDTV.
        rgb = mat3(1, 1, 1, 0, -0.18732, 1.8556, 1.57481, -0.46813, 0) * yuv;
        gl_FragColor = vec4(rgb, 1);
    });

// YUV 有多种格式 400, 420..
// Y, U, V三块分别存储的:
// 参考https://www.cnblogs.com/tristansun/archive/2013/06/15/3137659.html
// YUV连续分布的
// Android原生转换的
//#ifdef __ANDROID__
    const GLchar *const kYuv2RgbFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY (
            DEFAULT_PRECISION(mediump, float)
    precision mediump
    float;
    varying vec2
    sample_coordinate;

    uniform sampler2D
    y_texture;
    uniform sampler2D
    uv_texture;
    uniform int isHD;

    const mat3 yuv2rgb_forHD =
            mat3(1, 1, 1, 0, -0.343, 1.765,
                 1.400, -0.711, 0);

    const mat3 yuv2rgb_forSD =
            mat3(1, 1, 1, 0, -0.344, 1.772,
                 1.402, -0.714, 0);

    void main() {
        vec3 yuv;
        yuv.x = texture2D(y_texture,
                          sample_coordinate).r;
        yuv.yz = texture2D(uv_texture,
                           sample_coordinate).ar;
        vec3 rgb;
        if (isHD == 1)
            rgb = yuv2rgb_forHD *
                  (yuv - vec3(0.0, 0.5, 0.5));
        else
            rgb = yuv2rgb_forSD *
                  (yuv - vec3(0.0, 0.5, 0.5));
        gl_FragColor = vec4(rgb, 1.0);
    });
    //#else
//const GLchar *const kYuv2RgbFragmentShader = FRAGMENT_PREAMBLE _STRINGIFY(
//DEFAULT_PRECISION(mediump, float) precision mediump float;
//varying vec2 sample_coordinate;
//
//uniform sampler2D y_texture; uniform sampler2D uv_texture; uniform int isHD;
//
//const mat3 yuv2rgb_forHD =
//        mat3(1, 1, 1, 0, -0.343, 1.765, 1.400, -0.711, 0);
//
//const mat3 yuv2rgb_forSD =
//        mat3(1, 1, 1, 0, -0.344, 1.772, 1.402, -0.714, 0);
//
//void main() {
//    vec3 yuv;
//    yuv.x = texture2D(y_texture, sample_coordinate).r;
//    yuv.yz = texture2D(uv_texture, sample_coordinate).ra;
//    vec3 rgb;
//    if (isHD == 1)
//        rgb = yuv2rgb_forHD * (yuv - vec3(0.0, 0.5, 0.5));
//    else
//        rgb = yuv2rgb_forSD * (yuv - vec3(0.0, 0.5, 0.5));
//    gl_FragColor = vec4(rgb, 1.0);
//});
//#endif  //__ANDROID__

}  // namespace XRAppOS