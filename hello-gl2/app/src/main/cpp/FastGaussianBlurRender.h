//
// Created by malk on 24/11/18.
//

#ifndef HELLO_GL2_FASTGAUSSIANBLURRENDER_H
#define HELLO_GL2_FASTGAUSSIANBLURRENDER_H


#include <GLES2/gl2.h>

class FastGaussianBlurRender {
public:
    FastGaussianBlurRender();
    ~FastGaussianBlurRender();
    void setFrame(int wight, int height, int texID);
    void drawFrame();


    struct ShaderUniforms {
        GLuint sample;
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint iResolution;
        GLuint kernelSize;
        GLuint sigma;
    };

    struct ShaderAttributes {
        GLuint position;
        GLuint texCoords;
    };

private:
    int mWidth = 0;
    int mHeight = 0;
    GLuint gTexId;
    GLuint gBlurProgram;
    ShaderAttributes gBlurAttrs;
    ShaderUniforms gBlurSampleUniforms;
};


#endif //HELLO_GL2_FASTGAUSSIANBLURRENDER_H
