#pragma once
#include <string>

class cstringvalidate {

	// https://myfin.by/stati/view/11001-o-belorusskom-pasporte-i-ego-lichnom-kode
	// https://kadrovik.by/topic/44885/
	// http://www.portal.nalog.gov.by/grp/#!ul

public:
	/*
	*	1(A)-Минск
		2(B)-Брестская облать
		3(C)-Витебская облать
		4(E)-Гомельская облать
		5(H)-Гродненская облать
		6(K)-Минская облать
		7(M)-Могилевская облать
	*/
	static inline bool unp_by(const std::string& value,size_t& region) {
		static constexpr ssize_t w[] = { 29,23,19,17,13,7,5,3 };
		region = 0;
		if (value.length() == 9) {
			ssize_t sum = (value[7] - '0') * w[7] + (value[6] - '0') * w[6] + (value[5] - '0') * w[5] + (value[4] - '0') * w[4] +
				(value[3] - '0') * w[3] + (value[2] - '0') * w[2];
			switch (value[1]) {
			case 'A': sum += 0 * w[1]; break; case 'B': sum += 1 * w[1]; break; case 'C': sum += 2 * w[1]; break;
			case 'E': sum += 3 * w[1]; break; case 'H': sum += 4 * w[1]; break; case 'K': sum += 5 * w[1]; break;
			case 'M': sum += 6 * w[1]; break; case 'O': sum += 7 * w[1]; break;	case 'P': sum += 8 * w[1]; break;
			case 'T': sum += 9 * w[1]; break; default: sum += (value[1] - '0') * w[1]; break;
			}
			switch (value[0]) {
			case 'A': region = 1; sum += 10 * w[0]; break; case 'B': region = 2; sum += 11 * w[0]; break; 
			case 'C': region = 3; sum += 12 * w[0]; break; case 'E': region = 4; sum += 14 * w[0]; break; 
			case 'H': region = 5; sum += 17 * w[0]; break; case 'K': region = 6; sum += 20 * w[0]; break;
			case 'M': region = 7; sum += 22 * w[0]; break; default: region = (value[0] - '0'); sum += region * w[0]; break;
			}
			return (value[8] - '0') == (sum % 11);
		}
		return false;
	}
	static inline bool unp_by(const std::string& value) {
		size_t region;
		return unp_by(value, region);
	}
};