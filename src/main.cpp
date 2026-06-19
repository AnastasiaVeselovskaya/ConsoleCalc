#include "application.h"
#include "logger.h"
#include "server/server.h"

#include <pthread.h>

#include <boost/asio.hpp>

#include <csignal>
#include <thread>

int main()
{
    const int portNumber = 8080;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGINT);
    pthread_sigmask(SIG_BLOCK, &mask, nullptr);

    try
    {
        calc::Application app;
        app.initCache();

        boost::asio::io_context ioc;
        server::Server srv(ioc, portNumber, app);
        srv.Run();

        std::thread signalThread([&ioc, &mask]() {
            int sig;
            sigwait(&mask, &sig);
            Logger::GetInstance().LogInfo(
                "Received shutdown signal, stopping...");
            ioc.stop();
        });

        std::thread workerThread([&ioc]() { ioc.run(); });

        workerThread.join();
        signalThread.join();
    }
    catch (const std::exception& e)
    {
        Logger::GetInstance().LogCritical("Unexpected exception: " +
                                          std::string(e.what()));
        return 1;
    }

    return 0;
}