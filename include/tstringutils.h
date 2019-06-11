#pragma once
#include <string>
#include <vector>
#include <regex>
#include <set>
#include <list>
#include <climits>
#include <algorithm> 
#include <cstdarg>
#include "tchars.h"
#include "pch.h"

namespace tcl {
	namespace string {
		class utils {
			static forceinline bool icase_cmp(const uint8_t& ch1, const uint8_t& ch2) { return icase[ch1] == icase[ch2]; }
		public:

			template<typename CONTAINER>
			static inline std::vector<std::string> implode(const std::string& glue, const CONTAINER& pieces) {
				std::string result;
				auto it = pieces.begin();
				for (size_t i = pieces.size(); --i; it++) {
					result.append(*it).append(glue);
				}
				result.append(*it);
				return result;
			}

			static inline std::vector<std::string> explode(const std::string&& delimiter, const std::string& string, ssize_t limit = -1) {
				size_t start = 0;
				size_t end = string.find(delimiter);
				std::vector<std::string> list;
				if (!string.empty()) {
					while (--limit != 0 && end != std::string::npos)
					{
						list.emplace_back(string.begin() + start, string.begin() + end);
						start = end + delimiter.length();
						end = string.find(delimiter, start);
					}
					list.emplace_back(string.begin() + start, string.end());
				}

				return list;
			}

			static inline std::string& trim(std::string& s) {
				s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
					return !std::isspace(ch);
					}));
				s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
					return !std::isspace(ch);
					}).base(), s.end());
				return s;
			}

			static inline std::string& lowercase(std::string& s) {
				std::transform(s.begin(), s.end(), s.begin(), ::tolower);
				return s;
			}

			static forceinline std::string& uppercase(std::string& s) {
				std::transform(s.begin(), s.end(), s.begin(), ::toupper);
				return s;
			}

			static inline std::string format(const std::string& format, ...) {
				va_list args;
				va_start(args, format);
				size_t size = std::vsnprintf(nullptr, 0, format.c_str(), args) + 1;
				va_end(args);
				auto buffer = (char*)alloca(size);
				va_start(args, format);
				std::vsnprintf(buffer, size, format.c_str(), args);
				va_end(args);
				return std::string(buffer, buffer + size - 1);
			}

			template<size_t SZ>
			static forceinline std::string format(const std::string& format, ...) {
				va_list args;
				auto buffer = (char*)alloca(SZ + 1);
				va_start(args, format);
				std::vsnprintf(buffer, SZ, format.c_str(), args);
				va_end(args);
				return std::string(buffer, buffer + SZ - 1);
			}

			template<typename ... needle>
			static forceinline size_t match(const std::string& haystack, needle&& ... needles) {
				const uint8_t* haystack_from = (const uint8_t*)(&haystack.front()), * haystack_to = (const uint8_t*)(&haystack.back() + 1);
				if (haystack_from && haystack_from < haystack_to) {
					static const char* needle_values[] = { (const char*)needles... };
					const size_t needle_size = sizeof ... (needles);

					const uint8_t* needle_it = (const uint8_t*)needle_values[0];
					const uint8_t* haystack_it = (const uint8_t*)haystack_from;

					bool match = false;
					const uint8_t** needle_list = (const uint8_t * *)& needle_values[0];
					size_t needle_index = needle_size;
					for (; needle_index && !(match = *needle_it == '\0') && haystack_it < haystack_to; ) {
						if (!icase_cmp(*haystack_it, *needle_it)) {
							needle_index--;
							needle_it = *(++needle_list);
							haystack_it = haystack_from;
							continue;
						}
						needle_it++;
						haystack_it++;
					}
					if (match) {
						return  1 + needle_size - needle_index;
					}
				}
				return 0;
			}

			template<typename ... needle>
			static inline size_t compare(const std::string& haystack, needle&& ... needles) {
				const uint8_t* haystack_from = (const uint8_t*)(&haystack.front()), * haystack_to = (const uint8_t*)(&haystack.back() + 1);
				if (haystack_from && haystack_from < haystack_to) {
					static const char* needle_values[] = { (const char*)needles... };
					const size_t needle_size = sizeof ... (needles);

					const uint8_t* needle_it = (const uint8_t*)needle_values[0];
					const uint8_t* haystack_it = (const uint8_t*)haystack_from;

					bool match = false;
					const uint8_t** needle_list = (const uint8_t * *)& needle_values[0];
					size_t needle_index = needle_size;
					for (; needle_index && !(match = *needle_it == '\0') && haystack_it < haystack_to; ) {
						if ((*haystack_it) != (*needle_it)) {
							needle_index--;
							needle_it = *(++needle_list);
							haystack_it = haystack_from;
							continue;
						}
						needle_it++;
						haystack_it++;
					}
					if (match) {
						return 1 + needle_size - needle_index;
					}
				}
				return 0;
			}

			template<typename ... needle>
			static inline size_t find(const std::string& haystack, needle&& ... needles) {
				const uint8_t* haystack_from = (const uint8_t*)(&haystack.front()), * haystack_to = (const uint8_t*)(&haystack.back() + 1);
				if (haystack_from && haystack_from < haystack_to) {
					static const char* needle_values[] = { (const char*)needles... };
					const size_t needle_size = sizeof ... (needles);

					const uint8_t* needle_it = needle_values[0];
					const uint8_t* haystack_it = haystack_from;

					const uint8_t* n_it = nullptr, * h_it = nullptr;
					bool match = false;

					for (size_t needle_index = 0; haystack_it < haystack_to; needle_it = needle_values[needle_index % needle_size], !(needle_index % needle_size) && haystack_it++) {
						for (n_it = needle_it, h_it = haystack_it;
							h_it < haystack_to && icase_cmp(*h_it, *n_it) && !(match = (*(n_it + 1) == (uint8_t)'\0')); ++n_it, ++h_it) {
						}
						if (match) {
							return 1 + (needle_index % needle_size);
						}
						needle_index++;
					}
				}
				return 0;
			}
		};
	}
}