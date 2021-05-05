#include "pch.h"
#include "Event.pb.h"
#include "nat/Message.h"

//! Heartbeat pointer type.
typedef boost::shared_ptr<test::opennat::Heartbeat> HeartbeatPtr;
//! Event pointer type.
typedef boost::shared_ptr<test::opennat::Greeting> GreetingPtr;

void Loop();

// Usage:
// *.exe --server
// *.exe -s

// *.exe --client1
// *.exe -1

// *.exe --client2
// *.exe -2

// Handles connect.
void HandleConnect1(
	opennat::types::ClientPtr client,
	const boost::uuids::uuid& sessionId,
	const opennat::types::SessionType type);
// Handles connect.
void HandleConnect2(
	const boost::uuids::uuid& clientId,
	opennat::types::ClientPtr client,
	const boost::uuids::uuid& sessionId,
	const opennat::types::SessionType type);
// Handles error.
void HandleError(
	opennat::types::ClientPtr client,
	const boost::uuids::uuid& sessionId,
	const opennat::types::SessionType type);
// Handles receive.
void HandleReceiveClient(
	opennat::types::ClientPtr client,
	const boost::uuids::uuid& id,
	const opennat::types::Buff::value_type* data,
	const size_t bytes);
// Handles receive.
void HandleReceiveServer(
	opennat::types::ServerPtr server,
	const boost::uuids::uuid& id,
	const opennat::types::Buff::value_type* data,
	const size_t bytes);
// Ping.
void Ping(opennat::types::Buff& data);
// Pong.
void Pong(opennat::types::Buff& data);

