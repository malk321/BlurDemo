//
// Created by malk on 23/11/18.
//

#include <GLES2/gl2.h>

#ifndef HELLO_GL2_GAUSSIANBLURRENDER_H
#define HELLO_GL2_GAUSSIANBLURRENDER_H

#endif //HELLO_GL2_GAUSSIANBLURRENDER_H

class GaussianBlurRender {
public:
    GaussianBlurRender();
    ~GaussianBlurRender();
    void setFrame(int wight, int height, int texID);
    void drawFrame();

    struct ShaderUniforms {
        GLuint sample;
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint iResolution;
        GLuint iteration;
    };

    struct ShaderAttributes {
        GLuint position;
        GLuint texCoords;
    };

private:
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