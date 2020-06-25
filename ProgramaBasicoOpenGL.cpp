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
    Ponto alvo;//usado somente para os lobos
    //posição do alvo no grid
    int gl;
    int gc;

    int andando;//0 se estver andando no eixo X(cima) 1 se estiver andando no eixo Z(lado)

} Objeto;

Ponto User, Alvo;//usado em 1 pessoa
int posX3Pessoa, posZ3Pessoa;//posicionamento da 3 pessoa
//o user em 3 pessoa é fixo, e o alvo acompanha o user da 1 pessoa


Cenoura Cenouras[4];
Objeto Arvores[20];//guarda a posição de todas as árvores

Objeto Lobos[20];//guarda a posição de todos os lobos

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
                glTranslatef(Grid[j][i].p5.X, Y, Grid[j][i].p5.Z);
                glColor3f(Grid[j][i].r, Grid[j][i].g, Grid[j][i].b);
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
            Grid[l][c].p1.Set(c,0,l+2);
            Grid[l][c].p2.Set(c+2,0,l+2);
            Grid[l][c].p3.Set(c,0,l);
            Grid[l][c].p4.Set(c+2,0,l);
            Grid[l][c].p5.Set(c+1,0,l+1);

            mapa >> cor;
            Grid[l][c].setCor(cor);
        }
    }
}
// **********************************************************************
//
//		LOBOS
//
// **********************************************************************
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
    lobo.gl = lobo.eixo.Z;
    lobo.gc = lobo.eixo.X;
    lobo.andando = 0;//começa andando no eixo X

    //printf("Z %d",lobo.gl);//ok
    //printf("X %d",lobo.gc);//ok


    Lobos[0] = lobo;

}

int DirEsqLobos(Objeto *lobo){//fala para o lobo se deve seguir para direita ou esquerda

    if(lobo->andando == 0){//andando no eixo X
    if((Grid[lobo->gl-5][lobo->gc].cor == 1)&&(Grid[lobo->gl+5][lobo->gc].cor == 1)){
            printf("ambos X");
            lobo->andando = 1;
        return 2;//esquerda livre e direita livres
    }else if(Grid[lobo->gl-5][lobo->gc].cor == 1){//esquerda
        printf("esquerda X");
        lobo->andando = 1;
        return 1;
    }else if(Grid[lobo->gl+5][lobo->gc].cor == 1){//direita
        printf("direita X");
        lobo->andando = 1;
        return 3;
    }
    }else{ //andando no eixo Z
    if((Grid[lobo->gl][lobo->gc-5].cor == 1)&&(Grid[lobo->gl][lobo->gc+5].cor == 1)){
            printf("ambos Z");
            lobo->andando = 0;
        return 2;//esquerda livre e direita livres
    }else if(Grid[lobo->gl][lobo->gc-5].cor == 1){//esquerda
        printf("esquerda Z");
        lobo->andando = 0;
        return 1;
    }else if(Grid[lobo->gl][lobo->gc+5].cor == 1){//direita
        printf("direita Z");
        lobo->andando = 0;
        return 3;
    }
    }
    return 0;
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
            if(Grid[l][c].cor == 1){ //== cor da estrada
                if(((lobo->eixo.X >= Grid[l][c].p1.X)&&(lobo->eixo.X <= Grid[l][c].p2.X))&&((lobo->eixo.Z <= Grid[l][c].p1.Z)&&(lobo->eixo.Z >= Grid[l][c].p3.Z))){ //lob se encontra nesa area
                        lobo->gl = l;
                        lobo->gc = c;
                }
                if(((lobo->alvo.X >= Grid[l][c].p1.X)&&(lobo->alvo.X <= Grid[l][c].p2.X))&&((lobo->alvo.Z <= Grid[l][c].p1.Z)&&(lobo->alvo.Z >= Grid[l][c].p3.Z))){
                    res = 1;
                }
            }
        }
        //if((objeto.x>=p1.x && objeto.x<=p2.x) && (objeto.z<=p1.z && objeto.z>=p3)) )
    }
   return res;//ta fora do mapa
}

