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
 * ESCOPO DESTE MODULO:
 * - Modelagem das partes delimitadas em roxo no planejamento:
 *   1. Bloco / Plataforma retangular no plano de fundo (costa/mar, atras da arvore).
 *   2. Mesa principal em primeiro plano (tampo superior e face frontal/lateral).
 *   3. Curva Parametrica de Bezier Cubica aplicada no chanfro/borda da quina da mesa.
 * - Gradientes de cor via Gouraud Shading (GL_SMOOTH) reproduzindo a luz de Dali.
 * - Transformacoes geometricas completas (translacao, rotacao, escala).
 * - Primitivas variadas (GL_QUADS, GL_POLYGON, GL_TRIANGLE_STRIP, GL_LINES, etc.).
 * - Controles interativos via teclado com feedback em tempo real.
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

/* Dimensoes do Canvas de referencia (proporcional a obra original: 685 x 538) */
#define CANVAS_WIDTH  1000.0f
#define CANVAS_HEIGHT 785.0f

/* Numero de segmentos para amostragem da curva parametrica de Bezier */
#define BEZIER_SEGMENTOS 60

/* Estrutura para pontos 2D */
typedef struct {
    float x;
    float y;
} Ponto2D;

/* Estrutura para cores RGB */
typedef struct {
    float r, g, b;
} CorRGB;

/* ============================================================================
 * VARIAVEIS DE ESTADO E INTERACAO (CONFORME REQUISITOS DO EDITAL)
 * ============================================================================ */
static float g_transX = 0.0f;
static float g_transY = 0.0f;
static float g_rotZ   = 0.0f;
static float g_escala = 1.0f;

static bool g_modoWireframe       = false; /* Alterna GL_FILL e GL_LINE com 'M' */
static bool g_paletaNoturna       = false; /* Alterna paleta de cores com 'C' */
static bool g_exibirPontosBezier  = true;  /* Destaca pontos de controle com 'B' */
static bool g_exibirAjuda         = true;  /* Exibe instrucoes na tela com 'H' */

/* ============================================================================
 * DEFINICAO DA CURVA PARAMETRICA DE BEZIER CUBICA
 * B(t) = (1-t)^3 * P0 + 3(1-t)^2 * t * P1 + 3(1-t) * t^2 * P2 + t^3 * P3
 * ============================================================================ */
static Ponto2D g_ctrlPts[4] = {
    { 327.0f, 412.0f }, /* P0: Quina superior da mesa */
    { 290.0f, 320.0f }, /* P1: Primeiro ponto de controle (tangente superior) */
    { 180.0f, 160.0f }, /* P2: Segundo ponto de controle (curvatura intermediaria) */
    {  95.0f,   0.0f }  /* P3: Ponto inferior na base da tela */
};

/**
 * Avalia um ponto sobre a curva de Bezier cubica para um dado parametro t in [0, 1].
 */
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
 * PALETAS DE CORES (MODO CLASSICO DALI vs MODO NOTURNO SURREALISTA)
 * ============================================================================ */

/* Bloco de Fundo */
static CorRGB getCorBlocoTopo1(void) {
    return g_paletaNoturna ? (CorRGB){0.20f, 0.28f, 0.38f} : (CorRGB){0.55f, 0.72f, 0.83f};
}
static CorRGB getCorBlocoTopo2(void) {
    return g_paletaNoturna ? (CorRGB){0.28f, 0.35f, 0.45f} : (CorRGB){0.72f, 0.82f, 0.89f};
}
static CorRGB getCorBlocoFrente1(void) {
    return g_paletaNoturna ? (CorRGB){0.10f, 0.14f, 0.20f} : (CorRGB){0.22f, 0.28f, 0.33f};
}
static CorRGB getCorBlocoFrente2(void) {
    return g_paletaNoturna ? (CorRGB){0.05f, 0.08f, 0.12f} : (CorRGB){0.12f, 0.15f, 0.18f};
}

/* Mesa Principal - Tampo Superior */
static CorRGB getCorMesaTampoEsq(void) {
    return g_paletaNoturna ? (CorRGB){0.42f, 0.18f, 0.10f} : (CorRGB){0.88f, 0.42f, 0.14f};
}
static CorRGB getCorMesaTampoCentro(void) {
    return g_paletaNoturna ? (CorRGB){0.55f, 0.26f, 0.12f} : (CorRGB){0.95f, 0.58f, 0.18f};
}
static CorRGB getCorMesaTampoDir(void) {
    return g_paletaNoturna ? (CorRGB){0.35f, 0.14f, 0.08f} : (CorRGB){0.78f, 0.30f, 0.10f};
}

