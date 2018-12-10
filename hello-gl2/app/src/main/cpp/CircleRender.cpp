//
// Created by malk on 28/11/18.
//

#include "CircleRender.h"
#include "GLES20Utils.h"

CircleRender::CircleRender() {

}

CircleRender::~CircleRender() {

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

void CircleRender::setFrame(int wight, int height, int texID) {
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
                "uniform vec2 meshCenter;\n"
                "uniform float radius;\n"

                "float circle(vec2 pos, float radius) {\n"
                "   float d = length(TexCoords-pos);\n"
                "   return smoothstep(d,d+0.01,radius);\n"
                "}\n"

                "void main() {\n"
                "   vec2 pos = gl_FragCoord.xy - meshCenter;\n"
                "   float dist = circle(pos - vec2(0.0), radius);\n"
                "   vec4 color = texture2D(sampler,TexCoords);\n"
                "   gl_FragColor = color * (dist);\n"
                "}\n";



        gRoundProgram = GLES20Utils::createProgram(gVertexShader, gFragmentShader);
        gRoundAttrs.position = glGetAttribLocation(gRoundProgram, "vPosition");
        gRoundAttrs.texCoords = glGetAttribLocation(gRoundProgram, "vTexCoords");
        gRoundSampleUniforms.model = glGetUniformLocation(gRoundProgram, "model");
        gRoundSampleUniforms.view = glGetUniformLocation(gRoundProgram, "view");
        gRoundSampleUniforms.projection = glGetUniformLocation(gRoundProgram, "projection");
        gRoundSampleUniforms.meshCenter = glGetUniformLocation(gRoundProgram, "meshCenter");
        gRoundSampleUniforms.radius = glGetUniformLocation(gRoundProgram, "radius");
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

void CircleRender::drawFrame() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(gRoundProgram);
    glUniform2f(gRoundSampleUniforms.meshCenter, mWidth/2, mHeight/2);
    glUniform1f(gRoundSampleUniforms.radius, 200.0);
    glViewport(0, 0, mWidth, mHeight);

    glBindTexture(GL_TEXTURE_2D, gTexId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}