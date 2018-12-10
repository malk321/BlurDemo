//
// Created by malk on 27/11/18.
//

#include "RoundBlurRender2.h"
#include "GLES20Utils.h"

RoundBlurRender2::RoundBlurRender2() {

}

RoundBlurRender2::~RoundBlurRender2() {}

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

void RoundBlurRender2::setFrame(int wight, int height, int texID) {
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
        auto gFragmentShader =
                "precision mediump float;\n"
                "varying vec2 TexCoords;\n"
                "uniform sampler2D sampler;\n"
                "uniform vec2 iResolution;\n"
                "uniform float dim;\n"

                "float triangleNoise(const highp vec2 n) {\n"
                "   highp vec2 p = fract(n * vec2(5.3987, 5.4421));\n"
                "   p += dot(p.yx, p.xy + vec2(21.5351, 14.3137));\n"
                "   highp float xy = p.x * p.y;\n"
                "   return fract(xy * 95.4307) + fract(xy * 75.04961) - 1.0;\n"
                "}\n"

                "vec4 dither(const vec4 color) {\n"
                "   vec3 dithered = sqrt(color.rgb) + (triangleNoise(gl_FragCoord.xy * iResolution) / 255.0);\n"
                "   return vec4(dithered * dithered, color.a);\n"
                "}\n"

                "void main() {\n"
                "   gl_FragColor = dither(vec4(texture2D(sampler, TexCoords).rgb * dim, 1.0));\n"
                "}\n";



        gRoundProgram = GLES20Utils::createProgram(gVertexShader, gFragmentShader);
        gRoundAttrs.position = glGetAttribLocation(gRoundProgram, "vPosition");
        gRoundAttrs.texCoords = glGetAttribLocation(gRoundProgram, "vTexCoords");
        gRoundSampleUniforms.model = glGetUniformLocation(gRoundProgram, "model");
        gRoundSampleUniforms.view = glGetUniformLocation(gRoundProgram, "view");
        gRoundSampleUniforms.projection = glGetUniformLocation(gRoundProgram, "projection");
        gRoundSampleUniforms.iResolution = glGetUniformLocation(gRoundProgram, "iResolution");
        gRoundSampleUniforms.dim = glGetUniformLocation(gRoundProgram, "dim");
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
        glVertexAttribPointer(gRoundAttrs.position, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (GLvoid *) 0);
        glEnableVertexAttribArray(gRoundAttrs.position);

        // TexCoord attribute
        glVertexAttribPointer(gRoundAttrs.texCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (GLvoid *) (6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(gRoundAttrs.texCoords);
    }
}

void RoundBlurRender2::drawFrame() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(gRoundProgram);
    glViewport(0, 0, mWidth, mHeight);
    glUniform2f(gRoundSampleUniforms.iResolution, 1 / mWidth, 1 / mHeight);
    glUniform1f(gRoundSampleUniforms.dim, 0.5);

    glBindTexture(GL_TEXTURE_2D, gTexId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}