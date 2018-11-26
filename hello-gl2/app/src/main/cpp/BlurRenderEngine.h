//
// Created by malk on 18-11-17.
//

#ifndef HELLO_GL2_BLURRENDERENGINE_H
#define HELLO_GL2_BLURRENDERENGINE_H


#include <GLES2/gl2.h>

class BlurRenderEngine {
public:
    BlurRenderEngine();
    virtual ~BlurRenderEngine();
    virtual void drawFrame();
    virtual void setFrame(int w, int h, int texId);

    struct ShaderUniforms {
        GLuint sample;
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint invSize;
        GLuint iteration;
    };

    struct ShaderAttributes {
        GLuint position;
        GLuint texCoords;
    };

private:
    void downSample();
    void blurFilter();
    int mWidth = 0;
    int mHeight = 0;
    ShaderAttributes gDownSampleAttrs;
    ShaderAttributes gBlurAttrs;
    ShaderUniforms gDownSampleUniforms;
    ShaderUniforms gBlurSampleUniforms;
    GLuint gDownSampleProgram;
    GLuint gBlurProgram;
    GLuint gDownSampleTexture;
    GLuint gTexId;
    GLuint gDownSampleFBO;
    GLuint gBlurFilterFBO[2];
    GLuint gBlurFilterTex[2];
};


#endif //HELLO_GL2_BLURRENDERENGINE_H
