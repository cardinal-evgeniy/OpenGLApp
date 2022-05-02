#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

const float PI = 3.14159265f;
const float radPerDeg = PI / 180.f;

GLuint VAO, VBO, shader, uniformModel;

bool direction = true;
float triOffset = 0.f;
float triMaxOffset = 0.7f;
float triIncrement = 0.001f;

float currAngle = 0.f;

bool sizeDirection = true;
float currSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;


// Vertex Shader
static const char* vShader = "                                              \n\
#version 330                                                                \n\
                                                                            \n\
layout (location = 0) in vec3 pos;                                          \n\
                                                                            \n\
uniform mat4 model;                                                         \n\
                                                                            \n\
void main()                                                                 \n\
{                                                                           \n\
    gl_Position = model * vec4(pos, 1.0);                                   \n\
}";

// Fragment Shader
static const char* fShader = "                                      \n\
#version 330                                                        \n\
                                                                    \n\
out vec4 colour;                                                    \n\
                                                                    \n\
void main()                                                         \n\
{                                                                   \n\
    colour = vec4(1.0, 1.0, 0.0, 1.0);                              \n\
}";

void CreateTriangle()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );

    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
}

void AddShader( GLuint theProgram, const char* shaderCode, GLenum shaderType )
{
    GLuint theShader = glCreateShader( shaderType );

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen( shaderCode );

    glShaderSource( theShader, 1, theCode, codeLength );
    glCompileShader( theShader );

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv( theShader, GL_COMPILE_STATUS, &result );
    if ( !result )
    {
        glGetShaderInfoLog( theShader, sizeof( eLog ), NULL, eLog );
        std::cout << "Error compiling the " << shaderType << " shader: " << eLog << std::endl;
        return;
    }

    glAttachShader( theProgram, theShader );
}

void CompileShaders()
{
    shader = glCreateProgram();
    if ( !shader )
    {
        std::cout << "Error creating shader program!\n";
        return;
    }

    AddShader( shader, vShader, GL_VERTEX_SHADER );
    AddShader( shader, fShader, GL_FRAGMENT_SHADER );

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram( shader );
    glGetProgramiv( shader, GL_LINK_STATUS, &result );
    if ( !result )
    {
        glGetProgramInfoLog( shader, sizeof( eLog ), NULL, eLog );
        std::cout << "Error linking program: " << eLog << std::endl;
        return;
    }

    glValidateProgram( shader );
    glGetProgramiv( shader, GL_VALIDATE_STATUS, &result );
    if ( !result )
    {
        glGetProgramInfoLog( shader, sizeof( eLog ), NULL, eLog );
        std::cout << "Error validating program: " << eLog << std::endl;
        return;
    }

    uniformModel = glGetUniformLocation( shader, "model" );
}

int main()
{
    // Initialise GLFW
    if ( !glfwInit() )
    {
        std::cout << "GLFW initialisation failed!" << std::endl;
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    // OpenGL version
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );

    // Core profile = No Backwards Compatibility
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

    GLFWwindow* mainWindow = glfwCreateWindow( WIDTH, HEIGHT, "Test Window", NULL, NULL );
    if ( !mainWindow )
    {
        std::cout << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize( mainWindow, &bufferWidth, &bufferHeight );

    // Set context for GLEW to use
    glfwMakeContextCurrent( mainWindow );

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if ( glewInit() != GLEW_OK )
    {
        std::cout << "GLEW initialisation failed!" << std::endl;
        glfwDestroyWindow( mainWindow );
        glfwTerminate();
        return 1;
    }

    // Setup Viewport size
    glViewport( 0, 0, bufferWidth, bufferHeight );

    CreateTriangle();
    CompileShaders();

    // Loop until window closed
    while ( !glfwWindowShouldClose( mainWindow ) )
    {
        // Get & Handle user input events
        glfwPollEvents();

        {
            if ( direction )
            {
                triOffset += triIncrement;
            }
            else
            {
                triOffset -= triIncrement;
            }

            if ( abs( triOffset ) >= triMaxOffset )
            {
                direction = !direction;
            }
        }

        {
            currAngle += 0.01f;
            if ( currAngle >= 360.f )
            {
                currAngle -= 360.f;
            }
        }

        {
            if ( sizeDirection )
            {
                currSize += 0.001f;
            }
            else
            {
                currSize -= 0.001f;
            }

            if ( currSize >= maxSize || currSize <= minSize )
            {
                sizeDirection = !sizeDirection;
            }
        }

        // Clear window
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        glUseProgram( shader );

        glm::mat4 model( 1.f ); // should be initialized
        model = glm::translate( model, glm::vec3( triOffset, 0.f, 0.f ) );
        model = glm::rotate( model, currAngle * radPerDeg, glm::vec3( 0.f, 0.f, 1.f ) ); // the last parameter is the axis to rotate around
        model = glm::scale( model, glm::vec3( currSize, currSize, 1.f ) );

        glUniformMatrix4fv( uniformModel, 1, GL_FALSE, glm::value_ptr( model ) );
        //glUniform1f( uniformXMove, triOffset );

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, 3 );
        glBindVertexArray( 0 );

        glUseProgram( 0 );

        glfwSwapBuffers( mainWindow );
    }

    return 0;
}
