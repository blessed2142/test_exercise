#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

void socketInitServer(int &serverSock)
{
	/* Создаем сокет */

	int opt = 1;
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == -1)
	{
		std::cerr << "Cannot create a socket!" << std::endl;
		exit (1);
	}
	
	/* Эта команда настраивает сокет таким образом, что занятый им порт
	будет освобожден сразу же по завершении программы */

	setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	/* Создае структуру с настройками сокета: порта, адрес, тип. 
	Устанавливаем соединение. */

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6667);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0

	if (connect(serverSock, (sockaddr *)&addr, sizeof(addr)) == -1)
	{
		std::cerr << "Could not connect to server. Exitting." << std::endl;
		exit (5);
	}
}

void socketInitProxy(int &proxySock)
{
	/* Создаем сокет */

	int opt = 1;
	proxySock = socket(AF_INET, SOCK_STREAM, 0);
	if (proxySock == -1)
	{
		std::cerr << "Cannot create a proxy socket!" << std::endl;
		exit (1);
	}
	
	/* Эта команда настраивает сокет таким образом, что занятый им порт
	будет освобожден сразу же по завершении программы */

	setsockopt(proxySock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	/* Создае структуру с настройками сокета: порта, адрес, тип */

	sockaddr_in proxyAddr;
	proxyAddr.sin_family = AF_INET;
	proxyAddr.sin_port = htons(6668);
	proxyAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
	
	/* Присвоим адрес и порт сокету */

	if (bind(proxySock, (sockaddr *)&proxyAddr, sizeof(proxyAddr)) == -1)
	{
		std::cerr << "Could not bind proxy socket!" << std::endl;
		exit (2);
	}

	/* Сделаем сокет пассивным (то есть, ожидающим соединения клиентов) */

	if (listen(proxySock, 1) == -1)
	{
		std::cerr << "Could not make proxy socket passive" << std::endl;
		exit (3);
	}
}

void socketInitClient(int &clientSock, int &proxySock)
{
	sockaddr_in client;
	socklen_t size = sizeof(client);

	clientSock = accept(proxySock, (sockaddr *)&client, &size);
	if (clientSock == -1)
	{
		std::cerr << "Could not establish connection between proxy and client. ";
		std::cerr << "Exitting." << std::endl;
		exit (3);
	}
}

int main(int argc, char **argv)
{
	/* Проверяем входные аргументы  */
	if (argc != 2)
	{
		std::cout << "Usage: ./proxy loggingFlag. 1 - enable logs, 0 - disable."
		<< std::endl;
		return (0);
	}

	int log = atoi(argv[1]);
	if (log != 0 && log != 1)
	{
		std::cerr << "Usage: ./proxy loggingFlag. 1 - enable logs, 0 - disable."
		<< std::endl;
		exit (6);
	}

	/* Открываем сокеты, устанавливаем соединения */

	int proxySock, serverSock, clientSock;
	socketInitProxy(proxySock); // Создаем пассивный сокет прокси-сервера
	socketInitClient(clientSock, proxySock); // устанавливаем соединение с клиентом
	socketInitServer(serverSock); // устанавливаем соединение с сервером

	char buf[512];
	int bytes_received = 0;
	std::string message;

	/* Cоздаем файл для логирования */
	std::ofstream outf("logs.txt", std::ios::ate);
	if (!outf)
	{
		std::cerr << "Cannot create file\n";
		return (1);
	}

	/* Основной цикл. Получаем сообщение от клиента. Отправляем его на сервер. */

	while (true)
	{
		message.clear();
		bzero(buf, 512);
		bytes_received = recv(clientSock, buf, 512, 0);
		if (bytes_received < 1)
		{
			std::cout << "Error ocurred or client diconnected. Exitting."
			<< std::endl;
			exit(0);
		}
		message = buf;
		send(serverSock, message.c_str(), message.size(), 0);
		if (log)
		{
			outf << std::string(512, '_') << std::endl << std::endl;
			outf << message;
			outf << std::string(512, '_') << std::endl;
		}
	}
}
