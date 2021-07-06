#include "emilia-web.hpp"

// TODO: Refactor globals.
const std::string EMILIA_WEB_VERSION_STR =
										std::to_string(EMILIA_WEB_VERSION_MAJOR) + "." +
	std::to_string(EMILIA_WEB_VERSION_MINOR) + "." +
	std::to_string(EMILIA_WEB_VERSION_REVISION) + "." +
	std::to_string(EMILIA_WEB_VERSION_BUILD),
									RAIN_VERSION_STR = std::to_string(RAIN_VERSION_MAJOR) + "." +
	std::to_string(RAIN_VERSION_MINOR) + "." +
	std::to_string(RAIN_VERSION_REVISION) + "." +
	std::to_string(RAIN_VERSION_BUILD);
std::filesystem::path staticPath;
std::mutex newsSubsMtx;
std::set<std::string> newsSubs;
const std::filesystem::path NEWS_SUBS_PATH("../data/news-subs.txt");

bool Server::onRequest(Slave &slave, Request &req) noexcept {
	try {
		// MIME type deduction from file extension.
		const std::map<std::string, std::string> FILE_EXT_TO_MIME(
			{{".html", "text/html"},
				{".css", "text/css"},
				{".js", "text/javascript"},
				{".ico", "image/vnd.microsoft.icon"},
				{".jpg", "image/jpeg"},
				{".gif", "image/gif"},
				{".png", "image/png"},
				{".zip", "application/zip"},
				{".txt", "text/plain"},
				{".hdf5", "application/x-hdf5"},
				{".md", "text/markdown"},
				{".pdf", "application/pdf"},
				{".mp3", "audio/mpeg"}});

		static std::mutex coutMtx;
		{
			std::lock_guard<std::mutex> coutLck(coutMtx);
			std::cout << "[" << slave.getNativeSocket() << "] " << req.method << " "
								<< req.path << "\n";
		}

		// Cache up to 128 files of at most 4MB each.
		static Rain::Algorithm::LRUCache<std::string, std::string> fileCache(128);
		static const std::size_t MAX_FILE_CACHE_SIZE = 1 << 22;

		// 404 response.
		Response notFound(404, "Emilia couldn't find your page T_T");
		notFound.body.appendBytes("I'm sorry, I couldn't find what you wanted T_T");
		notFound.header["Content-Type"] = "text/html";
		notFound.header["Content-Length"] = std::to_string(notFound.body.getLength());
		if (req.method != "GET") {
			slave.send(notFound);
			return false;
		}

		Server::Response res;
		res.header["Server"] = "emilia-web";

		// Use custom endpoint handlers before defaulting to fileserver.
		if (req.path == "/version") {
			res.body.appendBytes(EMILIA_WEB_VERSION_STR);
			res.header["Content-Type"] = "text/html";
			res.header["Content-Length"] = std::to_string(res.body.getLength());
			slave.send(res);
		} else if (req.path.substr(0, 18) == "/newsletter/check/") {
			{
				std::lock_guard<std::mutex> newsSubsLck(newsSubsMtx);
				res.body.appendBytes(newsSubs.count(req.path.substr(18)) > 0
						? "subscribed"
						: "not subscribed");
			}
			res.header["Content-Type"] = "text/html";
			res.header["Content-Length"] = std::to_string(res.body.getLength());
			slave.send(res);
		} else if (req.path.substr(0, 19) == "/newsletter/toggle/") {
			// Sanitize the email of newlines, toggle it, and then flush it to disk.
			std::string email = req.path.substr(19);
			email.erase(std::remove(email.begin(), email.end(), '\n'), email.end());
			if (email.size() != 0) {
				if (newsSubs.count(email) > 0) {
					newsSubs.erase(email);
				} else {
					newsSubs.insert(email);
				}
				std::ofstream newsSubsOut(NEWS_SUBS_PATH, std::ios::binary);
				for (auto it : newsSubs) {
					newsSubsOut << it << "\n";
				}
				newsSubsOut.close();
				slave.send(res);
			}
		} else {
			res.header["Cache-Control"] = "max-age=600";	// 10 minutes.

			// Remove the "/" or the path gets replaced.
			std::filesystem::path reqPath(staticPath / (req.path.data() + 1));
			if (std::filesystem::is_directory(reqPath)) {
				// Default file for directory is index.html.
				reqPath /= "index.html";
			}
			if (!std::filesystem::exists(reqPath)) {
				// If object doesn't exist, send 404.
				slave.send(notFound);
				return false;
			}
			if (!Rain::Filesystem::subpath(
						staticPath, std::filesystem::canonical(reqPath))) {
				// If requested file is outside static directory, send 404.
				slave.send(notFound);
				return false;
			}

			// Set desired headers.
			auto it = FILE_EXT_TO_MIME.find(reqPath.extension().string());
			res.header["Content-Type"] =
				it == FILE_EXT_TO_MIME.end() ? "text/html" : it->second;

			static const std::size_t BUF_SZ = 16384;
			try {
				// If file is in cache, send that back.
				res.body.appendBytes(fileCache.at(reqPath.string()));
				res.header["Content-Length"] =
					std::to_string(res.body.getLength());
				slave.send(res);
			} catch (...) {
				std::ifstream file(reqPath, std::ios::binary | std::ios::ate);
				std::streamsize size = file.tellg();
				file.seekg(0, std::ios::beg);

				res.header["Content-Length"] = std::to_string(size);

				if (size < static_cast<std::streamsize>(MAX_FILE_CACHE_SIZE)) {
					// If the file is small enough, add it to the cache.
					auto ret =
						fileCache.insert_or_assign(reqPath.string(), std::string());
					ret.first->second->second.resize(static_cast<std::size_t>(size));
					file.read(ret.first->second->second.data(), size);
					res.body.appendBytes(ret.first->second->second);
					slave.send(res);
				} else {
					// Pipe back into the response via a generator.
					char buf[BUF_SZ];
					std::size_t remaining = static_cast<std::size_t>(size);
					const Rain::Networking::Http::Body::Generator generator =
						[&](char const **bytes) {
							// Are we done?
							if (remaining == 0) {
								return static_cast<std::size_t>(0);
							}

							std::size_t toRead = min(BUF_SZ, remaining);
							file.read(buf, toRead);
							*bytes = buf;
							remaining -= toRead;
							res.body.appendGenerator(generator);
							return toRead;
						};
					res.body.appendGenerator(generator);
					slave.send(res);
				}

				file.close();
			}
		}
		return false;
	} catch (...) {
		return true;
	}
}

int main(int argc, const char *argv[]) {
	std::cout << "emilia-web " << EMILIA_WEB_VERSION_STR << "\n"
						<< "using rain " << RAIN_VERSION_STR << "\n";

	// Parse command line.
	std::string port = "0", staticDirStr = "../../emilia-tan/static/";
	Rain::String::CommandLineParser parser;
	parser.addLayer("port", &port);
	parser.addLayer("static-dir", &staticDirStr);
	parser.parse(argc - 1, argv + 1);
	staticPath = std::filesystem::path(std::filesystem::canonical(staticDirStr));

	// Read newsletter file.
	std::ifstream newsSubsIn(NEWS_SUBS_PATH, std::ios::binary);
	std::string line;
	while (std::getline(newsSubsIn, line)) {
		newsSubs.insert(line);
	}
	newsSubsIn.close();

	Server server;
	server.serve(Rain::Networking::Host("", port), false);
	std::cout << "Serving on port " << server.getService().getCStr() << ".\n";
	std::string command;
	while (command != "exit") {
		std::cin >> command;
	}
	return 0;
}
