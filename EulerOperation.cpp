#include "EulerOperation.h"
#include <cmath>
using namespace std;

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
    if (lp->halfedges == NULL) {
        he2->next = he1;
        he1->prev = he2;
        lp->halfedges = he1;
    } else {
        HalfEdge *he_t = lp->halfedges;
        // 在环上搜索以新加边的起始点为终点的半边，设置新加半边的指向关系
        while (he_t->endv != sv) he_t = he_t->next;
        he2->next = he_t->next;
        he_t->next->prev = he2->next;
        he_t->next = he1;
        he1->prev = he_t;
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

    // 在环上搜索以sv,ev为终点的半边，重新设置半边的指向关系, 将两条新的半边加入环中
    HalfEdge *he_t = lp->halfedges, *he_a, *he_b;
    while (he_t->endv != sv) he_t = he_t->next;
    he_a = he_t;
    while (he_t->endv != ev) he_t = he_t->next;
    he_b = he_t;

    he2->next = he_a->next;
    he_a->next->prev = he2;
    he_a->next = he1;
    he1->prev = he_a;

    he1->next = he_b->next;
    he_b->next->prev = he1;
    he_b->next = he2;
    he2->prev = he_b;

    // 分离成两个独立的环   
    lp->halfedges = he1;
    he1->loop = lp;
    l->halfedges = he2;
    he2->loop = l;
    for (HalfEdge *he = he2->next; he != he2 ;he = he->next) he->loop = l;

    AddLoopIntoFace(l, f);
    AddFaceIntoSolid(f, s);

    return l;
}

// 功能：以环上两个给定点，删除该边建造一个内环
// 输入：sv-边的起点(外环、内环)，ev-边的终点(内环)，lp-环
Loop *EulerOperation::kemr(Vertex *sv, Vertex *ev, Loop *lp)
{
    Face *f = lp->face;
    Loop *l = new Loop;

    HalfEdge *he_t, *he_a, *he_b;
    // 找到连接内外环的两条半边
    he_t = lp->halfedges;
    while(he_t->startv != sv || he_t->endv != ev) he_t = he_t->next;
    he_a = he_t;
    while(he_t->startv != ev || he_t->endv != sv) he_t = he_t->next;
    he_b = he_t;

    // 分离成两个loop
    he_a->prev->next = he_b->next;
    he_b->next->prev = he_a->prev;
    he_b->prev->next = he_a->next;
    he_a->next->prev = he_b->prev;
    lp->halfedges = he_a->prev;
    l->halfedges = he_b->prev;

    // 把内环加入面的环链表中
    AddLoopIntoFace(l, f);

    delete he_a;
    delete he_b;

    return l;
}

// 功能：删除一个面，将其变成一个内环，同时形成一个柄
// 输入：f1-添加内环的面，f2-删除的面
void EulerOperation::kfmrh(Face *f1, Face *f2)
{
    Solid *s = f1->solid;
    Face *f = s->faces;
    Loop *l = f2->loops;

    // 把内环加入f1面的内环链表中
    AddLoopIntoFace(l, f1);

    // 将面从体的面表中删除
    if (f == f2) {
        s->faces = f->next;
        s->faces->prev = NULL;
    } else {
        while (f->next != f2 && f->next != NULL) f = f->next;
        f->next = f->next->next;
        f->next->prev = f;
    }

    delete f2;
}

// 平移扫成操作
// 输入：f-扫成面，dir-扫成方向，d-扫成距离
void EulerOperation::Sweep(Face *f, double dir[3], double d)
{
    Loop *l = f->loops, *lp, *lp_out = NULL;
    Vertex *v_first, *v_next, *v_up, *v_up_pre, *v_up_first;
    HalfEdge *he;
    double p[3];

    while (l != NULL) {
        // 建造环上第一个点和其向上扫成的up点之间的边
        he = l->halfedges;
        lp = he->adj->loop;
        v_first = he->startv;
        p[0] = v_first->vcoord[0] + d*dir[0];
        p[1] = v_first->vcoord[1] + d*dir[1];
        p[2] = v_first->vcoord[2] + d*dir[2];
        v_up_first = v_up_pre = mev(v_first, p, lp)->endv;

        // 循环扫成环上的其他点
        v_next = (he = he->next)->startv;
        while (v_next != v_first) {
            // 建造环上点和其向上扫成的up点之间的边
            p[0] = v_next->vcoord[0] + d*dir[0];
            p[1] = v_next->vcoord[1] + d*dir[1];
            p[2] = v_next->vcoord[2] + d*dir[2];
            v_up = mev(v_next, p, lp)->endv;
            // 链接当前up点和前一个up点构造侧面
            mef(v_up, v_up_pre, lp);

            v_up_pre = v_up;
            v_next = (he = he->next)->startv;
        }
        mef(v_up_first, v_up_pre, lp);

        // 内环建造柄
        if (lp_out == NULL) lp_out = lp;
        else kfmrh(lp_out->face, lp->face);

        l = l->next;
    }
}

