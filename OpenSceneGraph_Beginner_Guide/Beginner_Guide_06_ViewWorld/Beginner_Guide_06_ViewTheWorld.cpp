/************************************************************************/
/*jerrryzfc*/
/*8/29/2018 18:26:14*/
/* From world to screen
When drawing a point, a line, or a complex polygon in the 3D world, our final goal is
to display it on the screen. That is, the 3D object that we are going to represent will be
converted to a set of pixels in a 2D window. In this process, three major matrices are used to
determine the transformations between different coordinate systems. These are often called
the model, view, and projection matrices.
The model matrix is used to describe the specific location of an object in the world. It can
transform vertices from an object's local coordinate system into world coordinate system.
Both coordinates are right-handed.
The next step is to transform the entire world into view space, by using the view matrix.
Suppose we have a camera placed at a certain position in the world; the inverse of the
camera's transformation matrix is actually used as the view matrix. In the right-handed view
coordinate system, OpenGL defines that the camera is always located at the origin (0, 0, 0),
and facing towards the negative Z axis. Hence, we can represent the world on our camera's
screen.
Note that, there is no separate model matrix or view matrix in OpenGL. However, it defines
a model-view matrix to transform from the object's local space to view space, which is a
combination of both matrices. Thus, to transform the vertex V in local space to Ve in view
space, we have:
Ve = V * modelViewMatrix
The next important work is to determine how 3D objects are projected onto the screen
(perspective or orthogonal), and calculate the frustum from which objects get rendered.
The projection matrix is used to specify the frustum in the world coordinate system with
six clipping planes: the left, right, bottom, top, near, and far planes. OpenGL also provides
an additional gluPerspective() function to determine a field of view with camera lens
parameters.
The resulting coordinate system (called the normalized device coordinate system) ranges
from -1 to +1 in each of the axes, and is changed to left-handed now. And as a final step, we
project all result data onto the viewport (the window), define the window rectangle in which
the final image is mapped, and the z value of the window coordinates. After that, the 3D
scene is rendered to a rectangular area on your 2D screen. And finally, the screen coordinate
Vs can represent the local vertex V in the 3D world by using the so called MVPW matrix,
that is:
Vs = V * modelViewMatrix * projectionMatrix * windowMatrix                                                                     */
/************************************************************************/

/************************************************************************/
/*jerrryzfc*/
/*8/29/2018 18:26:31*/
/*
世界坐标系 

模型局部坐标系 & 相机视图逆矩阵==视图坐标系 Ve = V * modelViewMatrix

投影坐标系 裁剪视图 远近高低 深度 得到  -1 to +1 的范围  得到设备坐标系 projectionMatrix

屏幕坐标系 Vs window viewport windowMatrix

【MVPW】
Vs = V * modelViewMatrix * projectionMatrix * windowMatrix

The Vs is still a 3D vector that represents a 2D pixel location with a depth value.
By reversing this mapping process, we can get a line in the 3D space from a 2D screen point
( Xs , Ys ). That's because the 2D point can actually be treated as two points: one on the near
clipping plane ( Zs = 0 ), and the other on the far plane ( Zs = 1 ).
The inverse matrix of MVPW is used here to obtain the result of the "unproject" work:
V0 = (Xs, Ys, 0) * invMVPW
V1 = (Xs, Ys, 1) * invMVPW


*/
/************************************************************************/


#include <Windows.h>
#include "osgViewer/GraphicsWindow"

#include "osg/Node"
#include "osg/Group"

#include <osgViewer/CompositeViewer>
#include <osg/Camera>
#include <osg/DisplaySettings>

#include "osg/Texture2D"

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>



#include"osgGA/TrackballManipulator"


#include <iostream>