/* Mesa Principal - Face Frontal / Lateral */
static CorRGB getCorMesaFrenteTopo(void) {
    return g_paletaNoturna ? (CorRGB){0.28f, 0.08f, 0.08f} : (CorRGB){0.65f, 0.16f, 0.14f};
}
static CorRGB getCorMesaFrenteMeio(void) {
    return g_paletaNoturna ? (CorRGB){0.18f, 0.04f, 0.04f} : (CorRGB){0.45f, 0.09f, 0.08f};
}
static CorRGB getCorMesaFrenteBase(void) {
    return g_paletaNoturna ? (CorRGB){0.08f, 0.02f, 0.02f} : (CorRGB){0.20f, 0.04f, 0.04f};
}

/* ============================================================================
 * 1. FUNCAO MODULAR: BLOCO RETANGULAR DE FUNDO (ATRAS DA ARVORE)
 * Modelado em perspectiva com face superior plana e face frontal vertical.
 * ============================================================================ */
void desenharBlocoFundo(void) {
    /* Coordenadas das faces em perspectiva */
    Ponto2D topEsq  = {   0.0f, 566.0f };
    Ponto2D topDir  = { 266.0f, 563.0f };
    Ponto2D meioDir = { 245.0f, 508.0f };
    Ponto2D meioEsq = {   0.0f, 511.0f };
    Ponto2D baseDir = { 245.0f, 492.0f };
    Ponto2D baseEsq = {   0.0f, 495.0f };

    /* 1.1 Face Superior do Bloco (Reflexo suave do ceu e da agua) */
    glBegin(GL_QUADS);
    {
        CorRGB c1 = getCorBlocoTopo1();
        CorRGB c2 = getCorBlocoTopo2();

        glColor3f(c1.r, c1.g, c1.b);
        glVertex2f(topEsq.x, topEsq.y);

        glColor3f(c2.r, c2.g, c2.b);
        glVertex2f(topDir.x, topDir.y);

        glColor3f(c2.r * 0.90f, c2.g * 0.90f, c2.b * 0.90f);
        glVertex2f(meioDir.x, meioDir.y);

        glColor3f(c1.r * 0.90f, c1.g * 0.90f, c1.b * 0.90f);
        glVertex2f(meioEsq.x, meioEsq.y);
    }
    glEnd();

    /* 1.2 Face Frontal Vertical do Bloco (Sombra costeira) */
    glBegin(GL_QUADS);
    {
        CorRGB cFront1 = getCorBlocoFrente1();
        CorRGB cFront2 = getCorBlocoFrente2();

        glColor3f(cFront1.r, cFront1.g, cFront1.b);
        glVertex2f(meioEsq.x, meioEsq.y);

        glColor3f(cFront1.r * 1.15f, cFront1.g * 1.15f, cFront1.b * 1.15f);
        glVertex2f(meioDir.x, meioDir.y);

        glColor3f(cFront2.r * 1.15f, cFront2.g * 1.15f, cFront2.b * 1.15f);
        glVertex2f(baseDir.x, baseDir.y);

        glColor3f(cFront2.r, cFront2.g, cFront2.b);
        glVertex2f(baseEsq.x, baseEsq.y);
    }
    glEnd();

    /* 1.3 Arestas e contornos nitidos (GL_LINES) */
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    {
        /* Aresta superior iluminada */
        glColor3f(0.85f, 0.92f, 0.98f);
        glVertex2f(topEsq.x, topEsq.y);
        glVertex2f(topDir.x, topDir.y);

        /* Aresta divisoria entre topo e frente */
        glColor3f(0.35f, 0.45f, 0.52f);
        glVertex2f(meioEsq.x, meioEsq.y);
        glVertex2f(meioDir.x, meioDir.y);

        /* Aresta lateral direita */
        glVertex2f(topDir.x, topDir.y);
        glVertex2f(meioDir.x, meioDir.y);

        glVertex2f(meioDir.x, meioDir.y);
        glVertex2f(baseDir.x, baseDir.y);

        /* Aresta da base */
        glColor3f(0.08f, 0.10f, 0.12f);
        glVertex2f(baseEsq.x, baseEsq.y);
        glVertex2f(baseDir.x, baseDir.y);
    }
    glEnd();
}

