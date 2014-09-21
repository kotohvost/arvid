#include "mystring.h"

// constructor
mystring::mystring(void) {
	str = NULL;
};

// constructor
mystring::mystring(char const *Str) {
	str = NULL;
	if (Str && *Str) {
		str = new char[strlen(Str) + 1];
		memcpy(str, Str, strlen(Str) + 1);
	}
};

// constructor
mystring::mystring(char const *Str, unsigned int Len) {
	str = NULL;
	if (Str && *Str) {
		str = new char[Len + 1];
//		strncpy(Str, str, Len + 1);
		memcpy(str, Str, Len);
		str[Len + 1] = '\0';
	}
};

#if 0
// delete string
void mystring::del(void) {
	delete str;
};
#endif

// =
mystring &mystring::operator=(mystring &Str) {
	if (str) {
		delete str;
	}
	str = NULL;
	if (Str.str && *Str.str) {
		str = new char[strlen(Str.str) + 1];
//		strncpy(Str, str, Len + 1);
		memcpy(str, Str.str, strlen(Str.str) + 1);
//		str[Len + 1] = '\0';
	}
	return *this;
};

// = char
mystring &mystring::operator=(char const *Str) {
	if (str) {
		delete str;
	}
	str = NULL;
	if (Str && *Str) {
		str = new char[strlen(Str) + 1];
		memcpy(str, Str, strlen(Str) + 1);
	}
	return *this;
};

// +=
mystring mystring::operator+(mystring const &Str) {
char	*newstr;

	newstr = new char[strlen(str) + strlen(Str.str) + 1];
	memcpy(newstr, str, strlen(str));
	memcpy(newstr + strlen(str), Str.str, strlen(Str.str) + 1);
//	mystring my(newstr);
	if (str)
		delete str;
	str = newstr;
//	*this = newstr;
//	delete newstr;
//	*this = my;
	return *this;
};

// char *
mystring::operator char const *(void) const {
	return str;
};
