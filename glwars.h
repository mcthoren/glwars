#define BALLB		1
#define BALLR		2
#define BALLG		3
#define BALLM		4
#define BALLY		5
#define BALLC		6
#define BALLGRID	7
#define SHIP		8
#define BAR		9
#define BAR1		10
#define SHIP1		11
#define BALLSLICE	16
#define BALLSTACK	16
#define AMB		1.0
#define DFUS    	0.50
#define SPEC		1.00
#define ALPH		1.00
#define ALPH1		1.00
#define ALPH2		0.70
#define SHIN		100.0
#define PORT		"1309"
#define SLEEPWAIT	100000

void * threadsend(void *);
void * threadrecv(void *);

void init(void);
void display(void);
void reshape(int, int);
void lookmove();
void keyboard(unsigned char, int, int);
void kbspecial(int, int, int);
static void usage();
