#include <cstdio>
#include "include/tstringutils.h"
#include "include/tstringoption.h"
#include "include/tstringview.h"


int main()
{
	/* expand option string,sequence will be generate  */
	{
		auto res = tcl::string::option::sequence("eth1,	eth5 , eth4 - eth8, bond1.10- bond1.14, ifs9-,ifs77-ifs77, eno19-eno15");
		printf("Network interfaces: ");
		for (auto& ifs : res) {
			printf("%s, ", ifs.c_str());
		}
		printf("\n");

		//						[bond1.10                               bond1.14][eno19                       eno15][eth1][eth4                    eth8][ifs77-ifs77] [ifs9-]
		// Network interfaces:	bond1.10, bond1.11, bond1.12, bond1.13, bond1.14, eno15, eno16, eno17, eno18, eno19, eth1, eth4, eth5, eth6, eth7, eth8, ifs77,		   ifs9, 
	}

	{
		auto bytes = tcl::string::option::bytes(" 12 K");

		printf("Capacity: %zu bytes\n", bytes);
		// Capacity: 12288 bytes
	}

	{
		auto secs = tcl::string::option::seconds(" 3h 15m 11");

		printf("Time: %zu seconds\n", secs);
		// Time: 11711 seconds
	}

	{
		const char* str[] = {
			"mysql://user:passwd@host:80/db/table?opt1=ee&opt2=ffff",
			"mysql://user@host:80/db/table?opt1=ee&opt2=ffff",
			"mysql://host:80/db/table?opt1=ee&opt2=ffff",
			"mysql://host/db/table/?opt1=ee&opt2=ffff",
			"mysql://host/?opt1=ee&opt2=ffff",
			"mysql://host/db/table/",
			"mysql://host/",
			"mysql://host/db/table/index.html",};
		
		for(size_t s = 0;s<8;s++)
		{
			std::string proto, user, password, host, port, path, filename;
			std::unordered_map<std::string, std::string> options;
			auto result = tcl::string::option::dsn(str[s], std::move(proto), std::move(user), std::move(password), std::move(host), std::move(port), std::move(path), std::move(filename), std::move(options));
			printf("Proto (%s), User (%s), Password (%s), Host (%s), Port (%s), Path (%s), FileName (%s), Options ( ",
				proto.c_str(),user.c_str(),password.c_str(),host.c_str(),port.c_str(),
				path.c_str(),filename.c_str());
			for (auto&& op : options) {
				printf("%s (%s) ", op.first.c_str(), op.second.c_str());
			}
			printf(" )\n");
		}
	}

	{
		char str[] = "a\tcdb   a \tPOST\t\r\n\r  aaaaacdf  \tacd\0";
		char str1[] = "POST1\0";
		tcl::tstringview packet(str, str+sizeof(str)),
			packet2(str1,str1 + sizeof(str1));

		packet.trim({tcl::space_chars, "abcd\0" });
		packet2.trim();
	}

    return 0;
}