/* ============================================================================
 * 2. FUNCAO MODULAR: CURVA PARAMETRICA DE BEZIER E BORDA DA MESA
 * Avalia os pontos matematicos de Bezier e constroi o chanfro suave da borda.
 * ============================================================================ */
void desenharBordaBezier(void) {
    Ponto2D pontosBezier[BEZIER_SEGMENTOS + 1];
    int i;

    /* Amostragem da curva parametrica B(t) */
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        pontosBezier[i] = avaliarBezierCubica(g_ctrlPts[0], g_ctrlPts[1], g_ctrlPts[2], g_ctrlPts[3], t);
    }

    /* 2.1 Faixa chanfrada suave ao longo da curva (GL_TRIANGLE_STRIP) */
    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        Ponto2D p = pontosBezier[i];

        /* Cor do lado superior da borda (madeira iluminada) */
        CorRGB cTop = getCorMesaTampoCentro();
        /* Cor do lado inferior da borda (sombra imediata) */
        CorRGB cSombra = getCorMesaFrenteTopo();

        glColor3f(cTop.r * (1.0f - 0.2f * t), cTop.g * (1.0f - 0.2f * t), cTop.b * (1.0f - 0.2f * t));
        glVertex2f(p.x - 4.0f, p.y + 3.0f);

        glColor3f(cSombra.r * (1.0f - 0.4f * t), cSombra.g * (1.0f - 0.4f * t), cSombra.b * (1.0f - 0.4f * t));
        glVertex2f(p.x + 3.0f, p.y - 3.0f);
    }
    glEnd();

    /* 2.2 Linha de crista destacando a curva de Bezier (GL_LINE_STRIP) */
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        Ponto2D p = pontosBezier[i];
        /* Gradiente dourado/ocre ao longo da borda */
        if (!g_paletaNoturna) {
            glColor3f(0.98f - 0.25f * t, 0.75f - 0.35f * t, 0.30f - 0.15f * t);
        } else {
            glColor3f(0.55f - 0.20f * t, 0.30f - 0.15f * t, 0.18f - 0.08f * t);
        }
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

/* ============================================================================
 * 3. FUNCAO MODULAR: PONTOS DE CONTROLE DA CURVA DE BEZIER
 * Exibe os pontos P0, P1, P2, P3 e as retas do poligono de controle (tecla 'B').
 * ============================================================================ */
void desenharPontosControleBezier(void) {
    if (!g_exibirPontosBezier) return;

    int i;
    /* Linhas do poligono de controle (tracejado conceitual) */
    glLineWidth(1.5f);
    glColor3f(0.2f, 0.9f, 1.0f); /* Azul ciano brilhante */
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < 4; i++) {
        glVertex2f(g_ctrlPts[i].x, g_ctrlPts[i].y);
    }
    glEnd();

    /* Circulos nos 4 pontos de controle */
    for (i = 0; i < 4; i++) {
        if (i == 0 || i == 3) {
            glColor3f(1.0f, 0.85f, 0.1f); /* P0 e P3 (extremidades) em amarelo ouro */
        } else {
            glColor3f(0.2f, 1.0f, 0.3f);  /* P1 e P2 (tangentes) em verde esmeralda */
        }

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(g_ctrlPts[i].x, g_ctrlPts[i].y);
        int j;
        float raio = 6.5f;
        for (j = 0; j <= 20; j++) {
            float ang = j * 2.0f * (float)M_PI / 20.0f;
            glVertex2f(g_ctrlPts[i].x + cosf(ang) * raio, g_ctrlPts[i].y + sinf(ang) * raio);
        }
        glEnd();

        /* Borda preta no ponto de controle para alto contraste */
        glLineWidth(1.2f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        for (j = 0; j < 20; j++) {
            float ang = j * 2.0f * (float)M_PI / 20.0f;
            glVertex2f(g_ctrlPts[i].x + cosf(ang) * raio, g_ctrlPts[i].y + sinf(ang) * raio);
        }
        glEnd();
    }
}

/* ============================================================================
 * 4. FUNCAO MODULAR: MESA PRINCIPAL EM PRIMEIRO PLANO
 * Renderiza o tampo superior (iluminado) e a face frontal/lateral (em sombra)
 * utilizando malhas regulares de GL_TRIANGLE_STRIP ancoradas na curva de Bezier.
 * ============================================================================ */
