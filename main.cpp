#include <iostream>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <vector>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "camera.h"

// Tamaño de la ventana
SDL_Window* window = nullptr;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
// Bucle Principal
bool running = true;
SDL_Event event{};
//Vertex Specify
GLuint VAO{}, VBO{};
GLuint EBO{};
GLuint ShaderProgram{};
//Perspectiv things
float u_Offset{0.0f};
glm::mat4 u_perspective = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 10.0f);
camera Camera;

std::string gVertexShaderSource =
"#version 330 core \n"
"layout(location=0) in vec4 position;\n"
"layout(location=1) in vec3 rgbColors; \n"
"uniform float u_Offset;\n" //no se inicializan
"uniform mat4 u_Perspective;\n" 
"uniform mat4 u_Camera;\n"


"out vec3 v_rgbColors; \n"


"void main()\n"
"{\n"
    "v_rgbColors=rgbColors; \n"
    
    "gl_Position = u_Perspective *u_Camera* vec4(position.x, position.y, position.z-u_Offset, 1.0f);\n"

"}\n";

std::string gFragmentShaderSource =

"#version 330 core \n"
"in vec3 v_rgbColors;\n"
"out vec4 color;\n"

"void main()\n"
"{\n"
"  color = vec4(v_rgbColors.r, v_rgbColors.g, v_rgbColors.b, 1.0f);\n"
"}\n";

// Función para compilar shaders
GLuint compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}
// Función para crear el programa de shaders
GLuint createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, gVertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, gFragmentShaderSource);

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
    window = SDL_CreateWindow("Epic Shiny LGTV Triangle", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    SDL_SetWindowRelativeMouseMode(window, true);
}
void VertexSpecify() {
    //preparar GPU
    std::vector<GLfloat> vertices = {
        -0.5f,-0.5f,0.0f, // 0 Left
         1.0f,  0.0f, 0.0f, // colores
         0.5f,-0.5f, 0.0f, // 1 Right
         0.0f,  1.0f, 0.0f, // colores
         -0.5f, 0.5f, 0.0f, // 2 Top
         0.0f,  0.0f, 1.0f,  //colores

         0.5f, 0.5f, 0.0f, // 3 Top
         1.0f,  0.0f, 0.0f,  //colores

    }; std::vector<GLint> indexes = { 2,0,1,3,1,2 };
    
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //el espacio que voy a hacer en la GPU
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);//gl static draw: estara dibujado todo el tiempo, no es dinamico.
    //como se entiende en la GPU la primera data: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, (GLvoid*)0);
    glEnableVertexAttribArray(0);//Linkea el vao con el VBO, si no lo activo, el shader no recibirá los datos. (location =0)

    //como se entiende en la GPU la segunda data: color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(sizeof(GLfloat) * 3));//lo ultimo es de donde empieza (GLVoid)
    glEnableVertexAttribArray(1);

    //Cuadrado tactico
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLint), indexes.data(), GL_STATIC_DRAW);//gl static draw: estara dibujado todo el tiempo, no es dinamico.
    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(GLint), (GLvoid*)0);
    glEnableVertexAttribArray(2);

    ShaderProgram = createShaderProgram();
    //desvinculo
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);

    //Perspective Matrices

}
void MainLoop() {

    //input
    while (running) {
        glm::mat4 u_Camera = Camera.GetViewMatrix();

        GLuint camera_location = glad_glGetUniformLocation(ShaderProgram, "u_Camera");
        glUniformMatrix4fv(camera_location, 1, GL_FALSE, &u_Camera[0][0]);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                running = false;
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_C) {
                std::cout << "TECLA PRESIONADA SUS" << u_Offset<< "\n";
                u_Offset += 0.1f;


                GLuint uniform_location = glad_glGetUniformLocation(ShaderProgram, "u_Offset");
                GLuint perspective_location = glad_glGetUniformLocation(ShaderProgram, "u_Perspective");
                


                

                glUniform1f(uniform_location,u_Offset);
                glUniformMatrix4fv(perspective_location,1,GL_FALSE, &u_perspective[0][0]);

            }

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_W) {
                Camera.MoveForward(0.1f);
                std::cout << "TECLA PRESIONADA W" << u_Offset << "\n";
                

            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_S) {
                Camera.MoveBackward(0.1f);
                std::cout << "TECLA PRESIONADA S" << u_Offset << "\n";
                
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_A) {
                Camera.MoveLatLeft(0.1f);
                std::cout << "TECLA PRESIONADA W" << u_Offset << "\n";


            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_D) {
                Camera.MoveLatRigth(0.1f);
                std::cout << "TECLA PRESIONADA S" << u_Offset << "\n";

            }
            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                Camera.MouseMovement(event.motion.xrel, event.motion.yrel);

            }

                
        }
        
        // Renderizar
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(ShaderProgram);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }
}
void CleanUp() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(ShaderProgram);

    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    Config();
    VertexSpecify();
    MainLoop();
    CleanUp();
    
    
    return 0;
}