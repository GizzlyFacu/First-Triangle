#include <iostream>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <vector>
#include <string>
/*
creacion uniforms
hacer el fragment shader con el uniform
compilar y agregar el fragment al program
hacer el getuUniform
*/
GLuint VBO{};
GLuint VAO=0;
GLuint EBO=0; //Element array 
std::vector<GLfloat> uniformRGB{1.0f,0.0f,0.0f};
GLuint ShaderProgram=0; //Element array 

int LODS = 3;
SDL_Window* window = nullptr;

std::string gVertexShaderSource =
"#version 330 core \n"
"layout(location=0) in vec3 position;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(position.x, position.y, position.z, 1.0f);\n"
"}\n";

std::string gFragmentShaderSource =

"#version 330 core \n"
"uniform vec3 color;\n"
"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"  FragColor = vec4(color.r, color.g, color.b, 1.0f);\n"
"}\n";

std::vector<GLfloat> verticex = {
    // Quad Anti-ClockWise
    //Center
    -0.3f, -0.1f, 0.0f,   //L 0
     0.3f, -0.1f, 0.0f,   //R 1
     0.3f,  0.1f, 0.0f,   //TR 2 
    -0.3f,  0.1f, 0.0f,   //TL 3
    // Quad top
    -0.2f,  0.1f, 0.0f,   //L 4
     0.2f,  0.1f, 0.0f,   //R 5
     0.2f,  0.2f, 0.0f,   //TR 6 
    -0.2f,  0.2f, 0.0f,   //TL 7
    //Tris Top Left: Index Magic
    //Tris Top Right: Index Magic
    // Quad top
    -0.2f,  -0.1f, 0.0f,   //L 8
     0.2f,  -0.1f, 0.0f,   //R 9
     0.2f,  -0.2f, 0.0f,   //TR 10 
    -0.2f,  -0.2f, 0.0f,   //TL 11



};

std::vector<GLint> eboIndexes = {
    0,1,3,1,2,3,    // quad center
    4,5,7,5,6,7,    // quad top
    3,4,7,          // tris top left
    6,5,2,          // tris top right
    8,9,11,11,9,10, // quad bot
    0,8,11,         // tris bot left
    10,9,1          // tris bot right
    //mirror quien te conoce all my homies uses copy paste
};

std::vector<GLfloat> RGBcolors = {1.0f,0.0f,0.0f};



void VertexShaderMagic(std::string* VertexSource, std::string* FragmentSource) {
    // crear programa
    ShaderProgram = glCreateProgram();

    //----VertexShader
    //crear y compilar
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* v_src = VertexSource->c_str();
    glShaderSource(vertexShader, 1, &v_src, nullptr);
    glCompileShader(vertexShader);
    glAttachShader(ShaderProgram, vertexShader); //los aniade

    //----FragmentShader
    //crear y compilar
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* f_src = FragmentSource->c_str();
    glShaderSource(fragmentShader, 1, &f_src, nullptr);
    glCompileShader(fragmentShader);
    glAttachShader(ShaderProgram, fragmentShader); //los aniade

    //----Ensamble
    glLinkProgram(ShaderProgram);//ensambla el vertex y fragment en la pipeline (in-out variables)

    //Verificacion
    GLint success;
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
        std::cerr << "Error al linkear el programa: " << infoLog << std::endl;
    }

    //Clean
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}
void Config(std::vector<GLfloat> vertices) {

    //genero
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //----Vertices-----
    //selecciono 
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //envio
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }

    //Cuadrado tactico
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboIndexes.size() * sizeof(GLint), eboIndexes.data(), GL_STATIC_DRAW);
    // no hace falta glVertexAttribPointer y glEnableVertexAttribArray porque no son vertices xd son indices.
    //Se relacion el ebo con el vbo en glDrawElements();

    GLenum error2 = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error2 << std::endl;
    }




    glEnableVertexAttribArray(0);
    //reseteo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void Clean() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(ShaderProgram);

    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Touch the Button", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    //SDL_SetWindowOpacity(window, 0.1f);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
       
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    {
        Config(verticex);
        VertexShaderMagic(&gVertexShaderSource, &gFragmentShaderSource);
        bool running = true;
        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                //exit
                if (event.type == SDL_EVENT_QUIT || event.key.key == SDLK_ESCAPE) running = false;

                //changing button color
                if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    if (event.button.x>281 && event.button.x<520 && event.button.y>239 && event.button.y<359) {
                        std::cout << "Dentro del rango (" << event.button.x << ", " << event.button.y << ")" << std::endl;
                        uniformRGB={ 0.0f,1.0f,0.0f };
                    }
                    else
                    {
                        std::cout << "Fuera del Rango (" << event.button.x << ", " << event.button.y << ")" << std::endl;
                    }
                }
                else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && uniformRGB[0]==0.0f)
                {
                    uniformRGB = { 1.0f,0.0f,0.0f };
                }

            }

            glClearColor(0.f,0.f,0.f,0.f);
            glClear(GL_COLOR_BUFFER_BIT);
            if (VAO!=0)
            {

                glBindVertexArray(VAO);
                glUseProgram(ShaderProgram);
                glDrawElements(GL_TRIANGLES, (GLint)eboIndexes.size(), GL_UNSIGNED_INT, 0);
                
                glBindVertexArray(0);

            }
            else
            {
                Clean();
            }
            
            GLuint colorLocation = glGetUniformLocation(ShaderProgram, "color");
            glUniform3f(colorLocation, uniformRGB[0], uniformRGB[1], uniformRGB[2]);


            SDL_GL_SwapWindow(window);
        }
        Clean();
    }

    
    return 0;
}
