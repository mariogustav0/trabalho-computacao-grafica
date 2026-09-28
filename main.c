/**
 * ============================================================================
 * UNIVERSIDADE FEDERAL DO CEARA (UFC)
 * Computacao Grafica I - Semestre 2026.2
 * Professor: Gabriel Rudan Sales Matos
 * 
 * TRABALHO 01 - Modelagem Geometrica: Releitura de Obras Artisticas
 * Obra: "A Persistencia da Memoria" (Salvador Dali, 1931)
 * Integrante: Marlon Moura (Branch: pessoa-3-relogio_n_derretido)
 * 
 * FUNCOES DE MODELAGEM DAS FORMAS:
 *   1. desenharBlocoFundo(): Bloco / plataforma retangular no horizonte costeiro.
 *   2. desenharMesa(): Mesa em primeiro plano (tampo + face lateral com Bezier).
 *   3. desenharGalho(): Tronco da oliveira morta e o galho da bifurcacao do relogio.
 * 
 * COMPILACAO:
 *   gcc main.c -lglut -lopengl32 -lglu32 -o main.exe
 *   ./main.exe
 * ============================================================================
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Dimensoes do Canvas de referencia (proporcional a pintura original: 685 x 538) */
#define CANVAS_WIDTH  1000.0f
#define CANVAS_HEIGHT 785.0f

#define BEZIER_SEGMENTOS 60

typedef struct {
    float x;
    float y;
} Ponto2D;

typedef struct {
    float r, g, b;
} CorRGB;

static bool g_modoWireframe = false;

/* ============================================================================
 * CURVA PARAMETRICA DE BEZIER CUBICA (CHANFRO DA BORDA DA MESA)
 * ============================================================================ */
static Ponto2D g_ctrlPtsMesa[4] = {
    { 327.0f, 412.0f }, /* P0: Quina superior da mesa */
    { 290.0f, 320.0f }, /* P1: Primeiro ponto de controle (tangente superior) */
    { 180.0f, 160.0f }, /* P2: Segundo ponto de controle (curvatura intermediaria) */
    {  95.0f,   0.0f }  /* P3: Ponto inferior na base da tela */
};

static Ponto2D avaliarBezierCubica(Ponto2D p0, Ponto2D p1, Ponto2D p2, Ponto2D p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Ponto2D p;
    p.x = uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y;
    return p;
}

/* ============================================================================
 * 1. FORMA: BLOCO RETANGULAR DE FUNDO (ATRAS DA ARVORE)
 * ============================================================================ */
void desenharBlocoFundo(void) {
    Ponto2D topEsq  = {   0.0f, 566.0f };
    Ponto2D topDir  = { 266.0f, 563.0f };
    Ponto2D meioDir = { 245.0f, 508.0f };
    Ponto2D meioEsq = {   0.0f, 511.0f };
    Ponto2D baseDir = { 245.0f, 492.0f };
    Ponto2D baseEsq = {   0.0f, 495.0f };

    /* 1.1 Face Superior do Bloco (Reflexo claro do ceu) */
    glBegin(GL_QUADS);
    {
        glColor3f(0.55f, 0.72f, 0.83f);
        glVertex2f(topEsq.x, topEsq.y);

        glColor3f(0.72f, 0.82f, 0.89f);
        glVertex2f(topDir.x, topDir.y);

        glColor3f(0.65f, 0.74f, 0.80f);
        glVertex2f(meioDir.x, meioDir.y);

        glColor3f(0.50f, 0.65f, 0.75f);
        glVertex2f(meioEsq.x, meioEsq.y);
    }
    glEnd();

    /* 1.2 Face Frontal Vertical do Bloco (Sombra costeira) */
    glBegin(GL_QUADS);
    {
        glColor3f(0.22f, 0.28f, 0.33f);
        glVertex2f(meioEsq.x, meioEsq.y);

        glColor3f(0.25f, 0.32f, 0.38f);
        glVertex2f(meioDir.x, meioDir.y);

        glColor3f(0.14f, 0.17f, 0.21f);
        glVertex2f(baseDir.x, baseDir.y);

        glColor3f(0.12f, 0.15f, 0.18f);
        glVertex2f(baseEsq.x, baseEsq.y);
    }
    glEnd();

    /* 1.3 Arestas e contornos nitidos */
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    {
        glColor3f(0.85f, 0.92f, 0.98f);
        glVertex2f(topEsq.x, topEsq.y);
        glVertex2f(topDir.x, topDir.y);

        glColor3f(0.35f, 0.45f, 0.52f);
        glVertex2f(meioEsq.x, meioEsq.y);
        glVertex2f(meioDir.x, meioDir.y);

        glVertex2f(topDir.x, topDir.y);
        glVertex2f(meioDir.x, meioDir.y);

        glVertex2f(meioDir.x, meioDir.y);
        glVertex2f(baseDir.x, baseDir.y);

        glColor3f(0.08f, 0.10f, 0.12f);
        glVertex2f(baseEsq.x, baseEsq.y);
        glVertex2f(baseDir.x, baseDir.y);
    }
    glEnd();
}

