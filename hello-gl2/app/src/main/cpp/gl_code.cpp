/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// OpenGL ES 2.0 code

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>

#include "GLES20Utils.h"

#include "DataArrayRenderEngine.h"
//#include "DrawArrayRenderEngine.h"
//#include "VBORenderEngine.h"
#include "BlurRenderEngine.h"
#include "BlurOnlyRender.h"
#include "GaussianBlurRender.h"
#include "FastGaussianBlurRender.h"
#include "RoundBlurRender.h"
#include "RoundBlurRender2.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "TwoStepGaussianBlurRender.h"
#include "DualFilterRender.h"
#include "CircleRender.h"

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

auto gVertexShader =
    "attribute vec4 vPosition;\n"
    "attribute vec2 vTexCoords;\n"
    "varying vec2 texCoord;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "  texCoord = vTexCoords;\n"
    "}\n";

auto gFragmentShader =
    "precision mediump float;\n"
    "varying vec2 texCoord;\n"
    "uniform sampler2D sampler;\n"
    "void main() {\n"
    //"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "gl_FragColor = texture2D(sampler,texCoord);\n"
    "}\n";

auto gVertexShaderMatrix =
        "attribute vec4 vPosition;\n"
        "attribute vec2 vTexCoords;\n"
        "varying vec2 texCoord;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "  gl_Position = projection * vPosition;\n"
        "  texCoord = vTexCoords;\n"
        "}\n";

GLuint gProgram;
GLuint gvPositionHandle;
GLuint gvTexCoordHandle;
GLint modelLoc;
GLint viewLoc;
GLint projLoc;

GLuint mTexId;
BlurRenderEngine gBlurRenderEngine;
BlurOnlyRender gBlurOnlyRender;
GaussianBlurRender gGaussianBlurRender;
FastGaussianBlurRender gFastGaussianBlurRender;
TwoStepGaussianBlurRender gTwoStepGaussianBlurRender;
DualFilterRender gDualFilterRender;
RoundBlurRender gRoundBlurRender;
RoundBlurRender2 gRoundBlurRender2;
CircleRender mCircleRender;
void setTexture(int texId) {
    mTexId = texId;
}

bool setupGraphics(int w, int h) {
    GLES20Utils::printGLString("Version", GL_VERSION);
    GLES20Utils::printGLString("Vendor", GL_VENDOR);
    GLES20Utils::printGLString("Renderer", GL_RENDERER);
    GLES20Utils::printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);
    gProgram = GLES20Utils::createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    GLES20Utils::checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
            gvPositionHandle);

    gvTexCoordHandle = glGetAttribLocation(gProgram,"vTexCoords");
    glViewport(0, 0, w, h);
    GLES20Utils::checkGlError("glViewport");

//    modelLoc = glGetUniformLocation(gProgram, "model");
//    viewLoc = glGetUniformLocation(gProgram, "view");
//    projLoc = glGetUniformLocation(gProgram, "projection");
//    glm::mat4 projection = glm::ortho(0, w, 1, h, 0, 1);
//    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
//    gBlurRenderEngine.setFrame(w, h, mTexId);
//    gBlurOnlyRender.setFrame(w, h, mTexId);
//    gGaussianBlurRender.setFrame(w, h, mTexId);
//    gFastGaussianBlurRender.setFrame(w, h, mTexId);
//    gTwoStepGaussianBlurRender.setFrame(w, h, mTexId);
//    gDualFilterRender.setFrame(w, h, mTexId);
    mCircleRender.setFrame(w, h, mTexId);
    return true;
}


void renderFrame() {
//    RenderEngine* renderEngine = new DataArrayRenderEngine();
//    RenderEngine* renderEngine = new VBORenderEngine();
//    renderEngine->drawFrame(mTexId, gProgram, gvPositionHandle, gvTexCoordHandle);
//    gBlurRenderEngine.drawFrame();
//    gBlurOnlyRender.drawFrame();
//    gGaussianBlurRender.drawFrame();
//    gFastGaussianBlurRender.drawFrame();
//    gTwoStepGaussianBlurRender.drawFrame();
//    gDualFilterRender.drawFrame();
    mCircleRender.drawFrame();
}


extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setTexture(JNIEnv * env, jobject obj, jint texId);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    renderFrame();
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setTexture(JNIEnv * env, jobject obj, jint texId)
{
    setTexture(texId);
}
