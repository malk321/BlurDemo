//
// Created by malk on 18-11-16.
//

#ifndef HELLO_GL2_RENDERENGINE_H
#define HELLO_GL2_RENDERENGINE_H

#endif //HELLO_GL2_RENDERENGINE_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class RenderEngine {
public:
    RenderEngine ();
    virtual ~RenderEngine() = 0;
    virtual void drawFrame(GLuint texId, GLuint gProgram,
                           GLuint gvPositionHandle, GLuint gvTexCoordHandle) const;
};