/* ============================================================================
 * 2. FORMA: MESA PRINCIPAL EM PRIMEIRO PLANO
 * ============================================================================ */
void desenharMesa(void) {
    Ponto2D pontosBezier[BEZIER_SEGMENTOS + 1];
    int i;
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        pontosBezier[i] = avaliarBezierCubica(g_ctrlPtsMesa[0], g_ctrlPtsMesa[1], g_ctrlPtsMesa[2], g_ctrlPtsMesa[3], t);
    }

    /* 2.1 Tampo Superior da Mesa (Madeira com iluminacao quente) */
    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        Ponto2D pCurva = pontosBezier[i];
        float y = pCurva.y;

        /* Vertice na margem esquerda (x=0) */
        float fatorEsq = 0.85f - 0.25f * t;
        glColor3f(0.88f * fatorEsq, 0.42f * fatorEsq, 0.14f * fatorEsq);
        glVertex2f(0.0f, y);

        /* Vertice na curva de Bezier (quina iluminada em ambar) */
        float fatorDir = 1.0f - 0.20f * t;
        glColor3f(0.92f * fatorDir, 0.52f * fatorDir, 0.16f * fatorDir);
        glVertex2f(pCurva.x, pCurva.y);
    }
    glEnd();

    /* 2.2 Face Frontal / Lateral da Mesa (Sombra profunda de mogno e carmim) */
    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        Ponto2D pCurva = pontosBezier[i];
        float y = pCurva.y;

        /* Borda direita da mesa em perspectiva */
        float xDir;
        if (y > 220.0f) {
            xDir = 327.0f - (412.0f - y) * 0.02f;
        } else {
            float frac = (220.0f - y) / 220.0f;
            xDir = 323.0f - frac * 33.0f;
        }

        /* Gradiente vertical de sombra */
        float r, g, b;
        if (t < 0.45f) {
            float k = t / 0.45f;
            r = 0.65f * (1.0f - k) + 0.45f * k;
            g = 0.16f * (1.0f - k) + 0.09f * k;
            b = 0.14f * (1.0f - k) + 0.08f * k;
        } else {
            float k = (t - 0.45f) / 0.55f;
            r = 0.45f * (1.0f - k) + 0.20f * k;
            g = 0.09f * (1.0f - k) + 0.04f * k;
            b = 0.08f * (1.0f - k) + 0.04f * k;
        }

        glColor3f(r * 1.12f, g * 1.12f, b * 1.12f);
        glVertex2f(pCurva.x, pCurva.y);

        glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
        glVertex2f(xDir, y);
    }
    glEnd();

    /* 2.3 Chanfro Parametrico de Bezier na quina */
    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        Ponto2D p = pontosBezier[i];

        glColor3f(0.95f * (1.0f - 0.2f * t), 0.58f * (1.0f - 0.2f * t), 0.18f * (1.0f - 0.2f * t));
        glVertex2f(p.x - 3.5f, p.y + 2.5f);

        glColor3f(0.65f * (1.0f - 0.4f * t), 0.16f * (1.0f - 0.4f * t), 0.14f * (1.0f - 0.4f * t));
        glVertex2f(p.x + 2.5f, p.y - 2.5f);
    }
    glEnd();

    /* Linha de crista ao longo da curva */
    glLineWidth(2.2f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        Ponto2D p = pontosBezier[i];
        glColor3f(0.98f - 0.25f * t, 0.75f - 0.35f * t, 0.30f - 0.15f * t);
        glVertex2f(p.x, p.y);
    }
    glEnd();

    /* 2.4 Arestas de contorno */
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    {
        glColor3f(0.92f, 0.48f, 0.16f);
        glVertex2f(0.0f, 412.0f);
        glVertex2f(327.0f, 412.0f);

        glColor3f(0.18f, 0.04f, 0.04f);
        glVertex2f(327.0f, 412.0f);
        glVertex2f(323.0f, 220.0f);

        glVertex2f(323.0f, 220.0f);
        glVertex2f(290.0f, 0.0f);

        glVertex2f(290.0f, 0.0f);
        glVertex2f(0.0f, 0.0f);
    }
    glEnd();
}

