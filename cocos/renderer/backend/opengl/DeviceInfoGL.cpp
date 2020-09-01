/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2020 c4games.com

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
 
#include "DeviceInfoGL.h"
#include "platform/CCGL.h"

#if !defined(GL_COMPRESSED_RGBA8_ETC2_EAC)
#define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
// #define GL_COMPRESSED_RGB8_ETC2 0x9274
#endif

#if !defined(GL_COMPRESSED_RGBA_ASTC_4x4)
#define GL_COMPRESSED_RGBA_ASTC_4x4 0x93B0
// #define GL_COMPRESSED_RGBA_ASTC_8x8 0x93B7
#endif

CC_BACKEND_BEGIN

bool DeviceInfoGL::init()
{
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_maxAttributes);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);
    _glExtensions = (const char*)glGetString(GL_EXTENSIONS);
    return true;
}

const char* DeviceInfoGL::getVendor() const
{
    return (const char*)glGetString(GL_VENDOR);
}
const char* DeviceInfoGL::getRenderer() const
{
    return (const char*)glGetString(GL_RENDERER);
}

const char* DeviceInfoGL::getVersion() const
{
    return (const char*)glGetString(GL_VERSION);
}

const char* DeviceInfoGL::getExtension() const
{
    return _glExtensions.c_str();
}

bool DeviceInfoGL::checkForFeatureSupported(FeatureType feature)
{
    bool featureSupported = false;
    switch (feature)
    {
    case FeatureType::ETC1:
        featureSupported = checkForGLExtension("GL_OES_compressed_ETC1_RGB8_texture");
        break;
    case FeatureType::ETC2:
        featureSupported = checkSupportsCompressedFormat(GL_COMPRESSED_RGBA8_ETC2_EAC);
        break;
    case FeatureType::S3TC:
#ifdef GL_EXT_texture_compression_s3tc
        featureSupported = checkForGLExtension("GL_EXT_texture_compression_s3tc");
#endif
        break;
    case FeatureType::AMD_COMPRESSED_ATC:
        featureSupported = checkForGLExtension("GL_AMD_compressed_ATC_texture");
        break;
    case FeatureType::PVRTC:
        featureSupported = checkForGLExtension("GL_IMG_texture_compression_pvrtc");
        break;
    case FeatureType::IMG_FORMAT_BGRA8888:
        featureSupported = checkForGLExtension("GL_IMG_texture_format_BGRA8888");
        break;
    case FeatureType::DISCARD_FRAMEBUFFER:
        featureSupported = checkForGLExtension("GL_EXT_discard_framebuffer");
        break;
    case FeatureType::PACKED_DEPTH_STENCIL:
        featureSupported = checkForGLExtension("GL_OES_packed_depth_stencil");
        break;
    case FeatureType::VAO:
#ifdef CC_PLATFORM_PC
        featureSupported = checkForGLExtension("vertex_array_object");
#else
        featureSupported = checkForGLExtension("GL_OES_vertex_array_object");
#endif
        break;
    case FeatureType::MAPBUFFER:
        featureSupported = checkForGLExtension("GL_OES_mapbuffer");
        break;
    case FeatureType::DEPTH24:
        featureSupported = checkForGLExtension("GL_OES_depth24");
        break;
    case FeatureType::ASTC:
        featureSupported = checkForGLExtension("GL_OES_texture_compression_astc") || checkSupportsCompressedFormat(GL_COMPRESSED_RGBA_ASTC_4x4);
        break;
    default:
        break;
    }
    return featureSupported;
}

bool DeviceInfoGL::checkForGLExtension(const std::string &searchName) const
{
    return _glExtensions.find(searchName) != std::string::npos;
}

bool DeviceInfoGL::checkSupportsCompressedFormat(int compressedFormat)
{
    const int MAX_ALLOCA_SIZE = 512;
    
    GLint numFormats = 0;
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats);
    GLint* formats = nullptr;
    int buffersize = numFormats * sizeof(GLint);
    if(buffersize <= MAX_ALLOCA_SIZE)
        formats = (GLint*)alloca(buffersize);
    else
        formats = (GLint*)malloc(buffersize);
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formats);

    bool supported = false;
    for (GLint i = 0; i < numFormats; ++i)
    {
        if (formats[i] == compressedFormat) {
            supported = true;
            break;
        }
    }

    if (buffersize > MAX_ALLOCA_SIZE)
        free(formats);

    return supported;
}

CC_BACKEND_END
