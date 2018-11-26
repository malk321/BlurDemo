//
// Created by malk on 24/11/18.
//

#include "TwoStepGaussianBlurRender.h"
#include "GLES20Utils.h"
#include <android/log.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

TwoStepGaussianBlurRender::TwoStepGaussianBlurRender() {

}

TwoStepGaussianBlurRender::~TwoStepGaussianBlurRender() {

}

const GLfloat gRectangleVertices[] = {
        1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f,   1.0f, 0.0f
};

const GLuint indices[] = { // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
};
const GLfloat weight[5] = {
        0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
};

void TwoStepGaussianBlurRender::setFrame(int wight, int height, int texID) {
    mWidth = wight;
    mHeight = height;
    gTexId = texID;

    {
        auto gVertexShader =
                "attribute vec4 vPosition;\n"
                "attribute vec2 vTexCoords;\n"
                "varying vec2 TexCoords;\n"
                "uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 projection;\n"
                "void main() {\n"
                "  gl_Position =  vPosition;\n"
                "  TexCoords = vTexCoords;\n"
                "}\n";
        // see https://pastebin.com/bKLYdmdi
        // to gen sigma and kernel size
        auto gFragmentShader =
                "precision mediump float;\n"
                "uniform sampler2D sampler;\n"
                "uniform bool horizontal;\n"
                "uniform float weight[5];\n"
                "uniform vec2 iResolution;\n"
                "varying vec2 TexCoords;\n"

                "void main() {\n"
                // gets size of single texel
                "   vec2 tex_offset = 1.0 / iResolution.xy;\n "
                // current fragment's contribution
                "   vec3 result = texture2D(sampler, TexCoords).rgb * weight[0];\n"
                "   if(horizontal) {\n"
                "       for(int i = 1; i < 5; ++i) {\n"
                "           result += texture2D(sampler, TexCoords + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];\n"
                "           result += texture2D(sampler, TexCoords - vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];\n"
                "       }\n"
                "   } else {\n"
                "       for(int i = 1; i < 5; ++i) {\n"
                "           result += texture2D(sampler, TexCoords + vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];\n"
                "           result += texture2D(sampler, TexCoords - vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];\n"
                "       }\n"
                "   }\n"
                "   gl_FragColor = vec4(result, 1.0);\n"
                "}\n";

        gBlurProgram = GLES20Utils::createProgram(gVertexShader, gFragmentShader);
        gBlurAttrs.position = glGetAttribLocation(gBlurProgram, "vPosition");
        gBlurAttrs.texCoords = glGetAttribLocation(gBlurProgram, "vTexCoords");
        gBlurSampleUniforms.model = glGetUniformLocation(gBlurProgram, "model");
        gBlurSampleUniforms.view = glGetUniformLocation(gBlurProgram, "view");
        gBlurSampleUniforms.projection = glGetUniformLocation(gBlurProgram, "projection");
        gBlurSampleUniforms.horizontal = glGetUniformLocation(gBlurProgram, "horizontal");
        gBlurSampleUniforms.weight = glGetUniformLocation(gBlurProgram, "weight");
        gBlurSampleUniforms.iResolution = glGetUniformLocation(gBlurProgram, "iResolution");
    }

    {
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(gRectangleVertices), gRectangleVertices,
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(gBlurAttrs.position, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (GLvoid *) 0);
        glEnableVertexAttribArray(gBlurAttrs.position);

        // TexCoord attribute
        glVertexAttribPointer(gBlurAttrs.texCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (GLvoid *) (6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(gBlurAttrs.texCoords);
    }

    {
        // gen two fbo
        GLint prevFBO;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongBuffer);
        for (int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   pingpongBuffer[i], 0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    }
}

void TwoStepGaussianBlurRender::drawFrame() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // save
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    // prepare
    glUseProgram(gBlurProgram);
    //glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    glViewport(0, 0, mWidth, mHeight);
    glUniform1fv(gBlurSampleUniforms.weight, 5, weight);
    glUniform2f(gBlurSampleUniforms.iResolution, mWidth, mHeight);

    GLuint amount = 10;
    GLboolean horizontal = true, first_iteration = true;
    for (int i=0; i<amount; i++) {
        glUniform1i(gBlurSampleUniforms.horizontal, horizontal);
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        glBindTexture(
                GL_TEXTURE_2D, first_iteration ? gTexId : pingpongBuffer[!horizontal]
        );
        horizontal = !horizontal;
        if (first_iteration) {
            first_iteration = false;
        }
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    }

    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[horizontal]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}