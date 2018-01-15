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
#include "LinuxResProvider.hpp"
#include "configrawdata.hpp"
#include "PredicateTypes.hpp"


using namespace std;
// Added for the json-example:
using namespace boost::property_tree;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

void writeHtmlToString(std::shared_ptr<ConfigurationParser> confParser, std::string & resp){
	std::stringstream html, selections;
	int i = 0;
	for(auto && triggType : confParser->getTriggerTypes())
	{
		selections << "<option value=\"" << std::to_string(i++) << "\">" << triggType << "</option>\n";
	}

	html << "<html>\n<head>\n<title>Simple-Web-Server html-file</title>\n</head>\n<body>"
	"Choose your trigger type and subscribe for e-mail!\n"
	"</br>\n</br>\n<form action = \"/set_email\" method = \"get\">\n"
	   "Email: <input type = \"text\" name = \"email\">\n"
	   "User name: <input type = \"text\" name = \"name\" />\n"
	   "<input type = \"submit\" value = \"Apply\" />\n"
	"</form>\n<form method=\"GET\" action=\"/trigger_choose\">\n"
	"<select name=\"trigger0\" required>";
	html << selections.str();
	html << " </select>OR<select name=\"trigger1\" required>\n";
	html << selections.str();
	html << " </select>AND<select name=\"trigger2\" required>\n";
	html << selections.str();;
	html << "</select>\n<input type=\"submit\" value=\"Submit\">\n"
		"</form>\n<form method=\"GET\" action=\"/trigger_choose\">\n"
	  "<select name=\"trigger0\" required>";
	html << selections.str();;
	html << " </select>AND<select name=\"trigger1\" required>\n";
	html << selections.str();;
	html << "</select>\n<input type=\"submit\" value=\"Submit\">\n"
				"</form>\n<form method=\"GET\" action=\"/trigger_choose\">\n"
			  "<select name=\"trigger0\" required>";
	html << selections.str();;
	html << "</select>\n<input type=\"submit\" value=\"Submit\">\n"
						"</form>\n</body>\n</html>";
	resp = html.str();
}