// 贝塞尔平移扫成操作
// 输入：f-扫成面，path-贝塞尔曲线的控制顶点
void EulerOperation::BezierSweep(Face *f, vector<Point3D> path) {
    double degree = path.size() - 1;

    // 求取path所定义的bezier曲线的一阶导数和二阶导数的Control Point
    vector<Point3D> path_der;
    for(int i = 1; i < path.size(); i++)
        path_der.push_back(degree*(path[i] - path[i-1]));

    vector<Point3D> p, p_der;   // path path_der上的采样点值
    vector<Point3D> N, BN;
    int tess_num = 500;
    double delta_t = 10e-5, u_cur;
    for (int i = 0; i < tess_num; i++) {
        u_cur = (double)i/(double)tess_num;
        // 求取控制曲线上采样点的位置、一阶导数
        p.push_back(deCasteljau(path, u_cur));
        p_der.push_back(deCasteljau(path_der, u_cur));
        // 求取控制曲线上采样点的主法向和副法向
        Point3D p1 = deCasteljau(path_der, u_cur - delta_t).normalized();
        Point3D p2 = deCasteljau(path_der, u_cur + delta_t).normalized();
        N.push_back((p2-p1)/(2*delta_t));
        // 处理法向翻转
        if (i > 0) {
            if (N[i].dot(N[i-1]) < 0) N[i] = -N[i];
            if (N[i].norm() < 10e-6) N[i] = BN[i-1].cross(p_der[i]);
        }
        N[i] = N[i].normalized();
        BN.push_back(p_der[i].cross(N[i]).normalized());
    }

    vector<Vertex*> vertexs;    // 缓存扫成面上所有的顶点
    vector<Loop*> loops;        // 缓存扫成面上所有的loop
    vector<vector<Point3D> > paths;     // 每个顶点所属控制曲线上的采样点数组
    double theta_cos, theta_sin, dist;

    Loop *l = f->loops;
    Vertex *v_first, *v_next;
    HalfEdge *he;
    Point3D p_cur, p_new;
    // 求取各顶点所属新控制曲线上的控制点
    while (l != NULL) {
        he = l->halfedges;
        loops.push_back(he->adj->loop); // 缓存扫成loop
        v_next = v_first = he->startv;
        while (true) {
            vertexs.push_back(v_next);  // 缓存扫成点
            vector<Point3D> path_cur;
            p_cur = Point3D(v_next->vcoord[0], v_next->vcoord[1], v_next->vcoord[2]);
            // 计算扫成点与主副法向间的夹角，以及与曲线间的距离
            theta_cos = (p_cur - path[0]).normalized().dot(N[0]);
            theta_sin = (p_cur - path[0]).normalized().dot(BN[0]);
            dist = (p_cur - path[0]).norm();
            path_cur.push_back(p_cur);
            // 计算上新的曲线上的采样点
            for (int i = 1; i < tess_num; i++) {
                p_new = p[i] + dist * (N[i]*theta_cos + BN[i]*theta_sin);
                path_cur.push_back(p_new);
            }
            paths.push_back(path_cur);
            v_next = (he = he->next)->startv;
            if(v_next == v_first) break;
        }
        l = l->next;
    }

    int v_index = 0, v_first_index = 0, l_index = 0;
    // 循环细分曲面
    for (int i = 0; i < tess_num; i++) {
        v_index = l_index = 0;
        l = f->loops;
        while (l != NULL) {
            // 建造环上第一个点和其向上扫成的up点之间的边
            he = l->halfedges;
            v_first = he->startv;
            v_first_index = v_index;
            // 根据当前点所属的控制曲线计算细分的点
            p_cur = paths[v_index][i];
            vertexs[v_index] = mev(vertexs[v_index], p_cur.data(), loops[l_index])->endv;

            // 循环扫成环上的其他点
            v_next = (he = he->next)->startv;
            while (v_next != v_first) {
                v_index++;
                // 建造环上点和其向上扫成的up点之间的边
                p_cur = paths[v_index][i];
                vertexs[v_index] = mev(vertexs[v_index], p_cur.data(), loops[l_index])->endv;
                // 链接当前up点和前一个up点构造侧面
                mef(vertexs[v_index], vertexs[v_index-1], loops[l_index]);
                v_next = (he = he->next)->startv;
            }
            mef(vertexs[v_first_index], vertexs[v_index], loops[l_index]);
            v_index++;

            // 内环建造柄
            if (i == tess_num - 1 && l_index > 0)
                kfmrh(loops[0]->face, loops[l_index]->face);

            l = l->next;
            l_index++;
        }
    }
}

// 功能：获取控制曲线P上u位置点的值
Point3D EulerOperation::deCasteljau(vector<Point3D> &P, double u)  {
    int n = P.size();
    vector<Point3D> Q;
    Q.resize(n);
    for (int i = 0; i < n; i++)
        Q[i] = P[i];
    for (int j = 1; j < n; j++)
        for(int i = 0; i < n - j; i++)
            Q[i] = (1 - u)*Q[i] + u*Q[i + 1];

    return Q[0];
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

// 功能：将一个环添加到面的环链表中
inline void EulerOperation::AddLoopIntoFace(Loop *l, Face *f)
{
    Loop *lp_t = f->loops;
    if (lp_t == NULL) {
        f->loops = l;
    } else {
        while (lp_t->next != NULL) lp_t = lp_t->next;
        lp_t->next = l;
        l->prev = lp_t;
    }
    l->face = f;
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
