#include <stdio.h>
#include "include/glad/glad.h"
#include "glad.c"
#include "shader.eg"
#include "GLFW/glfw3.h"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_interpolation.hpp"
typedef unsigned int UI;
UI Program;

struct Data
{
    UI VAO;
    UI VBO;
};

int WND_WIDTH = 1000;
int WND_HEIGHT = 600;

Data BoxData()
{
    float Rec[] =
        {
            0, 0,
            0, 1,
            1, 1,
            1, 1,
            1, 0,
            0, 0};
    Data a = {};
    glGenVertexArrays(1, &a.VAO);
    glBindVertexArray(a.VAO);

    glGenBuffers(1, &a.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, a.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Rec), Rec, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0);

    return a;
};

void DrawBox(Data a, int scaleX, int scaleY, int transX, int transY, float colorX, float colorY, float colorZ)
{
    glm::mat4 Model(1);
    Model = glm::translate(Model, glm::vec3(transX, transY, 0));
    Model = glm::scale(Model, glm::vec3(scaleX, scaleY, 0));
    glUniformMatrix4fv(glGetUniformLocation(Program, "Model"), 1, 0, glm::value_ptr(Model));
    glUniform3f(glGetUniformLocation(Program, "ObjColor"), colorX, colorY, colorZ);
    glBindVertexArray(a.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, a.VBO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(WND_WIDTH, WND_HEIGHT, "Space Game", 0, 0);
    glfwMakeContextCurrent(window);

    gladLoadGL();

    UI Vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Vs, 1, &vertexShaderSource, 0);
    glCompileShader(Vs);

    UI Fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Fs, 1, &fragmentShaderSource, 0);
    glCompileShader(Fs);

    Program = glCreateProgram();
    glAttachShader(Program, Vs);
    glAttachShader(Program, Fs);
    glLinkProgram(Program);
    glUseProgram(Program);

    glm::mat4 Proj(1);
    Proj = glm::ortho(0.0f, WND_WIDTH / 1.0f, WND_HEIGHT / 1.0f, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(Program, "Proj"), 1, 0, glm::value_ptr(Proj));

    int SZ_CIRCLE = 360 * 3 * 2;
    float *Circle = (float *)malloc(sizeof(float) * SZ_CIRCLE);
    int index = 0;
    int alpha = 0;

    while (alpha < 360)
    {
        float x = sin((alpha * 3.14) / 180.0f);
        Circle[index++] = x;
        float y = cos((alpha * 3.14) / 180.0f);
        Circle[index++] = y;

        alpha++;

        Circle[index++] = 0;
        Circle[index++] = 0;

        Circle[index++] = sin((alpha * 3.14) / 180.0f);
        Circle[index++] = cos((alpha * 3.14) / 180.0f);
    };

    Data c;
    glGenVertexArrays(1, &c.VAO);
    glBindVertexArray(c.VAO);

    glGenBuffers(1, &c.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, c.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SZ_CIRCLE, Circle, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0);

    bool StartMove = true;
    Data a = BoxData();
    int PaddleOneY = 0;
    int PaddleTwoY = 0;
    int moveX = 150;
    int moveY = 150;
    int DirX = 1;
    int DirY = 1;
    bool Restart = false;
    glfwSwapInterval(3);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 1);

        DrawBox(a, 2, WND_HEIGHT, WND_WIDTH / 2, 0, 1, 1, 1);
        DrawBox(a, 25, 150, 10, PaddleOneY + 40, 1, 1, 1);
        DrawBox(a, 25, 150, WND_WIDTH - 50, PaddleTwoY + 40, 1, 1, 1);

        // TODO:
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && PaddleTwoY <= (WND_HEIGHT - 200))
        {
            PaddleTwoY += 25;
        }
        else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && PaddleTwoY >= 0)
        {
            PaddleTwoY -= 25;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && PaddleOneY >= 0)
        {
            PaddleOneY -= 25;
        }
        else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && PaddleOneY <= (WND_HEIGHT - 200))
        {
            PaddleOneY += 25;
        }

        // TODO:
        moveX += 10 * DirX;
        moveY += 10 * DirY;

        if (moveY >= WND_HEIGHT || moveY <= 10)
        {
            DirY *= -1;
        }

        // TODO:
        if (moveY >= PaddleOneY && moveY <= (PaddleOneY + 150))
        {
            if (moveX >= 10 && moveX <= 35)
            {
                DirX *= DirX;
                DirY *= -DirY;
            }
        }

        if (moveY >= PaddleTwoY && moveY <= (PaddleTwoY + 150))
        {
            if (moveX >= (WND_WIDTH - 50) && moveX <= (WND_WIDTH - 25))
            {
                DirX *= -DirX;
                DirY *= -DirY;
            }
        }

        // TODO:
        if (Restart == true)
        {
            if (moveX < 0 || moveX > WND_WIDTH)
            {
                moveX = WND_WIDTH / 2;
                moveY = WND_HEIGHT / 2;
            };
        }
        else
        {
            Restart = false;
        }

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            Restart = true;
        }

        glm::mat4 CModel(1);
        CModel = glm::translate(CModel, glm::vec3(moveX, moveY, 0));
        CModel = glm::scale(CModel, glm::vec3(10, 10, 0));
        glUniformMatrix4fv(glGetUniformLocation(Program, "Model"), 1, 0, glm::value_ptr(CModel));
        glUniform3f(glGetUniformLocation(Program, "ObjColor"), 1, 0, 0);
        glBindVertexArray(c.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, c.VBO);
        glDrawArrays(GL_TRIANGLES, 0, 360 * 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 07;
}