/*
 * 绘图网络，自建的VBO网络
 */
#ifndef GLTRIANGLEMESH_H
#define GLTRIANGLEMESH_H

//#include <GL/glew.h>
#include "glextensions.h"

#include <QtWidgets>
#include <QtOpenGL>

#include "glbuffers.h"

//三角形绘图网络
template<class TVertex, class TIndex>
class GLTriangleMesh
{
public:
    GLTriangleMesh(int vertexCount, int indexCount) : m_vb(vertexCount), m_ib(indexCount)
    {
    }

    virtual ~GLTriangleMesh()
    {
    }

    virtual void draw()
    {
        if (failed())
            return;

        int type = GL_UNSIGNED_INT;
        if (sizeof(TIndex) == sizeof(char)) type = GL_UNSIGNED_BYTE;
        if (sizeof(TIndex) == sizeof(short)) type = GL_UNSIGNED_SHORT;

        m_vb.bind();
        m_ib.bind();
        glDrawElements(GL_TRIANGLES, m_ib.length(), type, BUFFER_OFFSET(0));          //绘制三角形网络
        m_vb.unbind();
        m_ib.unbind();
    }

    bool failed()
    {
        return m_vb.failed() || m_ib.failed();
    }
protected:
    GLVertexBuffer<TVertex> m_vb;
    GLIndexBuffer<TIndex> m_ib;
};

//四边形绘图网络
template<class TVertex, class TIndex>
class GLQuadMesh
{
public:
    GLQuadMesh(int vertexCount, int indexCount) : m_vb(vertexCount), m_ib(indexCount)
    {
    }

    virtual ~GLQuadMesh()
    {
    }

    virtual void draw()
    {
        if (failed())
            return;

        int type = GL_UNSIGNED_INT;
        if (sizeof(TIndex) == sizeof(char)) type = GL_UNSIGNED_BYTE;
        if (sizeof(TIndex) == sizeof(short)) type = GL_UNSIGNED_SHORT;

        m_vb.bind();
        m_ib.bind();
        glDrawElements(GL_QUADS, m_ib.length(), type, BUFFER_OFFSET(0));                    //绘制四边形网络
        m_vb.unbind();
        m_ib.unbind();
    }

    bool failed()
    {
        return m_vb.failed() || m_ib.failed();
    }
protected:
    GLVertexBuffer<TVertex> m_vb;
    GLIndexBuffer<TIndex> m_ib;
};


#endif
