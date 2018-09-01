

#include <Windows.h>
#include "osgViewer/Viewer"
#include "osg/ShapeDrawable"
#include "osg/Geode"
#include "osg/TriangleFunctor"
#include <iostream>
#include <osgUtil/Tessellator>
#include "osgUtil/SmoothingVisitor"

//#include "glut.h"


/*
1. We will implement the template argument as a structure including an operator() .
  The first three 3D vector parameters represent the triangle vertices, and the last  one indicates whether these vertices come from a temporary vertex array or not:
  The osg::TemplatePrimitiveFunctor<T> not only collects triangle faces of a certain  drawable; it also has interfaces for obtaining a point, a line, and a quadrangle. It requires  implementations of these operators in the template argument:
*/
struct FaceCollector
{
  void operator()( const osg::Vec3& v1, const osg::Vec3& v2,
    const osg::Vec3& v3, bool )
  {
    std::cout << "Face vertices: " <<( float)v1.x()<<( float)v1.y()<<( float)v1.z() << "; "
      <<( float)v2.x()<<( float)v2.y()<<( float)v2.z() << "; "
     << ( float)v3.x()<<( float)v3.y()<<( float)v3.z() << "; "
      << std::endl;
  }
};

/*
class TeapotDrawable : public osg::Drawable
{
public:
  TeapotDrawable( float size=1.0f ) : _size(size) {}
  TeapotDrawable( const TeapotDrawable& copy,
    const osg::CopyOp&
    copyop=osg::CopyOp::SHALLOW_COPY )
    : osg::Drawable(copy, copyop), _size(copy._size) {}
  META_Object( osg, TeapotDrawable );
   
  virtual osg::BoundingBox computeBound() const;
  virtual void drawImplementation( osg::RenderInfo& ) const;
protected:
  float _size;
};



osg::BoundingBox TeapotDrawable::computeBound() const
{
  osg::Vec3 min(-_size,-_size,-_size), max(_size, _size, _size);
  return osg::BoundingBox(min, max);
}

void TeapotDrawable::drawImplementation( osg::RenderInfo&
  renderInfo ) const
{
  glFrontFace( GL_CW );
  glutSolidTeapot( _size );
  glFrontFace( GL_CCW );
}

*/




/************************************************************************/
/*jerrryzfc*/
/*8/8/2018 16:10:44*/
/* 01 basic geometry                                                                     */
/************************************************************************/
osg::ref_ptr<osg::Node> GenSceneNodeBasic()
{

  osg::ref_ptr<osg::Geode> root=new osg::Geode;
  osg::ref_ptr<osg::ShapeDrawable> shape1=new osg::ShapeDrawable;
  shape1->setShape(new osg::Box(osg::Vec3(-30.0f,0.0f,0.0f),1.0,2.0,0.8));
  shape1->setColor(osg::Vec4(1.0,1.0,0.0,1.0));

  osg::ref_ptr<osg::ShapeDrawable> shape2=new osg::ShapeDrawable();
  std::cout<<"shape2 init count:"<< shape2->referenceCount() <<std::endl;
  shape2->setShape(new osg::Cone(osg::Vec3(10.0,0.0,0.0),15.0,3.0));
  std::cout<<"shape2 setShape count:"<< shape2->referenceCount() <<std::endl;

  shape2->setColor(osg::Vec4(0.0,1.0,0.0,1.0));
  shape2->setName("zfcdageCone");
  //shape2->setUserData();

  std::cout<<"root init count:"<< root->referenceCount() <<std::endl;

  
  root->addChild(shape2); 

  std::cout<<"root shape2 count:"<< root->referenceCount() <<std::endl;

  root->addChild(shape1);
  std::cout<<"root shape1 count:"<< root->referenceCount() <<std::endl;


  

  root->addChild(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0,0.0,0.0),5)));

 std::cout<<"root addChild count:"<< root->referenceCount() <<std::endl;

  return root;


}

