void * threadsend(void *);
void * threadrecv(void *);

void init(void);
void display(void);
void reshape(int, int);
void lookmove();
void keyboard(unsigned char, int, int);
void kbspecial(int, int, int);
static void usage(char *);
