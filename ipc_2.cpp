#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <chrono>
#include <thread>

#define PORT 8080

void run_program1(const std::string& input_file);
void compare_and_write(const std::string& file1_path, const std::string& file2_path, const std::string& output_path);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <filename>\n";
        return 1;
    }

    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer1[1024] = {0};
    char buffer2[1024] = {0};
    
    run_program1(argv[1]);

    // Добавляем небольшую задержку перед попыткой подключения к серверу
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        std::cerr << "Invalid address/ Address not supported\n";
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed 1\n";
        return -1;
    }

    std::ofstream outfile1("temp1.txt");
    if (!outfile1) {
        std::cerr << "Failed to create output file\n";
        return 1;
    }

    while (true) {
        valread = read(sock, buffer1, 1024);
        if (valread <= 0) break;
        outfile1.write(buffer1, valread);
    }

    close(sock);

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    run_program1(argv[2]);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Еще одна задержка перед подключением

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        std::cerr << "Invalid address/ Address not supported\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed 2\n";
        return -1;
    }

    std::ofstream outfile2("temp2.txt");
    if (!outfile2) {
        std::cerr << "Failed to create output file\n";
        return 1;
    }

    while (true) {
        valread = read(sock, buffer2, 1024);
        if (valread <= 0) break;
        outfile2.write(buffer2, valread);
    }

    close(sock);
    outfile1.close();
    outfile2.close();
    compare_and_write("temp2.txt", "temp1.txt", "output.txt");
    return 0;
}


void run_program1(const std::string& input_file) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("./ipc_1.out", "./ipc_1.out", input_file.c_str(), nullptr);
        exit(1);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

void compare_and_write(const std::string& file1_path, const std::string& file2_path, const std::string& output_path) {
    std::ifstream file1(file1_path, std::ios::binary);
    std::ifstream file2(file2_path, std::ios::binary);
    std::ofstream output(output_path, std::ios::binary);

    if (!file1 || !file2 || !output) {
        std::cerr << "Failed to open files\n";
        return;
    }

    char ch1, ch2;
    while (file1.get(ch1) && file2.get(ch2)) {
        char result = ch1 ^ ch2;
        output.put(result);
    }

    file1.close();
    file2.close();
    output.close();
}