/************************************************************************/
/*jerrryzfc*/
/*8/8/2018 16:37:54*/
/* 02 colored quad 
类似opengl 的定点数组 法向量 颜色向量等输入每一个确定点来绘制图形的情况
*/
/************************************************************************/
osg::ref_ptr<osg::Node> GenSceneNodeVertexArray(){

   osg::ref_ptr<osg::Geode> root=new osg::Geode;


   osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
   vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
   vertices->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
   vertices->push_back( osg::Vec3(1.0f, 0.0f, 1.0f) );
   vertices->push_back( osg::Vec3(0.0f, 0.0f, 1.0f) );

   osg::ref_ptr<osg::Vec3Array> normals=new osg::Vec3Array;

   normals->push_back(osg::Vec3(0.0,-1.0f,0.0f));

   osg::ref_ptr<osg::Vec4Array> colors=new osg::Vec4Array;

   colors->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
   colors->push_back( osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) );
   colors->push_back( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );
   colors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

   osg::ref_ptr<osg::Geometry> quad=new osg::Geometry;
   quad->setVertexArray(vertices.get());
   quad->setColorArray(colors.get());
   quad->setNormalArray(normals.get());
   quad->setNormalBinding(osg::Geometry::BIND_OFF);
   quad->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

   quad->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));

   root->addDrawable(quad);

     
   return root;

}

/************************************************************************/
/*jerrryzfc*/
/*8/8/2018 17:35:01*/
/* octahedron indexing  点加上点的索引
类似opengl 的定点数组 法向量 颜色向量等输入点索引来来绘制图形的情况
*/
/************************************************************************/
osg::ref_ptr<osg::Node> GenSceneNodeIndexing(){

  osg::ref_ptr<osg::Geode> root=new osg::Geode;


  osg::ref_ptr<osg::Vec3Array> vertices=new osg::Vec3Array(6);

  (*vertices)[0].set( 0.0f, 0.0f, 1.0f);
  (*vertices)[1].set(-0.5f,-0.5f, 0.0f);
  (*vertices)[2].set( 0.5f,-0.5f, 0.0f);
  (*vertices)[3].set( 0.5f, 0.5f, 0.0f);
  (*vertices)[4].set(-0.5f, 0.5f, 0.0f);
  (*vertices)[5].set( 0.0f, 0.0f,-1.0f);

   osg::ref_ptr<osg::DrawElementsUInt> indices=new osg::DrawElementsUInt(GL_TRIANGLES,24);

   (*indices)[0] = 0; (*indices)[1] = 1; (*indices)[2] = 2;
   (*indices)[3] = 0; (*indices)[4] = 2; (*indices)[5] = 3;
   (*indices)[6] = 0; (*indices)[7] = 3; (*indices)[8] = 4;
   (*indices)[9] = 0; (*indices)[10]= 4; (*indices)[11]= 1;
   (*indices)[12]= 5; (*indices)[13]= 2; (*indices)[14]= 1;
   (*indices)[15]= 5; (*indices)[16]= 3; (*indices)[17]= 2;
   (*indices)[18]= 5; (*indices)[19]= 4; (*indices)[20]= 3;
   (*indices)[21]= 5; (*indices)[22]= 1; (*indices)[23]= 4;


   osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
   geom->setVertexArray( vertices.get() );
   geom->addPrimitiveSet( indices.get() );
   osgUtil::SmoothingVisitor::smooth( *geom );


    
   root->addDrawable( geom.get() );
   






  return root;

}


/************************************************************************/
/*jerrryzfc*/
/*8/8/2018 19:48:25*/
/* tessellating a polygon                                                                     */
/************************************************************************/
osg::ref_ptr<osg::Node> GenSceneNodeTessellating(){

osg::ref_ptr<osg::Geode> root=new osg::Geode;

osg::ref_ptr<osg::Vec3Array> vertices=new osg::Vec3Array;

vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
vertices->push_back( osg::Vec3(2.0f, 0.0f, 0.0f) );
vertices->push_back( osg::Vec3(2.0f, 0.0f, 1.0f) );
vertices->push_back( osg::Vec3(1.0f, 0.0f, 1.0f) );
vertices->push_back( osg::Vec3(1.0f, 0.0f, 2.0f) );
vertices->push_back( osg::Vec3(2.0f, 0.0f, 2.0f) );

vertices->push_back( osg::Vec3(2.0f, 0.0f, 3.0f) );
vertices->push_back( osg::Vec3(0.0f, 0.0f, 3.0f) );


osg::ref_ptr<osg::Vec3Array> normals=new osg::Vec3Array;
normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );

osg::ref_ptr<osg::Geometry> geom=new osg::Geometry;


geom->setVertexArray(vertices.get());
geom->setNormalArray(normals.get());
geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
geom->addPrimitiveSet(new osg::DrawArrays(GL_POLYGON,0,vertices.get()->size()));

