//
// Created by malk on 18-11-17.
//

#ifndef HELLO_GL2_CUBERENDERENGINE_H
#define HELLO_GL2_CUBERENDERENGINE_H

#include "RenderEngine.h"
class CubeRenderEngine : public RenderEngine{
public:
    CubeRenderEngine();
    virtual ~CubeRenderEngine();
    virtual void drawFrame (GLuint texId, GLuint gProgram,
                            GLuint gvPositionHandle, GLuint gvTexCoordHandle) const override;
};


#endif //HELLO_GL2_CUBERENDERENGINE_H
