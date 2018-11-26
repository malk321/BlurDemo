//
// Created by malk on 23/11/18.
//

#include "GaussianBlurRender.h"
#include "GLES20Utils.h"

GaussianBlurRender::GaussianBlurRender() {

}

GaussianBlurRender::~GaussianBlurRender() {}

void GaussianBlurRender::setFrame(int wight, int height, int texID) {
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
                "varying vec2 outTexCoords;\n"
                "vec4 mainImage(vec2 fragCoord )\n"
                "{\n"
                "   float Pi = 6.28318530718; // Pi*2\n"

                // GAUSSIAN BLUR SETTINGS {{{
                "   float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)\n"
                "   float Quality = 4.0; // BLUR QUALITY (Default 4.0 - More is better but slower)\n"
                "   float Size = 35.0; // BLUR SIZE (Radius)\n"
                // GAUSSIAN BLUR SETTINGS }}}

                "   vec2 Radius = Size/iResolution.xy;\n"

                // Normalized pixel coordinates (from 0 to 1)
                "   vec2 uv = fragCoord;\n"
                // Pixel colour
                "   vec4 Color = texture2D(sampler, uv);\n"

                // Blur calculations
                "   for( float d=0.0; d<Pi; d+=Pi/Directions)\n"
                "   {\n"
                "       for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)\n"
                "       {\n"
                "            Color += texture2D( sampler, uv+vec2(cos(d),sin(d))*Radius*i);\n"
                "       }\n"
                "   }\n"

                // Output to screen
                "   Color /= Quality * Directions + 1.0;\n"
                "   return Color;\n"
                "}\n"

                "void main() {\n"
//                "   vec4 color = mainImage(outTexCoords.xy);\n"
                "   gl_FragColor = mainImage(outTexCoords.xy);\n"
                "}\n";

/*        auto gFragmentShader =
                "precision mediump float;\n"
                "varying vec2 outTexCoords;\n"
                "uniform sampler2D sampler;\n"
                "void main() {\n"
                //"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
                "gl_FragColor = texture2D(sampler,outTexCoords);\n"
                "}\n";*/
        gBlurProgram = GLES20Utils::createProgram(gVertexShader, gFragmentShader);
        gBlurAttrs.position = glGetAttribLocation(gBlurProgram, "vPosition");
        gBlurAttrs.texCoords = glGetAttribLocation(gBlurProgram, "vTexCoords");
        gBlurSampleUniforms.model = glGetUniformLocation(gBlurProgram, "model");
        gBlurSampleUniforms.view = glGetUniformLocation(gBlurProgram, "view");
        gBlurSampleUniforms.projection = glGetUniformLocation(gBlurProgram, "projection");
        gBlurSampleUniforms.iResolution = glGetUniformLocation(gBlurProgram, "iResolution");
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

void GaussianBlurRender::drawFrame() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    // prepare
    glUseProgram(gBlurProgram);
    //glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    glViewport(0, 0, mWidth, mHeight);
    glUniform2f(gBlurSampleUniforms.iResolution, mWidth, mHeight);

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