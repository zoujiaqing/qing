#include <qing/net/net.hpp>

const std::string server_ip				= "127.0.0.1";
const unsigned short server_port		= 5555;
const std::string message_content		= "Hello";
uint32_t cur_time = 0;
class MyConnection : public qing::net::TCPConnection
{
public:
    MyConnection(socket_t sock_id, qing::net::CSockDataMgr* pSockMgr) : TCPConnection(sock_id, pSockMgr)
	{
		/* 使用消息封包模式，服务端和客户端这个设置要保持一致 */
        this->setMessagePacket(true);
	}

	~MyConnection()
	{
	}

	void onMessage(char* message, uint32_t length)
	{
        //static int num = 0; 
        //num++;
        //if (num >=100000)
        //{
        //    printf("now num = %d", num);
        //}
        //printf("now num = %d", num);
        
	}

    void SendMsg(char* message, uint32_t length)
    {
        DeliverSend(message, length);
    }

private:

};

class MyClient : public qing::net::TCPClient
{
public:
	MyClient()
	{
		// TODO
        _pmessage_work = new qing::net::MessageWorker(GetSockMgr());
        _pmessage_work->WorkInThread();
	}

	~MyClient()
	{
		// TODO
		_pmessage_work->StopThread();        _pmessage_work->JoinWork();
        delete _pmessage_work;
	}

    void onConnect(socket_t sock_id, int serverType)
	{
        qing::net::TCPConnection* connection = (new MyConnection(sock_id, GetSockMgr()));
        _sock_id = sock_id;
        connection = nullptr;
	}


    void onDisconnect_Tcp(socket_t sock_id, int session_id)
    {
    }

private:
    qing::net::MessageWorker*        _pmessage_work;
    socket_t                        _sock_id;
};

int main(int argc, char *argv[])
{
    int max_client_num = 1;

    if (argc == 2)
    {
	max_client_num = atoi(argv[1]);
    }

    MyClient* pRobotMgr = new MyClient;
    for (int i = 0; i < max_client_num; i++)
    {
        //pRobotMgr->connect("192.168.32.95", 10005);
        pRobotMgr->connect("127.0.0.1", 10005, -1);
        //pRobotMgr->connect("122.226.73.198", 10005);
    }
    std::thread serverthread = std::thread(std::bind(&MyClient::run, pRobotMgr));

    //cur_time = g_time_engine.time_sec();
    using namespace qing::net;
    if (pRobotMgr->GetSockMgr()->ConnectedSize() == max_client_num)
    {
	fprintf(stderr, "All client have connected. num [%d], every send 10000 pkt, pkt size 8", max_client_num);
        
        pRobotMgr->GetSockMgr()->testClient(); // send 10000 together
        //std::this_thread::sleep_for(std::chrono::seconds(5));
        //SocketMgr()->testClient();
        //SocketMgr()->PrintTimes();
    }

    //std::this_thread::sleep_for(std::chron2o::milliseconds(10));
    //using namespace qing::net;
    //auto client_map = SocketMgr()->GetSockMap();
    //auto itr = client_map.begin();
    //auto itrend = client_map.end();
    //for (; itr != itrend; itr++)
    //{
    //    auto conn = (MyConnection*)itr->second.pConn;
    //    conn->SendMsg("aaaa", 4);
    //}
    system("pause");
    std::this_thread::sleep_for(std::chrono::seconds(20));

	return 0;
}
