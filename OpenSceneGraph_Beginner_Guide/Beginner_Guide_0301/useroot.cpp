
#include <Windows.h>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include "osgViewer/GraphicsWindow"

int main( int argc, char** argv )
{
  osg::ref_ptr<osg::Node> root = osgDB::readNodeFile( "cessna.osg" );
  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );

  viewer.realize();// 必须调用Realize，否则窗口没有创建
  osgViewer::GraphicsWindow *pWnd = dynamic_cast<osgViewer::GraphicsWindow*>(viewer.getCamera()->getGraphicsContext());
  if (pWnd) {
    pWnd->setWindowRectangle(20, 20, 800, 600);
    pWnd->setWindowDecoration(true);
  } 

  return viewer.run();
}

