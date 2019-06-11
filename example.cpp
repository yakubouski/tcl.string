#include <cstdio>
#include "include/stringutils.h"
#include "include/stringoption.h"
#include "include/stringview.h"


decltype(auto) fast_hash(const char* from,const char* to) {
	static constexpr uint64_t seed = 0xc6a4a7935bd1e995;
	uint64_t hash = seed;
	const uint64_t* p_it = (const uint64_t*)from, * a_to = (const uint64_t*)to;
	for (; (p_it + 1) <= a_to; p_it++) {
		hash ^= *p_it;
	}
	if ((a_to - p_it)) {
		auto rsh = ((uint64_t)a_to - (uint64_t)p_it);
		auto mask = (~(0xFFFFFFFFFFFFFFFF << (rsh << 3)));
		hash = hash ^ (*p_it & (~(0xFFFFFFFFFFFFFFFF << (rsh << 3))));
	}
#if __x86_64__
	return hash;
#else
	return (uint32_t)hash ^ (uint32_t)(hash >> 32);
#endif
	
}

int main()
{
	{
		const char* str[] = {
			"a",
			"ab",
			"abcd",
			"abcdef",
			"abcdefgj",
			"abcdefgjkl",
			"abcdefgjkl12345",
			"abcdefgjkl123456",
			"abcdefgjkl123456789", };

		for (size_t s = 0; s < 9; s++)
		{
			printf("hash: %20s (%2zd) = %16zx\n", str[s], strlen(str[s]), fast_hash(str[s], str[s] + strlen(str[s])));
		}
	}

	/* expand option string,sequence will be generate  */
	{
		auto res = coption::sequence("eth1,	eth5 , eth4 - eth8, bond1.10- bond1.14, ifs9-,ifs77-ifs77, eno19-eno15");
		printf("Network interfaces: ");
		for (auto& ifs : res) {
			printf("%s, ", ifs.c_str());
		}
		printf("\n");

		//						[bond1.10                               bond1.14][eno19                       eno15][eth1][eth4                    eth8][ifs77-ifs77] [ifs9-]
		// Network interfaces:	bond1.10, bond1.11, bond1.12, bond1.13, bond1.14, eno15, eno16, eno17, eno18, eno19, eth1, eth4, eth5, eth6, eth7, eth8, ifs77,		   ifs9, 
	}

	{
		auto bytes = coption::bytes(" 12 K");

		printf("Capacity: %zu bytes\n", bytes);
		// Capacity: 12288 bytes
	}

	{
		auto secs = coption::seconds(" 3h 15m 11");

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
			auto result = coption::dsn(str[s], std::move(proto), std::move(user), std::move(password), std::move(host), std::move(port), std::move(path), std::move(filename), std::move(options));
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
		cstringview packet(str, str+sizeof(str)),
			packet2(str1,str1 + sizeof(str1));

		packet.trim(ccharset::spaces("abcd\0"));
		packet2.trim();
	}

    return 0;
}