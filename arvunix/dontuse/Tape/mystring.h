#include <stdio.h>

class mystring {
//	struct {
	char	*str;	// my string
	int	use;	// my use
//	} *string;
public:
	mystring(void);		// mystring xx;
	mystring(char const *);	// mystring xx("str");
	mystring(char const *, unsigned int);
				// mystring xx("str", 2);
//void	del(void);		// ???
mystring	&operator=(mystring &);
				// mystring xx = mystring yy;
mystring	&operator=(char const *);
				// mystring xx = "str";
mystring	operator+(mystring const &);
				// mystring xx += mystring yy;
	operator char const *(void) const;
				// (char *)mystring xx("ddd");
};
