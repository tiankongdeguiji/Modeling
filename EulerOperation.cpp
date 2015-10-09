#include "EulerOperation.h"

EulerOperation::EulerOperation()
{

}

// 功能：构造一个体，一个面，一个外环，一个点
// 输入：newv-新建的起始点的引用(返回), p-起始点的坐标
// 输出：新的体
Solid *EulerOperation::mvfs(Vertex *&newv, double p[3])
{
    Solid *s = new Solid;
    Face *f = new Face;
    Loop *l = new Loop;
    Vertex *v = new Vertex(p);

    // 建立体-面，面-环的关系
    s->faces = f;
    f->solid = s;
    f->loops = l;
    l->face = f;

    // 返回新建的一个点和一个体
    newv = v;
    return s;
}

// 功能：在一个loop上添加一个顶点和一条边
// 输入：sv-新加边的起始点，p-新加边的终点坐标，lp-新加边所在的环
// 输出: 新加边的一个半边
HalfEdge *EulerOperation::mev(Vertex *sv, double p[3], Loop *lp)
{
    Solid *s = lp->face->solid;
    Edge *e = new Edge;
    HalfEdge *he1 = new HalfEdge, *he2 = new HalfEdge;
    Vertex *ev = new Vertex(p);

    // 建立边、半边、顶点、体之间的关系
    SetUpEdge(e, sv, ev, he1, he2);
    AddEdgeIntoSolid(e, s);

    // 将新建的半边添加到环中
    he1->next = he2;
    he2->prev = he1;
    he1->loop = he2->loop = lp;
    if (lp->halfedges = NULL) {
        he2->next = he1;
        he1->prev = he2;
        lp->halfedges = he1;
    } else {
        LinkHEtoV(he2, he1, sv, lp);
    }

    return he1;
}

// 功能：以环上两个给定点，建造一条边和一个面
// 输入：sv-边的起点，ev-边的终点，lp-环
Loop *EulerOperation::mef(Vertex *sv, Vertex *ev, Loop *lp)
{
    Solid *s = lp->face->solid;
    Face *f = new Face;
    Loop *l = new Loop;
    HalfEdge *he1 = new HalfEdge, *he2 = new HalfEdge;
    Edge *e = new Edge;

    // 建立边、半边、顶点、体之间的关系
    SetUpEdge(e, sv, ev, he1, he2);
    AddEdgeIntoSolid(e, s);

    // 将两条新的半边加入环中    
    LinkHEtoV(he2, he1, sv, lp);
    LinkHEtoV(he1, he2, ev, lp);

    // 分离成两个独立的环
    lp->halfedges = he1;
    he1->loop = lp;
    l->halfedges = he2;
    he2->loop = l;
    for (HalfEdge *he = he2->next; he != he2 ;he = he->next) he->loop = l;

    f->loops = l;
    AddFaceIntoSolid(f, s);

    return l;
}

// 功能：以环上两个给定点，删除该边建造一个内环
// 输入：sv-边的起点(外环、内环)，ev-边的终点(内环)，lp-环
Loop *EulerOperation::kemr(Vertex *sv, Vertex *ev, Loop *lp) {
    Solid *s = lp->face->solid;
    Face *f = lp->face;
    Loop *l = new Loop;

    HalfEdge *he_t, *he_a, *he_b;
    // 找到连接内外环的两条半边
    while(he_t->startv != sv || he_t->endv != ev) he_t = he_t->next;
    he_a = he_t;
    while(he_t->startv != ev || he_t->endv != sv) he_t = he_t->next;
    he_b = he_t;

    // 分离成两个loop
    he_a->prev->next = he_b->next;
    he_b->prev->next = he_a->next;
    lp->halfedges = he_a->prev;
    l->halfedges = he_b->prev;

    // 把内环加入链表中
    Loop *lp_t = f->loops;
    while (lp_t->next != NULL) lp_t = lp_t->next;
    lp_t->next = l;
    l->prev = lp_t;

    delete he_a;
    delete he_b;

    return l;
}

// 功能：将一条半边添加到体的半边链表中
inline void EulerOperation::AddEdgeIntoSolid(Edge *e, Solid *s)
{
    Edge *e_t = s->edges;
    if (e_t == NULL) s->edges = e;
    else {
        while (e_t->next != NULL) e_t = e_t->next;
        e_t->next = e;
        e->prev = e_t;
    }
}

// 功能：将一个面添加到体的面链表中
inline void EulerOperation::AddFaceIntoSolid(Face *f, Solid *s)
{
    Face *f_t = s->faces;
    if(f_t == NULL) s->faces = f;
    else {
        while (f_t->next != NULL) f_t = f_t->next;
        f_t->next = f;
        f->prev = f_t;
    }
    f->solid = s;
}

// 功能：建立与边、半边、顶点间的联系
inline void EulerOperation::SetUpEdge(Edge *e, Vertex *sv, Vertex *ev, HalfEdge *he1, HalfEdge *he2)
{
    // 设置半边的起点和终点
    he1->startv = he2->endv = sv;
    he1->endv = he2->startv = ev;

    // 建立半边-边关系，并把边添加到体中
    e->he_l = he2->adj = he1;
    e->he_r = he1->adj = he2;
    he1->edge = he2->edge = e;
}

// 功能：将两条半边的链接到指定环的一个顶点上
// 输入：he_i-该顶点入边，he_o-该顶点出边，v-顶点，lp-顶点所在环
inline void EulerOperation::LinkHEtoV(HalfEdge *he_i, HalfEdge *he_o, Vertex *v, Loop *lp)
{
    HalfEdge *he_t = lp->halfedges;
    // 在环上搜索以该顶点为终点的半边，重新设置半边的指向关系
    while (he_t->endv != v) he_t = he_t->next;
    he_i->next = he_t->next;
    he_t->next->prev = he_i->next;
    he_t->next = he_o;
    he_o->prev = he_t;
}

