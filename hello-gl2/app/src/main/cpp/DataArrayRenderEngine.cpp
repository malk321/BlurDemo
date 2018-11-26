//
// Created by malk on 18-11-16.
//

#include "DataArrayRenderEngine.h"
#include "GLES20Utils.h"
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

DataArrayRenderEngine::DataArrayRenderEngine() {}
DataArrayRenderEngine::~DataArrayRenderEngine() {}

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

void DataArrayRenderEngine::drawFrame(GLuint texId, GLuint gProgram, GLuint gvPositionHandle,
                                      GLuint gvTexCoordHandle) const {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(gProgram);

    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gRectangleVertices), gRectangleVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(gvPositionHandle);

    // TexCoord attribute
    glVertexAttribPointer(gvTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(gvTexCoordHandle);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
