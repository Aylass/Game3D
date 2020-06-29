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
float t = 0.05; //usado no calculo de movimentacao

int terceirapessoa; //se o jogo esta em 3 pessoa ou não

GLfloat AspectRatio, AngY=0, AngYLobo= 0, Anima = 1;
int NObjetos = 6;

class Ponto  // Struct para armazenar um ponto
{
    public : float X,Y,Z;
    public : int gl,gc;
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

typedef struct  // Struct para armazenar as áreas do mapa
{
    //   p1        p2
    //
    //       P5
    //
    //   p3        p4
    Ponto p1;
    Ponto p2;
    Ponto p3;
    Ponto p4;
    Ponto p5;
    float r;
    float g;
    float b;
    int cor;

    void setCor(int cor) {
        this->cor = cor;
        switch (cor) {
           case 0: r = 0.0f;  // VERDE
               g =  255.0f;
               b = 0.0f;
               break;

          case  1: r = 255.0f; // AMARELO
               g =  255.0f;
               b = 0.0f;
               break;

          case  2: r = 0.0f; // AZUL
               g =  0.0f;
               b = 255.0f;
               break;

          case  3: r = 205.0f; // MARROM
               g =  133.0f;
               b = 63.0f;
               break;
        }
    }

} Area;
typedef struct  // Struct para armazenar as localizações das cenouras
{
    int comeu; //1 se a cenora foi comida / 0 se a cenoura não foi comida
    //posicao da cenoura
    //o HitBox é 1 posição ao redor
    double X;
    double Y;
    double Z;
} Cenoura;

typedef struct  // Struct para armazenar os objetos 3D  Árvores, Lobos
{
    int tipo; //guarda a posição do objeto no MundoVirtual
    //posicao
    Ponto eixo;

    Ponto scale;//tamanho dos objetos

    Ponto alvo;//usado somente para os lobos
    //posição do alvo no grid
    int gx;
    int gz;
    Ponto HitBox[3][3];//alvo = 0 1
                    //eixo = 1 1

    int andando;//0 se estver andando no eixo X(cima) 1 se estiver andando no eixo Z(lado)

} Objeto;

Ponto User, Alvo;//usado em 1 pessoa
int Vidas;//contador de vidas do coelho

int posX3Pessoa, posZ3Pessoa;//posicionamento da 3 pessoa
//o user em 3 pessoa é fixo, e o alvo acompanha o user da 1 pessoa


Cenoura Cenouras[4];
Objeto Arvores[30];//guarda a posição de todas as árvores

Objeto Lobos[10];//guarda a posição de todos os lobos

Area Grid[50][50];
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
    TPoint P1, P2, P3, res;
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
    TPoint A, B, RES;

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

        A.Set(faces[i].P2.X-faces[i].P1.X, faces[i].P2.Y-faces[i].P1.Y, faces[i].P2.Z-faces[i].P1.Z);
        B.Set(faces[i].P3.X-faces[i].P2.X, faces[i].P3.Y-faces[i].P2.Y, faces[i].P3.Z-faces[i].P2.Z);

        ProdVetorial(A, B, RES);
        VetUnitario(RES);
        faces[i].res = RES;