void exampleHttpsServerExecution(std::shared_ptr<ConfigurationParser> confParser,
		std::shared_ptr<PredicateEngine> predEngine) {
	// HTTP-server at port 8080 using 1 thread
	  // Unless you do more heavy non-threaded processing in the resources,
	  // 1 thread is usually faster than several threads
	  HttpServer server;
	  server.config.port = 8080;

	  std::string userName;
	  std::string userEmail;

	  // Add resources using path-regex and method-string, and an anonymous function
	  // POST-example for the path /string, responds the posted string
	  server.resource["^/string$"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
	    // Retrieve string:
	    auto content = request->content.string();
	    // request->content.string() is a convenience function for:
	    // stringstream ss;
	    // ss << request->content.rdbuf();
	    // auto content=ss.str();

	    *response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n"
	              << content;


	    // Alternatively, use one of the convenience functions, for instance:
	    // response->write(content);
	  };

	  server.resource["^/gettriggers$"]["GET"] = [&confParser](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
		  std::string resp;
		  writeHtmlToString(confParser, resp);
		  *response << "HTTP/1.1 200 OK\r\nContent-Length: " << resp.length() << "\r\n\r\n"
				  << resp;
	  };

	  server.resource["^/trigger_choose$"]["GET"] = [&confParser, &predEngine](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
		std::string resp("Your subscription has been saved!");
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

		std::function<void()> pf = [&](){
			SMTPClient mailc("smtp.wp.pl", 25, "piterek93p@wp.pl",
					"zprresmonitor!1");
			mailc.sendEmail("zpr_resmonitor@wp.pl", { "zpr_resmonitor@wp.pl"}, "tescikkk",
					"Hello from C++ SMTP Client!");
		};
	    std::shared_ptr<Predicate> pred;
	    if(query_fields.size() == 3){
	    	pred = std::make_shared<Predicate_1or2and3>(pf,
	    			confParser->getTriggerTypes().at(std::stoi(seglist.at(5))),
					confParser->getTriggerTypes().at(std::stoi(seglist.at(3))),
					confParser->getTriggerTypes().at(std::stoi(seglist.at(1))));
	    	predEngine->addPredicate(pred);
	    }
	    else if(query_fields.size() == 2){
	    	pred = std::make_shared<Predicate_1and2>(pf,
	    			confParser->getTriggerTypes().at(std::stoi(seglist.at(3))),
					confParser->getTriggerTypes().at(std::stoi(seglist.at(1))));
	    	predEngine->addPredicate(pred);
	    }
	    else if(query_fields.size() == 1){
	    	pred = std::make_shared<Predicate_1element>(pf,
					confParser->getTriggerTypes().at(std::stoi(seglist.at(1))));
	    	predEngine->addPredicate(pred);
	    }
	    else
	    	resp = "Wrong data!";

		std::cout<< "Request: \n"<<stream.str()<<std::endl;
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

	  // POST-example for the path /json, responds firstName+" "+lastName from the posted json
	  // Responds with an appropriate error message if the posted json is not valid, or if firstName or lastName is missing
	  // Example posted json:
	  // {
	  //   "firstName": "John",
	  //   "lastName": "Smith",
	  //   "age": 25
	  // }
	  server.resource["^/json$"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
	    try {
	      ptree pt;
	      read_json(request->content, pt);

	      auto name = pt.get<string>("firstName") + " " + pt.get<string>("lastName");

	      *response << "HTTP/1.1 200 OK\r\n"
	                << "Content-Length: " << name.length() << "\r\n\r\n"
	                << name;
	    }
	    catch(const exception &e) {
	      *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n"
	                << e.what();
	    }


	    // Alternatively, using a convenience function:
	    // try {
	    //     ptree pt;
	    //     read_json(request->content, pt);

	    //     auto name=pt.get<string>("firstName")+" "+pt.get<string>("lastName");
	    //     response->write(name);
	    // }
	    // catch(const exception &e) {
	    //     response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
	    // }
	  };

	  // GET-example for the path /info
	  // Responds with request-information
	  server.resource["^/info$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
	    stringstream stream;
	    stream << "<h1>Request from " << request->remote_endpoint_address() << ":" << request->remote_endpoint_port() << "</h1>";

	    stream << request->method << " " << request->path << " HTTP/" << request->http_version;

	    stream << "<h2>Query Fields</h2>";
	    auto query_fields = request->parse_query_string();
	    for(auto &field : query_fields)
	      stream << field.first << ": " << field.second << "<br>";

	    stream << "<h2>Header Fields</h2>";
	    for(auto &field : request->header)
	      stream << field.first << ": " << field.second << "<br>";

	    response->write(stream);
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

	  // GET-example simulating heavy work in a separate thread
	  server.resource["^/trigger$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> /*request*/) {
		  response->write("Work done");
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

	  // Wait for server to start so that the client can connect
	  this_thread::sleep_for(chrono::seconds(1));

	  // Client examples
	  HttpClient client("localhost:8080");

	  string json_string = "{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";

	  // Synchronous request examples
	  try {
	    auto r1 = client.request("GET", "/match/123");
	    cout << r1->content.rdbuf() << endl; // Alternatively, use the convenience function r1->content.string()

	    auto r2 = client.request("POST", "/string", json_string);
	    cout << r2->content.rdbuf() << endl;
	  }
	  catch(const SimpleWeb::system_error &e) {
	    cerr << "Client request error: " << e.what() << endl;
	  }

	  // Asynchronous request example
	  client.request("POST", "/json", json_string, [](shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code &ec) {
	    if(!ec)
	      cout << response->content.rdbuf() << endl;
	  });
	  client.io_service->run();

	server_thread.join();
}

int main() {
//	SMTPClient mailc("smtp.wp.pl", 25, "zpr_resmonitor@wp.pl",
//			"zprresmonitor!1");
//	mailc.sendEmail("zpr_resmonitor@wp.pl", { "zpr_resmonitor@wp.pl"}, "tescikkk",
//			"Hello from C++ SMTP Client!");

	std::ifstream configFile( "tests_src/conf_parser/example" );
	if ( configFile )
	{
		std::stringstream stream;
		stream << configFile.rdbuf();
		configFile.close();

		std::shared_ptr<ConfigurationParser> parser = std::make_shared<ConfigurationParser>(stream);
		parser->run();


		std::shared_ptr<ResUsageProvider> provider = std::make_shared<LinuxResProvider>();

		std::shared_ptr<PredicateEngine> engine = std::make_shared<PredicateEngine>();

		// samplerManagerMock is set to take only 10 samples
		// (as 10 samples in 10 second, but without any delay between them)
		std::unique_ptr<SamplingManager> samplingManager =
				std::make_unique<SamplingManager>(
						provider, parser->getLogTypes(), parser->getTriggerTypes(),
						[](const TriggerType & t){std::cout<<"Callback triggered\n"<<std::endl;}//<<t.resource<<", "<<t.fluctuationType<<", "<<t.triggerValue
				);
		samplingManager->startSampling();
		samplingManager->connectPredicateEngine(*engine);

		exampleHttpsServerExecution(parser, engine);
	}
}
