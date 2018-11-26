//
// Created by malk on 18-11-16.
//

#ifndef HELLO_GL2_DRAWARRAYRENDERENGINE_H
#define HELLO_GL2_DRAWARRAYRENDERENGINE_H

#endif //HELLO_GL2_DRAWARRAYRENDERENGINE_H
#include "RenderEngine.h"
class DrawArrayRenderEngine : public RenderEngine {
public:
    DrawArrayRenderEngine();
    ~DrawArrayRenderEngine();

    virtual void drawFrame (GLuint texId, GLuint gProgram,
                    GLuint gvPositionHandle, GLuint gvTexCoordHandle) const override;

private:

//    GLuint mTexId;
};