//
// Created by malk on 24/11/18.
//

#ifndef HELLO_GL2_TWOSTEPGAUSSIANBLURRENDER_H
#define HELLO_GL2_TWOSTEPGAUSSIANBLURRENDER_H


#include <GLES2/gl2.h>

class TwoStepGaussianBlurRender {
public:
    TwoStepGaussianBlurRender();
    ~TwoStepGaussianBlurRender();
    void setFrame(int wight, int height, int texID);
    void drawFrame();


    struct ShaderUniforms {
        GLuint sample;
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint horizontal;
        GLuint weight;
        GLuint iResolution;
    };

    struct ShaderAttributes {
        GLuint position;
        GLuint texCoords;
    };
private:
    int mWidth = 0;
    int mHeight = 0;
    GLuint gTexId;
    GLuint VBO, EBO;
    GLuint gBlurProgram;
    GLuint pingpongFBO[2];
    GLuint pingpongBuffer[2];
    ShaderAttributes gBlurAttrs;
    ShaderUniforms gBlurSampleUniforms;
};


#endif //HELLO_GL2_TWOSTEPGAUSSIANBLURRENDER_H
