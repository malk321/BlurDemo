//
// Created by malk on 26/11/18.
//

#include "DualFilterRender.h"
#include "GLES20Utils.h"
#include <android/log.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

DualFilterRender::DualFilterRender() {

}

DualFilterRender::~DualFilterRender() {

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

void DualFilterRender::setFrame(int wight, int height, int texID) {
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
        auto gSampleFragmentShader =
                "precision mediump float;\n"
                "uniform sampler2D sampler;\n"
                "uniform bool iDownSample;\n"
                "uniform vec2 iResolution;\n"
                "varying vec2 TexCoords;\n"

                "vec4 downsample(vec2 uv, vec2 halfpixel) {\n"
                "   vec4 sum = texture2D(sampler, uv) * 4.0;\n"
                "   sum += texture2D(sampler,uv - halfpixel.xy);\n"
                "   sum += texture2D(sampler,uv + halfpixel.xy);\n"
                "   sum += texture2D(sampler,uv + vec2(halfpixel.x, -halfpixel.y));\n"
                "   sum += texture2D(sampler,uv - vec2(halfpixel.x, -halfpixel.y));\n"
                "   return sum / 8.0;\n"
                "}\n"

                "vec4 upsample(vec2 uv, vec2 halfpixel){\n"
                "   vec4 sum = texture2D(sampler, uv + vec2(-halfpixel.x * 2.0, 0.0));\n"
                "   sum += texture2D(sampler, uv + vec2(-halfpixel.x, halfpixel.y)) * 2.0;\n"
                "   sum += texture2D(sampler, uv + vec2(0.0, halfpixel.y * 2.0));\n"
                "   sum += texture2D(sampler, uv + vec2(halfpixel.x, halfpixel.y)) * 2.0;\n"
                "   sum += texture2D(sampler, uv + vec2(halfpixel.x * 2.0, 0.0));\n"
                "   sum += texture2D(sampler, uv + vec2(halfpixel.x, -halfpixel.y)) * 2.0;\n"
                "   sum += texture2D(sampler, uv + vec2(0.0, -halfpixel.y * 2.0));\n"
                "   sum += texture2D(sampler, uv + vec2(-halfpixel.x, -halfpixel.y)) * 2.0;\n"
                "   return sum / 12.0;\n"
                "}\n"

                "void main() {\n"
                // gets size of single texel
                "   vec2 tex_offset = 1.0 / iResolution.xy;\n "
                "   vec4 color;"
                "   if (iDownSample) {\n"
                "       color = downsample(TexCoords, tex_offset/2.0);\n"
                "   } else {\n"
                "       color = upsample(TexCoords, tex_offset/2.0);\n"
                "   }\n"
                "   gl_FragColor = color;\n"
                "}\n";

        auto gFragmentShader =
                "precision mediump float;\n"
                "varying vec2 TexCoords;\n"
                "uniform sampler2D sampler;\n"
                "void main() {\n"
                "gl_FragColor = texture2D(sampler,TexCoords);\n"
                "}\n";

        gBlurProgram = GLES20Utils::createProgram(gVertexShader, gSampleFragmentShader);
        gBlurAttrs.position = glGetAttribLocation(gBlurProgram, "vPosition");
        gBlurAttrs.texCoords = glGetAttribLocation(gBlurProgram, "vTexCoords");
        gBlurSampleUniforms.model = glGetUniformLocation(gBlurProgram, "model");
        gBlurSampleUniforms.view = glGetUniformLocation(gBlurProgram, "view");
        gBlurSampleUniforms.projection = glGetUniformLocation(gBlurProgram, "projection");
        gBlurSampleUniforms.downSample = glGetUniformLocation(gBlurProgram, "iDownSample");
        gBlurSampleUniforms.iResolution = glGetUniformLocation(gBlurProgram, "iResolution");

        // normal
        gNormalProgram = GLES20Utils::createProgram(gVertexShader, gFragmentShader);
        gNormalAttrs.position = glGetAttribLocation(gNormalProgram, "vPosition");
        gNormalAttrs.texCoords = glGetAttribLocation(gNormalProgram, "texCoords");
        gNormalSampleUniforms.model = glGetUniformLocation(gNormalProgram, "model");
        gNormalSampleUniforms.view = glGetUniformLocation(gNormalProgram, "view");
        gNormalSampleUniforms.projection = glGetUniformLocation(gNormalProgram, "projection");
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
        glGenTextures(4, pingpongBuffer);
        int factor = 2;
        for (int i = 0; i < 4; i++) {
            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth/factor, mHeight/factor, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, 0);

            factor = factor * 2;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    }
}

void DualFilterRender::drawFrame() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // save
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    // prepare
    glUseProgram(gBlurProgram);

    // down sample
    int downSampleTime = 4;
    int factor = 2;
    int finalDownSampleTexture;
    int finalIndex = 0;
    glUniform1f(gBlurSampleUniforms.downSample, true);
    for (int i=0; i<downSampleTime; i++) {
        glViewport(0, 0, mWidth / factor, mHeight / factor);
        glUniform2f(gBlurSampleUniforms.iResolution, mWidth / factor, mHeight / factor);
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i%2]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               pingpongBuffer[i], 0);
        glBindTexture(GL_TEXTURE_2D, i==0 ? gTexId : pingpongBuffer[i - 1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        factor = factor * 2;
        finalDownSampleTexture = pingpongBuffer[i];
        finalIndex = i;
    }

    // up sample
    int upSampleTime = finalIndex + 1;
    factor = factor / 4;
    glUniform1f(gBlurSampleUniforms.downSample, false);

    for (int i = upSampleTime; i < 0; --i) {
        glViewport(0, 0, mWidth / factor, mHeight / factor);
        glUniform2f(gBlurSampleUniforms.iResolution, mWidth / factor, mHeight / factor);
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i%2]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[finalIndex -1]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               pingpongBuffer[finalIndex -1], 0);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[finalIndex]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        factor = factor / 2;
        finalDownSampleTexture = pingpongBuffer[finalIndex - 1];
        finalIndex = finalIndex - 1;
    }

    // normal
    glUseProgram(gNormalProgram);
    glViewport(0, 0, mWidth, mHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    // Position attribute
    glVertexAttribPointer(gNormalAttrs.position, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid *) 0);
    glEnableVertexAttribArray(gNormalAttrs.position);

    // TexCoord attribute
    glVertexAttribPointer(gNormalAttrs.texCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid *) (6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(gNormalAttrs.texCoords);
    glBindTexture(GL_TEXTURE_2D, finalDownSampleTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}