// **********************************************************************
// PUCRS/Escola Politécnica
// COMPUTAÇÃO GRÁFICA
//
// Programa básico para criar aplicacoes 3D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include "gl\glut.h"
    static DWORD last_idle_time;
#else
    #include <sys/time.h>
    static struct timeval last_idle_time;
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

//Variáveis Grobais

// 0.5 faz o salto ser muito grande
float t = 0.01; //usado no calculo de movimentacao

int terceirapessoa; //se o jogo esta em 3 pessoa ou não

GLfloat AspectRatio, AngY=0;

class Ponto  // Struct para armazenar um ponto
{
    public : float X,Y,Z;
public:
    void Set(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }
    void Imprime()
    {
        cout << "X: " << X << " Y: " << Y << " Z: " << Z;
    }
};

Ponto User, Alvo;//usado em 1 pessoa
//o user em 3 pessoa é fixo, e o alvo acompanha o user da 1 pessoa
// *********************************************************************
//   ESTRUTURAS A SEREM USADAS PARA ARMAZENAR UM OBJETO 3D
// *********************************************************************

typedef struct  // Struct para armazenar um ponto
{
    float X,Y,Z;
    void Set(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }
    void Imprime()
    {
        cout << "X: " << X << " Y: " << Y << " Z: " << Z;
    }
} TPoint;


typedef struct // Struct para armazenar um triângulo
{
    TPoint P1, P2, P3;
    float r, g, b;
    void imprime()
    {
       // cout << "P1 ";  P1.Imprime(); cout << endl;
       // cout << "P2 ";  P2.Imprime(); cout << endl;
       // cout << "P3 ";  P3.Imprime(); cout << endl;
        // cout << "Cor :" << cor  << endl;
    }
} TTriangle;

// Classe para armazenar um objeto 3D
class Objeto3D
{
    TTriangle *faces; // vetor de faces
    unsigned int nFaces; // Variavel que armazena o numero de faces do objeto
public:
    Objeto3D()
    {
        nFaces = 0;
        faces = NULL;
    }
    unsigned int getNFaces()
    {
        return nFaces;
    }
    void LeObjeto (char *Nome); // implementado fora da classe
    void ExibeObjeto(); // implementado fora da classe
};


Objeto3D *MundoVirtual;

void Objeto3D::LeObjeto (char *Nome)
{
    // ***************
    // Exercicio
    //      complete esta rotina fazendo a leitura do objeto
    // ***************

    ifstream arq;
    arq.open(Nome, ios::in);
    if (!arq)
    {
        cout << "Erro na abertura do arquivo " << Nome << "." << endl;
        exit(1);
    }
    arq >> nFaces;
    faces = new TTriangle[nFaces];
    float x,y,z;
    int c;
    for (int i=0;i<nFaces;i++)
    {
        // Le os trs vŽrtices
        arq >> x >> y >> z; // Vertice 1
        faces[i].P1.Set(x,y,z);
        arq >> x >> y >> z; // Vertice 2
        faces[i].P2.Set(x,y,z);
        arq >> x >> y >> z >> std::hex >> c; // Vertice 3
        faces[i].P3.Set(x,y,z);

        faces[i].r = GetRValue(c);
        faces[i].g = GetGValue(c);
        faces[i].b = GetBValue(c);

        faces[i].r = faces[i].r / 255.0f;
        faces[i].g = faces[i].g / 255.0f;
        faces[i].b = faces[i].b / 255.0f;

       // cout << std::hex << c << endl;
       // cout << i << ": ";
        faces[i].imprime();
        // Falta ler o RGB da face....
    }
}

// Rotina que faz um produto vetorial
void ProdVetorial (TPoint v1, TPoint v2, TPoint &vresult)
    {
        vresult.X = v1.Y * v2.Z - (v1.Z * v2.Y);
        vresult.Y = v1.Z * v2.X - (v1.X * v2.Z);
        vresult.Z = v1.X * v2.Y - (v1.Y * v2.X);
    }

// Esta rotina tem como funcao calcular um vetor unitario
void VetUnitario(TPoint &vet)
    {
        float modulo;

        modulo = sqrt (vet.X * vet.X + vet.Y * vet.Y + vet.Z * vet.Z);

        if (modulo == 0.0) return;

        vet.X /= modulo;
        vet.Y /= modulo;
        vet.Z /= modulo;
    }

