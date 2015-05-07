#include "glextensions.h"

#define RESOLVE_GL_FUNC(f) ok &= bool((f = (_gl##f) context->getProcAddress(QLatin1String("gl" #f))));

bool GLExtensionFunctions::resolve(const QGLContext *context)
{
    bool ok = true;

    RESOLVE_GL_FUNC(GenFramebuffersEXT)
    RESOLVE_GL_FUNC(GenRenderbuffersEXT)
    RESOLVE_GL_FUNC(BindRenderbufferEXT)
    RESOLVE_GL_FUNC(RenderbufferStorageEXT)
    RESOLVE_GL_FUNC(DeleteFramebuffersEXT)
    RESOLVE_GL_FUNC(DeleteRenderbuffersEXT)
    RESOLVE_GL_FUNC(BindFramebufferEXT)
    RESOLVE_GL_FUNC(FramebufferTexture2DEXT)
    RESOLVE_GL_FUNC(FramebufferRenderbufferEXT)
    RESOLVE_GL_FUNC(CheckFramebufferStatusEXT)

    RESOLVE_GL_FUNC(ActiveTexture)
    RESOLVE_GL_FUNC(TexImage3D)

    RESOLVE_GL_FUNC(GenBuffers)
    RESOLVE_GL_FUNC(BindBuffer)
    RESOLVE_GL_FUNC(BufferData)
    RESOLVE_GL_FUNC(DeleteBuffers)
    RESOLVE_GL_FUNC(MapBuffer)
    RESOLVE_GL_FUNC(UnmapBuffer)

    return ok;
}

bool GLExtensionFunctions::fboSupported() {
    return GenFramebuffersEXT
            && GenRenderbuffersEXT
            && BindRenderbufferEXT
            && RenderbufferStorageEXT
            && DeleteFramebuffersEXT
            && DeleteRenderbuffersEXT
            && BindFramebufferEXT
            && FramebufferTexture2DEXT
            && FramebufferRenderbufferEXT
            && CheckFramebufferStatusEXT;
}

bool GLExtensionFunctions::openGL15Supported() {
    return ActiveTexture
            && TexImage3D
            && GenBuffers
            && BindBuffer
            && BufferData
            && DeleteBuffers
            && MapBuffer
            && UnmapBuffer;
}

#undef RESOLVE_GL_FUNC
