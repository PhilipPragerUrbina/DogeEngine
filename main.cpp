#include <iostream>
#include "Engine.hpp"


int main(int argc, char* args[])
{
    std::string filename = "MetalRoughSpheres.glb";
    if (argc > 1)  filename = args[1];



    Engine engine("DogeEngine", 1000,700);
    engine.loadFile(filename);
    engine.Run();




    return 0;
}


