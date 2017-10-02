/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Mesh.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 02/10/2017
\brief
  Contains the interface for the Mesh class.
*/
/*****************************************************************************/
#pragma once

#include <string>
#include <vector>

#include "../Math/Vector3.h"

/*****************************************************************************/
/*!
\class Mesh
\brief
  Can be used to load model file types for vertex and index buffer data.
*/
/*****************************************************************************/
class Mesh
{
private:
  struct Vertex
  {
    Vertex() {}
    union {
      struct {
        float x, y, z;
        float nx, ny, nz;
      };
      float value[6];
    };
  };
  struct Face
  {
    Face() {}
    union {
      struct {
        unsigned a, b, c;
      };
      unsigned index[3];
    };
  };
  struct Line
  {
    Line() {}
    float sx, sy, sz;
    float ex, ey, ez;
  };
public:
  enum FileType
  {
    OBJ,
    NUM_FILETYPE
  };
public:
  Mesh(const std::string & file_name, FileType type);
  unsigned VertexCount();
  unsigned FaceCount();
  void * VertexData();
  void * IndexData();
  unsigned IndexDataSize();
  unsigned VertexDataSizeBytes();
  unsigned IndexDataSizeBytes();
  void * VertexNormalLineData();
  unsigned VertexNormalLineSizeBytes();
  unsigned Temp()
  {
    return _vertexNormalLines.size() * 2;
  }
private:
  void CalculateFaceNormals();
  void CalculateVertexNormals(std::vector<std::vector<unsigned> > * adjacencies);
  bool RemoveParallelFace(std::vector<unsigned> & vert_adjacencies);
  void CreateAdjacencyList(std::vector<std::vector<unsigned> > * adjacencies);
  void LoadObj(const std::string & file_name);
  std::vector<Vertex> _vertices;
  std::vector<Face> _faces;
  std::vector<Math::Vector3> _faceNormals;

  std::vector<Line> _vertexNormalLines;
  std::vector<Line> _faceNormalLines;

};