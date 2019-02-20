#include <iostream>


int main(int argc, char *argv[])
{
    try
    {
        argc = argc;
        argv = argv;
        return 0;
    }
    catch (const std::exception &e)
    {
        return 1;
    }
}