       // cout << std::hex << c << endl;
       // cout << i << ": ";
        // Falta ler o RGB da face....
    }
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

    for(int i=0; i<nFaces; i++) {
        glPushMatrix();
            glBegin(GL_TRIANGLES);
                glColor3f(faces[i].r, faces[i].g, faces[i].b);
                glNormal3f(faces[i].res.X, faces[i].res.X, faces[i].res.X);
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
void DesenhaPiso()
{
    glBegin ( GL_QUADS );
    glNormal3f(0,1,0);
    glVertex3f(-0.5f,  0.0f, -0.5f);
    glVertex3f(-0.5f,  0.0f,  0.5f);
    glVertex3f( 0.5f,  0.0f,  0.5f);
    glVertex3f( 0.5f,  0.0f, -0.5f);
    glEnd();

}

void PintaMapa(float Y) {
    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {
            glPushMatrix();
                glTranslatef(Grid[i][j].p5.X, Y, Grid[i][j].p5.Z);
                glColor3f(Grid[i][j].r, Grid[i][j].g, Grid[i][j].b);
                DesenhaPiso();
            glPopMatrix();
        }
    }
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

void LeMapa(const char *nome) {
    ifstream mapa;
    mapa.open(nome, ios::in);
    int cor;

     for(int c = 0; c < 50;c++){//colunas
        for(int l = 0; l < 50;l++){//linhas
            Grid[c][l].p1.Set(c,0,l+1);
            Grid[c][l].p2.Set(c+2,0,l+2);
            Grid[c][l].p3.Set(c,0,l);
            Grid[c][l].p4.Set(c+2,0,l);
            Grid[c][l].p5.Set(c+1,0,l+1);

            mapa >> cor;
            Grid[c][l].setCor(cor);
        }
    }
}
// **********************************************************************
//
//		CENÁRIO FLORESTA
//
// **********************************************************************
void ColocaFloresta(){
    Objeto arvore;
    //tiço de arvore: 5
    arvore.tipo = 4;
    arvore.eixo.X = 13;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 8;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[0] = arvore;

    arvore.tipo = 4;
    arvore.eixo.X = 15;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 18;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[1] = arvore;

    arvore.tipo = 4;
    arvore.eixo.X = 18;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 26;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[2] = arvore;

    arvore.tipo = 4;
    arvore.eixo.X = 23;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 33;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[3] = arvore;

    arvore.tipo = 4;
    arvore.eixo.X = 28;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 44;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[4] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 48;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 45;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[5] = arvore;

    arvore.tipo = 4;
    arvore.eixo.X = 48;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 12;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[6] = arvore;

    arvore.tipo = 4;
    arvore.eixo.X = 46;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 5;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[7] = arvore;

    arvore.tipo = 4;
    arvore.eixo.X = 34;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 7;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[8] = arvore;

    arvore.tipo = 4;
    arvore.eixo.X = 37;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 2;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[9] = arvore;

    //Arbustos

    arvore.tipo = 5;
    arvore.eixo.X = 25;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 44;
    arvore.scale.X = 0.3;
    arvore.scale.Y = 0.3;
    arvore.scale.Z = 0.3;
    Arvores[10] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 29;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 44;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[11] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 26;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 43;
    arvore.scale.X = 0.5;
    arvore.scale.Y = 0.5;
    arvore.scale.Z = 0.5;
    Arvores[12] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 21;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 34;
    arvore.scale.X = 0.2;
    arvore.scale.Y = 0.2;
    arvore.scale.Z = 0.2;
    Arvores[13] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 48;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 40;
    arvore.scale.X = 0.4;
    arvore.scale.Y = 0.4;
    arvore.scale.Z = 0.4;
    Arvores[14] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 49;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 31;
    arvore.scale.X = 0.2;
    arvore.scale.Y = 0.2;
    arvore.scale.Z = 0.2;
    Arvores[15] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 40;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 35;
    arvore.scale.X = 0.8;
    arvore.scale.Y = 0.8;
    arvore.scale.Z = 0.8;
    Arvores[16] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 25;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 43;
    arvore.scale.X = 0.3;
    arvore.scale.Y = 0.3;
    arvore.scale.Z = 0.3;
    Arvores[17] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 21;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 34;
    arvore.scale.X = 0.3;
    arvore.scale.Y = 0.3;
    arvore.scale.Z = 0.3;
    Arvores[18] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 31;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 26;
    arvore.scale.X = 0.7;
    arvore.scale.Y = 0.7;
    arvore.scale.Z = 0.7;
    Arvores[19] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 14;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 26;
    arvore.scale.X = 0.7;
    arvore.scale.Y = 0.7;
    arvore.scale.Z = 0.7;
    Arvores[20] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 16;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 16;
    arvore.scale.X = 1.2;
    arvore.scale.Y = 1.2;
    arvore.scale.Z = 1.2;
    Arvores[21] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 21;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 11;
    arvore.scale.X = 0.2;
    arvore.scale.Y = 0.2;
    arvore.scale.Z = 0.2;
    Arvores[22] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 25;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 3;
    arvore.scale.X = 0.2;
    arvore.scale.Y = 0.2;
    arvore.scale.Z = 0.2;
    Arvores[23] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 31;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 17;
    arvore.scale.X = 1.0;
    arvore.scale.Y = 1.0;
    arvore.scale.Z = 1.0;
    Arvores[24] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 38;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 20;
    arvore.scale.X = 0.3;
    arvore.scale.Y = 0.3;
    arvore.scale.Z = 0.3;
    Arvores[25] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 16;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 16;
    arvore.scale.X = 0.3;
    arvore.scale.Y = 0.3;
    arvore.scale.Z = 0.3;
    Arvores[26] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 21;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 10;
    arvore.scale.X = 0.8;
    arvore.scale.Y = 0.8;
    arvore.scale.Z = 0.8;
    Arvores[27] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 33;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 7;
    arvore.scale.X = 0.8;
    arvore.scale.Y = 0.8;
    arvore.scale.Z = 0.8;
    Arvores[28] = arvore;

    arvore.tipo = 5;
    arvore.eixo.X = 45;
    arvore.eixo.Y = 0;
    arvore.eixo.Z = 10;
    arvore.scale.X = 1.0;
    arvore.scale.Y = 1.0;
    arvore.scale.Z = 1.0;
    Arvores[29] = arvore;
}

// **********************************************************************
//
//		LOBOS
//
// **********************************************************************
void AtualizaHitBox(Objeto *lobo){
    lobo->HitBox[0][0].X = lobo->HitBox[1][1].X -1;
    lobo->HitBox[0][0].Z = lobo->HitBox[1][1].X -1;

    lobo->HitBox[1][0].X = lobo->HitBox[1][1].X -1;
    lobo->HitBox[1][0].Z = lobo->HitBox[1][1].Z;

    lobo->HitBox[2][0].X = lobo->HitBox[1][1].X +1;
    lobo->HitBox[2][0].Z = lobo->HitBox[1][1].Z-1;

    lobo->HitBox[0][1].X = lobo->HitBox[1][1].X -1;
    lobo->HitBox[0][1].Z = lobo->HitBox[1][1].Z;

    lobo->HitBox[2][1].X = lobo->HitBox[1][1].X +1;
    lobo->HitBox[2][1].Z = lobo->HitBox[1][1].Z;

    lobo->HitBox[0][2].X = lobo->HitBox[1][1].X -1;
    lobo->HitBox[0][2].Z = lobo->HitBox[1][1].Z+1;

    lobo->HitBox[1][2].X = lobo->HitBox[1][1].X ;
    lobo->HitBox[1][2].Z = lobo->HitBox[1][1].Z+1;

    lobo->HitBox[2][2].X = lobo->HitBox[1][1].X+1;
    lobo->HitBox[2][2].Z = lobo->HitBox[1][1].Z+1;


    printf("(%f %f) (%f %f) (%f %f)",lobo->HitBox[2][0].X,lobo->HitBox[2][0].Z,lobo->HitBox[1][0].X,lobo->HitBox[1][0].Z,lobo->HitBox[0][0].X,lobo->HitBox[0][0].Z);

}

void ColocaLobos(){
    Objeto lobo;
    //tiços de lobo: 1, 2
    lobo.tipo = 1;
    lobo.eixo.X = 5;
    lobo.eixo.Y = 0;
    lobo.eixo.Z = 22;
    lobo.alvo.X = 8;//+3
    lobo.alvo.Y = 0;
    lobo.alvo.Z = 22;
    lobo.gz = lobo.eixo.Z;
    lobo.gx = lobo.eixo.X;
    lobo.andando = 0;//começa andando no eixo X

    Lobos[0] = lobo;

}

void posloboGrid(Objeto *lobo){
    //   p1        p2
    //
    //      p5
    //
    //   p3        p4
for(int c = 0; c < 50;c++){//colunas
        for(int l = 0; l < 50;l++){//linhas
                //MUDAR AQUI ----------------------------------------------------
                if(((lobo->eixo.X >= Grid[c][l].p1.X)&&(lobo->eixo.X <= Grid[c][l].p2.X))&&((lobo->eixo.Z <= Grid[c][l].p1.Z)&&(lobo->eixo.Z >= Grid[c][l].p3.Z))){ //lob se encontra nesa area
                        lobo->gx = c;
                        lobo->gz = l;
                }
        }
}
}

int DirEsqLobos(Objeto *lobo){//fala para o lobo se deve seguir para direita ou esquerda e em caso de poder seguir nos dois escolhe pelo produtovetorial
    posloboGrid(lobo);
    int aux = 4;

    if(lobo->andando == 0){//andando no eixo X para BAIXO
            printf("LOBO ANDANDO X BAIXO\n");
            printf("\n\n esquerda--------   %d\n\n",Grid[lobo->gx][lobo->gz-aux].cor);
            printf("\n\n direita--------   %d\n\n" ,Grid[lobo->gx][lobo->gz+aux].cor);

        if((Grid[lobo->gx][lobo->gz-aux].cor == 1)&&(Grid[lobo->gx][lobo->gz+aux].cor == 1)){
            printf("ambos X\n");
                TPoint vetlobo;
                vetlobo.Set(lobo->eixo.X - lobo->alvo.X, lobo->eixo.Y - lobo->alvo.Y, lobo->eixo.Z - lobo->alvo.Z);

                TPoint vetcoelho;
                vetcoelho.Set(User.X-lobo->eixo.X,User.Y-lobo->eixo.Y,User.Z-lobo->eixo.Z);

                TPoint valor;
                ProdVetorial(vetlobo,vetcoelho,valor);


                if(valor.Z < 0){//direita
                    printf("negativo-Direita\n");
                    lobo->andando = 3;
                    return 3;
                }else{//esquerda
                     printf("positivo-Esquerda\n");
                     lobo->andando = 2;
                     return 1;
                }
        }
        if(Grid[lobo->gx][lobo->gz-aux].cor == 1){//esquerda
                printf("esquerda X B\n");
                lobo->andando = 2;
                return 1;
        }
        if(Grid[lobo->gx][lobo->gz+aux].cor == 1){//direita
                printf("direita X B\n");
                lobo->andando = 3;
                return 3;
        }
    }

 //---------------------------------------------------------------------------------
    if(lobo->andando == 1){//andando no eixo X para CIMA
            printf("LOBO ANDANDO X CIMA\n");
            printf("\n\n direita--------   %d\n\n",Grid[lobo->gx][lobo->gz-aux].cor);
            printf("\n\n esquerda--------   %d\n\n" ,Grid[lobo->gx][lobo->gz+aux].cor);

        if((Grid[lobo->gx][lobo->gz+aux].cor == 1)&&(Grid[lobo->gx][lobo->gz-aux].cor == 1)){
            printf("ambos X\n");
                TPoint vetlobo;
                vetlobo.Set(lobo->eixo.X - lobo->alvo.X, lobo->eixo.Y - lobo->alvo.Y, lobo->eixo.Z - lobo->alvo.Z);

                TPoint vetcoelho;
                vetcoelho.Set(User.X-lobo->eixo.X,User.Y-lobo->eixo.Y,User.Z-lobo->eixo.Z);

                TPoint valor;
                ProdVetorial(vetlobo,vetcoelho,valor);


                if(valor.Z > 0){//direita
                    printf("negativo Direita\n");
                    lobo->andando = 2;
                    return 3;
                }else{//esquerda
                     printf("positivo Esquerda\n");
                     lobo->andando = 3;
                     return 1;
                }
        }
        if(Grid[lobo->gx][lobo->gz+aux].cor == 1){//esquerda
                printf("esquerda X C\n");
                lobo->andando = 3;
                return 1;
        }
        if(Grid[lobo->gx][lobo->gz-aux].cor == 1){//direita
                printf("direita X C\n");
                lobo->andando = 2;
                return 3;
        }
    }

     //---------------------------------------------------------------------------------
    if(lobo->andando == 2){//andando no eixo Z para MENOR
            printf("LOBO ANDANDO Z MENOR\n");
            printf("\n\n esquerda--------   %d\n\n",Grid[lobo->gx-aux][lobo->gz].cor);
            printf("\n\n direita--------   %d\n\n" ,Grid[lobo->gx+aux][lobo->gz].cor);

        if((Grid[lobo->gx-aux][lobo->gz].cor == 1)&&(Grid[lobo->gx+aux][lobo->gz].cor == 1)){
            printf("ambos X\n");
                TPoint vetlobo;
                vetlobo.Set(lobo->eixo.X - lobo->alvo.X, lobo->eixo.Y - lobo->alvo.Y, lobo->eixo.Z - lobo->alvo.Z);

                TPoint vetcoelho;
                vetcoelho.Set(User.X-lobo->eixo.X,User.Y-lobo->eixo.Y,User.Z-lobo->eixo.Z);

                TPoint valor;
                ProdVetorial(vetlobo,vetcoelho,valor);


                if(valor.Z < 0){//direita
                    printf("negativo Direita\n");
                    lobo->andando = 0;
                    return 3;
                }else{//esquerda
                     printf("positivo Esquerda\n");
                     lobo->andando = 1;
                     return 1;
                }
        }
        if(Grid[lobo->gx-aux][lobo->gz].cor == 1){//esquerda
                printf("esquerda Z ME\n");
                lobo->andando = 1;
                return 1;
        }
        if(Grid[lobo->gx+aux][lobo->gz].cor == 1){//direita
                printf("direita Z ME\n");
                lobo->andando = 0;
                return 3;
        }
    }

        //---------------------------------------------------------------------------------
     if(lobo->andando == 3){//andando no eixo Z para MAIOR
            printf("LOBO ANDANDO Z MAIOR\n");
            printf("\n\n esquerda--------   %d\n\n",Grid[lobo->gx-aux][lobo->gz].cor);
            printf("\n\n direita--------   %d\n\n" ,Grid[lobo->gx+aux][lobo->gz].cor);


        if((Grid[lobo->gx+aux][lobo->gz].cor == 1)&&(Grid[lobo->gx-aux][lobo->gz].cor == 1)){
            printf("ambos X\n");
                TPoint vetlobo;
                vetlobo.Set(lobo->eixo.X - lobo->alvo.X, lobo->eixo.Y - lobo->alvo.Y, lobo->eixo.Z - lobo->alvo.Z);

                TPoint vetcoelho;
                vetcoelho.Set(User.X-lobo->eixo.X,User.Y-lobo->eixo.Y,User.Z-lobo->eixo.Z);

                TPoint valor;
                ProdVetorial(vetlobo,vetcoelho,valor);


                if(valor.Z > 0){//direita
                    printf("negativo Direita\n");
                    lobo->andando = 1;
                    return 3;
                }else{//esquerda
                     printf("positivo Esquerda\n");
                     lobo->andando = 0;
                     return 1;
                }
        }
        if(Grid[lobo->gx+aux][lobo->gz].cor == 1){//esquerda
                printf("esquerda Z MA\n");
                lobo->andando = 0;
                return 1;
        }
        if(Grid[lobo->gx-aux][lobo->gz].cor == 1){//direita
                printf("direita Z MA\n");
                lobo->andando = 1;
                return 3;
        }
    }
    //é encrusilhada
     if(lobo->andando == 3){//andando no eixo Z para MAIOR
        lobo->andando = 2;
    }if(lobo->andando == 2){//andando no eixo Z para MAIOR
        lobo->andando = 3;
    }if(lobo->andando == 1){//andando no eixo Z para MAIOR
        lobo->andando = 0;
    }if(lobo->andando == 0){//andando no eixo Z para MAIOR
        lobo->andando = 1;
    }
    return 0;
}

void ColisaoLoboCoelho(Objeto *lobo){
        for(int i = 0; i<1;i++){//percorre todos os lobos
        if(((User.X >= Lobos[i].eixo.X-1)&&(Lobos[i].eixo.X+1))&&((User.Z <= Lobos[i].eixo.Z+1)&&(User.Z >= Lobos[i].eixo.Z-1))){
            //entro em colisao com a cenoura
            Vidas--;
            printf("Vidas: %d",Vidas);
        }
    }
}

//Detecta colisao com os caminhos que os lobos podem andar
int ColisaoEstradaLobos(Objeto *lobo){
    //testa colisao com fora do mapa
     //   p1        p2
    //
    //      CENTRO
    //
    //   p3>=        <=p4
    int res = 0;
    for(int c = 0; c < 50;c++){//colunas
        for(int l = 0; l < 50;l++){//linhas
                //MUDAR AQUI ----------------------------------------------------
            if(Grid[c][l].cor == 1){ //== cor da estrada
                if(((lobo->alvo.X >= Grid[c][l].p1.X)&&(lobo->alvo.X <= Grid[c][l].p2.X))&&((lobo->alvo.Z <= Grid[c][l].p1.Z)&&(lobo->alvo.Z >= Grid[c][l].p3.Z))){
                    res = 1;
                }
            }
        }
        //if((objeto.x>=p1.x && objeto.x<=p2.x) && (objeto.z<=p1.z && objeto.z>=p3)) )
    }
   return res;//ta fora do mapa
}

void RotacionaLobos(float alfa,Objeto *lobo){
    if(alfa >0){AngYLobo = AngYLobo + 90;}else{AngYLobo = AngYLobo - 90;}//rotaciona o objeto visualmente

    alfa = alfa * (M_PI/180.0);//tranforma em radianos

//    cout << "1º:" << lobo.alvo.X << "," << lobo.alvo.Z << endl;
    //leva o alvo para a origem
    lobo->alvo.X = lobo->alvo.X - lobo->eixo.X;
    lobo->alvo.Y = lobo->alvo.Y;
    lobo->alvo.Z = lobo->alvo.Z - lobo->eixo.Z;
    Ponto novoAlvo;
   // cout << "2º:" << lobo.alvo.X << "," << lobo.alvo.Z << endl;

    //faz o calculo da rotação
    novoAlvo.X = lobo->alvo.X * cos(alfa) + lobo->alvo.Z * sin(alfa);
    novoAlvo.Z = (-lobo->alvo.X) * sin(alfa) + lobo->alvo.Z * cos(alfa);

    //retorna ao lugar do alvo original com os novos dados
    lobo->alvo.X = novoAlvo.X + lobo->eixo.X;
    lobo->alvo.Z = novoAlvo.Z + lobo->eixo.Z;
   // cout << "3º:" << lobo.alvo.X << "," << lobo.alvo.Z << endl;
}

float angulo;
//LOBOS
//Movimentaçao Equação Paramétrica da Reta
void MovimentacaoLobos(){

    for(int i = 0; i<1;i++){//para cada lobo
        Ponto loboauxEixo = Lobos[i].eixo, loboauxAlvo = Lobos[i].alvo;
        //atualiza observador
        Lobos[i].eixo.X = Lobos[i].eixo.X + ((Lobos[i].alvo.X - Lobos[i].eixo.X) * 0.01);
        Lobos[i].eixo.Z = Lobos[i].eixo.Z + ((Lobos[i].alvo.Z - Lobos[i].eixo.Z) * 0.01);

        //cout << "Antes:"<< Lobos[i].alvo.X << ","<< Lobos[i].alvo.Y << "," << Lobos[i].alvo.Z << endl;
        // atualiza alvo
        Lobos[i].alvo.X = Lobos[i].alvo.X + ((Lobos[i].alvo.X - Lobos[i].eixo.X) * 0.01);
        Lobos[i].alvo.Z = Lobos[i].alvo.Z + ((Lobos[i].alvo.Z - Lobos[i].eixo.Z) * 0.01);
        //cout << "Depois:"<< Lobos[i].alvo.X << "," << Lobos[i].alvo.Y << "," << Lobos[i].alvo.Z << endl;


        int colisao = ColisaoEstradaLobos(&Lobos[i]);
        if(colisao == 0){//existe colisao
            Lobos[i].eixo = loboauxEixo;
            Lobos[i].alvo = loboauxAlvo;

            int direcao = DirEsqLobos(&Lobos[i]);
            //int direcao = QualLado(&Lobos[i]);

            if(direcao == 0){//nao tem pra onde ir
                printf("encrusilhada");
                angulo = 180;
            }
             if(direcao == 1){//esquerda
                angulo = 90;
            }else if(direcao == 3){//direita
                angulo = -90;
            }
          //  cout << "Antes:"<< Lobos[i].alvo.X << ","<< Lobos[i].alvo.Y << "," << Lobos[i].alvo.Z << endl;
            RotacionaLobos(angulo, &Lobos[i]);
           // cout << "Depois:"<< Lobos[i].alvo.X << "," << Lobos[i].alvo.Y << "," << Lobos[i].alvo.Z << endl;
        }
    }

}

//Coloca as 4 cenoras no mapa
void ColocaCenouras(){

    //   p1        p2
    //
    //       P5
    //
    //   p3        p4
    //Percorre o vetor de cenouras
    //Cenoura 01
        Cenoura c;
        c.comeu = 0;
        c.X = 7;
        c.Y = 1;
        c.Z = 4;
        Cenouras[0] = c;

        //Cenoura 02
        c.comeu = 0;//cenoura cmeça comida para testes
        c.X = 30;
        c.Y = 1;
        c.Z = 32;
        Cenouras[1] = c;

        //Cenoura 03
        c.comeu = 0;//cenoura cmeça comida para testes
        c.X = 48;
        c.Y = 1;
        c.Z = 48;
        Cenouras[2] = c;

        //Cenoura 04
        c.comeu = 0;//cenoura cmeça comida para testes
        c.X = 40;
        c.Y = 1;
        c.Z = 4;
        Cenouras[3] = c;
}

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

    User.Set(5,1,5);
    Alvo.Set(1,1,-6);
    //   p1        p2
    //
    //      p5
    //
    //   p3        p4
    LeMapa("Mapa.txt");

    Vidas = 5;//quantidade de vidas do coelho

    ColocaCenouras();
    ColocaLobos();
    //ColocaFloresta();
}

