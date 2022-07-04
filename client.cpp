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
	int opt = 1;
	int proxySock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6668);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	setsockopt(proxySock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	if (connect(proxySock, (sockaddr *)&addr, sizeof(addr)) == -1)
	{
		std::cerr << "Could not connect to proxy." << std::endl;
		exit (1);
	}

	char buf[512];
	int bytes_read = 0;
	while (true)
	{
		bzero(buf, 512);
		bytes_read =  read(0, buf, 512);
		if (bytes_read < 1)
			return (0);
		send(proxySock, std::string(buf).c_str(),
		std::string(buf).size(), 0);
	}
	close(proxySock);
	return (0);
}
