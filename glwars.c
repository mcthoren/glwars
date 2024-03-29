#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <GL/glut.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BALLB           1
#define BALLR           2
#define BALLG           3
#define BALLM           4
#define BALLY           5
#define BALLC           6
#define BALLGRID        7
#define SHIP            8
#define BAR             9
#define BAR1            10
#define SHIP1           11
#define BALLSLICE       16
#define BALLSTACK       16
#define AMB             1.0
#define DFUS            0.50
#define SPEC            1.00
#define ALPH            1.00
#define ALPH1           1.00
#define ALPH2           0.70
#define SHIN            100.0
#define PORT            "1309"
#define SLEEPWAIT       100000

void * threadsend(void *);
void * threadrecv(void *);

void init(void);
void display(void);
void reshape(int, int);
void lookmove();
void keyboard(unsigned char, int, int);
void kbspecial(int, int, int);
static void usage();

GLfloat ex, ey, ez, cx, cy, cz, ro, phi, sp;
GLfloat barrot = 0, shieldrot = 0, bary0 = 2.0, bary1 = 2.0;
GLint chasetoggle = 1, autonormtoggle = 0, laser_time = 0, hit = 0, lives = 4;

GLfloat mat_specularshield[] = {0.00, 0.00, 0.010, ALPH2};
GLfloat mat_diffuseshield[] = {0.0, 0.00, 0.00, ALPH2};
GLfloat lmodel_ambientshield[] = {0.0, 0.90, 0.00, ALPH2};
GLfloat mat_shininesshield[] = {30};

pthread_rwlock_t cordss_lock, cordsr_lock;

struct cp{
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLint hit;
};

// coorindates{sent,recvd}_{,temp,used}
struct cp cordss, cordsr, cordsr_t, cordsr_u = {
	.x = 0,
	.y = 0,
	.z = 0,
	.hit = 0,
};

// timeout for the coordinates received lock
struct timespec cordsr_l_to = {
	.tv_sec = 0,
	.tv_nsec = 1000,
};

