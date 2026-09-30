#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define LARGURA 947
#define ALTURA 750

/* ---------- Estado da interacao ---------- */
int   rosa   = 0;     /* 0 = cores originais, 1 = tudo rosa */
float angulo = 0.0f;  /* rotacao da cena, em graus */


/* Define a cor: no modo "rosa" todas as formas ficam rosa,
   caso contrario usa a cor original de cada forma */
void definirCor(float r, float g, float b)
{
    if (rosa)
        glColor3f(1.0f, 0.4f, 0.7f);
    else
        glColor3f(r, g, b);
}


void desenharLinha(float x1, float y1, float x2, float y2)
{
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
}

// Desenha um poligono ou sequencia de linhas
void desenharPoligono(float pontos[][2], int quantidade, int fechado)
{
    int i;

    if (fechado)
        glBegin(GL_LINE_LOOP);
    else
        glBegin(GL_LINE_STRIP);

    for (i = 0; i < quantidade; i++)
        glVertex2f(pontos[i][0], pontos[i][1]);

    glEnd();
}

// Desenha uma elipse com curvas parametricas
void desenharElipse(float cx, float cy, float rx, float ry)
{
    int i;
    float t;
    float x, y;

    glBegin(GL_LINE_LOOP);

    for (i = 0; i < 100; i++)
    {
        t = 2.0f * 3.14159f * i / 100.0f;

        x = cx + rx * cosf(t);
        y = cy + ry * sinf(t);

        glVertex2f(x, y);
    }

    glEnd();
}

// Desenha uma curva usando pontos intermediarios
void desenharCurva(float pontos[][2], int quantidade)
{
    int i;

    glBegin(GL_LINE_STRIP);

    for (i = 0; i < quantidade; i++)
        glVertex2f(pontos[i][0], pontos[i][1]);

    glEnd();
}

void desenharPaisagemVerde(void)
{
    float mastro[][2] = {
        {90,355},
        {148,60},
        {152,355}
    };

    float galho[][2] = {
        {158,215},
        {245,165},
        {412,155}
    };

    float colina[][2] = {
        {565,215},
        {572,195},
        {620,165},
        {700,140},
        {800,125},
        {915,123}
    };

    definirCor(0.20f, 0.85f, 0.40f);
    glLineWidth(5.0f);

    // Arvore
    desenharPoligono(mastro, 3, 0);

    desenharLinha(88, 355, 152, 355);

    /* Galho */
    desenharPoligono(galho, 3, 0);

    /* Graveto */
    desenharLinha(245, 165, 262, 115);

    /* Linha do horizonte */
    desenharLinha(390, 232, 930, 228);

    desenharCurva(colina, 6);
}


void desenharMesa(void)
{
    float gancho[][2] = {
        {188,208},
        {250,212},
        {210,265},
        {165,265}
    };

    definirCor(0.62f, 0.22f, 0.0f);
    glLineWidth(5.0f);

    desenharLinha(40, 205, 108, 210);
    desenharLinha(37, 263, 103, 260);

    desenharPoligono(gancho, 4, 0);

    desenharLinha(37, 345, 327, 345);
    desenharLinha(322, 345, 322, 610);
    desenharLinha(322, 345, 100, 720);
    desenharLinha(322, 610, 275, 720);
}