int main(int argc, char* argv[])
{
	try
	{
		boost::program_options::options_description desc("All options");
		desc.add_options()
			("help", "Produce help message")
			("client1,1", "Client mode")
			("client2,2", "Client mode")
			("server,s", "Server mode");

		boost::program_options::variables_map vm;
		try
		{
			boost::program_options::store(
				boost::program_options::parse_command_line(argc, argv, desc), vm);
		}
		catch (const boost::program_options::error& e)
		{
			LERR_ << "Failed to parse command line: " << e.what();
		}

		boost::uuids::uuid clientId = boost::uuids::string_generator()("0d32bebc-f846-11e6-bc64-92361f002671");
		boost::program_options::notify(vm);
		if (vm.count("help") || argc < 2 || vm.empty())
		{
			std::stringstream stream;
			desc.print(stream);
			LERR_ << stream.str();

			return EXIT_FAILURE;
		}
		else if (vm.count("client1") > 0)
		{
			opennat::types::ClientPtr client1;
			client1 = opennat::GetKernel().GetClient(clientId);
			client1->DoOnConnected(boost::bind(HandleConnect1, client1, _1, _2));
			client1->DoOnError(boost::bind(HandleError, client1, _1, _2));
			client1->DoOnReceive(boost::bind(HandleReceiveClient, client1, _1, _2, _3));

			boost::asio::ip::tcp::resolver::query query(
				boost::asio::ip::tcp::v4(), /*"192.168.25.199"*/"192.168.25.2"/*boost::asio::ip::host_name()*/, "21042");
			boost::asio::io_service ioService;
			boost::asio::ip::tcp::resolver resolver(ioService);
			boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
			client1->Connect(*it, client1->GetId());

			Loop();
		}
		else if (vm.count("client2") > 0)
		{
			opennat::types::ClientPtr client2;
			client2 = opennat::GetKernel().GetClient(boost::uuids::random_generator()());
			client2->StartUPNP();

			client2->DoOnConnected(boost::bind(HandleConnect2, clientId, client2, _1, _2));
			client2->DoOnError(boost::bind(HandleError, client2, _1, _2));
			client2->DoOnReceive(boost::bind(HandleReceiveClient, client2, _1, _2, _3));

			boost::asio::ip::tcp::resolver::query query(
				boost::asio::ip::tcp::v4(), boost::asio::ip::host_name(), "21042");
			boost::asio::io_service ioService;
			boost::asio::ip::tcp::resolver resolver(ioService);
			boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
			client2->AsyncConnect(*it, client2->GetId());

			Loop();
		}
		else if (vm.count("server") > 0)
		{
			opennat::types::ServerPtr server;
			server = opennat::GetKernel().GetServer();
			server->DoOnReceive(boost::bind(&HandleReceiveServer, server, _1, _2, _3));
			server->Start(21042);

			Loop();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

void HandleConnect1(
	opennat::types::ClientPtr client,
	const boost::uuids::uuid& id,
	const opennat::types::SessionType type)
{
	if (id == client->GetId() && type == opennat::types::STUN)
	{
		LDBG_ << "Client connected...";

// 		GreetingPtr greeting(new test::opennat::Greeting());
// 		greeting->set_client_id(boost::uuids::to_string(client->GetId()));
// 		opennat::types::MessagePtr msg(new opennat::Message());
// 		msg->SetBuff(greeting);
// 
// 		client->Send2Server(msg);
	}
	else if (type == opennat::types::DIRECT)
	{
		LDBG_ << "Direct session: " << boost::uuids::to_string(id) << " has been connected";
	}
	else if (type == opennat::types::PROXYDOWN || type == opennat::types::PROXYUP)
	{
		LDBG_ << "Proxy session: " << boost::uuids::to_string(id) << " has been connected";
	}
}

void HandleConnect2(
	const boost::uuids::uuid& clientId,
	opennat::types::ClientPtr client,
	const boost::uuids::uuid& id,
	const opennat::types::SessionType type)
{
	if (id == client->GetId() && type == opennat::types::STUN)
	{
		LDBG_ << "Client connected...";
		client->Direct(clientId, boost::uuids::random_generator()());
// 		opennat::types::SessionConfigPtr config(new opennat::types::SessionConfig());
// 		config->clientId_ = clientId;
// 		config->sessionId_ = boost::uuids::random_generator()();
// 		config->sessionType_ = opennat::types::PROXYUP;
// 		client->Proxy(config);
	}
	else if (type == opennat::types::DIRECT)
	{
		LDBG_ << "Direct session: " << boost::uuids::to_string(id) << " has been connected";

		HeartbeatPtr heartbeat(new test::opennat::Heartbeat);
		heartbeat->set_type(test::opennat::Heartbeat_Type_Ping);

		opennat::types::MessagePtr message(new opennat::Message());
		message->SetBuff(heartbeat);

		client->Send(id, message);
	}
	else if (type == opennat::types::PROXYDOWN || type == opennat::types::PROXYUP)
	{
		LDBG_ << "Proxy session: " << boost::uuids::to_string(id) << " has been connected";

// 		HeartbeatPtr heartbeat(new test::opennat::Heartbeat);
// 		heartbeat->set_type(test::opennat::Heartbeat_Type_Ping);
// 
// 		opennat::types::MessagePtr message(new opennat::Message());
// 		message->SetBuff(heartbeat);
// 
// 		client->Send(id, message);
	}
}

void HandleError(
	opennat::types::ClientPtr client,
	const boost::uuids::uuid& id,
	const opennat::types::SessionType type)
{
	if (id == client->GetId() && type == opennat::types::STUN)
	{
		LDBG_ << "Client error...";
	}
	else if (type == opennat::types::DIRECT)
	{
		LDBG_ << "Direct session: " << boost::uuids::to_string(id) << " error";
	}
	else if (type == opennat::types::PROXYDOWN || type == opennat::types::PROXYUP)
	{
		LDBG_ << "Proxy session: " << boost::uuids::to_string(id) << " error";
	}
	else
	{
		LERR_ << "Unknown error from session: " << boost::uuids::to_string(id);
	}
}

void HandleReceiveClient(
	opennat::types::ClientPtr client,
	const boost::uuids::uuid& id,
	const opennat::types::Buff::value_type* data,
	const size_t bytes)
{
	HeartbeatPtr heartbeat(new test::opennat::Heartbeat);
	if (!heartbeat->ParseFromArray(data, bytes))
	{
		LERR_ << "Unable to parse protocol buffer";
		return;
	}

	opennat::types::MessagePtr message(new opennat::Message);
	if (heartbeat->type() == test::opennat::Heartbeat_Type_Pong)
	{
		LAPP_ << "--->> Ping";
		heartbeat->set_type(test::opennat::Heartbeat_Type_Ping);
		message->SetBuff(heartbeat);
		boost::this_thread::sleep_for(boost::chrono::seconds(1));
	}
	else if (heartbeat->type() == test::opennat::Heartbeat_Type_Ping)
	{
		LAPP_ << "<<--- Pong";
		heartbeat->set_type(test::opennat::Heartbeat_Type_Pong);
		message->SetBuff(heartbeat);
		boost::this_thread::sleep_for(boost::chrono::seconds(1));
	}
	client->Send(id, message);
}

void HandleReceiveServer(
	opennat::types::ServerPtr server,
	const boost::uuids::uuid& id,
	const opennat::types::Buff::value_type* data,
	const size_t bytes)
{
	GreetingPtr greeting(new test::opennat::Greeting);
	if (!greeting->ParseFromArray(data, bytes))
	{
		LERR_ << "Unable to parse protocol buffer";
		return;
	}

	LDBG_ << "Greeting from client with id: " << greeting->client_id();
}

void Loop()
{
	MSG msg;
	while (!GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}