void
init(void)
{
	GLfloat x, y, z;
	GLint ball_iter = 1;

	GLfloat mat_specularr[] = {SPEC, 0.0, 0.0, ALPH};
	GLfloat mat_specularg[] = {0.0, SPEC, 0.0, ALPH};
	GLfloat mat_specularb[] = {0.0, 0.0, SPEC, ALPH};
	GLfloat mat_specularc[] = {0.0, SPEC, SPEC, ALPH};
	GLfloat mat_specularm[] = {SPEC, 0.0, SPEC, ALPH};
	GLfloat mat_speculary[] = {SPEC, SPEC, 0.0, ALPH};

	GLfloat mat_diffuser[] = {DFUS, 0.0, 0.0, ALPH};
	GLfloat mat_diffuseg[] = {0.0, DFUS, 0.0, ALPH};
	GLfloat mat_diffuseb[] = {0.0, 0.0, DFUS, ALPH};
	GLfloat mat_diffusec[] = {0.0, DFUS, DFUS, ALPH};
	GLfloat mat_diffusem[] = {DFUS, 0.0, DFUS, ALPH};
	GLfloat mat_diffusey[] = {DFUS, DFUS, 0.0, ALPH};

	GLfloat lmodel_ambientr[] = {AMB, 0.00, 0.00, ALPH};
	GLfloat lmodel_ambientg[] = {0.00, AMB, 0.00, ALPH};
	GLfloat lmodel_ambientb[] = {0.00, 0.00, AMB, ALPH};
	GLfloat lmodel_ambientc[] = {0.00, AMB, AMB, ALPH};
	GLfloat lmodel_ambientm[] = {AMB, 0.00, AMB, ALPH};
	GLfloat lmodel_ambienty[] = {AMB, AMB, 0.00, ALPH};

	GLfloat mat_specularship[] = {0.0, 0.0, 0.0, ALPH};
	GLfloat mat_diffuseship[] = {0.4, 0.0, 0.0, ALPH};
	GLfloat lmodel_ambientship[] = {0.2, 0.0, 0.00, ALPH};

	GLfloat mat_specularglass[] = {0.00, 0.00, 0.010, ALPH1};
	GLfloat mat_diffuseglass[] = {0.0, 0.40, 0.50, ALPH1};
	GLfloat lmodel_ambientglass[] = {0.0, 0.30, 0.30, ALPH1};
	GLfloat mat_shininessglass[] = {200};

        GLfloat mat_specularshield1[] = {0.00, 0.00, 0.010, ALPH2};
        GLfloat mat_diffuseshield1[] = {0.0, 0.00, 0.00, ALPH2};
        GLfloat lmodel_ambientshield1[] = {0.0, 0.90, 0.00, ALPH2};
        GLfloat mat_shininesshield1[] = {30};

	GLfloat mat_shininess[] = {SHIN};
	GLfloat light_position[] = {200.0, 200.0, 200.0, 0.0};
	GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
	//GLfloat lmodel_ambient[] = {0.75, 0.75, 0.75, 1.0};

	ex = ey = ez = -100;
	cx = cy = cz = -2;
	phi = 45;
	ro = 0;
	sp = 1;
	cordss.hit = cordsr.hit = 0;

        //glEnable (GL_BLEND);  
        //glBlendFunc (GL_SRC_ALPHA, GL_DST_ALPHA);
        glShadeModel (GL_SMOOTH); 
        glClearColor (0.0, 0.0, 0.0, 0.0); 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
        //glEnable(GL_AUTO_NORMAL);


	glNewList(BALLB, GL_COMPILE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuseb);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position); 
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light); 
		glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientb);
		glutSolidSphere(1.0, BALLSLICE, BALLSTACK);
	glEndList();

	glNewList(BALLR, GL_COMPILE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularr);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuser);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientr);
		glutSolidSphere(1.0, BALLSLICE, BALLSTACK);
	glEndList();

	glNewList(BALLG, GL_COMPILE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularg);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuseg);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientg);
		glutSolidSphere(1.0, BALLSLICE, BALLSTACK);
	glEndList();

	glNewList(BALLC, GL_COMPILE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularc);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffusec);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientc);
		glutSolidSphere(1.0, BALLSLICE, BALLSTACK);
	glEndList();

	glNewList(BALLM, GL_COMPILE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularm);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffusem);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientm);
		glutSolidSphere(1.0, BALLSLICE, BALLSTACK);
	glEndList();

	glNewList(BALLY, GL_COMPILE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_speculary);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffusey);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambienty);
		glutSolidSphere(1.0, BALLSLICE, BALLSTACK);
	glEndList();

	glNewList(BALLGRID, GL_COMPILE);
		ball_iter = 1;
		for (x = -1400.0; x <= 1400.0; x += 200) {
			for(y = -1400.0; y <= 1400.0; y += 200) {
				for(z = -1400.0; z <= 1400.0; z += 200) {
					glTranslatef(x, y, z);
					glCallList(ball_iter);
					ball_iter++;
					if (ball_iter > 6 ) ball_iter = 1;
					glTranslatef(-x, -y, -z);
                       		}
			}
		}
	glEndList();

	glNewList(SHIP, GL_COMPILE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularship);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuseship);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientship);
		glScalef(1.0, 0.3, 1.0);
		glutSolidSphere(2.0, 12, 12);
		glScalef(1.0, 1/0.3, 1.0);

		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_DST_ALPHA);

		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularglass);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininessglass);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuseglass);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientglass);

		glScalef(0.8, 0.8, 0.8);
		glTranslatef(0.0, 0.5, 0.0);
		glutSolidSphere(1.0, 12, 12);
		glScalef(1/0.8, 1/0.8, 1/0.8);
		glTranslatef(0.0, -0.5, 0.0);

		glDisable(GL_BLEND);
	glEndList();

	glNewList(SHIP1, GL_COMPILE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularship);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuseship);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientship);
		glScalef(1.0, 0.3, 1.0);
		glutSolidSphere(2.0, 12, 12);
		glScalef(1.0, 1/0.3, 1.0);

		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_DST_ALPHA);

		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularglass);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininessglass);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuseglass);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientglass);

		glScalef(0.8, 0.8, 0.8);
		glTranslatef(0.0, 0.5, 0.0);
		glutSolidSphere(1.0, 12, 12);
		glScalef(1/0.8, 1/0.8, 1/0.8);
		glTranslatef(0.0, -0.5, 0.0);

		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularshield1);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininesshield1);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuseshield1);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientshield1);
		glTranslatef(0.0, 0.4, 0.0);
		glutSolidSphere(2.3, 22, 22);

		glDisable(GL_BLEND);
	glEndList();


	glNewList(BAR, GL_COMPILE);
		glDisable(GL_LIGHTING);
		glColor4f(0.30, 0.90, 0.10, 1.000);
		glScalef(0.7, 1.0, 0.7);
		glutSolidCube(1);
		glScalef(1/0.7, 1.0, 1/0.7);
		glEnable(GL_LIGHTING);
	glEndList();

	glNewList(BAR1, GL_COMPILE);
		glDisable(GL_LIGHTING);
		glColor4f(0.30, 0.10, 0.90, 1.00);
		glScalef(0.7, 1.0, 0.7);
		glutSolidCube(1);
		glScalef(1/0.7, 1.0, 1/0.7);
		glEnable(GL_LIGHTING);
	glEndList();


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void
display(void)
{
	GLint j = 0, k = 0;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLoadIdentity();

	if (autonormtoggle == 1) {
		glEnable(GL_AUTO_NORMAL);
	}
	else if (autonormtoggle == 0) {
		glDisable(GL_AUTO_NORMAL);
	}
	
	glScalef(0.10, bary0, 0.10);
	glTranslatef(-10.9, 0.0, -20.0);
	glRotatef(barrot, 0, 1, 0);
	glCallList(BAR);
	glLoadIdentity();

	glScalef(0.10, bary1, 0.10);
	glTranslatef(10.9, 0.0, -20.0);
	glRotatef(-barrot, 0, 1, 0);
	glCallList(BAR1);
	glLoadIdentity();

	k = -4.0;
	for (j = lives - 1; j >= 1; j--) {
		glTranslatef(k, 5.15, -10.0);
		glRotatef(-30.0, 0.0, 0.0, 1.0);
		glRotatef(20.0, 1.0, 0.0, 0.0);
		glScalef(0.20, 0.20, 0.20);
		glCallList(SHIP1);
		k += 4.0;
		glLoadIdentity();
	}
	
	if (pthread_rwlock_timedrdlock(&cordsr_lock, &cordsr_l_to) == 0) {
		cordsr_u = cordsr;
		pthread_rwlock_unlock(&cordsr_lock);
	}
	glTranslatef(cordsr_u.x, cordsr_u.y, cordsr_u.z);
	glCallList(SHIP1);
	glLoadIdentity();

	gluLookAt(ex, ey, ez, cx, cy, cz, 0, 1, 0);	 
	glCallList(BALLGRID);
	glLoadIdentity();

	if (laser_time > 0) {
		glDisable(GL_LIGHTING);
		glColor4f(1.0, 0.0, 1.0, 1.0);
		if (chasetoggle == 0) {
			glBegin(GL_LINES);
			glVertex3f(-0.20, -0.50, 0.0);
			glVertex3f(0.0, 0.0, -800.0);
			glVertex3f(0.20, -0.50, 0.0);
			glVertex3f(0.0, 0.0, -800.0);
			glEnd();
		}
		if (chasetoggle == 1) {
			glBegin(GL_LINES);
			glVertex3f(-0.20, -2.00, -10.0);
			glVertex3f(-0.50, -20.0, -800.0);
			glVertex3f(0.20, -2.00, -10.0);
			glVertex3f(0.50, -20.0, -800.0);
			glEnd();
		}
		glEnable(GL_LIGHTING);
		laser_time--;
		//glLoadIdentity();
	}

	if (chasetoggle == 1) {
		glTranslatef(0.0, -2.0, -10.0);
		glRotatef(barrot/2, 0, 1, 0);
		glCallList(SHIP);

		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_DST_ALPHA);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specularshield);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininesshield);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuseshield);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambientshield);

		glTranslatef(0.0, 0.4, 0.0);
		glutSolidSphere(2.3, 22, 22);

		glDisable(GL_BLEND);

		glLoadIdentity();
	}
	glutSwapBuffers();
}

