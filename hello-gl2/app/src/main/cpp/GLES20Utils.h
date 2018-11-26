//
// Created by malk on 18-11-16.
//

#ifndef HELLO_GL2_GLES20UTILS_H
#define HELLO_GL2_GLES20UTILS_H

#endif //HELLO_GL2_GLES20UTILS_H
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class GLES20Utils {
public:
    static void checkGlError(const char* op);
    static void printGLString(const char *name, GLenum s);
    static GLuint loadShader(GLenum shaderType, const char* pSource);
    static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
};