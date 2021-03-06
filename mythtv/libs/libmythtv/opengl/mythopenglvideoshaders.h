#ifndef MYTH_OPENGLVIDEOSHADERS_H
#define MYTH_OPENGLVIDEOSHADERS_H

#include <QString>

static const QString DefaultVertexShader =
"attribute highp vec2 a_position;\n"
"attribute highp vec2 a_texcoord0;\n"
"varying   highp vec2 v_texcoord0;\n"
"uniform   highp mat4 u_projection;\n"
"void main()\n"
"{\n"
"    gl_Position = u_projection * vec4(a_position, 0.0, 1.0);\n"
"    v_texcoord0 = a_texcoord0;\n"
"}\n";

static const QString RGBFragmentShader =
"uniform sampler2D s_texture0;\n"
"varying highp vec2 v_texcoord0;\n"
"void main(void)\n"
"{\n"
"    highp vec4 color = texture2D(s_texture0, v_texcoord0);\n"
"    gl_FragColor = vec4(color.rgb, 1.0);\n"
"}\n";

#ifdef USING_MEDIACODEC
static const QString MediaCodecVertexShader =
"attribute highp vec2 a_position;\n"
"attribute highp vec2 a_texcoord0;\n"
"varying   highp vec2 v_texcoord0;\n"
"uniform   highp mat4 u_projection;\n"
"uniform   highp mat4 u_transform;\n"
"void main()\n"
"{\n"
"    gl_Position = u_projection * vec4(a_position, 0.0, 1.0);\n"
"    v_texcoord0 = (u_transform * vec4(a_texcoord0, 0.0, 1.0)).xy;\n"
"}\n";
#endif

// these need to come first but obviously after the defines...
static const QString YUVFragmentExtensions =
"#define LINEHEIGHT m_frameData.x\n"
"#define COLUMN     m_frameData.y\n"
"#define MAXHEIGHT  m_frameData.z\n"
"#define FIELDSIZE  m_frameData.w\n"
"#ifdef MYTHTV_RECTS\n"
"#extension GL_ARB_texture_rectangle : enable\n"
"#define texture2D texture2DRect\n"
"#define sampler2D sampler2DRect\n"
"#endif\n"
"#ifdef MYTHTV_EXTOES\n"
"#extension GL_OES_EGL_image_external : require\n"
"#define sampler2D samplerExternalOES\n"
"#endif\n";

static const QString YUVFragmentShader =
"uniform highp mat4 m_colourMatrix;\n"
"uniform highp vec4 m_frameData;\n"
"varying highp vec2 v_texcoord0;\n"
"#ifdef MYTHTV_COLOURMAPPING\n"
"uniform highp mat4 m_primaryMatrix;\n"
"uniform highp float m_colourGamma;\n"
"uniform highp float m_displayGamma;\n"
"highp vec4 ColourMap(highp vec4 color)\n"
"{\n"
"    highp vec4 res = clamp(color, 0.0, 1.0);\n"
"    res.rgb = pow(res.rgb, vec3(m_colourGamma));\n"
"    res = m_primaryMatrix * res;\n"
"    return vec4(pow(res.rgb, vec3(m_displayGamma)), res.a);\n"
"}\n"
"#endif\n"

// Chroma for lines 1 and 3 comes from line 1-2
// Chroma for lines 2 and 4 comes from line 3-4
// This is a simple resample that ensures temporal consistency. A more advanced
// multitap filter would smooth the chroma - but at significant cost and potentially
// undesirable loss in detail.
"#ifdef MYTHTV_CUE\n"
"highp vec2 chromaLocation(highp vec2 xy)\n"
"{\n"
"    highp float temp = xy.y * FIELDSIZE;\n"
"    highp float onetwo = min((floor(temp / 2.0) / (FIELDSIZE / 2.0)) + LINEHEIGHT, MAXHEIGHT);\n"
"    return vec2(xy.x, mix(onetwo, min(onetwo + (2.0 * LINEHEIGHT), MAXHEIGHT), step(0.5, fract(temp))));\n"
"}\n"
"#endif\n"

