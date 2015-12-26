#pragma once

namespace qing
{
	namespace net
	{
		class TCPServer //: public std::enable_shared_from_this<TCPServer>
		{
		public:
			TCPServer();

			virtual ~TCPServer();

            virtual void OnAccept_Tcp(socket_t sock_id) = 0;
            virtual void onDisconnect_Tcp(socket_t sock_id) = 0;
            virtual void DoSomething(){}

			void run();
			void stop(){ _running = false; }

			bool listen(std::string ip, unsigned short port);
			bool listen(unsigned short port);

			bool running();

			PollerPtr pollerPtr();
            CSockDataMgr*   GetSockMgr(){ return &_socket_mgr; }
            //TimerThread& timer_engine(){ return _time_engine; }
		private:
			void update();

		protected:
            Acceptor*		_accept_ptr = nullptr;
            //PollerPtr			_pollerPtr = nullptr;
            bool			_running = false;
            CSockDataMgr    _socket_mgr;
            //TimerThread     _time_engine;

		};
	}
}
