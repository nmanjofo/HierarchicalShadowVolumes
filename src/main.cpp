#include "Application.hpp"

int main(int argc, char* argv[])
{
    Application app;
    bool ret = app.Run();

    if(ret)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}
