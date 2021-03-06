/*
 *标准1X1方块的
 * Author:Gxin
 */

#ifndef BOXMESH_H
#define BOXMESH_H

//#include <GL/glew.h>
#include "glextensions.h"

#include <QtWidgets>
#include <QtOpenGL>

#include "glmeshs.h"
#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>
#include "glbuffers.h"
#include "block.h"

struct P3T2N3Vertex
{
    QVector3D position;
    QVector3D texCoord;
    QVector3D normal;
    static VertexDescription description[];
};

class ChunkMesh:public GLQuadMesh<P3T2N3Vertex, unsigned short>
{
public:
    explicit ChunkMesh(int nFace);
    void addFace(Face *face);
private:
    int vidx,iidx;
};

class LineMesh:public GLLineMesh<P3T2N3Vertex, unsigned short>
{
public:
    explicit LineMesh(int nLine);
    void addLine(const QVector3D &aPoint,const QVector3D &bPoint);
    void clear();

private:
    int vidx,iidx;
};

#endif // BOXMESH_H
