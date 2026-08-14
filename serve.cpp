// clang++ serve.cpp -o serve.out && ./serve.out a 1234

#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace std;

map<string, string> types = {
    {"css", "text/css"},
    {"html", "text/html"},
    {"ico", "image/x-icon"},
    {"js", "application/javascript"}
};

void serve (string folder, int port) {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int i = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, & i, sizeof(i));
    sockaddr_in sock{};
    sock.sin_port = htons(port);
    bind(server, (struct sockaddr *) & sock, sizeof(sock));
    listen(server, 5);
    cout << "localhost:" << port << "\n";
    while (true) {
        socklen_t length = sizeof(sock);
        int client = accept(server, (struct sockaddr *) & sock, & length);
        char buffer[1024] = {0};
        read(client, buffer, 1024);
        stringstream t(buffer);
        string file;
        t >> buffer >> file;
        while ((i = file.find("%20")) != -1) {
            file.replace(i, 3, " ");
        }
        string type = "";
        ifstream f(folder + file);
        if (file.front() != '/' || f.peek() == ifstream::traits_type::eof()) {
            f.close();
            f.open(folder + "/x.html");
            type = "text/html";
        } else {
            type = types[file.substr(file.find_last_of('.') + 1)];
        }
        if (f.is_open()) {
            stringstream t;
            t << f.rdbuf();
            string content = t.str();
            f.close();
            string response = "HTTP/1.\ncontent-type:" + type + "\n\n" + content;
            send(client, response.c_str(), response.size(), 0);
        }
        close(client);
    }
}

int main (int argc, char * argv[]) {
    serve(argc > 1 ? argv[1] : "a", argc > 2 ? stoi(argv[2]) : 1234);
    return 0;
}