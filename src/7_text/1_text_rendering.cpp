#include <common/window.h>
#include <common/shader.h>
#include <common/font.h>

class freetypefont : public Window
{
private:
    Font *_font;
    Shader *_shader;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    freetypefont(std::string name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~freetypefont();
};

/**************************************************** freetypefont ****************************************************/

freetypefont::~freetypefont()
{
    delete _shader;
    delete _font;
    _shader = nullptr;
    _font = nullptr;
}

void freetypefont::onInit(GLFWwindow *window)
{
    _shader = new Shader("common/freetypefont.vs", "common/freetypefont.fs");
    _font = new Font("CONSOLA.TTF", 0, 48);
}

void freetypefont::onUpdate(GLFWwindow *window)
{
    int width = 0, height = 0;
    glfwGetWindowSize(window, &width, &height);
    _font->DrawText(_shader, "Hello, World", width / 3.0f, height / 2.0f, 1.0f, glm::vec3(0.0f, 1.0f, 1.0f), width, height);
}

/**************************************************** test ****************************************************/

int main()
{

    // fft
    Window *window1 = new freetypefont("1_freetypefont", 800, 600);
    window1->start();
    delete window1;

    return 0;
}