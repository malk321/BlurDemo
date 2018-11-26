//
// Created by malk on 18-11-16.
//

#ifndef HELLO_GL2_DATAARRAYRENDERENGINE_H
#define HELLO_GL2_DATAARRAYRENDERENGINE_H

#include "RenderEngine.h"
class DataArrayRenderEngine : public RenderEngine {
public:
    DataArrayRenderEngine();
    ~DataArrayRenderEngine();
    virtual void drawFrame (GLuint texId, GLuint gProgram,
                            GLuint gvPositionHandle, GLuint gvTexCoordHandle) const override;
};


#endif //HELLO_GL2_DATAARRAYRENDERENGINE_H
