CPPFLAGS = -Wall -Werror -Wextra -std=c++17

all:
	g++ $(CPPFLAGS) ipc_1.cpp -o ipc_1.out
	g++ $(CPPFLAGS) ipc_2.cpp -o ipc_2.out

clean:
	rm -rf *.out

rebuild:
	clean 
	all