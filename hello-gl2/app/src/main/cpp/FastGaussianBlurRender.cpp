//
// Created by malk on 24/11/18.
//

#include "FastGaussianBlurRender.h"
#include "GLES20Utils.h"

FastGaussianBlurRender::FastGaussianBlurRender() {

}

FastGaussianBlurRender::~FastGaussianBlurRender() {

}

void FastGaussianBlurRender::setFrame(int wight, int height, int texID) {
    mWidth = wight;
    mHeight = height;
    gTexId = texID;

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
                "uniform vec2 iResolution;\n"
                "uniform float mSigma;\n"
                "uniform int kernelSize;\n"
                "varying vec2 outTexCoords;\n"
                "float normpdf(in float x, in float sigma) {\n"
                "  return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;\n"
                "}\n"

                "vec4 mainImage(in vec2 fragCoord) {\n"
                "   vec3 c = texture2D(sampler, fragCoord.xy).rgb;\n"
                "   const int kerSize = 11;"
                "   const int kSize = (kerSize-1)/2;\n"
                "   float kernel[kerSize];\n"
                "   vec3 final_colour = vec3(0.0);\n"
                "   float Z = 0.0;\n"
                "   for (int j = 0; j <= kSize; ++j){\n"
                "       kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), mSigma);\n"
                "   }\n"
                "   for (int j = 0; j < kerSize; ++j){\n"
                "       Z += kernel[j];\n"
                "   }\n"
                "   for (int i=-kSize; i <= kSize; ++i) {\n"
                "       for (int j=-kSize; j <= kSize; ++j) {\n"
                "           final_colour += kernel[kSize+j]*kernel[kSize+i]*texture2D(sampler, fragCoord.xy+ vec2(float(i),float(j)) / iResolution.xy).rgb;\n"
                "       }\n"
                "   }\n"
                "return vec4(final_colour/(Z*Z), 1.0);\n"
                "}\n"

                "void main() {\n"
                "   gl_FragColor = mainImage(outTexCoords.xy);\n"
                "}\n";

        gBlurProgram = GLES20Utils::createProgram(gVertexShader, gFragmentShader);
        gBlurAttrs.position = glGetAttribLocation(gBlurProgram, "vPosition");
        gBlurAttrs.texCoords = glGetAttribLocation(gBlurProgram, "vTexCoords");
        gBlurSampleUniforms.model = glGetUniformLocation(gBlurProgram, "model");
        gBlurSampleUniforms.view = glGetUniformLocation(gBlurProgram, "view");
        gBlurSampleUniforms.projection = glGetUniformLocation(gBlurProgram, "projection");
        gBlurSampleUniforms.iResolution = glGetUniformLocation(gBlurProgram, "iResolution");
        gBlurSampleUniforms.sigma = glGetUniformLocation(gBlurProgram, "mSigma");
        gBlurSampleUniforms.kernelSize = glGetUniformLocation(gBlurProgram, "kernelSize");
    }
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


void FastGaussianBlurRender::drawFrame() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    // prepare
    glUseProgram(gBlurProgram);
    //glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    glViewport(0, 0, mWidth, mHeight);
    glUniform2f(gBlurSampleUniforms.iResolution, mWidth, mHeight);
    // see http://dev.theomader.com/gaussian-kernel-calculator/
    glUniform1f(gBlurSampleUniforms.sigma, 1.0);

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

    glBindTexture(GL_TEXTURE_2D, gTexId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}