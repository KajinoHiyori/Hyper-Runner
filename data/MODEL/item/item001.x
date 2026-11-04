xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 24;
 -7.15935;10.00000;-3.80000;,
 7.15935;10.00000;-3.80000;,
 10.00000;3.33333;-5.30775;,
 -10.00000;3.33333;-5.30775;,
 10.00000;-3.33333;-5.30775;,
 -10.00000;-3.33333;-5.30775;,
 4.09519;-10.00000;-2.17362;,
 -4.09519;-10.00000;-2.17362;,
 7.15935;10.00000;3.80000;,
 10.00000;3.33333;5.30775;,
 10.00000;-3.33333;5.30775;,
 4.09519;-10.00000;2.17362;,
 -7.15935;10.00000;3.80000;,
 -10.00000;3.33333;5.30775;,
 -10.00000;-3.33333;5.30775;,
 -4.09519;-10.00000;2.17362;,
 -7.15935;10.00000;3.80000;,
 7.15935;10.00000;3.80000;,
 7.15935;10.00000;-3.80000;,
 -7.15935;10.00000;-3.80000;,
 -4.09519;-10.00000;-2.17362;,
 4.09519;-10.00000;-2.17362;,
 4.09519;-10.00000;2.17362;,
 -4.09519;-10.00000;2.17362;;
 
 14;
 4;0,1,2,3;,
 4;3,2,4,5;,
 4;5,4,6,7;,
 4;1,8,9,2;,
 4;2,9,10,4;,
 4;4,10,11,6;,
 4;8,12,13,9;,
 4;9,13,14,10;,
 4;10,14,15,11;,
 4;12,0,3,13;,
 4;13,3,5,14;,
 4;14,5,7,15;,
 4;16,17,18,19;,
 4;20,21,22,23;;
 
 MeshMaterialList {
  3;
  14;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  1,
  2;;
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "data\\TEXTURE\\corn.jpg";
   }
  }
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "data\\TEXTURE\\corn.jpg";
   }
  }
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "data\\TEXTURE\\corn.jpg";
   }
  }
 }
 MeshNormals {
  17;
  0.000000;0.220590;-0.975366;,
  0.000000;0.110981;-0.993823;,
  0.000000;-0.217965;-0.975957;,
  0.000000;-0.425449;-0.904982;,
  0.919967;0.391996;0.000000;,
  0.979788;0.200041;0.000000;,
  0.935036;-0.354552;-0.000000;,
  0.410464;-0.911877;-0.000000;,
  0.000000;0.220590;0.975366;,
  0.000000;0.110981;0.993823;,
  0.000000;-0.217965;0.975957;,
  0.000000;-0.425449;0.904982;,
  -0.919967;0.391996;0.000000;,
  -0.979788;0.200041;0.000000;,
  -0.935036;-0.354552;0.000000;,
  -0.410464;-0.911877;0.000000;,
  0.000000;1.000000;0.000000;;
  14;
  4;0,0,1,1;,
  4;1,1,2,2;,
  4;2,2,3,3;,
  4;4,4,5,5;,
  4;5,5,6,6;,
  4;6,6,7,7;,
  4;8,8,9,9;,
  4;9,9,10,10;,
  4;10,10,11,11;,
  4;12,12,13,13;,
  4;13,13,14,14;,
  4;14,14,15,15;,
  4;16,16,16,16;,
  4;15,7,7,15;;
 }
 MeshTextureCoords {
  24;
  0.142032;0.000000;,
  0.857967;0.000000;,
  1.000000;0.333333;,
  0.000000;0.333333;,
  1.000000;0.666667;,
  0.000000;0.666667;,
  0.704759;1.000000;,
  0.295241;1.000000;,
  0.857967;0.000000;,
  1.000000;0.333333;,
  1.000000;0.666667;,
  0.704759;1.000000;,
  0.142032;0.000000;,
  0.000000;0.333333;,
  0.000000;0.666667;,
  0.295241;1.000000;,
  0.142027;0.535500;,
  0.857962;0.535519;,
  0.857973;0.172124;,
  0.142038;0.172105;,
  0.295244;0.881659;,
  0.704762;0.881665;,
  0.704756;0.988698;,
  0.295238;0.988693;;
 }
}
