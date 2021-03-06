// photo_socket.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <stdio.h>  
#include<iostream>
#include <winsock2.h>  
#include <stdlib.h>
#include <windows.h>
#include <shellapi.h>
#include<fstream>
#include<string>
#include "json.hpp"
#include"base64.h"
#define DATA_SIZE 10000 //这个数根据需要写
using json = nlohmann::json;
//using namespace std;
#pragma comment(lib,"ws2_32.lib")	//把ws2_32.lib加到Link页的连接库  
#define PORT 15001					//通信的端口（指服务器端）
#define ERROR 0  
#define BUFFER_SIZE 10			
/*

*/
int mousemove(int x, int y) {
	::SetCursorPos(x, y);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	return 0;
}
std::string getIDCard()
{
	std::ifstream ifle;
	std::string temp = "0000000000000000000000000000000000000";
	std::string IDCard = "000000000000000000";
	//std::string tiem = "2018-9-26";
	std::string tiem = "2018-09-16";
	std::string IDCardroute = "C:\\Program Files\\AuditRealNameMain\\Logs\\" + tiem +  "\\XinZhongXin_DKQ-116D.log";
	ifle.open(IDCardroute);
	std::string pfile, ppre;
	ifle >> ppre;
	while (!ifle.eof()) {
		/*pfile = ppre;*/
		pfile = ppre;
		ifle >> ppre;
	}
	ifle.close();
	std::cout << pfile << std::endl;
	//获得数字
	int j = 0;
	for (int i = 0; i < pfile.length(); i++) {
		if (pfile[i] >= '0'&&pfile[i] <= '9') {
			temp[j] = pfile[i];
			j++;
		}
	}
	//获得身份证号
	for (int x = 0; x < IDCard.length(); x++) {
		IDCard[x] = temp[x + 1];
	}
	std::cout << IDCard << std::endl;
	return IDCard;
}
int serversocket()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) == SOCKET_ERROR)  //启动winsock ，WSAStartup()函数对Winsock DLL进行初始化
	{

		exit(1);
	}
	SOCKET sock;										//服务进程创建套接字句柄（用于监听）
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)		//调用socket()函数创建一个流套接字，参数（网络地址类型，套接字类型，网络协议）
	{

		WSACleanup();
		exit(1);
	}
	struct sockaddr_in ServerAddr;			//sockaddr_in结构用来标识TCP/IP协议下的地址，可强制转换为sockaddr结构
	ServerAddr.sin_family = AF_INET;			//sin_family字段必须设为AF_INET，表示该Socket处于Internet域
	ServerAddr.sin_port = htons(PORT);		//sin_port字段用于指定服务端口，注意避免冲突
	ServerAddr.sin_addr.s_addr = INADDR_ANY;  //sin_addr字段用于把一个IP地址保存为一个4字节的数，无符号长整型，根据不同用法还可表示本地或远程IP地址
	if (bind(sock, (LPSOCKADDR)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)  //调用bind()函数将本地地址绑定到所创建的套接字上，以在网络上标识该套接字
	{

		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	if (listen(sock, 10) == SOCKET_ERROR)		//调用listen()函数将套接字置入监听模式并准备接受连接请求，参数（已捆绑未连接的套接字描述字，正在等待连接的最大队列长度）
	{

		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	SOCKET msgsock;			//创建一个新的套接字（用于接收accept函数的返回值，即表示已经接受的那个客户端的连接，进而接收Client发来的数据）
	char buf[BUFFER_SIZE];
	while (1)
	{
		if ((msgsock = accept(sock, (LPSOCKADDR)0, (int *)0)) == INVALID_SOCKET)  //进入监听状态后，调用accept()函数接收客户端的连接请求，并把连接传给msgsock套接字，原sock套接字继续监听其他客户机连接请求
		{

			continue;
		}
		memset(buf, 0, sizeof(buf));
		recv(msgsock, buf, BUFFER_SIZE, 0);
		printf("客户端消息>%s\n", buf);  //接收客户端发送过来的数据  
		if (buf[0] == '1')
		{
			FILE *fIn1;
			fIn1 = fopen("tmp.jpg", "rb");

			if (fIn1 == NULL)
			{
				printf("打开读取文件失败");
			}


			int nRead1;
			char chBuf1[DATA_SIZE];
			nRead1 = fread(chBuf1, sizeof(char), DATA_SIZE, fIn1);
			std::string imgBase64_1 = base64_encode(chBuf1, nRead1);

				json j;
				j["command"] = "00";
				j["idcard"] = "411425199411123011";
				j["imag"] = imgBase64_1;
				std::string s = j.dump();
				s = s + "\n";
				const char *sendData = s.c_str();//还有头像传输
				std::cout << s << std::endl;
				//std::cout << *sendData<< std::endl;
				send(msgsock, sendData, s.length(), 0);
		
		}

		closesocket(sock); //关闭套接字  
		WSACleanup();	   //终止对Winsock DLL的使用，并释放资源
		return 0;
	}
}
int main() {
	serversocket();
	std::cout << "已经完成了图片传输" << std::endl;
	std::cin.get();
	return 0;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