void desenharMesa(void) {
    Ponto2D pontosBezier[BEZIER_SEGMENTOS + 1];
    int i;
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        pontosBezier[i] = avaliarBezierCubica(g_ctrlPts[0], g_ctrlPts[1], g_ctrlPts[2], g_ctrlPts[3], t);
    }

    CorRGB cEsq    = getCorMesaTampoEsq();
    CorRGB cCentro = getCorMesaTampoCentro();
    CorRGB cDir    = getCorMesaTampoDir();

    /* 4.1 Tampo Superior da Mesa: GL_TRIANGLE_STRIP da margem x=0 ate a curva de Bezier */
    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        Ponto2D pCurva = pontosBezier[i];
        float y = pCurva.y;

        /* Vertice na margem esquerda (x=0) com degradê vertical */
        float fatorEsq = 0.85f - 0.25f * t;
        glColor3f(cEsq.r * fatorEsq, cEsq.g * fatorEsq, cEsq.b * fatorEsq);
        glVertex2f(0.0f, y);

        /* Vertice na curva de Bezier (quina iluminada com reflexo ambar/dourado) */
        float fatorDir = 1.0f - 0.20f * t;
        float r = (cCentro.r * 0.6f + cDir.r * 0.4f) * fatorDir;
        float g = (cCentro.g * 0.6f + cDir.g * 0.4f) * fatorDir;
        float b = (cCentro.b * 0.6f + cDir.b * 0.4f) * fatorDir;
        glColor3f(r, g, b);
        glVertex2f(pCurva.x, pCurva.y);
    }
    glEnd();

    /* 4.2 Face Frontal / Lateral da Mesa: GL_TRIANGLE_STRIP da curva de Bezier ate a borda direita */
    CorRGB cFrontTopo = getCorMesaFrenteTopo();
    CorRGB cFrontMeio = getCorMesaFrenteMeio();
    CorRGB cFrontBase = getCorMesaFrenteBase();

    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= BEZIER_SEGMENTOS; i++) {
        float t = (float)i / (float)BEZIER_SEGMENTOS;
        Ponto2D pCurva = pontosBezier[i];
        float y = pCurva.y;

        /* Calculo da borda direita da mesa acompanhando a perspectiva */
        float xDir;
        if (y > 220.0f) {
            xDir = 327.0f - (412.0f - y) * 0.02f;
        } else {
            float frac = (220.0f - y) / 220.0f;
            xDir = 323.0f - frac * 33.0f; /* vai de ~323 ate 290 na base */
        }

        /* Gradiente de sombra vertical */
        float r, g, b;
        if (t < 0.45f) {
            float k = t / 0.45f;
            r = cFrontTopo.r * (1.0f - k) + cFrontMeio.r * k;
            g = cFrontTopo.g * (1.0f - k) + cFrontMeio.g * k;
            b = cFrontTopo.b * (1.0f - k) + cFrontMeio.b * k;
        } else {
            float k = (t - 0.45f) / 0.55f;
            r = cFrontMeio.r * (1.0f - k) + cFrontBase.r * k;
            g = cFrontMeio.g * (1.0f - k) + cFrontBase.g * k;
            b = cFrontMeio.b * (1.0f - k) + cFrontBase.b * k;
        }

        /* Vertice na curva de Bezier (transicao chanfrada com leve brilho ambiente) */
        glColor3f(r * 1.12f, g * 1.12f, b * 1.12f);
        glVertex2f(pCurva.x, pCurva.y);

        /* Vertice na lateral direita (em sombra profunda) */
        glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
        glVertex2f(xDir, y);
    }
    glEnd();

    /* 4.3 Desenho do Chanfro Parametrico de Bezier na transicao */
    desenharBordaBezier();

    /* 4.4 Linhas de contorno da mesa (GL_LINES) */
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    {
        /* Topo horizontal da mesa */
        glColor3f(0.92f, 0.48f, 0.16f);
        glVertex2f(0.0f, 412.0f);
        glVertex2f(327.0f, 412.0f);

        /* Aresta lateral direita */
        glColor3f(0.18f, 0.04f, 0.04f);
        glVertex2f(327.0f, 412.0f);
        glVertex2f(323.0f, 220.0f);

        glVertex2f(323.0f, 220.0f);
        glVertex2f(290.0f, 0.0f);

        /* Linha de base da mesa */
        glVertex2f(290.0f, 0.0f);
        glVertex2f(0.0f, 0.0f);
    }
    glEnd();
}

