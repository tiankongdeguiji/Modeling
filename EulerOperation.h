#ifndef EULEROPERATION_H
#define EULEROPERATION_H

#include "BRep.h"
#include "Eigen/Dense"
#include "vector"
typedef Eigen::Vector3d Point3D;

class EulerOperation
{
private:
    void AddEdgeIntoSolid(Edge *e,Solid *s);
    void AddFaceIntoSolid(Face *f, Solid *s);
    void AddLoopIntoFace(Loop *l, Face *f);
    void SetUpEdge(Edge *e, Vertex *sv, Vertex *ev, HalfEdge *he1, HalfEdge *he2);

    double SearchCtrlNode(std::vector<Point3D> &P, int i);
    Point3D deCasteljau(std::vector<Point3D> &P, double u);

public:
    EulerOperation();

    Solid *mvfs(Vertex *&newv, double p[3]);
    HalfEdge *mev(Vertex *sv, double p[3], Loop *lp);
    Loop *mef(Vertex *sv, Vertex *ev, Loop *lp);
    Loop *kemr(Vertex *sv, Vertex *ev, Loop *lp);
    void kfmrh(Face *f1, Face *f2);

    void Sweep(Face *f, double dir[3], double d);
    void BezierSweep(Face *f, std::vector<Point3D> path);
};

#endif // EULEROPERATION_H
