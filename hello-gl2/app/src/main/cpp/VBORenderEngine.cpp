//
// Created by malk on 18-11-16.
//
#include "VBORenderEngine.h"

VBORenderEngine::VBORenderEngine() {}
VBORenderEngine::~VBORenderEngine() {}

const GLfloat gRectangleVertices[] = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
};

const GLfloat gRectangleTexCoords[] = {
//        1.0f, 1.0f,
//        1.0f, 0.0f,
//        0.0f, 0.0f,
//        0.0f, 1.0f
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
};

const GLuint indices[] = { // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
};
void VBORenderEngine::drawFrame(GLuint texId, GLuint gProgram, GLuint gvPositionHandle,
                                GLuint gvTexCoordHandle) const {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(gProgram);

    // 使用顶点缓冲对象(Vertex Buffer Objects, VBO)管理GPU内存(通常被称为显存)中储存大量顶点
    GLuint VBO[2];
    // glGenBuffers函数和一个缓冲ID生成一个VBO对象
    glGenBuffers(2, VBO);
    // OpenGL有很多缓冲对象类型，顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER。OpenGL允许我们同时绑定多个缓冲，只要它们是不同的缓冲类型。
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // 把顶点数据复制到缓冲的内存中
    // GL_STATIC_DRAW：表示该缓存区不会被修改；
    // GL_DYNAMIC_DRAW：表示该缓存区会被周期性更改；
    // GL_STREAM_DRAW：表示该缓存区会被频繁更改；
    // 如果顶点数据一经初始化就不会被修改，那么就应该尽量使用 GL_STATIC_DRAW，这样能获得更好的性能。
    glBufferData(GL_ARRAY_BUFFER, sizeof(gRectangleVertices), gRectangleVertices, GL_STATIC_DRAW);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    // 顶点着色器允许我们指定任何以顶点属性为形式的输入。
    // 这使其具有很强的灵活性的同时，它还的确意味着我们必须手动指定输入数据的哪一个部分对应顶点着色器的哪一个顶点属性。
    // 所以，我们必须在渲染前指定OpenGL该如何解释顶点数据。
    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    // 为了性能，顶点着色器的属性是关闭的，即使数据上传到了GPU，不调用该函数，顶点数据仍然不可见
    glEnableVertexAttribArray(gvPositionHandle);

    // 传递纹理数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gRectangleTexCoords), gRectangleTexCoords, GL_STATIC_DRAW);

    // TexCoord attribute
    glVertexAttribPointer(gvTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(gvTexCoordHandle);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}