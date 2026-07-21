#include <iostream>
#include <boost/asio.hpp>
#include <grpcpp/grpcpp.h>
#include "const.h"
#include "ConfigMgr.h"
#include "StatusServerImpl.h"


void runserver() {
    auto config_mgr = ConfigMgr::Get_instance();
    std::string host = (*config_mgr)["StatusServer"]["Host"];
    std::string port = (*config_mgr)["StatusServer"]["Port"];

    StatusServerImpl service;
    std::string server_addr = host + ":" + port;
    
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

    std::cout << "StatusServer started in " << server_addr << std::endl;
    boost::asio::io_context ioc;
    boost::asio::signal_set sigs(ioc,SIGINT,SIGTERM);
    sigs.async_wait([&server](boost::system::error_code ec, int sig) {
        if (!ec) {
            std::cout << "server close..." << std::endl;
            server->Shutdown();
        }
        });
    std::thread([&ioc]() {ioc.run(); }).detach();
    server->Wait();
    ioc.stop();
}

int main()
{
    try {
        runserver();
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return -1;
    }
}