// **********************************************************************
// void ExibeObjeto (TTriangle **Objeto)
// **********************************************************************
void Objeto3D::ExibeObjeto ()
{
    // ***************
    // Exercicio
    //      complete esta rotina fazendo a exibicao do objeto
    // ***************
    TPoint A, B, RES;

    for(int i=0; i<nFaces; i++) {
        A.Set(faces[i].P2.X-faces[i].P1.X, faces[i].P2.Y-faces[i].P1.Y, faces[i].P2.Z-faces[i].P1.Z);
        B.Set(faces[i].P3.X-faces[i].P2.X, faces[i].P3.Y-faces[i].P2.Y, faces[i].P3.Z-faces[i].P2.Z);

        ProdVetorial(A, B, RES);
        VetUnitario(RES);

        glPushMatrix();
            glBegin(GL_TRIANGLES);
                glColor3f(faces[i].r, faces[i].g, faces[i].b);
                glNormal3f(RES.X, RES.Y, RES.Z);
                glVertex3f(faces[i].P1.X, faces[i].P1.Y, faces[i].P1.Z);
                glVertex3f(faces[i].P2.X, faces[i].P2.Y, faces[i].P2.Z);
                glVertex3f(faces[i].P3.X, faces[i].P3.Y, faces[i].P3.Z);
            glEnd();
        glPopMatrix();
    }


}

// **********************************************************************
//  void DesenhaCubo()
//
//
// **********************************************************************
void DesenhaCubo()
{
    glBegin ( GL_QUADS );
    // Front Face
    glNormal3f(0,0,1);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    // Back Face
    glNormal3f(0,0,-1);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    // Top Face
    glNormal3f(0,1,0);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    // Bottom Face
    glNormal3f(0,-1,0);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    // Right face
    glNormal3f(1,0,0);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    // Left Face
    glNormal3f(-1,0,0);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}
// **********************************************************************
//  void DefineLuz(void)
//
//
// **********************************************************************
void DefineLuz(void)
{
  // Define cores para um objeto dourado
  GLfloat LuzAmbiente[]   = {0.4, 0.4, 0.4f } ;
  GLfloat LuzDifusa[]   = {0.7, 0.7, 0.7};
  GLfloat LuzEspecular[] = {0.9f, 0.9f, 0.9 };
  GLfloat PosicaoLuz0[]  = {0.0f, 3.0f, 5.0f };  // Posição da Luz
  GLfloat Especularidade[] = {1.0f, 1.0f, 1.0f};

   // ****************  Fonte de Luz 0

 glEnable ( GL_COLOR_MATERIAL );

   // Habilita o uso de iluminação
  glEnable(GL_LIGHTING);

  // Ativa o uso da luz ambiente
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LuzAmbiente);
  // Define os parametros da luz número Zero
  glLightfv(GL_LIGHT0, GL_AMBIENT, LuzAmbiente);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LuzDifusa  );
  glLightfv(GL_LIGHT0, GL_SPECULAR, LuzEspecular  );
  glLightfv(GL_LIGHT0, GL_POSITION, PosicaoLuz0 );
  glEnable(GL_LIGHT0);

  // Ativa o "Color Tracking"
  glEnable(GL_COLOR_MATERIAL);

  // Define a reflectancia do material
  glMaterialfv(GL_FRONT,GL_SPECULAR, Especularidade);

  // Define a concentraçãoo do brilho.
  // Quanto maior o valor do Segundo parametro, mais
  // concentrado será o brilho. (Valores válidos: de 0 a 128)
  glMateriali(GL_FRONT,GL_SHININESS,51);

}


// **********************************************************************
//  void init(void)
//		Inicializa os par‚metros globais de OpenGL
//
// **********************************************************************
void init(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fundo de tela preto

	glShadeModel(GL_SMOOTH);
	glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	glEnable(GL_DEPTH_TEST);
	glEnable ( GL_CULL_FACE );

    // Obtem o tempo inicial
#ifdef WIN32
    last_idle_time = GetTickCount();
#else
    gettimeofday (&last_idle_time, NULL);
#endif

    terceirapessoa = 0;//desabilitado

    User.Set(0,1,0);
    Alvo.Set(0,0,-5);
}

