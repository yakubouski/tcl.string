#pragma once

#ifdef DEBUG
#	define forceinline inline
#else
#	ifdef _MSC_VER
#		define forceinline __forceinline
#	else
#		define forceinline __attribute__((always_inline))
#	endif
#endif