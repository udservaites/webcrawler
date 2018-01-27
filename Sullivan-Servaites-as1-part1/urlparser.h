/*
Kelsea Sullivan
Kathryn Rebecca Servaites

Assignment 1 Part 1
CPS 570 Spring 2018
01-23-2018

urlparser.h
This class uses a regular expression to parse the host, port, path, and query from a URL string.
*/

#pragma once
#include "common.h"
#include <regex>

class URLParser {

public:

	// constructor 
	URLParser(string link)  // pass link to this function
	{
		url = link;
		host = "";
		port = 80;  // default port is 80 for web server
		path = "/";  // if path is empty, use "/"
		query = "";

		parseURL();
	}


	/* url format:
	* scheme://[user:pass@]host[:port][/path][?query][#fragment]
	*/

	// e.g., url: "http://cs.somepage.edu:467/index.php?addrbook.php"
	// host: "cs.somepage.edu"
	string getHost()
	{
		return host;
	}

	// e.g., url: "http://cs.somepage.edu:467/index.php?addrbook.php"
	// port: 467
	short getPort()
	{
		return port;
	}

	// url: "http://cs.somepage.edu:467/index.php?addrbook.php"
	// path is "/index.php"
	string getPath()
	{
		return path;
	}

	// url: "http://cs.somepage.edu:467/index.php?addrbook.php"
	// query is "?addrbook.php"
	string getQuery()
	{
		return query;
	}


private:
	string url;
	string host;
	short port;
	string path;
	string query;

	void parseURL()
	{
		// find url, host, port, path, and query here
		cout << "\tParsing URL... ";
		regex ex("(http|https)://([^/ :]+):?([^/ ]*)/?([^ #?]*)\??([^ #]*)#?([^ ]*)");
		cmatch what;
		string tmp_port;
		if (regex_match(url.c_str(), what, ex))
		{
			host = string(what[2].first, what[2].second);
			tmp_port = string(what[3].first, what[3].second);
			if (tmp_port != "")
				port = stoi(tmp_port);
			path = string(what[4].first, what[4].second);
			query = string(what[5].first, what[5].second);

			printf("host %s, port %d\n", host.c_str(), port);
		}


	}

};