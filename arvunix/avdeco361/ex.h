class InitFile {
char	*getlin(void);

public:
void	read(void);
void	write(void);
};

extern InitFile initfile;
void writeinitfile(void);

/*
 * User defined menu
 */
struct umenu {
	char key;
	char hist;
	char wait;
	char menu;
	char *str;
	char *cmd;
	char *cex;
};

# define EXSZ	64

class UserDefMenu {
	umenu	um[EXSZ];	// users menu
	int	nm;		// num menu

int	open(char *filename);
int	init(char *filename, char *scale);
void	print(void);
char	*run(char *scale);
void	free(void);

public:
void	menu(void);
};

extern UserDefMenu udm;
void usermenu(void);
void excommand (char *cp, char *fname);
