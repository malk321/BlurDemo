//
// Created by malk on 23/11/18.
//

#include "BlurOnlyRender.h"
#include "GLES20Utils.h"

static int KawaseBlurPassKernelSize[] = { 0, 1, 2, 3, 4, 4, 5, 6, 7 };
static int KawaseBlurPasses = sizeof(KawaseBlurPassKernelSize) / sizeof(KawaseBlurPassKernelSize[0]);

BlurOnlyRender::BlurOnlyRender() {}
BlurOnlyRender::~BlurOnlyRender() {}

void BlurOnlyRender::setFrame(int width, int height, int texId) {
    mWidth = width;
    mHeight = height;
    gTexId = texId;
    {
        auto gVertexShader =
                "attribute vec4 vPosition;\n"
                "attribute vec2 vTexCoords;\n"
                "varying vec2 outTexCoords;\n"
                "uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 projection;\n"
                "void main() {\n"
                "  gl_Position =  vPosition;\n"
                "  outTexCoords = vTexCoords;\n"
                "}\n";

        auto gFragmentShader =
                "precision mediump float;\n"
                "uniform sampler2D sampler;\n"
                "varying vec2 outTexCoords;\n"
                "uniform vec2 uInvSize;\n"
                "uniform float uIteration;\n"
                "vec3 kawaseBlur(sampler2D tex, vec2 texCoord, vec2 pixelSize, float iteration)\n"
                "{\n"
                "vec2 texCoordSample;\n"
                "vec2 halfPixelSize = pixelSize / 2.0;\n"
                "vec2 dUV = ( pixelSize.xy * vec2( iteration, iteration ) ) + halfPixelSize.xy;\n"
                "vec3 cOut;\n"
                "/* Sample top left pixel */"
                "texCoordSample.x = texCoord.x - dUV.x;\n"
                "texCoordSample.y = texCoord.y + dUV.y;\n"
                "cOut = texture2D( tex, texCoordSample ).xyz;\n"
                "/* Sample top right pixel */"
                "texCoordSample.x = texCoord.x + dUV.x;\n"
                "texCoordSample.y = texCoord.y + dUV.y;\n"
                "cOut += texture2D( tex, texCoordSample ).xyz;\n"
                "/* Sample bottom right pixel */"
                "texCoordSample.x = texCoord.x + dUV.x;\n"
                "texCoordSample.y = texCoord.y - dUV.y;\n"
                "cOut += texture2D( tex, texCoordSample ).xyz;\n"
                "/* Sample bottom left pixel */"
                "texCoordSample.x = texCoord.x - dUV.x;\n"
                "texCoordSample.y = texCoord.y - dUV.y;\n"
                "cOut += texture2D( tex, texCoordSample ).xyz;\n"
                "/* Average */"
                "cOut *= 0.25;\n"
                "return cOut;\n"
                "}\n"
                "void main() {\n"
                "vec3 color = kawaseBlur(sampler, outTexCoords.xy, uInvSize, uIteration);\n"
                "gl_FragColor.rgb = color;\n"
                "gl_FragColor.a = 1.0;\n"
                "}\n";
        gBlurProgram = GLES20Utils::createProgram(gVertexShader,gFragmentShader);
        gBlurAttrs.position = glGetAttribLocation(gBlurProgram, "vPosition");
        gBlurAttrs.texCoords = glGetAttribLocation(gBlurProgram, "vTexCoords");
        gBlurSampleUniforms.model = glGetUniformLocation(gBlurProgram, "model");
        gBlurSampleUniforms.view = glGetUniformLocation(gBlurProgram, "view");
        gBlurSampleUniforms.projection = glGetUniformLocation(gBlurProgram, "projection");
        gBlurSampleUniforms.invSize = glGetUniformLocation(gBlurProgram, "uInvSize");
        gBlurSampleUniforms.iteration = glGetUniformLocation(gBlurProgram, "uIteration");
        gBlurSampleUniforms.pixelWight = glGetUniformLocation(gBlurProgram, "pixelWidth");
        gBlurSampleUniforms.pixelHeight = glGetUniformLocation(gBlurProgram, "pixelHeight");


        GLint prevFBO;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
        // bind texture to framebuffer.
        // Framebuffers
        glGenFramebuffers(2, gBlurFilterFBO);

        // Create a color attachment texture
        glGenTextures(2, gBlurFilterTex);

        // fb1 and texture2
        glBindTexture(GL_TEXTURE_2D, gBlurFilterTex[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, gBlurFilterFBO[0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBlurFilterTex[0], 0);

        // fb2 and texture2
        glBindTexture(GL_TEXTURE_2D, gBlurFilterTex[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, gBlurFilterFBO[1]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBlurFilterTex[1], 0);

        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    }
}

const GLfloat gRectangleVertices[] = {
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   1.0f, 0.0f
};

const GLuint indices[] = { // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
};

void BlurOnlyRender::drawFrame() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, mWidth, mHeight);
    glUseProgram(gBlurProgram);

    int iteration = KawaseBlurPassKernelSize[4];

    // save
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);


    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gRectangleVertices), gRectangleVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(gBlurAttrs.position, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(gBlurAttrs.position);

    // TexCoord attribute
    glVertexAttribPointer(gBlurAttrs.texCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(gBlurAttrs.texCoords);

    // blur filter start
    const int * shaderKernel = KawaseBlurPassKernelSize;
    GLuint lastSourceTex = gTexId;
    for (int i=0; i<KawaseBlurPasses; i++) {
        int iteration = shaderKernel[i];
        GLuint srcTex = lastSourceTex;
        GLuint dstFBO = (i % 2) == 0 ? gBlurFilterFBO[1]:gBlurFilterFBO[0];
        glBindFramebuffer(GL_FRAMEBUFFER, dstFBO);
        glUseProgram(gBlurProgram);
        glUniform1f(gBlurSampleUniforms.iteration, iteration);
        glUniform2f(gBlurSampleUniforms.invSize, 1.0/mWidth, 1.0/mHeight);
        glBindTexture(GL_TEXTURE_2D,srcTex);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        lastSourceTex = ( (i % 2) == 0 )?( gBlurFilterTex[1] ) : ( gBlurFilterTex[0] );
    }

    // swap to main framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    glBindTexture(GL_TEXTURE_2D, gTexId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}