class FindTextureVisitor : public osg::NodeVisitor
{
public:
  FindTextureVisitor( osg::Texture* tex ) : _texture(tex)
  {
    setTraversalMode(
      osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
  }
  virtual void apply( osg::Node& node );
  virtual void apply( osg::Geode& geode );
  void replaceTexture( osg::StateSet* ss );
protected:
  osg::ref_ptr<osg::Texture> _texture;
};

void FindTextureVisitor::replaceTexture( osg::StateSet* ss )
{
  if ( ss )
  {
    osg::Texture* oldTexture = dynamic_cast<osg::Texture*>(
      ss->getTextureAttribute(0,osg::StateAttribute::TEXTURE)
      );
    if ( oldTexture ) ss->setTextureAttribute(
      0,_texture.get() );
  }
}

void FindTextureVisitor::apply( osg::Node& node )
{
  replaceTexture( node.getStateSet() );
  traverse( node );
}
void FindTextureVisitor::apply( osg::Geode& geode )
{
  replaceTexture( geode.getStateSet() );
  for ( unsigned int i=0; i<geode.getNumDrawables(); ++i )
  {
    replaceTexture( geode.getDrawable(i)->getStateSet() );
  }
  traverse( geode );
}

/************************************************************************/
/*jerrryzfc*/
/*8/29/2018 19:10:24*/
/* A head-up display (HUD) can render data without requiring users to look away from their
usual viewpoints. It is widely used in 3D scenes, for displaying important 2D texts, computer
game statistics, and flight and cockpit instruments. This time, we are going to design an
HUD camera, which contains a model that should be placed in front of other scene objects
at any time.


*/
/************************************************************************/
osg::ref_ptr<osg::Node> CreateSceneHDDCamera()
{
  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("lz.osg");
  osg::ref_ptr<osg::Node> hud_model = osgDB::readNodeFile("glider.osg");

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
 
  /************************************************************************/
  /*jerrryzfc*/
  /*8/29/2018 19:13:16*/
  /* 01、The HUD camera and its children must be rendered after the regular scene
  is finished being drawn on the screen. It will overwrite all present pixel data,
  regardless of its location and depth. That is why we use GL_DEPTH_BUFFER_BIT to
  clear the depth buffer. The GL_COLOR_BUFFER_BIT is not set here, to ensure that
  the color buffer is correctly reserved.  
  02、The HUD camera should not be affected by the viewer or any other parent nodes,
  so it needs to be changed to the absolute reference frame, and be set as a custom
  fixed view matrix. The glider is also added to the camera node, used as the content
  to be displayed:  */
  /************************************************************************/
  camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  camera->setRenderOrder( osg::Camera::POST_RENDER );
  camera->setReferenceFrame( osg::Camera::ABSOLUTE_RF );
  camera->setViewMatrixAsLookAt(
    osg::Vec3(0.0f,0.0f,5.0f), osg::Vec3(),
    osg::Vec3(0.0f,1.0f,1.0f)
    );
  camera->addChild( hud_model.get() );

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild( model.get() );
  root->addChild( camera.get() );
    
  return root; 
}


osgViewer::View* createView( int x, int y, int w, int h,
  osg::Node* scene )
{
  osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
  view->setSceneData( scene );
  view->setUpViewInWindow( x, y, w, h );
  return view.release();
}

osg::ref_ptr<osg::Node> CreateScene()
{
  osg::ref_ptr<osg::Node> root = osgDB::readNodeFile( "lz.osg" );
  

return root; 
}



int MultipleWindowView()
{
  osg::ref_ptr<osg::Node> model1 = osgDB::readNodeFile("cessna.osg");
  osg::ref_ptr<osg::Node> model2 = osgDB::readNodeFile("cow.osg");
  osg::ref_ptr<osg::Node> model3 = osgDB::readNodeFile("glider.osg");

  osgViewer::View* view1 = createView(50, 50, 320, 240, model1);
  osgViewer::View* view2 = createView(370, 50, 320, 240, model2);
  osgViewer::View* view3 = createView(185, 310, 320, 240, model3);

  osgViewer::CompositeViewer viewer;
  viewer.addView( view1 );
  viewer.addView( view2 );
  viewer.addView( view3 );
  return viewer.run();
}


//3D glasses
void Multiple3DanaglyphView()  
{
   //osg::DisplaySettings::instance()->setNumMultiSamples( 4 );

  osg::DisplaySettings::instance()->setStereoMode(
    osg::DisplaySettings::ANAGLYPHIC );
  osg::DisplaySettings::instance()->setEyeSeparation( 0.05f );
  osg::DisplaySettings::instance()->setStereo( true );


  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("cessna.osg" );
 
   osg::ref_ptr<osgViewer::Viewer> viewer=new osgViewer::Viewer;

  viewer-> setSceneData( model.get() );
  //viewer->realize();
  //viewer->setUpViewInWindow(10,10,800,600);
  //viewer-> run();

  viewer->realize();
  osgViewer::GraphicsWindow* gwindow= static_cast<osgViewer::GraphicsWindow*> ( viewer->getCamera()->getGraphicsContext());

  if (gwindow)
  {
    gwindow->setWindowRectangle(15,15,800,600);
  } 
  viewer->run();
  
}


//drawing aircrafts on a loaded terrain
void RenderToBuffer()
{

  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("lz.osg");
  osg::ref_ptr<osg::Node> sub_model = osgDB::readNodeFile("glider.osg");

  int tex_width = 1024, tex_height = 1024;
  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  texture->setTextureSize( tex_width, tex_height );
  texture->setInternalFormat( GL_RGBA );
  texture->setFilter( osg::Texture2D::MIN_FILTER,
    osg::Texture2D::LINEAR );
  texture->setFilter( osg::Texture2D::MAG_FILTER,
    osg::Texture2D::LINEAR );

  FindTextureVisitor ftv( texture.get() );
  if ( model.valid() ) model->accept( ftv );


  osg::ref_ptr<osg::Camera> camera = new osg::Camera;
  camera->setViewport( 0, 0, tex_width, tex_height );
  camera->setClearColor( osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f) );
  camera->setClearMask( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

  camera->setRenderOrder( osg::Camera::PRE_RENDER );
  camera->setRenderTargetImplementation(
    osg::Camera::FRAME_BUFFER_OBJECT );
  camera->attach( osg::Camera::COLOR_BUFFER, texture.get() );

  camera->setReferenceFrame( osg::Camera::ABSOLUTE_RF );
  camera->addChild( sub_model.get() );

  osgViewer::Viewer viewer;
  viewer.setSceneData( model.get() );
  viewer.setCameraManipulator( new osgGA::TrackballManipulator );

  float delta = 0.1f, bias = 0.0f;
  osg::Vec3 eye(0.0f,-5.0f, 5.0f);
  while ( !viewer.done() )
  {
    if ( bias<-1.0f ) delta = 0.1f;
    else if ( bias>1.0f ) delta = -0.1f;
    bias += delta;
    camera->setViewMatrixAsLookAt( eye, osg::Vec3(),osg::Vec3(bias, 1.0f, 1.0f) );
    viewer.frame();
  }







}

/*
osg::ref_ptr<osg::Node> CreateScene()
{
osg::ref_ptr<osg::Node> root = osgDB::readNodeFile( "lz.osg" );


return root; 
}

 




*/







int main(_In_ int _Argc, _In_count_(_Argc) _Pre_z_ char ** _Argv, _In_z_ char ** _Env)  
{

  //Multiple3DanaglyphView();
    RenderToBuffer();


  return 0;




  /*
  osgViewer::Viewer viewer;
  viewer.setSceneData( CreateScene().get() );
  viewer.setCameraManipulator( new osgGA::TrackballManipulator );
  while ( !viewer.done() )
  {
    viewer.frame();
    std::cout << "Frame number: " <<
      viewer.getFrameStamp()->getFrameNumber() << std::endl;
  }
  return 0;


 osg::ref_ptr<osgViewer::Viewer> viewer=new  osgViewer::Viewer;


  viewer->setSceneData(CreateScene().get());

  viewer->setCameraManipulator(new osgGA::TrackballManipulator);

 

  viewer->realize();

  
 osgViewer::GraphicsWindow* window= dynamic_cast<osgViewer::GraphicsWindow*>(viewer->getCamera()->getGraphicsContext());

 if (window)
 {
   window->setWindowRectangle(10,10,800,600);
 }


 while (!viewer->done())
 {
   viewer->frame();
   std::cout<<"Frame num:"<<viewer->getFrameNumber() << std::endl;


 }

 return 0;*/
  

}




