#ifndef BREP_STRUCTURE
#define BREP_STRUCTURE

#include <stdlib.h>

struct Solid;
struct Face;
struct Loop;
struct Edge;
struct HalfEdge;
struct Vertex;

struct Solid
{
    Solid *prev;
    Solid *next;

    Face *faces;
    Edge *edges;
};

struct Face
{
    Face *prev;
    Face *next;

    Loop *loops;
    Solid *solid;
};

struct Loop
{
    Loop *prev;
    Loop *next;

    HalfEdge *halfedges;
    Face *face;
};

struct HalfEdge
{
    HalfEdge *prev;
    HalfEdge *next;

    Vertex *startv;
//    Vertex *endv;
    Loop *loop;
    Edge *edge;
};

struct Edge
{
    Edge *prev;
    Edge *next;

    HalfEdge *he_l;
    HalfEdge *he_r;
};

struct Vertex
{
    Vertex *prev;
    Vertex *next;

    double vcoord[3];

    Vertex(double coord[3]) : prev(NULL), next(NULL)
    {
        vcoord[0] = coord[0];
        vcoord[1] = coord[1];
        vcoord[2] = coord[2];
    }
};

#endif // BREP_STRUCTURE

