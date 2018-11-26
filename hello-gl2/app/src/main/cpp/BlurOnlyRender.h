//
// Created by malk on 23/11/18.
//

#ifndef HELLO_GL2_BLURONLYRENDER_H
#define HELLO_GL2_BLURONLYRENDER_H


#include <GLES2/gl2.h>

class BlurOnlyRender {
public:
    BlurOnlyRender();
    ~BlurOnlyRender();
    void setFrame(int width, int height, int texId);
    void drawFrame();

    struct ShaderUniforms {
        GLuint sample;
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint invSize;
        GLuint iteration;
        GLuint pixelWight;
        GLuint pixelHeight;
    };

    struct ShaderAttributes {
        GLuint position;
        GLuint texCoords;
    };

private:
    int mWidth = 0;
    int mHeight = 0;
    int gTexId;
    GLuint gBlurFilterFBO[2];
    GLuint gBlurFilterTex[2];
    GLuint gBlurProgram;
    ShaderAttributes gBlurAttrs;
    ShaderUniforms gBlurSampleUniforms;
};


#endif //HELLO_GL2_BLURONLYRENDER_H
