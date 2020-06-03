
#ifdef WIN32
#include <windows.h>
#include "gl\glut.h"
#endif


#ifdef __APPLE__
#include <GLUT/glut.h>
#endif


class Linha {
	float minx,miny, maxx, maxy; // envelope

public:
	float x1,y1,x2,y2;

	void geraLinha();
    void geraLinhaCurta();
	void desenhaLinha();

};
