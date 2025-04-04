#include <iostream>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <vector>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "camera.h"
#include <stb_image.h>
class ImageLoader
{
public:
    ImageLoader(const char *imagepath) {
        stbi_set_flip_vertically_on_load(true);

        data = stbi_load(imagepath, &width, &height, &numColorChannels, STBI_rgb_alpha);
        std::cout << "Imagen cargada: " << width << "x" << height << ", Canales: " << numColorChannels << std::endl;
    }
    ~ImageLoader() {

    }

    int width;//si no le pongo 512 explota
    int  height;
    int  numColorChannels;
    unsigned char* data = nullptr;
    GLuint textureGLuint = 0;
};


struct App {
    SDL_Window* mWindow = nullptr;
    SDL_GLContext mGlContext     = nullptr;
    const int mSCREEN_WIDTH      = 800;
    const int mSCREEN_HEIGHT     = 600;
    bool mRunning                = true;
    SDL_Event mEvent{};
    GLuint mShaderProgram{};
    camera mCamera;
    glm::mat4 mCameraPerspective = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 1.0f, 10.0f);

};

struct Mesh3D {
    GLuint mVAO{}, mVBO{};
    GLuint mEBO{};
    float mU_Offset{ 0.0f };

    std::string mVertexShaderSource =
        "#version 330 core \n"
        "layout(location=0) in vec4 position;\n"
        "layout(location=1) in vec3 rgbColors; \n"
        "layout(location=2) in vec2 uvsCordinates; \n"

        "uniform float u_Offset;\n" //no se inicializan
        "uniform mat4 u_Perspective;\n"
        "uniform mat4 u_Camera;\n"


        "out vec3 v_rgbColors; \n"
        "out vec2 v_uvsCordinates; \n"



        "void main()\n"
        "{\n"
        "v_rgbColors = rgbColors; \n"
        "v_uvsCordinates = uvsCordinates; \n"

        "gl_Position = u_Perspective *u_Camera* vec4(position.x, position.y, position.z-u_Offset, 1.0f);\n"

        "}\n";

    std::string mFragmentShaderSource =

        "#version 330 core \n"
        "in vec3 v_rgbColors;\n"
        "in vec2 v_uvsCordinates; \n"
        "uniform sampler2D u_tex0;\n"

        "out vec4 color;\n"

        "void main()\n"
        "{\n"
        "  color = texture(u_tex0,v_uvsCordinates); \n"
        "}\n";
};
//"  color = vec4(v_rgbColors.r, v_rgbColors.g, v_rgbColors.b, 1.0f);\n"

App TrisApp;
Mesh3D tris1;
ImageLoader epicImage("pop_cat.png");






