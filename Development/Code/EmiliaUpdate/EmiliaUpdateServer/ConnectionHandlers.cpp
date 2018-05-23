#include "ConnectionHandlers.h"

namespace Monochrome3 {
	namespace EmiliaUpdateServer {
		static const std::string headerDelim = "\r\n\r\n";

		void onConnectionInit(void *funcParam) {
			Rain::WSA2ListenThreadRecvFuncDelegateParam &ltrfdParam = *reinterpret_cast<Rain::WSA2ListenThreadRecvFuncDelegateParam *>(funcParam);
			ConnectionCallerParam &ccParam = *reinterpret_cast<ConnectionCallerParam *>(ltrfdParam.callerParam);

			if (ccParam.clientConnected)
				return; //TODO: if another connection is active, close this one
			ccParam.clientConnected = true;

			//create the delegate parameter for the first time
			ConnectionDelegateParam *cdParam = new ConnectionDelegateParam();
			ltrfdParam.delegateParam = reinterpret_cast<void *>(cdParam);

			//initialize cdParam here
			cdParam->request = "";
			cdParam->requestLength = 0;

			cdParam->authenticated = false;
		}
		void onConnectionExit(void *funcParam) {
			reinterpret_cast<ConnectionCallerParam *>(reinterpret_cast<Rain::WSA2ListenThreadRecvFuncDelegateParam *>(funcParam)->callerParam)->clientConnected = false;

			//free the delegate parameter
			delete reinterpret_cast<Rain::WSA2ListenThreadRecvFuncDelegateParam *>(funcParam)->delegateParam;
		}
		int onConnectionProcessMessage(void *funcParam) {
			Rain::WSA2ListenThreadRecvFuncDelegateParam &ltrfdParam = *reinterpret_cast<Rain::WSA2ListenThreadRecvFuncDelegateParam *>(funcParam);
			ConnectionDelegateParam &cdParam = *reinterpret_cast<ConnectionDelegateParam *>(ltrfdParam.delegateParam);

			//delegate to request handlers once the message is complete
			//message/request length is at the beginning, as a base-10 string, before a space
			cdParam.request += ltrfdParam.message;

			int ret = 0;
			while (true) {
				if (cdParam.requestLength == 0) {
					std::size_t firstSpace = cdParam.request.find(' ');
					if (firstSpace != std::string::npos) {
						cdParam.requestLength = Rain::strToT<std::size_t>(cdParam.request.substr(0, firstSpace));
						cdParam.request = cdParam.request.substr(firstSpace + 1, cdParam.request.length());
					}
				}

				//if message is complete
				if (cdParam.requestLength != 0 && cdParam.request.length() >= cdParam.requestLength) {
					std::string fragment = cdParam.request.substr(cdParam.requestLength, cdParam.request.length());
					cdParam.request = cdParam.request.substr(0, cdParam.requestLength);

					int hrReturn = HandleRequest(ltrfdParam);
					if (hrReturn < 0 || (hrReturn > 0 && ret >= 0))
						ret = hrReturn;

					cdParam.request = fragment;
					cdParam.requestLength = 0;
				} else
					break;
			}

			//< 0 is error, 0 is keep-alive, and > 0 is peacefully close
			return 0;
		}
	}
}