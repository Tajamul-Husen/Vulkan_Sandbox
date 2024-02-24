#include "RenderLayer.h"
#include "Application.h"


int main(int argc, char *argv[])
{
    ApplicationConfig config;
    config.width = 800;
    config.height = 600;
    config.title = "Vulkan Sandbox";
    config.layer = new RenderLayer();

    Application sandboxApp(config);
    sandboxApp.Run();

    return 0;
};