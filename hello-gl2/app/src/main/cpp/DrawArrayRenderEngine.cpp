//
// Created by malk on 18-11-16.
//
#include "DrawArrayRenderEngine.h"

DrawArrayRenderEngine::DrawArrayRenderEngine() {}
DrawArrayRenderEngine::~DrawArrayRenderEngine() {}
const GLfloat gRectangleVertices[] = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
};

const GLfloat gRectangleTexCoords[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
};
void DrawArrayRenderEngine::drawFrame(GLuint texId, GLuint gProgram, GLuint gvPositionHandle,
                                      GLuint gvTexCoordHandle) const {

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(gProgram);

    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, gRectangleVertices);
    glVertexAttribPointer(gvTexCoordHandle, 2, GL_FLOAT,GL_FALSE, 0, gRectangleTexCoords);
    glEnableVertexAttribArray(gvPositionHandle);
    glEnableVertexAttribArray(gvTexCoordHandle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