//Movimentaçao Equação Paramétrica da Reta
void Movimentacao(){

    //atualiza observador
    User.X = User.X + ((Alvo.X - User.X) * t);
    User.Z = User.Z + ((Alvo.Z - User.Z) * t);

    // atualiza alvo
    Alvo.X = Alvo.X + ((Alvo.X - User.X) * t);
    Alvo.Z = Alvo.Z + ((Alvo.Z - User.Z) * t);


   /* printf("User X: %f", User.X);
    printf("   User Z: %f \n", User.Z);

    printf("Alvo X: %f", Alvo.X);
    printf("   Alvo Z: %f\n", Alvo.Z);*/
}

//Rotaciona o alvo horizontalmente
void Rotaciona(float alfa){


    alfa = alfa * (M_PI/180.0);//tranforma em radianos

    //leva o alvo para a origem
    Alvo.X = Alvo.X - User.X;
    Alvo.Y = Alvo.Y;
    Alvo.Z = Alvo.Z - User.Z;
    Ponto novoAlvo;

    //faz o calculo da rotação
    novoAlvo.X = Alvo.X * cos(alfa) + Alvo.Z * sin(alfa);
    novoAlvo.Z = (-Alvo.X) * sin(alfa) + Alvo.Z * cos(alfa);

    //retorna ao lugar do alvo original com os novos dados
    Alvo.X = novoAlvo.X + User.X;
    Alvo.Z = novoAlvo.Z + User.Z;

    /*if(terceirapessoa == 1){//3 pessoa ligada
        Alvo.X = novoAlvo.X + User.X;
        Alvo.Z = novoAlvo.Z + User.Z;
    }*/
}

//Rotaciona o alvo verticalmente
void RotacionaVert(int valor){
    float aux;
    aux = Alvo.Y + (valor);

    if((aux < 8)&&(aux > -2)){//limite de olhar para cima e para baixo
       Alvo.Y = aux;
    }
}

// **********************************************************************
//  void PosicUser()
//
//
// **********************************************************************
void PosicUser()
{
	// Define os parâmetros da projeção Perspectiva
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90,AspectRatio,0.01,200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    if (!terceirapessoa)
    {
        gluLookAt(User.X, User.Y, User.Z,   // Posição do Observador
                  Alvo.X,Alvo.Y,Alvo.Z,     // Posição do Alvo
                  0.0f,1.0f,0.0f);
    }
    else {
        //cout << "Posicionando 3a..." ;
        gluLookAt(5, 5, 5,   // Posição do Observador é fixa
                  User.X, User.Y, User.Z,   // Posição do Alvo
                  0.0f,1.0f,0.0f);
    }

}

// **********************************************************************
//  void reshape( int w, int h )
//		trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{

	// Evita divisão por zero, no caso de uam janela com largura 0.
	if(h == 0) h = 1;
    // Ajusta a relação entre largura e altura para evitar distorção na imagem.
    // Veja função "PosicUser".
	AspectRatio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Seta a viewport para ocupar toda a janela
    glViewport(0, 0, w, h);
    //cout << "Largura" << w << endl;

	PosicUser();

}

// **********************************************************************
//  void display( void )
//
//
// **********************************************************************
void display( void )
{

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	DefineLuz();

    PosicUser();

	glMatrixMode(GL_MODELVIEW);
//cenário
//   CNRA        LB1
//       ALVO
//       OBS
//
//   LB2        ARV

    //Lobo3
	glPushMatrix();
		glTranslatef ( 5.0f, 0.0f, 5.0f );
        glScalef(0.4f, 0.4f, 0.4f);
        glRotatef(220,0,1,0);
		MundoVirtual[1].ExibeObjeto();
	glPopMatrix();

	//Lobo 2
	glPushMatrix();
		glTranslatef ( -5.0f, 0.0f, -5.0f );
        glScalef(0.02f, 0.02f, 0.02f);
		glRotatef(45,0,1,0);
		MundoVirtual[2].ExibeObjeto();
	glPopMatrix();

	//Cenoura
	glPushMatrix();
		glTranslatef ( -5.0f, 0.0f, 5.0f );
		glScalef(0.01f, 0.01f, 0.01f);
		glRotatef(0,0,1,0);
		MundoVirtual[3].ExibeObjeto();
	glPopMatrix();

	//Árvore
	glPushMatrix();
		glTranslatef ( 5.0f, 0.0f, -5.0f );
		glScalef(1.2f, 1.2f, 1.2f);
		glRotatef(0,0,1,0);
		MundoVirtual[4].ExibeObjeto();
	glPopMatrix();

	if(terceirapessoa == 1){
        //Jogador 3 pessoa
        // Coelho
        glPushMatrix();
            glTranslatef ( User.X, User.Y, User.Z );
            glScalef(0.05f, 0.05f, 0.05f);
            glRotatef(AngY,0,1,0);
            glColor3f(0.5f,0.5f,0.7f);
            MundoVirtual[0].ExibeObjeto();
            //DesenhaCubo();
        glPopMatrix();
	}

	 // Exibicao do objeto lido de arquivo
   // glPushMatrix();
     //   glTranslatef ( 2.0f, 0.0f, -20.0f );
       // glRotatef(65,0,0,1);
        //glRotatef(AngY,1,0,0);
        //MundoVirtual[0].ExibeObjeto();
    //glPopMatrix();

	glutSwapBuffers();
}

