#include "application.h"

int main(int argc, char** argv)
{
    try {
        calc::Application app;
        app.applicationRun(argc, argv);
    } catch (const std::exception& e) {
        Logger::GetInstance().LogCritical("Unexpected exception caught: " +
                                          std::string(e.what()));
    }
}
