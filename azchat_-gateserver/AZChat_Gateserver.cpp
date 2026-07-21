#include "CServer.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"

#include <assert.h>
void TestRedisMgr() {
    assert(RedisMgr::Get_instance()->Set("blogwebsite", "llfc.club"));
    std::string value = "";
    assert(RedisMgr::Get_instance()->Get("blogwebsite", value));
    assert(RedisMgr::Get_instance()->Get("nonekey", value) == false);
    assert(RedisMgr::Get_instance()->HSet("bloginfo", "blogwebsite", "llfc.club"));
    assert(RedisMgr::Get_instance()->HGet("bloginfo", "blogwebsite") != "");
    assert(RedisMgr::Get_instance()->ExistsKey("bloginfo"));
    assert(RedisMgr::Get_instance()->Del("bloginfo"));
    assert(RedisMgr::Get_instance()->Del("bloginfo"));
    assert(RedisMgr::Get_instance()->ExistsKey("bloginfo") == false);
    assert(RedisMgr::Get_instance()->LPush("lpushkey1", "lpushvalue1"));
    assert(RedisMgr::Get_instance()->LPush("lpushkey1", "lpushvalue2"));
    assert(RedisMgr::Get_instance()->LPush("lpushkey1", "lpushvalue3"));
    assert(RedisMgr::Get_instance()->RPop("lpushkey1", value));
    assert(RedisMgr::Get_instance()->RPop("lpushkey1", value));
    assert(RedisMgr::Get_instance()->LPop("lpushkey1", value));
    assert(RedisMgr::Get_instance()->LPop("lpushkey2", value) == false);
    
}

int main() {
	try {
        //TestRedisMgr();
		auto manager = ConfigMgr::Get_instance();
		std::string gate_port = (*manager)["GateServer"]["Port"];
		uint16_t port = 8080;
		boost::asio::io_context ioc;
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](boost::system::error_code ec, size_t bt) {
			if (ec) {
				return;
			}
			ioc.stop();
			});
		std::make_shared<CServer>(ioc, port)->start();
		ioc.run();
	}
	catch (const std::exception e) {
		std::cout << "Exception is " << e.what() << std::endl;
		return -1;
	}
}