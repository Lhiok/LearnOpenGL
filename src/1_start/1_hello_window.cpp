#include <common/window.h>

class hello_window : public Window
{
protected:
    virtual void onInit();
    virtual void onUpdate();
public:
    hello_window(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) {}
    ~hello_window();
};

/**************************************************** hello_window ****************************************************/

hello_window::~hello_window()
{
    
}

void hello_window::onInit()
{
    std::cout << "hello window init" << std::endl;
}

void hello_window::onUpdate()
{
    // std::cout << "hello window update" << std::endl;
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window = new hello_window("1_hello_window", 800, 600);
    window->start();
    return 0;
}
