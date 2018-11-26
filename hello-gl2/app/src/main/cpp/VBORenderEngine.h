//
// Created by malk on 18-11-16.
//
#pragma once
#ifndef HELLO_GL2_VBORENDERENGINE_H
#define HELLO_GL2_VBORENDERENGINE_H

#endif //HELLO_GL2_VBORENDERENGINE_H
//
// Created by malk on 18-11-16.
//
#include "RenderEngine.h"
class VBORenderEngine : public RenderEngine {
public:
    VBORenderEngine();
    ~VBORenderEngine();
    virtual void drawFrame (GLuint texId, GLuint gProgram,
                    GLuint gvPositionHandle, GLuint gvTexCoordHandle) const override;
};