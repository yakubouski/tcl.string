#pragma once
#include <cinttypes>
#include <string>
#include "pch.h"
#include "stringchars.h"

class cstringview {
	using tp = uint8_t;
	const tp* _begin;
	const tp* _end;
public:
	cstringview(const tp* from = nullptr, const tp* to = nullptr) : _begin(from), _end(to) { ; }
	template<typename VT>
	cstringview(const VT* from, const VT* to) : _begin((const tp*)from), _end((const tp*)to) { ; }
	cstringview(const cstringview& v) : _begin(v._begin), _end(v._end) { ; }

	~cstringview() { ; }

public:
	forceinline cstringview& operator = (const cstringview& v) { _begin = v._begin; _end = v._end; return *this; }
	template<typename VT>
	forceinline cstringview& attach(const VT* from, const VT* to) { _begin = (const tp*)from; _end = (const tp*)to; return *this; }
	forceinline void detach() { _begin = nullptr; _end = nullptr; }
	forceinline bool empty() const { return _begin == _end; }
	forceinline size_t size() const { return _end - _begin; }
	forceinline ssize_t compare(const cstringview& v) const { return compare(std::move(v)); }
	forceinline ssize_t compare(const char* needle) const {
		if (_begin && _begin < _end) {
			const tp* n_it = (const tp*)needle;
			const tp* h_it = _begin;
			for (; h_it < _end && *n_it && *h_it == *n_it; ++h_it, ++n_it) { ; }
			return *n_it == '\0' ? 0 : (*n_it > * h_it ? 1 : -1);
		}
		return -1;
	}
	forceinline ssize_t compare(const cstringview&& needle) const {
		if (_begin && _begin < _end && needle._begin && needle._begin < needle._end) {
			const tp* n_it = needle._begin;
			const tp* h_it = _begin;
			for (; h_it < _end && n_it < needle._end && *h_it == *n_it; ++h_it, ++n_it) { ; }
			return *(n_it - 1) == *(h_it - 1) ? 0 : (*(n_it - 1) > * (h_it - 1) ? 1 : -1);
		}
		return -1;
	}
	forceinline size_t compare(const char* needle_values[], const size_t needle_size, cstringview&& v = {}) const {
		if (_begin && _begin < _end) {
			const uint8_t* needle_it = (const uint8_t*)needle_values[0];
			const uint8_t* haystack_it = (const uint8_t*)_begin;

			bool match = false;
			const uint8_t** needle_list = (const uint8_t * *)& needle_values[0];
			size_t needle_index = needle_size;
			for (; needle_index && !(match = *needle_it == '\0') && haystack_it < _end; ) {
				if ((*haystack_it) != (*needle_it)) {
					needle_index--;
					needle_it = *(++needle_list);
					haystack_it = _begin;
					continue;
				}
				needle_it++;
				haystack_it++;
			}
			if (match) {
				v._begin = _begin;
				v._end = haystack_it;
				return 1 + needle_size - needle_index;
			}
		}
		v._begin = _begin;
		v._end = _begin;
		return 0;
	}

	forceinline size_t match(const char* needle_values[], const size_t needle_size, cstringview&& v = {}) const {
		if (_begin && _begin < _end) {
			const uint8_t* needle_it = (const uint8_t*)needle_values[0];
			const uint8_t* haystack_it = (const uint8_t*)_begin;

			bool match = false;
			const uint8_t** needle_list = (const uint8_t * *)& needle_values[0];
			size_t needle_index = needle_size;
			for (; needle_index && !(match = *needle_it == '\0') && haystack_it < _end; ) {
				if (!ccharset::icase_compare(*haystack_it, *needle_it)) {
					needle_index--;
					needle_it = *(++needle_list);
					haystack_it = _begin;
					continue;
				}
				needle_it++;
				haystack_it++;
			}
			if (match) {
				v._begin = _begin;
				v._end = haystack_it;
				return 1 + needle_size - needle_index;
			}
		}
		v._begin = _begin;
		v._end = _begin;
		return 0;
	}

	forceinline size_t find(const char* needle_values[], const size_t needle_size, cstringview&& v = {}) const {
		if (_begin && _begin < _end) {
			const uint8_t* needle_it = (const uint8_t*)needle_values[0];
			const uint8_t* haystack_it = _begin;

			const uint8_t* n_it = nullptr, * h_it = nullptr;
			bool match = false;

			for (size_t needle_index = 0; haystack_it < _end; needle_it = (const uint8_t*)needle_values[needle_index % needle_size], !(needle_index % needle_size) && haystack_it++) {
				for (n_it = needle_it, h_it = haystack_it;
					h_it < _end && ccharset::icase_compare(*h_it, *n_it) && !(match = (*(n_it + 1) == (uint8_t)'\0')); ++n_it, ++h_it) {
				}
				if (match) {
					v._begin = haystack_it;
					v._end = h_it + 1;
					return 1 + (needle_index % needle_size);
				}
				needle_index++;
			}
		}
		v._begin = _begin;
		v._end = _begin;
		return 0;
	}


	forceinline void trim() {
		if (_begin != _end) {
			while (((!*_begin || std::isspace(*_begin)) && ((++_begin) < _end))) { ; }
			while ((_end > _begin && (!*(_end - 1) || std::isspace(*(_end - 1))) && (--_end))) { ; }
		}
	}

	forceinline void trim(const ccharset& charset) {
		if (_begin != _end) {
			while (((!*_begin || charset.is(*_begin)) && ((++_begin) < _end))) { ; }
			while ((_end > _begin && (!*(_end - 1) || charset.is(*(_end - 1))) && (--_end))) { ; }
		}
	}

	forceinline decltype(auto) begin() const { return _begin; }
	forceinline decltype(auto) end() const { return _end; }

	forceinline std::string str() const { if (_begin < _end) return { _begin,_end }; return {}; }
};
namespace std {
	template <> 
	struct hash<cstringview>
	{
		inline size_t operator()(const cstringview& v) const {
			/* unsafe fast hash implementation */
			uint64_t hash = 0xc6a4a7935bd1e995;
			const uint64_t* p_it = (const uint64_t*)v.begin(), * a_to = (const uint64_t*)v.end();
			for (; (p_it + 1) <= a_to; hash ^= (*p_it), ++p_it) { ; }
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
	};

	template <>
	struct equal_to<cstringview>
	{
		inline bool operator()(const cstringview& a, const cstringview& b) const {
			if (a.size() == b.size()) {
				size_t length = a.size();
				for (const uint8_t* a_it = a.begin(), *b_it = b.begin(); length && ccharset::icase_compare(*a_it, *b_it); --length) { ; }
				return length == 0;
			}
			return false; 
		}
	};
}