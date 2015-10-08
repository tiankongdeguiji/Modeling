#ifndef EULEROPERATION_H
#define EULEROPERATION_H

#include "brep_structure.h"

class EulerOperation
{
public:
    EulerOperation();

    Solid *mvfs(Vertex *&newv, double p[3]);
    HalfEdge *mev(Vertex *sv, double p[3], Loop *lp);
    Loop *mef(Vertex *sv, Vertex *ev, Loop *lp);
    Loop *kemr(Vertex *sv, Vertex *ev, Loop *lp);
    void kfmrh(Face *f1, Face *f2);
};

#endif // EULEROPERATION_H