/* ============================================================================
 * 3. FORMA: TRONCO E GALHO DA ARVORE SECA (ONDE O RELOGIO SE APOIA)
 * Modelagem da oliveira morta sobre a mesa e do galho horizontal bifurcado.
 * ============================================================================ */
void desenharGalho(void) {
    /* Perfil do tronco da oliveira morta (niveis de altura y) */
    const int numNiveis = 7;
    Ponto2D perfilEsq[7] = {
        {  65.0f, 412.0f }, /* Base alargada na mesa */
        {  68.0f, 460.0f },
        {  74.0f, 520.0f },
        {  82.0f, 580.0f },
        {  90.0f, 630.0f },
        {  96.0f, 680.0f },
        { 102.0f, 720.0f }  /* Topo cortado */
    };
    Ponto2D perfilDir[7] = {
        { 128.0f, 412.0f }, /* Base direita */
        { 122.0f, 460.0f },
        { 118.0f, 520.0f },
        { 124.0f, 580.0f },
        { 132.0f, 630.0f }, /* Origem do galho */
        { 124.0f, 680.0f },
        { 118.0f, 720.0f }  /* Topo cortado */
    };

    int i;

    /* 3.1 Corpo do Tronco (Madeira envelhecida com Gouraud Shading) */
    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i < numNiveis; i++) {
        float t = (float)i / (float)(numNiveis - 1);

        /* Lado esquerdo iluminado (luz fria e suave do ceu) */
        float rEsq = 0.58f + 0.12f * t;
        float gEsq = 0.56f + 0.10f * t;
        float bEsq = 0.52f + 0.08f * t;
        glColor3f(rEsq, gEsq, bEsq);
        glVertex2f(perfilEsq[i].x, perfilEsq[i].y);

        /* Lado direito em sombra terrosa */
        float rDir = 0.28f + 0.08f * t;
        float gDir = 0.24f + 0.06f * t;
        float bDir = 0.20f + 0.05f * t;
        glColor3f(rDir, gDir, bDir);
        glVertex2f(perfilDir[i].x, perfilDir[i].y);
    }
    glEnd();

    /* 3.2 Toco cortado no topo do tronco */
    glColor3f(0.72f, 0.68f, 0.58f);
    glBegin(GL_POLYGON);
    {
        glVertex2f(102.0f, 720.0f);
        glVertex2f(108.0f, 725.0f);
        glVertex2f(118.0f, 720.0f);
        glVertex2f(112.0f, 715.0f);
    }
    glEnd();

    /* 3.3 Galho Principal Diagonal (do tronco ate a bifurcacao) */
    /* Curva suave de saida do galho */
    Ponto2D gBaseInf = { 130.0f, 618.0f };
    Ponto2D gBaseSup = { 128.0f, 638.0f };
    Ponto2D gBifInf  = { 252.0f, 634.0f };
    Ponto2D gBifSup  = { 255.0f, 646.0f };

    glBegin(GL_QUADS);
    {
        /* Base do galho junto ao tronco */
        glColor3f(0.45f, 0.38f, 0.30f);
        glVertex2f(gBaseSup.x, gBaseSup.y);

        glColor3f(0.30f, 0.24f, 0.18f);
        glVertex2f(gBaseInf.x, gBaseInf.y);

        /* Bifurcacao intermediaria */
        glColor3f(0.40f, 0.32f, 0.22f);
        glVertex2f(gBifInf.x, gBifInf.y);

        glColor3f(0.68f, 0.56f, 0.40f);
        glVertex2f(gBifSup.x, gBifSup.y);
    }
    glEnd();

    /* 3.4 Bifurcacao 1: Ramo Pequeno que sobe */
    glBegin(GL_TRIANGLES);
    {
        glColor3f(0.68f, 0.56f, 0.40f);
        glVertex2f(gBifSup.x, gBifSup.y);

        glColor3f(0.50f, 0.40f, 0.28f);
        glVertex2f(262.0f, 648.0f);

        /* Ponta afilada do ramo */
        glColor3f(0.82f, 0.72f, 0.55f);
        glVertex2f(278.0f, 674.0f);
    }
    glEnd();

    /* 3.5 Bifurcacao 2: Ramo Horizontal Principal (Apoio do Relogio Derretido) */
    /* Estende-se para a direita onde o relogio pendurado se apoia */
    Ponto2D pontaSup = { 342.0f, 637.0f };
    Ponto2D pontaInf = { 340.0f, 629.0f };

    glBegin(GL_QUADS);
    {
        glColor3f(0.68f, 0.56f, 0.40f);
        glVertex2f(gBifSup.x, gBifSup.y);

        glColor3f(0.40f, 0.32f, 0.22f);
        glVertex2f(gBifInf.x, gBifInf.y);

        /* Ponta direita onde a casca e clara/iluminada */
        glColor3f(0.55f, 0.42f, 0.28f);
        glVertex2f(pontaInf.x, pontaInf.y);

        glColor3f(0.85f, 0.74f, 0.56f);
        glVertex2f(pontaSup.x, pontaSup.y);
    }
    glEnd();

    /* Ponta arredondada/aparada da extremidade do galho */
    glBegin(GL_TRIANGLES);
    {
        glColor3f(0.85f, 0.74f, 0.56f);
        glVertex2f(pontaSup.x, pontaSup.y);

        glColor3f(0.55f, 0.42f, 0.28f);
        glVertex2f(pontaInf.x, pontaInf.y);

        glColor3f(0.92f, 0.82f, 0.65f);
        glVertex2f(346.0f, 633.0f);
    }
    glEnd();

    /* 3.6 Linhas de contorno da madeira seca */
    glLineWidth(1.6f);
    glBegin(GL_LINES);
    {
        /* Silhueta esquerda do tronco */
        glColor3f(0.42f, 0.40f, 0.36f);
        for (i = 0; i < numNiveis - 1; i++) {
            glVertex2f(perfilEsq[i].x, perfilEsq[i].y);
            glVertex2f(perfilEsq[i+1].x, perfilEsq[i+1].y);
        }

        /* Silhueta direita do tronco (inferior) */
        glColor3f(0.20f, 0.16f, 0.14f);
        glVertex2f(perfilDir[0].x, perfilDir[0].y);
        glVertex2f(perfilDir[1].x, perfilDir[1].y);
        glVertex2f(perfilDir[1].x, perfilDir[1].y);
        glVertex2f(perfilDir[2].x, perfilDir[2].y);
        glVertex2f(perfilDir[2].x, perfilDir[2].y);
        glVertex2f(perfilDir[3].x, perfilDir[3].y);

        /* Silhueta direita do tronco (superior ao galho) */
        glVertex2f(gBaseSup.x, gBaseSup.y);
        glVertex2f(perfilDir[5].x, perfilDir[5].y);
        glVertex2f(perfilDir[5].x, perfilDir[5].y);
        glVertex2f(perfilDir[6].x, perfilDir[6].y);

        /* Contorno superior do galho horizontal (iluminado) */
        glColor3f(0.75f, 0.65f, 0.48f);
        glVertex2f(gBaseSup.x, gBaseSup.y);
        glVertex2f(gBifSup.x, gBifSup.y);
        glVertex2f(gBifSup.x, gBifSup.y);
        glVertex2f(pontaSup.x, pontaSup.y);

        /* Contorno inferior do galho horizontal (sombra) */
        glColor3f(0.22f, 0.18f, 0.14f);
        glVertex2f(gBaseInf.x, gBaseInf.y);
        glVertex2f(gBifInf.x, gBifInf.y);
        glVertex2f(gBifInf.x, gBifInf.y);
        glVertex2f(pontaInf.x, pontaInf.y);

        /* Ramo fino */
        glVertex2f(gBifSup.x, gBifSup.y);
        glVertex2f(278.0f, 674.0f);
    }
    glEnd();
}

/* ============================================================================
 * OPENGL / FREEGLUT CALLBACKS (MINIMALISTA, APENAS VISUALIZACAO DAS FORMAS)
 * ============================================================================ */
void display(void) {
    /* Fundo neutro suave (atmosfera de horizonte de Dali) */
    glClearColor(0.88f, 0.85f, 0.76f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (g_modoWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Renderizacao exclusiva das formas modeladas */
    desenharBlocoFundo();
    desenharMesa();
    desenharGalho();

    glutSwapBuffers();
}

void reshape(int largura, int altura) {
    if (altura == 0) altura = 1;
    glViewport(0, 0, largura, altura);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, CANVAS_WIDTH, 0.0, CANVAS_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case 27: /* ESC */
            exit(0);
            break;
        case 'm': case 'M':
            g_modoWireframe = !g_modoWireframe;
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 785);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("CG I - Releitura Salvador Dali (Bloco, Mesa e Galho)");

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