// **********************************************************************
//  void animate ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void animate()
{
    static float dt;
    static float AccumTime=0;

#ifdef _WIN32
    DWORD time_now;
    time_now = GetTickCount();
    dt = (float) (time_now - last_idle_time) / 1000.0;
#else
    // Figure out time elapsed since last call to idle function
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) +
    1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec);
#endif
    AccumTime +=dt;
    if (AccumTime >=3) // imprime o FPS a cada 3 segundos
    {
        cout << 1.0/dt << " FPS"<< endl;
        AccumTime = 0;
    }
    //cout << "AccumTime: " << AccumTime << endl;
    // Anima cubos
    //AngY++;

    // Sa;va o tempo para o próximo ciclo de rendering
    last_idle_time = time_now;

        //if  (GetAsyncKeyState(32) & 0x8000) != 0)
          //  cout << "Espaco Pressionado" << endl;

    // Redesenha
    glutPostRedisplay();
}

// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{
	switch ( key )
	{
    case 27:        // Termina o programa qdo
      exit ( 0 );   // a tecla ESC for pressionada
      break;
    case 32: //Tecla de espaço
      Movimentacao();
      break;
    case 'p': //Tecla P
    case 'P': //Tecla P
            terceirapessoa = !terceirapessoa;
      break;
    default:
            cout << key;
      break;
  }
}

// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
		case GLUT_KEY_UP:       // When Up Arrow Is Pressed...
			//glutFullScreen ( ); // Go Into Full Screen Mode
			RotacionaVert(2);
			break;
	    case GLUT_KEY_DOWN:     // When Down Arrow Is Pressed...
			//glutInitWindowSize  ( 700, 500 );
			RotacionaVert(-2);
			break;
        case GLUT_KEY_LEFT:
            Rotaciona(5);
            AngY = AngY + 5;//rotaciona o objeto
            break;
            case GLUT_KEY_RIGHT:
            Rotaciona(-5);
            AngY = AngY - 5;
            break;
		default:
			break;
	}
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
//
// **********************************************************************
int main ( int argc, char** argv )
{
	glutInit            ( &argc, argv );
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
	glutInitWindowPosition (0,0);
	glutInitWindowSize  ( 700, 500 );
	glutCreateWindow    ( "Computacao Grafica - Exemplo Basico 3D" );

	init ();
    //system("pwd");

	glutDisplayFunc ( display );
	glutReshapeFunc ( reshape );
	glutKeyboardFunc ( keyboard );
	glutSpecialFunc ( arrow_keys );
	glutIdleFunc ( animate );


	 // Le o obejto do arquivo
    char Nome[] = "rabbit.tri";
    char Nome2[] = "dog.tri";
    char Nome3[] = "Wolf.tri";
    char Nome4[] = "carrot.tri";
    char Nome5[] = "tree.tri";

	// aloca mem—ria para 5 objetos
    MundoVirtual = new Objeto3D[5];
    // carrega o objeto 0
    MundoVirtual[0].LeObjeto (Nome);
    MundoVirtual[1].LeObjeto (Nome2);
    MundoVirtual[2].LeObjeto (Nome3);
    MundoVirtual[3].LeObjeto (Nome4);
    MundoVirtual[4].LeObjeto (Nome5);

	glutMainLoop ( );
	return 0;
}


