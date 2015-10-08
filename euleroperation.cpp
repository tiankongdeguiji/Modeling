#include "euleroperation.h"

EulerOperation::EulerOperation()
{

}

// 功能：构造一个体，一个面，一个外环，一个点
// 输入：newv-新建的起始点的引用, p-起始点的坐标
// 输出：新的体
Solid *EulerOperation::mvfs(Vertex *&newv, double p[3])
{
    Solid *s = new Solid;
    Face *f = new Face;
    Loop *l = new Loop;
    Vertex *v = new Vertex(p);

    s->faces = f;
    f->solid = s;
    f->loops = l;
    l->face = f;

    newv = v;
    return s;
}

// 功能：在一个loop上添加一个顶点和一条边
// 输入：sv-新加边的起始点，p-新加边的终点坐标，lp-新加边所在的环
// 输出: 新加边的一个半边
HalfEdge *mev(Vertex *sv, double p[3], Loop *lp)
{
    HalfEdge *he1 = new HalfEdge, *he2 = new HalfEdge;
    Vertex *ev = new Vertex(p);
    Edge *e = new Edge;

    he1->startv = sv;
    he2->startv = ev;
    he1->edge = he2->edge = e;

    he1->next = he2;
    he1->loop = he2->loop = lp;

    if (lp->halfedges = NULL) {
        he2->next = he1;
        lp->halfedges = he1;
    } else {
        for (HalfEdge *he = lp->halfedges; he->next->startv != sv; he = he->next) {
            he2->next = he->next;
            he->next = he1;
        }
    }

    return he1;
}
