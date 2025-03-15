#pragma once

#include <glad/glad.h>
#include <vector>
#include <iostream>

class Tris
{
public:
    GLuint VAO, VBO;
    Tris() {
        // Datos del triángulo
        std::vector<GLfloat> vertices = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };
        //mandar la info al gpu
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        std::cout << "Triángulo creado (VAO: " << VAO << ", VBO: " << VBO << ")\n";
    }

    // Destructor: Libera recursos automáticamente
    ~Tris() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        std::cout << "Triángulo eliminado (VAO: " << VAO << ", VBO: " << VBO << ")\n";
    }

    // Dibujar el triángulo
    void draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

};