"#ifdef MYTHTV_NV12\n"
"highp vec4 sampleYUV(in sampler2D texture1, in sampler2D texture2, highp vec2 texcoord)\n"
"{\n"
"#ifdef MYTHTV_CUE\n"
"#ifdef MYTHTV_RECTS\n"
// TODO this needs checking
"    return vec4(texture2D(texture1, texcoord).r, texture2D(texture2, chromaLocation(texcoord) * vec2(0.5, 0.5)).rg, 1.0);\n"
"#else\n"
"    return vec4(texture2D(texture1, texcoord).r, texture2D(texture2, chromaLocation(texcoord)).rg, 1.0);\n"
"#endif\n"
"#else\n"
"#ifdef MYTHTV_RECTS\n"
"    return vec4(texture2D(texture1, texcoord).r, texture2D(texture2, texcoord * vec2(0.5, 0.5)).rg, 1.0);\n"
"#else\n"
"    return vec4(texture2D(texture1, texcoord).r, texture2D(texture2, texcoord).rg, 1.0);\n"
"#endif\n"
"#endif\n"
"}\n"
"#endif\n"

"#ifdef MYTHTV_YV12\n"
"highp vec4 sampleYUV(in sampler2D texture1, in sampler2D texture2, in sampler2D texture3, highp vec2 texcoord)\n"
"{\n"
"#ifdef MYTHTV_CUE\n"
"    highp vec2 field = chromaLocation(texcoord);\n"
"    return vec4(texture2D(texture1, texcoord).r,\n"
"                texture2D(texture2, field).r,\n"
"                texture2D(texture3, field).r,\n"
"                1.0);\n"
"#else\n"
"    return vec4(texture2D(texture1, texcoord).r,\n"
"                texture2D(texture2, texcoord).r,\n"
"                texture2D(texture3, texcoord).r,\n"
"                1.0);\n"
"#endif\n"
"}\n"
"#endif\n"

"#ifdef MYTHTV_YUY2\n"
"highp vec4 sampleYUV(in sampler2D texture1, highp vec2 texcoord)\n"
"{\n"
"    return texture2D(texture1, texcoord);\n"
"}\n"
"#endif\n"

"#ifdef MYTHTV_KERNEL\n"
"highp vec4 kernel(in highp vec4 yuv, sampler2D kernelTex0, sampler2D kernelTex1)\n"
"{\n"
"    highp vec2 twoup   = vec2(v_texcoord0.x, max(v_texcoord0.y - (2.0 * LINEHEIGHT), LINEHEIGHT));\n"
"    highp vec2 twodown = vec2(v_texcoord0.x, min(v_texcoord0.y + (2.0 * LINEHEIGHT), MAXHEIGHT));\n"
"    yuv *=  0.125;\n"
"    yuv +=  0.125  * sampleYUV(kernelTex1, v_texcoord0);\n"
"    yuv +=  0.5    * sampleYUV(kernelTex0, vec2(v_texcoord0.x, max(v_texcoord0.y - LINEHEIGHT, LINEHEIGHT)));\n"
"    yuv +=  0.5    * sampleYUV(kernelTex0, vec2(v_texcoord0.x, min(v_texcoord0.y + LINEHEIGHT, MAXHEIGHT)));\n"
"    yuv += -0.0625 * sampleYUV(kernelTex0, twoup);\n"
"    yuv += -0.0625 * sampleYUV(kernelTex0, twodown);\n"
"    yuv += -0.0625 * sampleYUV(kernelTex1, twoup);\n"
"    yuv += -0.0625 * sampleYUV(kernelTex1, twodown);\n"
"    return yuv;\n"
"}\n"
"#endif\n"

"void main(void)\n"
"{\n"
"#ifdef MYTHTV_ONEFIELD\n"
"#ifdef MYTHTV_TOPFIELD\n"
"    highp float field = min(v_texcoord0.y + (step(0.5, fract(v_texcoord0.y * FIELDSIZE))) * LINEHEIGHT, MAXHEIGHT);\n"
"#else\n"
"    highp float field = max(v_texcoord0.y + (step(0.5, 1.0 - fract(v_texcoord0.y * FIELDSIZE))) * LINEHEIGHT, 0.0);\n"
"#endif\n"
"    highp vec4 yuv = sampleYUV(s_texture0, vec2(v_texcoord0.x, field));\n"
"#else\n"
"#ifdef MYTHTV_KERNEL\n"
"    highp vec4 yuv = sampleYUV(s_texture1, v_texcoord0);\n"
"#else\n"
"    highp vec4 yuv = sampleYUV(s_texture0, v_texcoord0);\n"
"#endif\n"
"#endif\n"

