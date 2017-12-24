#ifndef INCLUDE_SMTP_CLIENT_HPP_
#define INCLUDE_SMTP_CLIENT_HPP_

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
using boost::asio::ip::tcp;
using namespace boost::archive::iterators;
typedef base64_from_binary<transform_width<const char *, 6, 8> > base64_text;

class SMTPClient {
public:
	SMTPClient(const std::string &pServer, unsigned int pPort,
			const std::string &pUser, const std::string &pPassword) :
			mServer(pServer), mPort(pPort), mUserName(pUser), mPassword(
					pPassword), mSocket(mIOService), mResolver(mIOService) {
		tcp::resolver::query qry(mServer,
				boost::lexical_cast<std::string>(mPort));
		mResolver.async_resolve(qry,
				boost::bind(&SMTPClient::handleResolve, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::iterator));
	}
	bool sendEmail(const std::string &pFrom, const std::vector<std::string> &pTo,
			const std::string &pSubject, const std::string &pMessage) {
		mFrom = pFrom;
		mTo = pTo;
		mSubject = pSubject;
		mMessage = pMessage;
		mIOService.run();
		return mHasError;
	}
private:
	std::string encodeBase64(const std::string &pData) {
		std::stringstream os;
		size_t sz = pData.size();
		std::copy(base64_text(pData.c_str()), base64_text(pData.c_str() + sz),
				ostream_iterator<char>(os));
		return os.str();
	}
	void handleResolve(const boost::system::error_code& err,
			tcp::resolver::iterator endpoint_iterator) {
		if (!err) {
			tcp::endpoint endpoint = *endpoint_iterator;
			mSocket.async_connect(endpoint,
					boost::bind(&SMTPClient::handleConnect, this,
							boost::asio::placeholders::error,
							++endpoint_iterator));
		} else {
			mHasError = true;
			std::cout << "resolve error\n";
		}
	}
	void writeLine(std::string pData) {
		std::ostream req_strm(&mRequest);
		req_strm << pData << "\r\n";
		std::cout << pData << std::endl;
		boost::asio::write(mSocket, mRequest);
		req_strm.clear();
	}
	void handleConnect(const boost::system::error_code& err,
			tcp::resolver::iterator endpoint_iterator) {
		if (!err) {
			boost::asio::async_read_until(mSocket, mResponse, '\r',
					[this](const boost::system::error_code& error, std::size_t bytes_transferred) {
						std::cerr << ">>> " << &mResponse;
					});
			// The connection was successful. Send the request.
			std::ostream req_strm(&mRequest);
			writeLine("EHLO " + mServer);
			writeLine("AUTH LOGIN");
			writeLine(encodeBase64(mUserName));
			writeLine(encodeBase64(mPassword));
			writeLine("MAIL FROM: <" + mFrom + ">");
			for (auto &&address : mTo) {
				writeLine("RCPT TO: <" + address + ">");
			}
			writeLine("DATA");
			writeLine("SUBJECT: " + mSubject);
			writeLine("From: <" + mFrom + ">");
			// no BCC or TO <== anonymity of users
			writeLine("");
			writeLine(mMessage);
			writeLine(".");
			writeLine("");
			std::this_thread::sleep_for(std::chrono::seconds(2));
		} else {
			mHasError = true;
			std::cout << "some error" << std::endl;
		}
	}
	std::string mServer;
	std::string mUserName;
	std::string mPassword;
	std::string mFrom;
	std::vector<std::string> mTo;
	std::string mSubject;
	std::string mMessage;
	unsigned int mPort;
	boost::asio::io_service mIOService;
	tcp::resolver mResolver;
	tcp::socket mSocket;
	boost::asio::streambuf mRequest;
	boost::asio::streambuf mResponse;
	bool mHasError;
	std::string mErrorMsg;

};

#endif /* INCLUDE_SMTP_CLIENT_HPP_ */