// Función para compilar shaders
GLuint compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}
// Función para crear el programa de shaders
GLuint createShaderProgram(const std::string& VertexShaderSource, const std::string& FragmentShaderSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, VertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, FragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void Config() {
    // Inicializar everything
    SDL_Init(SDL_INIT_VIDEO);
    TrisApp.mWindow = SDL_CreateWindow("Epic Shiny LGTV Rotative Quad", TrisApp.mSCREEN_WIDTH, TrisApp.mSCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    TrisApp.mGlContext = SDL_GL_CreateContext(TrisApp.mWindow);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    SDL_SetWindowRelativeMouseMode(TrisApp.mWindow, true);
}
void VertexSpecify() {
    //preparar GPU
    std::vector<GLfloat> vertices = {
        -0.5f,-0.5f,0.0f,   // 0 Left
         1.0f,  0.0f, 0.0f, // colores
         0.0f, 0.0f,        //UV Left

         0.5f,-0.5f, 0.0f,  // 1 Right
         0.0f,  1.0f, 0.0f, // colores
         1.0f, 0.0f,        //UV Right

         -0.5f, 0.5f, 0.0f, // 2 Top Left
         0.0f,  0.0f, 1.0f, //colores
         0.0f, 1.0f,        //UV Top Left


         0.5f, 0.5f, 0.0f,  // 3 Top Right
         1.0f,  0.0f, 0.0f, //colores
         1.0f, 1.0f         //UV Top Right


    }; std::vector<GLint> indexes = { 2,0,1,3,1,2 };
    
    
    glGenVertexArrays(1, &tris1.mVAO);
    glGenBuffers(1, &tris1.mVBO);
    glGenBuffers(1, &tris1.mEBO);
    //---Texturas
    glGenTextures(1, &epicImage.textureGLuint);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, epicImage.textureGLuint);
    //---TexturasConfigs
    //si las quiero pixeleadas o interpoladas mmm k riko
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //como quiero que se comporte la imagen fuera de los corners: en este caso se repite la imagen
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //Compiling the final texture
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,epicImage.width,epicImage.height,0,GL_RGBA,GL_UNSIGNED_BYTE,epicImage.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    //-------------
    glBindVertexArray(tris1.mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tris1.mVBO);
    //el espacio que voy a hacer en la GPU
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);//gl static draw: estara dibujado todo el tiempo, no es dinamico.
    //como se entiende en la GPU la primera data: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, (GLvoid*)0);
    glEnableVertexAttribArray(0);//Linkea el vao con el VBO, si no lo activo, el shader no recibirá los datos. (location =0)

    //como se entiende en la GPU la segunda data: color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)(sizeof(GLfloat) * 3));//lo ultimo es de donde empieza (GLVoid)
    glEnableVertexAttribArray(1);
    //como se entiende en la GPU la tercera data: UVs
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)(sizeof(GLfloat) * 6));//lo ultimo es de donde empieza (GLVoid)
    glEnableVertexAttribArray(2);

    //Cuadrado tactico
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tris1.mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLint), indexes.data(), GL_STATIC_DRAW);//gl static draw: estara dibujado todo el tiempo, no es dinamico.
    //glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(GLint), (GLvoid*)0);
    //glEnableVertexAttribArray(2);

    TrisApp.mShaderProgram = createShaderProgram(tris1.mVertexShaderSource,tris1.mFragmentShaderSource);
    //desvinculo
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);

    //Perspective Matrices

}
void MainLoop() {
    tris1.mU_Offset += 5.0f;
    GLuint uniform_location = glad_glGetUniformLocation(TrisApp.mShaderProgram, "u_Offset");
    GLuint perspective_location = glad_glGetUniformLocation(TrisApp.mShaderProgram, "u_Perspective");
    glUniform1f(uniform_location, tris1.mU_Offset);
    glUniformMatrix4fv(perspective_location, 1, GL_FALSE, &TrisApp.mCameraPerspective[0][0]);

    //input
    while (TrisApp.mRunning) {
        glm::mat4 u_Camera = TrisApp.mCamera.GetViewMatrix();
        //"tex0"
        GLuint camera_location = glad_glGetUniformLocation(TrisApp.mShaderProgram, "u_Camera");
        glUniformMatrix4fv(camera_location, 1, GL_FALSE, &u_Camera[0][0]);
        //init image
        
        GLuint perspective_location = glad_glGetUniformLocation(TrisApp.mShaderProgram, "u_Perspective");
        glUniformMatrix4fv(perspective_location, 1, GL_FALSE, &TrisApp.mCameraPerspective[0][0]);
        GLuint uniform_location = glad_glGetUniformLocation(TrisApp.mShaderProgram, "u_Offset");
        glUniform1f(uniform_location, tris1.mU_Offset);

        GLuint tex0 = glad_glGetUniformLocation(TrisApp.mShaderProgram, "u_tex0");
        glUniform1i(tex0,0);


        while (SDL_PollEvent(&TrisApp.mEvent)) {
            if (TrisApp.mEvent.type == SDL_EVENT_QUIT || TrisApp.mEvent.type == SDL_EVENT_KEY_DOWN && TrisApp.mEvent.key.key == SDLK_ESCAPE)
                TrisApp.mRunning = false;
            if (TrisApp.mEvent.type == SDL_EVENT_KEY_DOWN && TrisApp.mEvent.key.key == SDLK_C) {
                std::cout << "TECLA PRESIONADA SUS" << tris1.mU_Offset<< "\n";
                tris1.mU_Offset += 0.1f;
                GLuint perspective_location = glad_glGetUniformLocation(TrisApp.mShaderProgram, "u_Perspective");

                glUniformMatrix4fv(perspective_location, 1, GL_FALSE, &TrisApp.mCameraPerspective[0][0]);
                GLuint uniform_location = glad_glGetUniformLocation(TrisApp.mShaderProgram, "u_Offset");
                glUniform1f(uniform_location, tris1.mU_Offset);
            }

            if (TrisApp.mEvent.type == SDL_EVENT_KEY_DOWN && TrisApp.mEvent.key.key == SDLK_W) {
                TrisApp.mCamera.MoveForward(0.1f);
                std::cout << "TECLA PRESIONADA W" << tris1.mU_Offset << "\n";
                

            }
            if (TrisApp.mEvent.type == SDL_EVENT_KEY_DOWN && TrisApp.mEvent.key.key == SDLK_S) {
                TrisApp.mCamera.MoveBackward(0.1f);
                std::cout << "TECLA PRESIONADA S" << tris1.mU_Offset << "\n";
                
            }
            if (TrisApp.mEvent.type == SDL_EVENT_KEY_DOWN && TrisApp.mEvent.key.key == SDLK_A) {
                TrisApp.mCamera.MoveLatLeft(0.1f);
                std::cout << "TECLA PRESIONADA W" << tris1.mU_Offset << "\n";


            }
            if (TrisApp.mEvent.type == SDL_EVENT_KEY_DOWN && TrisApp.mEvent.key.key == SDLK_D) {
                TrisApp.mCamera.MoveLatRigth(0.1f);
                std::cout << "TECLA PRESIONADA S" << tris1.mU_Offset << "\n";

            }
            if (TrisApp.mEvent.type == SDL_EVENT_MOUSE_MOTION) {
                TrisApp.mCamera.MouseMovement(TrisApp.mEvent.motion.xrel, TrisApp.mEvent.motion.yrel);

            }
            
                
        }
        
        // Renderizar
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(TrisApp.mShaderProgram);
        glBindVertexArray(tris1.mVAO);
        glBindTexture(GL_TEXTURE_2D, epicImage.textureGLuint);


        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        SDL_GL_SwapWindow(TrisApp.mWindow);
        SDL_Delay(16);
    }
}
void CleanUp() {
    glDeleteTextures(1, &epicImage.textureGLuint);
    stbi_image_free(epicImage.data);

    glDeleteVertexArrays(1, &tris1.mVAO);
    glDeleteBuffers(1, &tris1.mVBO);
    glDeleteProgram(TrisApp.mShaderProgram);

    SDL_DestroyWindow(TrisApp.mWindow);
    SDL_Quit();
}

int main() {
    Config();
    VertexSpecify();
    MainLoop();
    CleanUp();
    
    
    return 0;
}