#include "Start.h"

namespace Mono3 {
	namespace SMTPServer {
		int start() {
			//read config
			std::map<std::string, std::string> config;
			Rain::readParameterFile("config\\config.ini", config);

			//debugging
			Rain::redirectCerrFile(config["errorLog"]);
			Rain::logMemoryLeaks(config["memoryLeakLog"]);

			//setup listening on the port
			WSADATA wsaData;
			struct addrinfo *sAddr;
			SOCKET lSocket;
			if (Rain::quickServerInit(wsaData, config["smtpPort"], &sAddr, lSocket)) {
				Rain::reportError(GetLastError(), "error in quickServerInit");
				return -1;
			}
			if (Rain::listenServSocket(lSocket)) {
				Rain::reportError(GetLastError(), "error in listenServSocket");
				return -1;
			}
			std::cout << "Listening...\r\n";
			Rain::fastOutputFile(config["logFile"], "Listening...\r\n", true);

			//similar to EMTServer setup
			//main thread responsible for capturing cin and commands
			//spawn listening threads to deal with socket connections
			//arrange threads in a linked list, with dummy beginning and end nodes
			//ltParam needs to be dynamically allocated, because it will be freed by listenThread
			//however, the mutex should be allocated outside ltParam, because we will need to use it again even after the threads have exited
			std::mutex *ltLLMutex = new std::mutex;

			ListenThreadParam *ltParam = new ListenThreadParam();
			ltParam->lSocket = &lSocket;
			ltParam->ltLLMutex = ltLLMutex;
			ltParam->config = &config;

			//create dummy head and tail nodes for the ListenThreadParam linked list, and put the new LTP in between
			ListenThreadParam LTPLLHead, LTPLLTail;
			LTPLLHead.prevLTP = NULL;
			LTPLLHead.nextLTP = ltParam;
			ltParam->prevLTP = &LTPLLHead;
			ltParam->nextLTP = &LTPLLTail;
			LTPLLTail.prevLTP = ltParam;
			LTPLLTail.nextLTP = NULL;

			CreateThread(NULL, 0, listenThread, reinterpret_cast<LPVOID>(ltParam), 0, NULL);

			//at this point, we should assume that ltParam may be deleted already

			//wait for commands
			while (true) {
				static std::string command, tmp;
				std::cout << "Accepting commands...\r\n";
				std::cin >> command;
				std::getline(std::cin, tmp);

				if (command == "exit") {
					//first, close the listening socket so that all calls to 'accept' terminate
					closesocket(lSocket);
					std::cout << "Socket listen closed\r\n";
					Rain::fastOutputFile(config["logFile"], "Socket listen closed\r\n", true);

					//shutdown threads by walking through the linked list and sending the end message
					//while walking through linked list, keep track of any thread handles
					std::vector<HANDLE> hThreads;

					ltLLMutex->lock();
					ListenThreadParam *curLTP = LTPLLHead.nextLTP;
					while (curLTP != &LTPLLTail) {
						hThreads.push_back(curLTP->hThread);

						//use postmessage here because we want the thread of the window to process the message, allowing destroywindow to be called
						//WM_RAINAVAILABLE + 1 is the end message
						PostMessage(curLTP->rainWnd.hwnd, WM_LISTENWNDEND, 0, 0);
						curLTP = curLTP->nextLTP;
					}
					ltLLMutex->unlock();

					std::cout << "Found " << hThreads.size() << " active ListenThreads\r\n";
					Rain::fastOutputFile(config["logFile"], "Found " + Rain::tToStr(hThreads.size()) + " active ListenThreads\r\n", true);

					//join all handles previously found in the linked list, waiting for all the threads to terminate
					//no need to close thread handles, GetCurrentThread doesn't require that
					for (HANDLE hThread : hThreads)
						WaitForSingleObject(hThread, 0);

					std::cout << "All threads finished and joined\r\n";
					Rain::fastOutputFile(config["logFile"], "All threads finished and joined\r\n", true);

					//no need to freeaddinfo here because RainWSA2 does that for us

					WSACleanup();
					break;
				} else if (command == "help") {
					std::cout << "\"exit\" to terminate server\r\n";
				} else {
					std::cout << "Command not recognized\r\n";
				}
			}

			delete ltLLMutex;

			std::cout << "EMTSMTPServer has exited. Exiting in 2 seconds...\r\n\r\n";
			Rain::fastOutputFile(config["logFile"], "EMTSMTPServer has exited. Exiting in 2 seconds...\r\n\r\n", true);
			Sleep(2000);

			return 0;
		}
	}
}