void RotacionaLobos(float alfa,Objeto *lobo){
    if(alfa >0){AngYLobo = AngYLobo + alfa;}else{AngYLobo = AngYLobo -  alfa;}//rotaciona o objeto visualmente

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

int ComparaPosLoboCoelho(Objeto *lobo){//método para decidir que lado o lobo deve seguir baseado na menor distancia dele com a do coelbo
    float novoX,novoZ;

    if(lobo->andando == 1){ //andando no eixo Z
        novoX = lobo->eixo.X - User.X;

    if(novoX>=lobo->eixo.X){//ta pra direita
        return 1;
    }else{//esquerda
        return 3;
    }
    }else{//andando no eixo X
    novoZ = lobo->eixo.Z - User.Z;

    if(novoZ>=lobo->eixo.Z){//ta pra direita
        return 1;
    }else{//esquerda
        return 3;
    }
    }
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

        //Lobos[i].eixo.X++;

        int colisao = ColisaoEstradaLobos(&Lobos[i]);
        if(colisao == 0){//existe colisao
            Lobos[i].eixo = loboauxEixo;
            Lobos[i].alvo = loboauxAlvo;

            int direcao = DirEsqLobos(&Lobos[i]);
            //int direcao = ComparaPosLoboCoelho(&Lobos[i]);

            if(direcao == 0){//nao tem pra onde ir
                angulo = 180;
                printf("encrusilhada");
            }else if(direcao == 2){
                direcao = ComparaPosLoboCoelho(&Lobos[i]);
                printf("escolhe lado");
            }else if(direcao == 1){//esquerda
                angulo = 90;
            }else if(direcao == 3){//direita
                angulo = -90;
            }
          //  cout << "Antes:"<< Lobos[i].alvo.X << ","<< Lobos[i].alvo.Y << "," << Lobos[i].alvo.Z << endl;
            RotacionaLobos(angulo, &Lobos[i]);
           // cout << "Depois:"<< Lobos[i].alvo.X << "," << Lobos[i].alvo.Y << "," << Lobos[i].alvo.Z << endl;
            //printf("oi");
        }
    }

   /* printf("User X: %f", User.X);
    printf("   User Z: %f \n", User.Z);

    printf("Alvo X: %f", Alvo.X);
    printf("   Alvo Z: %f\n", Alvo.Z);*/
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
        c.Z = 3;
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
        c.X = 48;
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
    ColocaCenouras();
    ColocaLobos();
}

//Detecta colizão do coelho com as cenouras
ColisaoCenoura(){
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
            if(Grid[l][c].cor == 1){ //== cor da estrada
                if(((User.X >= Grid[l][c].p1.X)&&(User.X <= Grid[l][c].p2.X))&&((User.Z <= Grid[l][c].p1.Z)&&(User.Z >= Grid[l][c].p3.Z))){//ta dentro dessa área
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

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	DefineLuz();

    PosicUser();

    ColisaoCenoura();

    //RotacionaLobos(90,Lobos[0]);

	glMatrixMode(GL_MODELVIEW);
//cenário
//   CNRA        LB1
//       ALVO
//       OBS
//
//   LB2        ARV

    MovimentacaoLobos();

   for(int i = 0; i<1;i++){//tamanho do vetor que guarda a posição dos lobos
        //Lobo3 Azul
        glPushMatrix();
            glTranslatef ( Lobos[i].eixo.X, 0.0f, Lobos[i].eixo.Z );
            glScalef(0.4f, 0.4f, 0.4f);
            glRotatef(AngYLobo-280,0,1,0);
            MundoVirtual[1].ExibeObjeto();
        glPopMatrix();
   }


	//Lobo 2 Vermelho
	glPushMatrix();
		glTranslatef ( 10.0f, 0.0f, 10.0f );
        glScalef(0.02f, 0.02f, 0.02f);
		glRotatef(45,0,1,0);
		MundoVirtual[2].ExibeObjeto();
	glPopMatrix();


	/*for(int i = 0; i<20;i++){//tamanho do vetor que guarda a posição das árvores
        //Árvore
        glPushMatrix();
            glTranslatef ( Arvores[i].X, Arvores[i].Y, Arvores[i].Z );
            glScalef(1.2f, 1.2f, 1.2f);
            glRotatef(0,0,1,0);
            MundoVirtual[4].ExibeObjeto();
        glPopMatrix();
	}*/

	//Arbusto
	glPushMatrix();
		glTranslatef( 10.0f, 0.0f, 20.0f );
		glScalef(0.3f, 0.3f, 0.3f);
	//	glRotatef(0,0,1,0);
		MundoVirtual[5].ExibeObjeto();
	glPopMatrix();

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


