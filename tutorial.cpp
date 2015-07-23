#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>

// Shader sources

// use the vec2 position, and then 0.0 (no 3rd dimension) and 1.0 as w, putting the vec into
// 4d homogenous coordinates. w is usually 1.0, which gets used for translations. the other
// coords are divided by w to perform mangification and minification.
const GLchar* vertexSource =
    "#version 150 core\n"
    "in vec2 position;"
    "void main() {"
    "   gl_Position = vec4(position, 0.0, 1.0);"
    "}";

// applies a vec4 color, currently white
const GLchar* fragmentSource =
    "#version 150 core\n"
    "out vec4 outColor;"
    "void main() {"
    "   outColor = vec4(1.0, 1.0, 1.0, 1.0);"
    "}";

int main()
{
    // we use GLFW to create a window on our OS with an opengl context, make that
    // context current, and receive inputs/events
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window); // call before glewinit
    // glew checks for availability of modern opengl functions, as implemented by the graphics
    // card driver, and wires the addresses of those funcs to their common names.
    // Some older functions, (seemingly including glDrawArrays) are not looked by by GLEW
    // because they are part of the API going back to at least opengl1.1, and are pulled in
    // by linking to libGL.so.
    // experimental tells glew to use modern methods for checking on funciton availability
    glewExperimental = GL_TRUE;
    glewInit();

    // generate a vertex array, which stores instructions for populating vertex attributes
    // from the currently bound buffer, linking buffered data and inbound vertex shader
    // variables
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // generate a buffer, which we will use for our vertex data
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // some vertex data
    float vertices[] = {
        0.0f,  0.5f, // Vertex 1 (X, Y)
        0.5f, -0.5f, // Vertex 2 (X, Y)
        -0.5f, -0.5f  // Vertex 3 (X, Y)
    };
    // make our vertexbuffer the currently active buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // buffer the data in, to be written once and used for drawing commands.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // a new vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // using the const *text above
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    // compile it
    glCompileShader(vertexShader);
    // do it again for the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // create a program, which combines shaders. You can have multiple programs, and swap
    // between them.  typically you'd also swap between vertex arrays at that time as well,
    // because different shaders read different buffered data.
    GLuint shaderProgram = glCreateProgram();
    // attach our shaders
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // link the program, after which shader changes won't affect what runs, unless it
    // is linked again
    glLinkProgram(shaderProgram);
    // make this program the currently active program
    glUseProgram(shaderProgram);

    // get the index of the position attribute, currently 0. not uncommon to skip this
    // and just hardcode the indexes.
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    // vertex array attribs need to be enabled
    glEnableVertexAttribArray(posAttrib);
    // this call describes how data for the attrib is acquired from a vertex buffer object.
    // each time the vertex shader is called in parallel, it will receive in the position
    // variable a different value from the buffer, separate by the stride (here 0, which is 
    // the distance in bytes between consecutive values of this attribute) and starting from
    // the offset (here 0, which points to the first location in the vertex array that the
    // attrib can be found). Size is the dimensionality of attribute, so 2 GLfloats will
    // be placed in the position variable, which makes sense, because position is a vec2.
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // a loop that breaks when glfw thinks we should close our window
    while(!glfwWindowShouldClose(window)) {
            // uses the bound buffer, enabled vertex attrib array(s) with the defined
            // vertex attrib pointer(s) to draw primitives to the screen, in this case
            // a TRIANGLE.
            glDrawArrays(GL_TRIANGLES, 0, 3);
            // swap the back buffer to the front buffer after draw is complete
            glfwSwapBuffers(window);
            // retrieve window events
            glfwPollEvents();
    }
    // cleanup
    glfwTerminate();
}
