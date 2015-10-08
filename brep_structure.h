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

    Solid():prev(NULL), next(NULL), faces(NULL), edges(NULL) {}
};

struct Face
{
    Face *prev;
    Face *next;

    Loop *loops;
    Solid *solid;

    Face():prev(NULL), next(NULL), loops(NULL), solid(NULL) {}
};

struct Loop
{
    Loop *prev;
    Loop *next;

    HalfEdge *halfedges;
    Face *face;

    Loop():prev(NULL), next(NULL), halfedges(NULL), face(NULL) {}
};

struct HalfEdge
{
    HalfEdge *prev;
    HalfEdge *next;
    HalfEdge *adj;

    Vertex *startv;
    Vertex *endv;
    Loop *loop;
    Edge *edge;

    HalfEdge():prev(NULL), next(NULL), adj(NULL), startv(NULL), endv(NULL), loop(NULL), edge(NULL) {}
};

struct Edge
{
    Edge *prev;
    Edge *next;

    HalfEdge *he_l;
    HalfEdge *he_r;

    Edge():prev(NULL), next(NULL), he_l(NULL), he_r(NULL) {}
};

struct Vertex
{
    Vertex *prev;
    Vertex *next;

    double vcoord[3];

    Vertex (double coord[3]) : prev(NULL), next(NULL) {
        vcoord[0] = coord[0];
        vcoord[1] = coord[1];
        vcoord[2] = coord[2];
    }
};

#endif // BREP_STRUCTURE