"#ifdef MYTHTV_KERNEL\n"
"#ifdef MYTHTV_TOPFIELD\n"
"    yuv = mix(kernel(yuv, s_texture1, s_texture2), yuv, step(fract(v_texcoord0.y * FIELDSIZE), 0.5));\n"
"#else\n"
"    yuv = mix(yuv, kernel(yuv, s_texture1, s_texture0), step(fract(v_texcoord0.y * FIELDSIZE), 0.5));\n"
"#endif\n"
"#endif\n"

"#ifdef MYTHTV_LINEARBLEND\n"
"    highp vec4 above = sampleYUV(s_texture0, vec2(v_texcoord0.x, min(v_texcoord0.y + LINEHEIGHT, MAXHEIGHT)));\n"
"    highp vec4 below = sampleYUV(s_texture0, vec2(v_texcoord0.x, max(v_texcoord0.y - LINEHEIGHT, 0.0)));\n"
"#ifdef MYTHTV_TOPFIELD\n"
"    yuv = mix(mix(above, below, 0.5), yuv, step(fract(v_texcoord0.y * FIELDSIZE), 0.5));\n"
"#else\n"
"    yuv = mix(yuv, mix(above, below, 0.5), step(fract(v_texcoord0.y * FIELDSIZE), 0.5));\n"
"#endif\n"
"#endif\n"

"#ifdef MYTHTV_YUY2\n"
"    gl_FragColor = vec4(mix(yuv.arb, yuv.grb, step(fract(v_texcoord0.x * COLUMN), 0.5)), 1.0) * m_colourMatrix;\n"
"#else\n"
"    gl_FragColor = yuv * m_colourMatrix;\n"
"#ifdef MYTHTV_COLOURMAPPING\n"
"    gl_FragColor = ColourMap(gl_FragColor);\n"
"#endif\n"
"#endif\n"
"}\n";

// N.B. Currently unused
static const QString BicubicShader =
"uniform sampler2D s_texture0;\n"
"uniform sampler2D s_texture1;\n"
"varying highp vec2 v_texcoord0;\n"
"void main(void)\n"
"{\n"
"    highp vec2 coord = (v_texcoord0 * vec2(%BICUBICWIDTH%, %BICUBICHEIGHT%)) - vec2(0.5, 0.5);\n"
"    highp vec4 parmx = texture2D(s_texture1, vec2(coord.x, 0.0));\n"
"    highp vec4 parmy = texture2D(s_texture1, vec2(coord.y, 0.0));\n"
"    highp vec2 e_x = vec2(%BICUBICCWIDTH%, 0.0);\n"
"    highp vec2 e_y = vec2(0.0, LINEHEIGHT);\n"
"    highp vec2 coord10 = v_texcoord0 + parmx.x * e_x;\n"
"    highp vec2 coord00 = v_texcoord0 - parmx.y * e_x;\n"
"    highp vec2 coord11 = coord10     + parmy.x * e_y;\n"
"    highp vec2 coord01 = coord00     + parmy.x * e_y;\n"
"    coord10            = coord10     - parmy.y * e_y;\n"
"    coord00            = coord00     - parmy.y * e_y;\n"
"    highp vec4 tex00   = texture2D(s_texture0, coord00);\n"
"    highp vec4 tex10   = texture2D(s_texture0, coord10);\n"
"    highp vec4 tex01   = texture2D(s_texture0, coord01);\n"
"    highp vec4 tex11   = texture2D(s_texture0, coord11);\n"
"    tex00        = mix(tex00, tex01, parmy.z);\n"
"    tex10        = mix(tex10, tex11, parmy.z);\n"
"    gl_FragColor = mix(tex00, tex10, parmx.z);\n"
"}\n";
#endif // MYTH_OPENGLVIDEOSHADERS_H
