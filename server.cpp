#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

int main(void)
{
	/* Создаем сокет */

	int opt = 1;
	int serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == -1)
	{
		std::cerr << "Cannot create a socket!" << std::endl;
		exit (1);
	}
	
	/* Эта команда настраивает сокет таким образом, что занятый им порт
	будет освобожден сразу же по завершении программы */

	setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	/* Создае структуру с настройками сокета: порта, адрес, тип */

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6667);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0

	/* Присвоим адрес и порт сокету */

	if (bind(serverSock, (sockaddr *)&addr, sizeof(addr)) == -1)
	{
		std::cerr << "Could not bind socket!" << std::endl;
		exit (2);
	}

	/* Сделаем сокет пассивным (то есть, ожидающим соединения клиентов) */

	if (listen(serverSock, 1) == -1)
	{
		std::cerr << "Could not make socket passive" << std::endl;
		exit (3);
	}
	
	/* Установим соединение с proxy */
	sockaddr_in proxy;
	socklen_t size = sizeof(proxy);
	int proxySock = accept(serverSock, (sockaddr *)&proxy, &size);
	if (proxySock == -1)
	{
		std::cerr << "Could not establish connection with proxy. Exitting." << std::endl;
		exit (5);
	}
	else
		std::cout << "Connection with proxy established." << std::endl;


	char buf[512];
	int bytes_received = 0;
	while (1)
	{
		memset(buf, 0, 512);
		bytes_received = recv(proxySock, buf, 512, 0);
		if (bytes_received < 1)
			exit(0);
		std::cout << buf;
	}
	close(serverSock);
	close(proxySock);
	return (0);
}