/************************************************************************/
/*jerrryzfc*/
/*8/8/2018 19:59:49*/
/* To render the concave polygon correctly, we should use an
osgUtil::Tessellator to re-tessellate it:
A concave polygon without any tessellations will not be rendered as we expect in most cases.
In order to optimize performance, OpenGL will treat them as simple polygons or just ignore
them, and this always generates unexpected results.
The osgUtil::Tessellator uses OpenGL tessellation routines to process concave
polygons saved in osg::Geoemtry objects. It decides on the most efficient primitive type
while performing tessellation. For the previous case, it will use GL_TRIANGLE_STRIP to
triangulate the original polygon, that is, to separate it into a few triangles.

*/
/************************************************************************/
osgUtil::Tessellator tessellator;
tessellator.retessellatePolygons( *geom );

root->addDrawable(geom.get());






return root;

}

/************************************************************************/
/*jerrryzfc*/
/*8/25/2018 13:53:33*/
/* Customizing a primitive functor
实现获取点线面信息
2. We will create a wall-like object by using GL_QUAD_STRIP , which means that the
geometry was not originally formed by triangles. This object includes eight vertices
and four quad faces:
*/
/************************************************************************/
osg::ref_ptr<osg::Node> GenSceneNodeGetGeoInfo(){

osg::ref_ptr<osg::Geode> root=new osg::Geode;


osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
vertices->push_back( osg::Vec3(0.0f, 0.0f, 1.0f) );
vertices->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
vertices->push_back( osg::Vec3(1.0f, 0.0f, 1.5f) );
vertices->push_back( osg::Vec3(2.0f, 0.0f, 0.0f) );
vertices->push_back( osg::Vec3(2.0f, 0.0f, 1.0f) );
vertices->push_back( osg::Vec3(3.0f, 0.0f, 0.0f) );
vertices->push_back( osg::Vec3(3.0f, 0.0f, 1.5f) );
vertices->push_back( osg::Vec3(4.0f, 0.0f, 0.0f) );
vertices->push_back( osg::Vec3(4.0f, 0.0f, 1.0f) );
osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
geom->setVertexArray( vertices.get() );
geom->setNormalArray( normals.get() );
geom->setNormalBinding( osg::Geometry::BIND_OVERALL );
geom->addPrimitiveSet( new osg::DrawArrays(GL_QUAD_STRIP, 0, 10));


osg::TriangleFunctor<FaceCollector> functor;
geom->accept( functor );


root->addDrawable( geom.get() );



return root;

}

 


/************************************************************************/
/*jerrryzfc*/
/*8/25/2018 14:19:16*/
/* Using OpenGL drawing calls    teapot                                                                 
osg::ref_ptr<osg::Node> GenSceneNode(){

osg::ref_ptr<osg::Geode> root=new osg::Geode;





return root;




}
*/
/************************************************************************/









osg::ref_ptr<osg::Node> GenSceneNode(){

osg::ref_ptr<osg::Geode> root=new osg::Geode;

return root;

}



/*

osg::ref_ptr<osg::Node> GenSceneNode(){

osg::ref_ptr<osg::Geode> root=new osg::Geode;

return root;

}


osg::ref_ptr<osg::Node> GenSceneNode(){

osg::ref_ptr<osg::Geode> root=new osg::Geode;

return root;



}


osg::ref_ptr<osg::Node> GenSceneNode(){

osg::ref_ptr<osg::Geode> root=new osg::Geode;

return root;

}

osg::ref_ptr<osg::Node> GenSceneNode(){

osg::ref_ptr<osg::Geode> root=new osg::Geode;

return root;

}

*/










int main(_In_ int _Argc, _In_count_(_Argc) _Pre_z_ char ** _Argv, _In_z_ char ** _Env)
{
  osg::ref_ptr<osgViewer::Viewer> viewer=new osgViewer::Viewer;



  viewer->setSceneData(GenSceneNode().get());

  viewer->realize();
 osgViewer::GraphicsWindow* gwindow= static_cast<osgViewer::GraphicsWindow*> ( viewer->getCamera()->getGraphicsContext());

 if (gwindow)
 {

   gwindow->setWindowRectangle(15,15,800,600);

 }


  viewer->run();

   std::cout<<"root addChild count:"<< viewer->referenceCount() <<std::endl;

  return 0;



}








