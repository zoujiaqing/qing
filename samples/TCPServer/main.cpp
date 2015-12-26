
#include <qing/net/net.hpp>

int main()
{
    short port = 8000;

	std::shared_ptr<qing::net::TcpListener> server(new qing::net::TcpListener("127.0.0.1", 8000);

    if (!server->listen())
    {
        QLOG_ERROR("Can't listen port(%d), port is used.", port);
    }

    system("pause");

	return 0;
}
