//
// Created by malk on 26/11/18.
//

#ifndef HELLO_GL2_DUALFILTERRENDER_H
#define HELLO_GL2_DUALFILTERRENDER_H


#include <GLES2/gl2.h>

class DualFilterRender {
public:
    DualFilterRender();
    ~DualFilterRender();
    void setFrame(int wight, int height, int texID);
    void drawFrame();

    struct ShaderUniforms {
        GLuint sample;
        GLuint model;
        GLuint view;
        GLuint projection;
        GLuint downSample;
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
    GLuint gNormalProgram;
    GLuint pingpongFBO[2];
    GLuint pingpongBuffer[4];
    ShaderAttributes gBlurAttrs,gNormalAttrs;
    ShaderUniforms gBlurSampleUniforms,gNormalSampleUniforms;
};


#endif //HELLO_GL2_DUALFILTERRENDER_H
