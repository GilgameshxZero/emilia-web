#pragma once

#include "../RainLibrary3/RainLibraries.h"

#define WM_LISTENWNDINIT	WM_RAINAVAILABLE
#define WM_LISTENWNDEND		WM_RAINAVAILABLE + 1

namespace Monochrome3 {
	namespace EMTSMTPServer {
		//parameter passed to any listening thread
		//one for each ListenThread
		struct ListenThreadParam {
			//program-wide configuration access
			std::map<std::string, std::string> *config;

			//persistent socket on which to listen for clients
			SOCKET *lSocket;

			//mutex to lock the link list when modifications are being made to it
			std::mutex *ltLLMutex;

			//mutex to lock the smtp client while it is being used by another thread
			std::mutex *smtpClientMutex;

			//window representing this thread's message queue, implemented by RainWindow
			Rain::RainWindow rainWnd;

			//current listen thread handle
			HANDLE hThread;

			//pointer to ListenThreadParams before and after this, in a linked list
			ListenThreadParam *prevLTP, *nextLTP;

			//socket that represents connection to one client
			SOCKET cSocket;

			//handle to the RecvThread associated with this ListenThread/HWND
			HANDLE hRecvThread;

			//standardized parameter to pass to RecvThread
			Rain::WSA2RecvParam recvParam;
		};
	}
}