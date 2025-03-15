#include <iostream>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <vector>
#include <string>
#include "CreateTris.h"
#include <memory>
GLuint VBO=0;
GLuint VAO=0;
int LODS = 3;
std::vector<GLfloat> verticex = {
    // Triángulo 1 LRT
    -1.0f, 0.0f, 0.0f,  -0.8f, 0.0f, 0.0f,  -0.9f,  1.0f, 0.0f,
    // Triángulo 2
    0.8f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,    0.9f,  1.0f, 0.0f,
    // Triángulo 3
    -0.5f, 0.0f, 0.0f,  0.5f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f
};

void Config(std::vector<GLfloat> vertices) {

    //selecciono
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //envio
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    //reseteo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Smart Pointers OpenGL", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    //SDL_SetWindowOpacity(window, 0.1f);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
       
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //Crear Triangulos Ejemplos
    
    {

        // Render loop simplificado


        Config(verticex);
        bool running = true;
        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT || event.key.key == SDLK_ESCAPE) running = false;
                else if (event.type == SDL_EVENT_KEY_UP)
                {
                    if (LODS<9)
                    {
                        LODS = LODS + 3;
                    }
                    else
                    {
                        std::cout << "Llegaste al maximo de triangulitos" << "\n";
                    }

                }
                else if (event.type == SDL_EVENT_KEY_DOWN) {
                    if (event.key.key == SDLK_SPACE) {
                        std::cout << "¡Se presionó la barra espaciadora!" << std::endl;
                    }
                }
            }
            glClearColor(0.f,0.f,0.f,0.f);
            

            glClear(GL_COLOR_BUFFER_BIT);
            if (VAO!=0)
            {
                
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, LODS);
                glBindVertexArray(0);

            }
    
            SDL_GL_SwapWindow(window);
        }
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}




/*
// Tamaño de la ventana
SDL_Window* window = nullptr;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
// Bucle Principal
bool running = true;
SDL_Event event{};

//Vertex Specify

GLuint VAO{}, VBO{};
GLuint ShaderProgram{};

std::string gVertexShaderSource =
"#version 330 core \n"
"layout(location=0) in vec4 position;\n"
"layout(location=1) in vec3 rgbColors; \n"
"out vec3 v_rgbColors; \n"

"void main()\n"
"{\n"
    "v_rgbColors=rgbColors; \n"

"  gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
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
}
void VertexSpecify() {
    //preparar GPU
    std::vector<GLfloat> vertices = {
        -0.8f,-0.8f,0.0f, //left
         1.0f,  0.0f, 0.0f, //colores
         0.8f,-0.8f, 0.0f, //right
         0.0f,  1.0f, 0.0f, //colores
         0.0f, 0.8f, 0.0f, //top
         0.0f,  0.0f, 1.0f  //colores
    };
    
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
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
    ShaderProgram = createShaderProgram();
    //desvinculo
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
}
void MainLoop() {

    //input
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // Renderizar
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(ShaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(window);
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
*/