void
reshape(int w, int h)
{
	GLint H;

	H = h;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	if (w <= H) {
		gluPerspective(60, (h / w), .70, 1200);
	}
	else {
		gluPerspective(60, (w / h), .70, 1200);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void
lookmove()
{
	GLfloat a, b, c, d;

	mat_specularshield[3] = sin(shieldrot);
	mat_diffuseshield[3] = sin(shieldrot);
	lmodel_ambientshield[3] = sin(shieldrot);

	cordss.x = ex = cx;
	cordss.y = ey = cy;
	cordss.z = ez = cz;
	cx = sp * (ex + sin(phi));
	cy = sp * (ey + sin(ro));
	cz = sp * (ez + cos(phi));

	if (laser_time > 0) {
		a = cx * cx + cy + cy + cz + cz;
		b = 2 * (cx * (ex - cordsr.x) + cy * (ey - cordsr.y) + cz * (ez - cordsr.z));
		c = ((ex - cordsr.x) * (ex - cordsr.x)) + ((ey - cordsr.y) * (ey - cordsr.y)) + ((ez - cordsr.z) * (ez - cordsr.z)) - 5.29;
		d = a + b + c;
		if (d < 0) keyboard('k', 1, 1);
	  }

	if (cordsr.hit != hit) {
		hit = cordsr.hit;	
		bary0 -= 0.2;
		if (bary0 <= 0) {
			bary0 = 2.0;
			lives--;
			bary1 = 2.0;
			if (lives <= 0) exit(1);
		}
	 }

	barrot += 10;
	if (barrot >= 3600) barrot = 0;

	shieldrot += 0.02;
	if (shieldrot >= (3.1415926534)) shieldrot = 0;

	glutPostRedisplay();
	glutTimerFunc(3, lookmove, 1);
}

void
keyboard(unsigned char key, int x __attribute__((__unused__)), int y __attribute__((__unused__)))
{
	switch (key) {
		case 27:
		case 'q':
		case 'Q':
			exit(0);
     			break;
		case 'x':
		case 'X':
			sp += 0.030;
			break;
		case 's':
		case 'S':
			sp -= 0.030;
			break;
		case 'c':
		case 'C':
			if (chasetoggle == 0) chasetoggle = 1;
			else if (chasetoggle == 1) chasetoggle = 0;
			break;
		case 'a':
		case 'A':
			if (autonormtoggle == 0) autonormtoggle = 1;
			else if (autonormtoggle == 1) autonormtoggle = 0;
			break;
		case ' ':
			if (bary1 > 0.0){
				if (bary1 <= 0.05)
					bary1 = 0;
				else
					bary1 -= 0.050;
			}
			if (bary1 > 0.0) laser_time = 7;
			break;
		case 'd':
		case 'D':
			if (cordsr.hit == 0) cordsr.hit = 1;
			else if (cordsr.hit == 1) cordsr.hit = 0;
			break;
		case 'k':
		case 'K':
			if (cordss.hit == 0) cordss.hit = 1;
			else if (cordss.hit == 1) cordss.hit = 0;
			break;
	}
}

void
kbspecial(int key, int x __attribute__((__unused__)), int y __attribute__((__unused__)))
{
	switch(key){
		case GLUT_KEY_DOWN:
			ro += 0.070;
			break;
		case GLUT_KEY_UP:
			ro -= 0.070;
			break;
		case GLUT_KEY_RIGHT:
			phi -= .080;
			if (phi <= 0) phi = 360;
			break;
		case GLUT_KEY_LEFT:
			phi += .080;
			if (phi >= 360) phi = 0;
			break;
	}
}

static void
usage(void)
{
	extern char *__progname;        /* from crt0.o */
	(void) fprintf(stderr, "usage: %s host\n", __progname);
	exit(1);
}

int
main(int argc, char** argv)
{
	pthread_t tid[2];
	int pt_send_err = -1, pt_recv_err = -1, gai_err = -1;
	struct addrinfo *enemy_ai;
	char ip[INET6_ADDRSTRLEN];

	if (argc != 2) {
		usage();
	}

	pthread_rwlock_init(&cordss_lock, NULL);
	pthread_rwlock_init(&cordsr_lock, NULL);

	gai_err = getaddrinfo(argv[1], PORT, NULL, &enemy_ai);

	if (gai_err != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_err));
		return 2;
	}

	if (enemy_ai->ai_family == AF_INET) {
		printf("addr:\t%s\n", inet_ntop(AF_INET, &((struct sockaddr_in *)enemy_ai->ai_addr)->sin_addr, ip, INET6_ADDRSTRLEN));
	} else if (enemy_ai->ai_family == AF_INET6) {
		printf("addr:\t%s\n", inet_ntop(AF_INET6, &((struct sockaddr_in6 *)enemy_ai->ai_addr)->sin6_addr, ip, INET6_ADDRSTRLEN));
	} else {
		perror("invalid address family");
		freeaddrinfo(enemy_ai);
		exit(1);
	}
	printf("port:\t%d\n", ntohs(((struct sockaddr_in *)enemy_ai->ai_addr)->sin_port));

	pt_send_err = pthread_create(&tid[0], NULL, threadsend, enemy_ai);
	pt_recv_err = pthread_create(&tid[1], NULL, threadrecv, enemy_ai);

	if (pt_send_err != 0) {
		perror("send thread error");
		freeaddrinfo(enemy_ai);
		exit(1);
	}

	if (pt_recv_err != 0) {
		perror("recv thread error");
		freeaddrinfo(enemy_ai);
		exit(1);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(1200, 730);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(kbspecial);
	glutTimerFunc(3, lookmove, 1);
	glutMainLoop();

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	freeaddrinfo(enemy_ai);
	pthread_rwlock_destroy(&cordss_lock);
	pthread_rwlock_destroy(&cordsr_lock);
	return 0;
} 	 

void *
threadsend(void * void_enemy_ai)
{
	int sockfd, bind_err = 0;
	struct sockaddr_in tsend, tlocal;
	struct sockaddr_in6 tsend6, tlocal6;

	struct addrinfo *enemy_ai = (struct addrinfo *) void_enemy_ai;
	int ts_size = sizeof(tsend);
	int cords_size = sizeof(struct cp);

	memset(&tlocal, 0, sizeof(tlocal));
	memset(&tsend, 0, sizeof(tsend));
	memset(&tlocal6, 0, sizeof(tlocal6));
	memset(&tsend6, 0, sizeof(tsend6));

	if (enemy_ai->ai_family == AF_INET) {
		tlocal.sin_family = enemy_ai->ai_family;
		tlocal.sin_port = INADDR_ANY;
		tlocal.sin_addr.s_addr = INADDR_ANY;

		tsend.sin_family = enemy_ai->ai_family;
		tsend.sin_port = ((struct sockaddr_in *)enemy_ai->ai_addr)->sin_port;
		tsend.sin_addr = (((struct sockaddr_in *)enemy_ai->ai_addr)->sin_addr);
	}

	if (enemy_ai->ai_family == AF_INET6) {
		tlocal6.sin6_family = enemy_ai->ai_family;
		tlocal6.sin6_port = INADDR_ANY;
		// tlocal6.sin6_addr.s6_addr = INADDR_ANY;

		tsend6.sin6_family = enemy_ai->ai_family;
		tsend6.sin6_port = ((struct sockaddr_in6 *)enemy_ai->ai_addr)->sin6_port;
		tsend6.sin6_addr = (((struct sockaddr_in6 *)enemy_ai->ai_addr)->sin6_addr);
	}

	sockfd = socket(enemy_ai->ai_family, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("bad sock");
		exit(1);
	}

	if (enemy_ai->ai_family == AF_INET) {
		bind_err = bind(sockfd, (struct sockaddr *) &tlocal, sizeof(tlocal));
	}

	if (enemy_ai->ai_family == AF_INET6) {
		bind_err = bind(sockfd, (struct sockaddr *) &tlocal6, sizeof(tlocal6));
	}

	if (bind_err < 0) {
		perror("send thread unbound");
		close(sockfd);
		exit(1);
	}

	while(1) {
		usleep(SLEEPWAIT);
		// pthread_rwlock_rdlock(&cordss_lock);
		sendto(sockfd, &cordss, cords_size, 0, (struct sockaddr *) &tsend, ts_size);
		// pthread_rwlock_unlock(&cordss_lock);
		// printf("\nsent:\t%f, %f, %f", cordsr.x, cordsr.y, cordsr.z);
	}

	return NULL;
}

void *
threadrecv(void * void_enemy_ai)
{
	int sockfd, bind_err = 0;
	struct sockaddr_in trecv;
	struct sockaddr_in6 trecv6;

	struct addrinfo *enemy_ai = (struct addrinfo *) void_enemy_ai;
	int cords_size = sizeof(struct cp);

	memset(&trecv, 0, sizeof(trecv));
	memset(&trecv6, 0, sizeof(trecv6));

	if (enemy_ai->ai_family == AF_INET) {
		trecv.sin_family = enemy_ai->ai_family;
		trecv.sin_port = ((struct sockaddr_in *)enemy_ai->ai_addr)->sin_port;
	}

	if (enemy_ai->ai_family == AF_INET6) {
		trecv6.sin6_family = enemy_ai->ai_family;
		trecv6.sin6_port = ((struct sockaddr_in6 *)enemy_ai->ai_addr)->sin6_port;
	}

	sockfd = socket(enemy_ai->ai_family, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("bad sock");
		exit(1);
	}

	if (enemy_ai->ai_family == AF_INET) {
		bind_err = bind(sockfd, (struct sockaddr *) &trecv, sizeof(trecv));
	}

	if (enemy_ai->ai_family == AF_INET6) {
		bind_err = bind(sockfd, (struct sockaddr *) &trecv6, sizeof(trecv6));
	}

	if (bind_err < 0) {
		perror("receive thread unbound");
		close(sockfd);
		exit(1);
	}

	while(1) {
		usleep(SLEEPWAIT - 1000);
		recv(sockfd, &cordsr_t, cords_size, 0);
		pthread_rwlock_wrlock(&cordsr_lock);
		cordsr = cordsr_t;
		pthread_rwlock_unlock(&cordsr_lock);
		printf("\nrecv:\t%f, %f, %f", cordsr.x, cordsr.y, cordsr.z);
	}

	return NULL;
}
