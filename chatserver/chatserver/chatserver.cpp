#include <iostream>
#include "const.h"
#include "COnfigMgr.h"
#include <memory>
#include "Cserver.h"
#include "AsioContextPool.h"
#include "RedisMgr.h"
#include "ChatServiceImpl.h"
int main()
{
    SetConsoleOutputCP(CP_UTF8); // 输出 UTF-8
    SetConsoleCP(CP_UTF8);       // 输入 UTF-8
    //std::cout << "你好 世界" << std::endl;
    try {
        auto conf_mgr = ConfigMgr::Get_instance();
        std::string port = (*conf_mgr)["SelfServer"]["Port"];
        RedisMgr::Get_instance()->HSet(LOGIN_COUNT, SELF_SERVER, "0");
        
        ChatServiceImpl service;
        grpc::ServerBuilder builder;
        std::string addr = (*conf_mgr)["SelfServer"]["Host"] + ":" + (*conf_mgr)["SelfServer"]["RPCPort"];
        builder.AddListeningPort(addr,grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> grpc_server = builder.BuildAndStart();
        std::thread grpc_thread = std::thread([&grpc_server]() {
            grpc_server->Wait();
            });

        auto pool = AsioContextPool::Get_instance();
        unsigned port_s = static_cast<unsigned short>(std::stoi(port));
        boost::asio::io_context ioc;
        std::unique_ptr<Cserver> server(new Cserver(ioc,port_s));
        boost::asio::signal_set sigs(ioc, SIGINT, SIGTERM);
        sigs.async_wait([&ioc,pool,&grpc_server](auto, auto) {
            ioc.stop();
            pool->Stop();
            grpc_server->Shutdown();
            });
        
        ioc.run();
        RedisMgr::Get_instance()->Hdel(LOGIN_COUNT, SELF_SERVER);
        RedisMgr::Get_instance()->Close();
        grpc_thread.join();
    }
    catch (const std::exception e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}

