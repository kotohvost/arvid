#define	HISTSZ	16		/* length of history */

class CMD {
//	char	*history[HISTSZ];	/* previous executed commands */

int	insname(char *name);
int	input(int plen);
void	histcmd(void);
int	mycmd(struct dir *cur, int);
int	mycmd2(struct dir *cur, int);
char	*getpar(char *);
void	erasecmd(int);
int	strbcmp(register char *s, register char *b);

public:
void	delcmd(void);
void	endcmd(void);
void	leftcmd(void);
void	upcmd(void);
void	downcmd(void);
void	nextcmd(void);
void	prevcmd(void);
void	namecmd(struct dir *cur);
void	inscmd(int key);
void	homecmd(void);
void	rightcmd(void);
void	drawcmd(struct dir *cur, struct dir *left, struct dir *right);
void	exec(struct dir *cur, struct dir *left, struct dir *right, int hit, int savhist);
void	histmenu(struct dir *cur, struct dir *left, struct dir *right);
};

extern CMD cmd;
void histmenu(void);