void ContaVidas(){ //Detecta se as vidas do coelho zeraram
    if(Vidas <=0){
        printf("\n\t /)__/)\t(-------)\n\t(>x.x<)\t| R.I.P |\n\t(')_(')\t|	|\n\n");//ok
        printf(" Yummy.. Yummy ...\n Os Lobinhos encheram a barriga! ....\n ");
        //exit ( 0 );
    }
}

void ContaCenouras(){//Detecta se todas as cenouras foram comidas
    int cont = 0;
    for(int i = 0; i<4;i++){//percorre as cenouras
        if(Cenouras[i].comeu == 1){//se ela foi comida
            cont++;
        }
    }
    if(cont == 4){//comeu todas as cenouras
        printf("\n\t /)__/)\n\t(>'.'<)\n\t(')_(')\n\n");//ok
        printf(" Bolinha de Neve encheu sua barriga! ...\n e os lobinhos passaram fome....\n ");
        exit ( 0 );
    }
}

//Detecta colizão do coelho com as cenouras
void ColisaoCenoura(){
    for(int i = 0; i<4;i++){//percorre as cenouras
        if(((User.X >= Cenouras[i].X-1)&&(User.X <= Cenouras[i].X+1))&&((User.Z <= Cenouras[i].Z+2)&&(User.Z >= Cenouras[i].Z-1))){
            //entro em colisao com a cenoura
            Cenouras[i].comeu = 1;
        }
    }
}

