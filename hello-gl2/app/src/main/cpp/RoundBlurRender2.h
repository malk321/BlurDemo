//
// Created by malk on 27/11/18.
//

#ifndef HELLO_GL2_ROUNDBLURRENDER2_H
#define HELLO_GL2_ROUNDBLURRENDER2_H


#include <GLES2/gl2.h>

class RoundBlurRender2 {
public:
    RoundBlurRender2();
    ~RoundBlurRender2();
    void setFrame(int wight, int height, int texID);
    void drawFrame();

    struct ShaderUniforms {
        GLuint sample;
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint iResolution;
        GLuint dim;
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
    GLuint gRoundProgram;
    ShaderAttributes gRoundAttrs;
    ShaderUniforms gRoundSampleUniforms;
};


#endif //HELLO_GL2_ROUNDBLURRENDER2_H
