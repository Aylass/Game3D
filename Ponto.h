#include <iostream>
using namespace std;

class Ponto {

public:
	float x,y,z;
	void set(float x, float y) {		this->x = x;		this->y = y;	}	void imprime() {		cout << "(" << x << ", " << y << ")" << flush;	}} ;