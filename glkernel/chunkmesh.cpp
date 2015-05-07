#include "chunkmesh.h"
#include <cstring>
//============================================================================//
//                                P3T2N3Vertex                                //
//============================================================================//

/*
VertexDescription P3T2N3Vertex::description[] = {
    {VertexDescription::Position, GL_FLOAT, SIZE_OF_MEMBER(P3T2N3Vertex, position) / sizeof(float), 0, 0},
    {VertexDescription::TexCoord, GL_FLOAT, SIZE_OF_MEMBER(P3T2N3Vertex, texCoord) / sizeof(float), sizeof(QVector3D), 0},
    {VertexDescription::Normal, GL_FLOAT, SIZE_OF_MEMBER(P3T2N3Vertex, normal) / sizeof(float), sizeof(QVector3D) + sizeof(QVector2D), 0},
    {VertexDescription::Null, 0, 0, 0, 0},
};
*/

VertexDescription P3T2N3Vertex::description[] = {
    {VertexDescription::Position, GL_FLOAT, SIZE_OF_MEMBER(P3T2N3Vertex, position) / sizeof(float), offsetof(P3T2N3Vertex, position), 0},
    {VertexDescription::TexCoord, GL_FLOAT, SIZE_OF_MEMBER(P3T2N3Vertex, texCoord) / sizeof(float), offsetof(P3T2N3Vertex, texCoord), 0},
    {VertexDescription::Normal, GL_FLOAT, SIZE_OF_MEMBER(P3T2N3Vertex, normal) / sizeof(float), offsetof(P3T2N3Vertex, normal), 0},
    {VertexDescription::Null, 0, 0, 0, 0},
};

//float lerp(float a, float b, float t)
//{
//    return a * (1.0f - t) + b * t;
//}

//逆时针绘图，纹理左上角起点，宽x，高y。

ChunkMesh::ChunkMesh(int nFace)
    :GLQuadMesh<P3T2N3Vertex,unsigned short>(4*nFace,4*nFace)
{
    vidx=iidx=0;

    //    P3T2N3Vertex *vp=m_vb.lock();
    //    unsigned short *ip=m_ib.lock();

    //    if(!vp || !ip){
    //        qWarning("ChunkMesh::ChunkMesh: Failed to lock vertex buffer and/or index buffer.");
    //        m_ib.unlock();
    //        m_vb.unlock();
    //        return;
    //    }

    //    m_ib.unlock();
    //    m_vb.unlock();
}

void ChunkMesh::addFace(Face *face)
{
    P3T2N3Vertex *vp=m_vb.lock();
    unsigned short *ip=m_ib.lock();
    if(!vp || !ip){
        qWarning("ChunkMesh::ChunkMesh: Failed to lock vertex buffer and/or index buffer.");
        m_ib.unlock();
        m_vb.unlock();
        return;
    }
    for(int i=0;i<4;i++){
        vp[vidx].position=face->vert[i].position;
        vp[vidx].normal=face->vert[i].normal;
        vp[vidx].texCoord=face->vert[i].texCoord;
        ip[iidx++]=vidx;
        vidx++;
    }
    m_ib.unlock();
    m_vb.unlock();
}


LineMesh::LineMesh(int nLine)
    :GLLineMesh<P3T2N3Vertex,unsigned short>(2*nLine,2*nLine)
{
    vidx=iidx=0;
}

void LineMesh::addLine(const QVector3D &aPoint, const QVector3D &bPoint)
{
    P3T2N3Vertex *vp=m_vb.lock();
    unsigned short *ip=m_ib.lock();
    if(!vp || !ip){
        qWarning("ChunkMesh::ChunkMesh: Failed to lock vertex buffer and/or index buffer.");
        m_ib.unlock();
        m_vb.unlock();
        return;
    }
    vp[vidx].position=aPoint;
    ip[iidx++]=vidx;
    vidx++;

    vp[vidx].position=bPoint;
    ip[iidx++]=vidx;
    vidx++;

    m_ib.unlock();
    m_vb.unlock();
}