//Detecta colisao com os caminhos que o usuário pode andar
int ColisaoEstrada(){
    //testa colisao com fora do mapa
     //   p1        p2
    //
    //      CENTRO
    //
    //   p3>=        <=p4
    for(int c = 0; c < 50;c++){//colunas
        for(int l = 0; l < 50;l++){//linhas
                //MUDAR AQUI ----------------------------------------------------
            if(Grid[c][l].cor == 1){ //== cor da estrada
                if(((User.X >= Grid[c][l].p1.X)&&(User.X <= Grid[c][l].p2.X))&&((User.Z <= Grid[c][l].p1.Z)&&(User.Z >= Grid[c][l].p3.Z))){//ta dentro dessa área
                    User.gc = c;
                    User.gl = l;
                    return 1;
                }
            }
        }
        //if((objeto.x>=p1.x && objeto.x<=p2.x) && (objeto.z<=p1.z && objeto.z>=p3)) )
    }
   return 0;//ta fora do mapa
}


//Movimentaçao Equação Paramétrica da Reta
void Movimentacao(){

    Ponto useraux = User, alvoaux = Alvo;
    //atualiza observador
    User.X = User.X + ((Alvo.X - User.X) * t);
    User.Z = User.Z + ((Alvo.Z - User.Z) * t);

    // atualiza alvo
    Alvo.X = Alvo.X + ((Alvo.X - User.X) * t);
    Alvo.Z = Alvo.Z + ((Alvo.Z - User.Z) * t);

    int colisao = ColisaoEstrada();
    if(colisao == 0){//existe colisao
        User = useraux;
        Alvo = alvoaux;
    }

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


	if((User.X <= 25)&&(User.Z <= 25)){
        posX3Pessoa = 13;
        posZ3Pessoa = 8;
	}
	if((User.X > 25)&&(User.Z <= 25)){
        posX3Pessoa = 43;
        posZ3Pessoa = 13;
	}
	if((User.X <= 25)&&(User.Z > 25)){
        posX3Pessoa = 8;
        posZ3Pessoa = 40;
	}
	if((User.X > 25)&&(User.Z > 25)){
        posX3Pessoa = 43;
        posZ3Pessoa = 41;
	}
    if (!terceirapessoa)
    {
        gluLookAt(User.X, User.Y, User.Z,   // Posição do Observador
                  Alvo.X,Alvo.Y,Alvo.Z,     // Posição do Alvo
                  0.0f,1.0f,0.0f);
    }
    else {
        //cout << "Posicionando 3a..." ;
        gluLookAt(posX3Pessoa, 5, posZ3Pessoa,   // Posição do Observador é fixa
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
    ContaVidas();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	DefineLuz();

    PosicUser();

    ColisaoCenoura();

    ContaCenouras();


	glMatrixMode(GL_MODELVIEW);
//cenário
//   CNRA        LB1
//       ALVO
//       OBS
//
//   LB2        ARV

    //MovimentacaoLobos();

   for(int i = 0; i<1;i++){//tamanho do vetor que guarda a posição dos lobos
        //Lobo3 Azul
        glPushMatrix();
            glTranslatef ( Lobos[i].eixo.X, 0.0f, Lobos[i].eixo.Z );
            glScalef(0.4f, 0.4f, 0.4f);
            glRotatef(AngYLobo-280,0,1,0);
            MundoVirtual[Lobos[i].tipo].ExibeObjeto();
        glPopMatrix();
   }


	//Lobo 2 Vermelho
	glPushMatrix();
		glTranslatef ( 10.0f, 0.0f, 10.0f );
        glScalef(0.02f, 0.02f, 0.02f);
		glRotatef(45,0,1,0);
		MundoVirtual[2].ExibeObjeto();
	glPopMatrix();


	/*for(int i = 0; i<30;i++){//tamanho do vetor que guarda a posição das árvores e arbustos
        //Árvore
        glPushMatrix();
            glTranslatef ( Arvores[i].eixo.X, Arvores[i].eixo.Y, Arvores[i].eixo.Z );
            glScalef(Arvores[i].scale.X,Arvores[i].scale.Y, Arvores[i].scale.Z);
            glRotatef(0,0,1,0);
            MundoVirtual[Arvores[i].tipo].ExibeObjeto();
        glPopMatrix();
	}*/


    //Cenoura
    for(int i = 0; i<4;i++){
            if(Cenouras[i].comeu == 0){//cenoura não foi comida, entao desenha ela
                glPushMatrix();
                    glTranslatef ( Cenouras[i].X, Cenouras[i].Y, Cenouras[i].Z );
                    glScalef(1.0f, 0.5f, 1.0f);
                    glRotatef(Anima,0,1,0);
                    MundoVirtual[3].ExibeObjeto();
                glPopMatrix();
            }
    }

	if(terceirapessoa == 1){
        //Jogador 3 pessoa
        // Coelho
        glPushMatrix();
            glTranslatef ( User.X, 0, User.Z );
            glScalef(0.05f, 0.05f, 0.05f);
            glRotatef(AngY-180,0,1,0);
           // glColor3f(0.5f,0.5f,0.7f);
            MundoVirtual[0].ExibeObjeto();
            //DesenhaCubo();
        glPopMatrix();
	}

        PintaMapa(0);
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
        ColisaoLoboCoelho(&Lobos[0]);
        cout << 1.0/dt << " FPS"<< endl;
        AccumTime = 0;
    }
    //cout << "AccumTime: " << AccumTime << endl;
    // Anima as cenouras
    Anima++;
    //Lobos[0].eixo.X++;

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
       case 'l': //Tecla L
            //RotacionaLobos(5,Lobos[0]);
            printf("X: %f,   Z: %f",User.X,User.Z);
      break;
      case 'g':
            ColisaoEstrada();
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

	// string Nomes[] = {"rabbit.tri", "dog.tri", "Wolf.tri", "carrot.tri", "tree2.tri", "fern.tri"};

	// aloca mem—ria para 5 objetos
    MundoVirtual = new Objeto3D[NObjetos];
    // carrega o objeto 0
    MundoVirtual[0].LeObjeto ("rabbit.tri");
    MundoVirtual[1].LeObjeto ("dog.tri");
    MundoVirtual[2].LeObjeto ("Wolf.tri");
    MundoVirtual[3].LeObjeto ("carrot.tri");
    MundoVirtual[4].LeObjeto ("tree2.tri");
    MundoVirtual[5].LeObjeto ("fern.tri");

	glutMainLoop ( );
	return 0;
}