void desenharRelogiosDerretido(void)
{
    float gota1[][2] = {
        {280,175},
        {276,240},
        {290,272},
        {318,270},
        {315,170}
    };

    float gota2[][2] = {
        {315,170},
        {325,265},
        {345,300},
        {365,290},
        {375,230},
        {375,163}
    };

    float formaEsquerda[][2] = {
        {100,422},
        {140,405},
        {190,395},
        {260,395},
        {285,410},
        {295,450},
        {300,520},
        {290,575},
        {270,588},
        {250,565},
        {232,510},
        {222,480},
        {185,468},
        {150,445},
        {130,430}
    };

    float formaCentro[][2] = {
        {435,432},
        {470,415},
        {505,392},
        {525,398},
        {542,420},
        {540,455},
        {525,470},
        {505,466},
        {480,452},
        {455,442}
    };

    definirCor(1.00f, 1.00f, 1.0f);
    glLineWidth(5.0f);

    /* Duas gotas */
    desenharCurva(gota1, 5);
    desenharCurva(gota2, 6);

    /* Forma da esquerda */
    desenharPoligono(formaEsquerda, 15, 1);

    /* Pequena elipse */
    desenharElipse(122, 421, 9, 7);

    /* Forma central */
    desenharPoligono(formaCentro, 10, 1);

    /* Pequena aba */
    glBegin(GL_LINE_LOOP);
        glVertex2f(518, 470);
        glVertex2f(545, 472);
        glVertex2f(543, 498);
        glVertex2f(520, 496);
    glEnd();
}

/* INTERACAO DO MOUSE: a cor depende da variavel "rosa" */
void desenharElipsesRosas(void)
{
    definirCor(1.0f, 0.60f, 0.95f);

    glLineWidth(5.0f);

    desenharElipse(110, 552, 70, 40);
    desenharElipse(170, 511, 16, 12);
}

void desenharBandeira(void)
{
    float curvaSuperior[][2] = {
        {530,388},
        {560,375},
        {600,378},
        {650,390},
        {700,395},
        {760,393}
    };

    float curvaInferior[][2] = {
        {440,445},
        {400,460},
        {378,500},
        {375,545},
        {390,575},
        {430,600},
        {468,613},
        {470,630},
        {450,633}
    };

    float curvaDireita[][2] = {
        {785,428},
        {720,435},
        {650,450},
        {600,462},
        {585,490},
        {575,520},
        {540,525},
        {500,540},
        {478,570},
        {472,610}
    };

    definirCor(1.0f, 0.10f, 0.10f);
    glLineWidth(5.0f);

    // Transformacao geometrica
    glPushMatrix();
        glTranslatef(20.0f, 0.0f, 0.0f);

        desenharCurva(curvaSuperior, 6);
        desenharCurva(curvaInferior, 9);
        desenharCurva(curvaDireita, 10);

    glPopMatrix();
}


void desenharMoldura(void)
{
    definirCor(1.0f, 1.0f, 1.0f);
    glLineWidth(4.0f);

    glBegin(GL_LINE_LOOP);
        glVertex2f(32, 8);
        glVertex2f(925, 8);
        glVertex2f(925, 725);
        glVertex2f(32, 725);
    glEnd();
}


/* ---------- Callbacks de interacao ---------- */

/* Clique esquerdo alterna a cor de TODAS as formas (original <-> rosa) */
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        rosa = !rosa;
        glutPostRedisplay();
    }
}

/* R = gira para a direita, L = gira para a esquerda */
void teclado(unsigned char tecla, int x, int y)
{
    /* O eixo Y esta invertido (glOrtho com ALTURA em cima),
       entao angulo positivo gira no sentido horario na tela */
    if (tecla == 'r' || tecla == 'R')
        angulo += 10.0f;

    if (tecla == 'l' || tecla == 'L')
        angulo -= 10.0f;

    glutPostRedisplay();
}


void display(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Rotaciona a cena em torno do centro da tela */
    glPushMatrix();
        glTranslatef(LARGURA / 2.0f, ALTURA / 2.0f, 0.0f);
        glRotatef(angulo, 0.0f, 0.0f, 1.0f);
        glTranslatef(-LARGURA / 2.0f, -ALTURA / 2.0f, 0.0f);

        desenharPaisagemVerde();
        desenharMesa();
        desenharRelogiosDerretido();
        desenharElipsesRosas();
        desenharBandeira();
    glPopMatrix();

    /* A moldura fica fixa */
    desenharMoldura();

    glutSwapBuffers();
}


void reshape(int w, int h)
{
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, LARGURA, ALTURA, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(LARGURA, ALTURA);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("A Persistencia da Memoria - Dali");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);        
    glutKeyboardFunc(teclado);   

    glutMainLoop();
    return 0;
}