/* ============================================================================
 * 5. OVERLAY TEXTUAL E INSTRUCOES NA TELA (GLUT BITMAP)
 * ============================================================================ */
static void renderizarTexto(float x, float y, const char* str) {
    glRasterPos2f(x, y);
    while (*str) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *str);
        str++;
    }
}

void desenharInterfaceAjuda(void) {
    if (!g_exibirAjuda) return;

    /* Salva projecao e desabilita transformacoes da cena para o HUD */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, CANVAS_WIDTH, 0, CANVAS_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    /* Fundo semitransparente para a caixa de informacoes */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.05f, 0.08f, 0.12f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(CANVAS_WIDTH - 360.0f, CANVAS_HEIGHT - 20.0f);
    glVertex2f(CANVAS_WIDTH - 15.0f,  CANVAS_HEIGHT - 20.0f);
    glVertex2f(CANVAS_WIDTH - 15.0f,  CANVAS_HEIGHT - 230.0f);
    glVertex2f(CANVAS_WIDTH - 360.0f, CANVAS_HEIGHT - 230.0f);
    glEnd();

    /* Borda da caixa de informacoes */
    glLineWidth(1.5f);
    glColor3f(0.3f, 0.6f, 0.9f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(CANVAS_WIDTH - 360.0f, CANVAS_HEIGHT - 20.0f);
    glVertex2f(CANVAS_WIDTH - 15.0f,  CANVAS_HEIGHT - 20.0f);
    glVertex2f(CANVAS_WIDTH - 15.0f,  CANVAS_HEIGHT - 230.0f);
    glVertex2f(CANVAS_WIDTH - 360.0f, CANVAS_HEIGHT - 230.0f);
    glEnd();

    /* Textos informativos */
    glColor3f(1.0f, 0.9f, 0.4f);
    renderizarTexto(CANVAS_WIDTH - 345.0f, CANVAS_HEIGHT - 45.0f,  "CG I - A Persistencia da Memoria (Dali)");
    renderizarTexto(CANVAS_WIDTH - 345.0f, CANVAS_HEIGHT - 65.0f,  "Modulo: Mesa & Bloco de Fundo (Marlon)");

    glColor3f(0.85f, 0.95f, 1.0f);
    renderizarTexto(CANVAS_WIDTH - 345.0f, CANVAS_HEIGHT - 95.0f,  "[Setas / WASD] : Mover (Translacao)");
    renderizarTexto(CANVAS_WIDTH - 345.0f, CANVAS_HEIGHT - 115.0f, "[R / r] : Rotacionar cena");
    renderizarTexto(CANVAS_WIDTH - 345.0f, CANVAS_HEIGHT - 135.0f, "[+ / -] : Zoom (Escala)");
    renderizarTexto(CANVAS_WIDTH - 345.0f, CANVAS_HEIGHT - 155.0f, "[B / b] : Ligar/Desligar Curva de Bezier");
    renderizarTexto(CANVAS_WIDTH - 345.0f, CANVAS_HEIGHT - 175.0f, "[M / m] : Alternar Wireframe / Solido");
    renderizarTexto(CANVAS_WIDTH - 345.0f, CANVAS_HEIGHT - 195.0f, "[C / c] : Alternar Paleta de Cores");
    renderizarTexto(CANVAS_WIDTH - 345.0f, CANVAS_HEIGHT - 215.0f, "[Espaco] : Resetar | [H] : Ocultar HUD");

    glDisable(GL_BLEND);

    /* Restaura matrizes */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/* ============================================================================
 * 6. CALLBACKS PRINCIPAIS DO OPENGL / FREEGLUT
 * ============================================================================ */
void display(void) {
    /* Cor de fundo ambiente suave (areia/creme suave do horizonte) */
    if (!g_paletaNoturna) {
        glClearColor(0.88f, 0.85f, 0.76f, 1.0f);
    } else {
        glClearColor(0.08f, 0.10f, 0.16f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT);

    /* Modo de rasterizacao (GL_FILL solido ou GL_LINE aramado) */
    if (g_modoWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    /* Shading Gouraud suave para transicoes de cor */
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* APLICACAO DAS TRANSFORMACOES GEOMETRICAS (REQUISITO EDITAL) */
    glPushMatrix();
    {
        /* Translacao interativa */
        glTranslatef(g_transX, g_transY, 0.0f);

        /* Rotacao em torno do centro da mesa */
        glTranslatef(200.0f, 300.0f, 0.0f);
        glRotatef(g_rotZ, 0.0f, 0.0f, 1.0f);
        glTranslatef(-200.0f, -300.0f, 0.0f);

        /* Escala / Zoom interativo */
        glTranslatef(CANVAS_WIDTH / 2.0f, CANVAS_HEIGHT / 2.0f, 0.0f);
        glScalef(g_escala, g_escala, 1.0f);
        glTranslatef(-CANVAS_WIDTH / 2.0f, -CANVAS_HEIGHT / 2.0f, 0.0f);

        /* Renderizacao dos componentes modelados (partes em roxo) */
        desenharBlocoFundo();
        desenharMesa();
        desenharPontosControleBezier();
    }
    glPopMatrix();

    /* Renderiza o HUD / Guia de comandos (sempre no topo, fixo) */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    desenharInterfaceAjuda();

    glutSwapBuffers();
}

void reshape(int largura, int altura) {
    if (altura == 0) altura = 1;
    glViewport(0, 0, largura, altura);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /* Mantem o sistema de coordenadas 2D fixo no Canvas proporcional da pintura */
    gluOrtho2D(0.0, CANVAS_WIDTH, 0.0, CANVAS_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case 27: /* ESC */
            exit(0);
            break;
        case 'w': case 'W':
            g_transY += 15.0f;
            break;
        case 's': case 'S':
            g_transY -= 15.0f;
            break;
        case 'a': case 'A':
            g_transX -= 15.0f;
            break;
        case 'd': case 'D':
            g_transX += 15.0f;
            break;
        case 'r':
            g_rotZ += 3.0f;
            break;
        case 'R':
            g_rotZ -= 3.0f;
            break;
        case '+': case '=':
            g_escala *= 1.05f;
            break;
        case '-': case '_':
            g_escala *= 0.95f;
            break;
        case 'm': case 'M':
            g_modoWireframe = !g_modoWireframe;
            break;
        case 'c': case 'C':
            g_paletaNoturna = !g_paletaNoturna;
            break;
        case 'b': case 'B':
            g_exibirPontosBezier = !g_exibirPontosBezier;
            break;
        case 'h': case 'H':
            g_exibirAjuda = !g_exibirAjuda;
            break;
        case ' ': /* Reset */
            g_transX = 0.0f;
            g_transY = 0.0f;
            g_rotZ   = 0.0f;
            g_escala = 1.0f;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case GLUT_KEY_UP:
            g_transY += 15.0f;
            break;
        case GLUT_KEY_DOWN:
            g_transY -= 15.0f;
            break;
        case GLUT_KEY_LEFT:
            g_transX -= 15.0f;
            break;
        case GLUT_KEY_RIGHT:
            g_transX += 15.0f;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

/* ============================================================================
 * MAIN
 * ============================================================================ */
int main(int argc, char** argv) {
    printf("================================================================\n");
    printf(" COMPUTACAO GRAFICA I - UFC 2026.2\n");
    printf(" TRABALHO 01 - Releitura de 'A Persistencia da Memoria' (Dali)\n");
    printf(" Integrante: Marlon Moura (Branch: pessoa-3-relogio_n_derretido)\n");
    printf("================================================================\n");
    printf(" Elementos modelados (partes em roxo do trab.png):\n");
    printf("   1. Bloco / Plataforma de fundo em perspectiva\n");
    printf("   2. Mesa principal em primeiro plano (tampo + face lateral)\n");
    printf("   3. Curva Parametrica de Bezier Cubica no chanfro da borda\n");
    printf("----------------------------------------------------------------\n");
    printf(" Comandos disponiveis:\n");
    printf("   [Setas / WASD]  : Translacao da cena\n");
    printf("   [R / r]         : Rotacao em torno do centro\n");
    printf("   [+ / -]         : Zoom / Escala\n");
    printf("   [B / b]         : Alternar visualizacao dos pontos de Bezier\n");
    printf("   [M / m]         : Alternar modo Wireframe / Preenchido\n");
    printf("   [C / c]         : Alternar paleta Diurna / Noturna\n");
    printf("   [Espaco]        : Resetar transformacoes para o padrao\n");
    printf("   [H / h]         : Ocultar / Exibir guia na tela\n");
    printf("   [ESC]           : Encerrar o programa\n");
    printf("================================================================\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 785);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("CG I - Releitura Salvador Dali (Mesa e Bloco)");

    /* Ativa suavizacao de linhas */
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    /* Registra callbacks */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
