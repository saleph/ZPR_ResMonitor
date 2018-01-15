// magic define for windows' boost
#ifdef _WIN32
#define _WIN32_WINNT 0x601
#endif

#include "client_http.hpp"
#include "server_http.hpp"
#include "smtp_client.hpp"

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include "crypto.hpp"
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>
#include <memory>

#include "ConfigurationParser.hpp"
#include "SamplingManager.hpp"
#ifdef __linux__
#include "LinuxResProvider.hpp"
#elif _WIN32
#include "WindowsResProvider.hpp"
#endif
#include "configrawdata.hpp"
#include "PredicateTypes.hpp"

using namespace std;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

void writeHtmlToString(std::shared_ptr<ConfigurationParser> confParser, std::string & resp){
	std::stringstream html, triggerSelections, logSelections;
	int i = 0;
	for(auto && triggType : confParser->getTriggerTypes())
	{
		triggerSelections << "<option value=\"" << std::to_string(i++) << "\">" << triggType << "</option>\n";
	}
	i = 0;
	for(auto && logType : confParser->getLogTypes())
	{
		logSelections << "<option value=\"" << std::to_string(i++) << "\">" << logType << "</option>\n";
	}

	html << "<html>\n<head>\n<title>Simple-Web-Server html-file</title>\n</head>\n<body>"
	"Choose your trigger type and subscribe for e-mail!\n"
	"</br></br></br>\n<form action = \"/set_email\" method = \"get\">\n"
	   "Email: <input type = \"text\" name = \"email\">\n"
	   "User name: <input type = \"text\" name = \"name\" />\n"
	   "<input type = \"submit\" value = \"Apply\" />\n</br></br>"
	"</form>\n<form method=\"GET\" action=\"/trigger_choose\">\n"
	"<select name=\"trigger0\" required>";
	html << triggerSelections.str();
	html << " </select>OR<select name=\"trigger1\" required>\n";
	html << triggerSelections.str();
	html << " </select>AND<select name=\"trigger2\" required>\n";
	html << triggerSelections.str();
	html << "</select>\n<input type=\"submit\" value=\"Submit\">\n"
		"</form>\n<form method=\"GET\" action=\"/trigger_choose\">\n"
	  "<select name=\"trigger0\" required>";
	html << triggerSelections.str();;
	html << " </select>AND<select name=\"trigger1\" required>\n";
	html << triggerSelections.str();;
	html << "</select>\n<input type=\"submit\" value=\"Submit\">\n"
				"</form>\n<form method=\"GET\" action=\"/trigger_choose\">\n"
			  "<select name=\"trigger0\" required>";
	html << triggerSelections.str();;
	html << "</select>\n<input type=\"submit\" value=\"Submit\">\n"
			"</form>\n</br></br></br><form method=\"GET\" action=\"/getlogs\">\n"
				"Show logs of type\n"
			  "<select name=\"trigger\" required>\n";
	html << logSelections.str();
	html << "</select>\n"
			"<input type=\"submit\" value=\"Submit\">\n"
			"</form>\n</body>\n</html>";
	resp = html.str();
}

