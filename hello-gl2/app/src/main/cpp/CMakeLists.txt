cmake_minimum_required(VERSION 3.4.1)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

# see https://stackoverflow.com/questions/16859727/how-to-use-glm-in-android-ndk-application/16860052
# and we also should motify glm/CmakeList
add_subdirectory(glm)

add_library(gl2jni SHARED
            gl_code.cpp
            GLES20Utils.cpp
            RenderEngine.cpp
            DrawArrayRenderEngine.cpp
            DataArrayRenderEngine.cpp
            VBORenderEngine.cpp
            CubeRenderEngine.cpp
            BlurOnlyRender.cpp
            FastGaussianBlurRender.cpp
            TwoStepGaussianBlurRender.cpp
            DualFilterRender.cpp
            GaussianBlurRender.cpp
            BlurRenderEngine.cpp)

# add lib dependencies
target_link_libraries(gl2jni
                      android
                      log
                      EGL
                      GLESv2
                      glm)

