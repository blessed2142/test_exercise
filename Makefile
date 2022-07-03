all:
		c++ -Wall -Wextra -Werror server.cpp -o server
		c++ -Wall -Wextra -Werror proxy.cpp -o proxy
		c++ -Wall -Wextra -Werror client.cpp -o client

clean:
		rm proxy server client logs.txt