void exampleHttpsServerExecution(std::shared_ptr<ConfigurationParser> confParser,
		std::shared_ptr<PredicateEngine> predEngine, std::shared_ptr<SamplingManager> samplingManager) {
	  // HTTP-server at port 8080 using 1 thread
	  // Unless you do more heavy non-threaded processing in the resources,
	  // 1 thread is usually faster than several threads
	  HttpServer server;
	  server.config.port = 8080;

	  std::string userName;
	  std::string userEmail;
	  std::vector<std::shared_ptr<Predicate>> predicates;

	  server.resource["^/getlogs$"]["GET"] = [&confParser, &samplingManager](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
		  	std::string resp;
			stringstream stream;
			auto query_fields = request->parse_query_string();
			for(auto &field : query_fields)
				stream << field.first << ": " << field.second << ":";
			std::string segment;
			std::vector<std::string> seglist;
			while(std::getline(stream, segment, ':'))
			{
				seglist.push_back(segment);
			}
			stringstream newStream;
			unsigned logNumber = std::stoi(seglist.at(1));
			const LogType selectedLogType = confParser->getLogTypes().at(logNumber);
			if(selectedLogType.resource == LogType::Resource::CPU){
				for(auto && cpuLog : *(samplingManager->getCpuLog(selectedLogType))){
					newStream << cpuLog << "\n";
				}
			}
			else if(selectedLogType.resource == LogType::Resource::MEMORY){
				for(auto && ramLog : *(samplingManager->getRamLog(selectedLogType))){
					newStream << ramLog << "\n";
				}
			}
			else if(selectedLogType.resource == LogType::Resource::DISK){
				for(auto && hddLog : *(samplingManager->getHddLog(selectedLogType))){
					newStream << hddLog << "\n";
				}
			}

		  resp = newStream.str();
		  *response << "HTTP/1.1 200 OK\r\nContent-Length: " << resp.length() << "\r\n\r\n"
				  << resp;
	  };

	  server.resource["^/trigger_choose$"]["GET"] = [&](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
		std::string resp("Your subscription has been saved!");
	    stringstream stream;
	    auto query_fields = request->parse_query_string();
	    for(auto &field : query_fields)
	      stream << field.first << ": " << field.second << ":";
		std::string auxString;
		std::vector<std::string> seglist;
		while(std::getline(stream, auxString, ':'))
		{
		   seglist.push_back(auxString);
		}

	    std::shared_ptr<Predicate> pred = nullptr;
	    if(query_fields.size() == 3){
	    	pred = std::make_shared<Predicate_1or2and3>([](){},
	    			confParser->getTriggerTypes().at(std::stoi(seglist.at(5))),
					confParser->getTriggerTypes().at(std::stoi(seglist.at(3))),
					confParser->getTriggerTypes().at(std::stoi(seglist.at(1))));
	    	predicates.push_back(pred);
	    }
	    else if(query_fields.size() == 2){
	    	pred = std::make_shared<Predicate_1and2>([](){},
	    			confParser->getTriggerTypes().at(std::stoi(seglist.at(3))),
					confParser->getTriggerTypes().at(std::stoi(seglist.at(1))));
	    	predicates.push_back(pred);
	    }
	    else if(query_fields.size() == 1){
	    	pred = std::make_shared<Predicate_1element>([](){},
					confParser->getTriggerTypes().at(std::stoi(seglist.at(1))));
	    	predicates.push_back(pred);
	    }
	    else
	    	resp = "Wrong data!";

	    if(pred != nullptr){
	    	stringstream ss;
	    	ss << *pred;
	    	auxString = ss.str();
	    }
		std::function<void()> sendMailFunction = [userName, userEmail, auxString](){
			SMTPClient mailc("smtp.wp.pl", 25, "zpr_resmonitor@wp.pl",
					"zprresmonitor!1");
			mailc.sendEmail("zpr_resmonitor@wp.pl", {userEmail}, "ZPR Monitor trigger notification",
					"Hello" + userName + ",\nfollowing trigger has been exceeded:\n" + auxString + "\n\nYour ZPRMonitor");
		};
		if(pred != nullptr){
			pred->setCallback(sendMailFunction);
	    	predEngine->addPredicate(pred);
		}

		*response << "HTTP/1.1 200 OK\r\nContent-Length: " << resp.length() << "\r\n\r\n"
				  << resp;
	  };


	  server.resource["^/set_email"]["GET"] = [&](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
		std::string resp;
		stringstream stream;
		auto query_fields = request->parse_query_string();
		if(query_fields.size() != 2)
		{
			resp = "Wrong name or email!";
			*response << "HTTP/1.1 200 OK\r\nContent-Length: " << resp.length() << "\r\n\r\n"
							  << resp;
			return;
		}
		for(auto &field : query_fields)
		  stream << field.first << ": " << field.second << ":";
		std::string segment;
		std::vector<std::string> seglist;
		while(std::getline(stream, segment, ':'))
		{
		   seglist.push_back(segment);
		}
		userName = seglist.at(1);
		userEmail = seglist.at(3);
		resp = userName + " - " + userEmail;

		std::cout<< "Name ("<<userName<<") and email: ("<<userEmail<<") has been set"<<std::endl;
		writeHtmlToString(confParser, resp);
		*response << "HTTP/1.1 200 OK\r\nContent-Length: " << resp.length() << "\r\n\r\n"
				  << resp;
	  };

	  // GET-example for the path /match/[number], responds with the matched string in path (number)
	  // For instance a request GET /match/123 will receive: 123
	  server.resource["^/match/([0-9]+)$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
	    response->write(request->path_match[1]);
	  };

	  // GET-example simulating heavy work in a separate thread
	  server.resource["^/work$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> /*request*/) {
	    thread work_thread([response] {
	      this_thread::sleep_for(chrono::seconds(5));
	      response->write("Work done");
	    });
	    work_thread.detach();
	  };

	  // Default GET-example. If no other matches, this anonymous function will be called.
	  // Will respond with content in the web/-directory, and its subdirectories.
	  // Default file: index.html
	  // Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
	  server.default_resource["GET"] = [&confParser](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
		  std::string resp;
		  writeHtmlToString(confParser, resp);
		  *response << "HTTP/1.1 200 OK\r\nContent-Length: " << resp.length() << "\r\n\r\n"
				  << resp;
	  };

	  server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
	    // Handle errors here
	  };

	  thread server_thread([&server]() {
	    // Start server
	    server.start();
	  });

	server_thread.join();
}

int main() {
	std::ifstream configFile( "tests_src/conf_parser/example" );

	if (configFile)
	{
		std::stringstream stream;
		stream << configFile.rdbuf();
		configFile.close();

		std::shared_ptr<ConfigurationParser> parser = std::make_shared<ConfigurationParser>(stream);
		parser->run();

		#ifdef __linux__
		std::shared_ptr<ResUsageProvider> provider = std::make_shared<LinuxResProvider>();
		#elif _WIN32
		std::shared_ptr<ResUsageProvider> provider = std::make_shared<WindowsResProvider>();
		#endif

		std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();

		std::shared_ptr<SamplingManager> samplingManager =
				std::make_unique<SamplingManager>(
						provider, parser->getLogTypes(), parser->getTriggerTypes(),
						[](const TriggerType & t){std::cout<<"Callback triggered\n"<<std::endl;}
				);
		samplingManager->startSampling();
		samplingManager->connectPredicateEngine(*engine);

		exampleHttpsServerExecution(parser, engine, samplingManager);
	}
}
