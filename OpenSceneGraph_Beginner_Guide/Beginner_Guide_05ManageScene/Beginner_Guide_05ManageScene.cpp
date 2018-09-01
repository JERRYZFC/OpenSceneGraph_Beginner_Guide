
#include <Windows.h>
#include <sal.h>  



#include "osg/Group"
#include "osgDB/ReadFile"
#include "osgViewer/Viewer"
#include "osg/MatrixTransform"
#include "osg/Switch"
#include "osg/ProxyNode"
#include "osg/PagedLOD"

#include "osgUtil/Simplifier"

#include "osg/LOD"


 


osg::ref_ptr< osg::Node> CreateSceneBasic()
{ 
  osg::ref_ptr<osg::Node> model1=osgDB::readNodeFile("cessna.osg");
  osg::ref_ptr<osg::Node> model2=osgDB::readNodeFile("cow.osg");

  osg::ref_ptr<osg::Group> gp=new osg::Group;
  gp  ->addChild(model1.get());
  gp  ->addChild(model2.get()); 

  return gp;
}



/************************************************************************/
/*jerrryzfc*/
/*8/25/2018 15:00:32*/
/* performing translations of child nodes  */
/************************************************************************/
osg::ref_ptr< osg::Node> CreateSceneMatrix()
{ 
 osg::ref_ptr<osg::Node> model1=osgDB::readNodeFile("cessna.osg");
 osg::ref_ptr<osg::Node> model2=osgDB::readNodeFile("cow.osg");

osg::ref_ptr< osg::MatrixTransform> tf0=new osg::MatrixTransform;

tf0->setMatrix(osg::Matrix::translate(-25.0,0.0,0.0));
tf0->addChild(model2.get());

osg::ref_ptr<osg::MatrixTransform> tf1=new osg::MatrixTransform;
tf1->setMatrix(osg::Matrix::translate(25.0,0.0,0));
tf1->addChild(model2.get());


osg::ref_ptr<osg::MatrixTransform> tfSon=new osg::MatrixTransform;

tfSon->setMatrix(osg::Matrix::translate(0.0,0.0,10));
tfSon->addChild(model1.get());

tf1->addChild(tfSon.get());




osg::ref_ptr<osg::Group> gp=new osg::Group;
gp  ->addChild(tf1.get());
gp  ->addChild(tf0.get());

gp  ->addChild(model1.get()); 
 
  return gp;
}


/************************************************************************/
/*jerrryzfc*/
/*8/25/2018 15:34:05*/
/* switching between the normal and damaged Cessna */
/************************************************************************/

osg::ref_ptr<osg::Node> CreateSceneSwitch()
{
 /*osg::ref_ptr< osg::Group> gp=new osg::Group ;*/

 osg::ref_ptr<osg::Node> model1=osgDB::readNodeFile("cessna.osg");
osg::ref_ptr<osg::Node> model2=  osgDB::readNodeFile("cessnafire.osg");


osg::ref_ptr<osg::Switch> swit=new osg::Switch;

swit->addChild(model1.get(),false);

swit->addChild(model2.get(),true);
 
 return swit  ;
  
}

/************************************************************************/
/*jerrryzfc*/
/*8/25/2018 15:44:13*/
/*  LOD Cessna                                                                     */
/************************************************************************/

osg::ref_ptr<osg::Node> CreateSceneBasicLod()
{
  /************************************************************************/
  /*jerrryzfc*/
  /*8/25/2018 16:11:38*/
  /* Have you noticed that the Cessna model should be copied twice to prepare for different level
  polygons? The modelL3 pointer can't be shared here, because the simplifier will directly
  work on the geometric data in application memory, which will affect all pointers sharing the
  same memory. In fact, this is called a shallow copy.
  In this example, we introduce a clone() method, which can be used by all scene nodes,
  drawables, and objects. It is able to perform a deep copy, that is, to make copies of all
  dynamically-allocated memory used by the source object. The modelL2 and modelL1
  pointers thus manage newly-allocated memories, which are filled with the same data as
  modelL3                                                                      */
  /************************************************************************/
  osg::ref_ptr<osg::Node> modelL3=osgDB::readNodeFile("glider.osg");
  osg::ref_ptr<osg::Node> modelL2=dynamic_cast<osg::Node*>(modelL3->clone(osg::CopyOp::DEEP_COPY_ALL));

  osg::ref_ptr<osg::Node> modelL1=dynamic_cast<osg::Node*>(modelL3->clone(osg::CopyOp::DEEP_COPY_ALL));

  osgUtil::Simplifier simper;
  simper.setSampleRatio(0.5);  
  modelL2->accept(simper);
  simper.setSampleRatio(0.1);
  modelL1->accept(simper);

  osg::ref_ptr<osg::LOD> lod=new   osg::LOD;

  lod->addChild(modelL1.get(),80.f,FLT_MAX);
  lod->addChild(modelL2.get(),30.0f,50.0f);
  lod->addChild(modelL3.get(),0.0f,20.0f);
  return lod;











}



/************************************************************************/
/*jerrryzfc*/
/*8/25/2018 16:14:45*/
/*  loading a model at runtime   */
/************************************************************************/

osg::ref_ptr<osg::Node> CreateSceneProxyNOde()
{
  osg::ref_ptr<osg::ProxyNode> root=new osg::ProxyNode;

  root->setFileName(0,"cow.osg");
   

  return root; 
}

/************************************************************************/
/*jerrryzfc*/
/*8/25/2018 16:19:42*/
/* working with the PagedLOD class     */
/************************************************************************/
osg::ref_ptr<osg::Node> CreateScenePagelod()
{
  osg::ref_ptr<osg::PagedLOD> root=new osg::PagedLOD;
   osg::ref_ptr<osg::Node> modelL3=osgDB::readNodeFile("cow.osg");
  root->addChild(modelL3,200.f,FLT_MAX);
  root->setFileName(1,"cessna.osg");
  root->setRange(1,1.0f,200.0f);


  return root; 
}




/*
Customizing your own NodeKits
The most important step in customizing a node and extending new features is to override
the virtual method traverse() . This method is called every frame by the OSG rendering
backend. The traverse() method has an input parameter, osg::NodeVisitor& , which
actually indicates the type of traversals (update, event, or cull). Most OSG NodeKits override
traverse() to implement their own functionalities, along with some other exclusive
attributes and methods.
Note that overriding the traverse() method is a bit dangerous sometimes, because it affects
the traversing process and may lead to the incorrect rendering of results if developers are not
careful enough. It is also a little awkward if you want to add the same new feature to multiple
node types by extending each node type to a new customized class. In these cases, consider
using node callbacks instead, which will be discussed in Chapter 8, Animating Scene Objects.

*/





class AnimatingSwitch : public osg::Switch
{
public:
  AnimatingSwitch() : osg::Switch(), _count(0) {}
  AnimatingSwitch( const AnimatingSwitch& copy,
    const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY )
    : osg::Switch(copy, copyop), _count(copy._count) {}
  META_Node( osg, AnimatingSwitch );
  virtual void traverse( osg::NodeVisitor& nv );
protected:
  unsigned int _count;
};

void AnimatingSwitch::traverse( osg::NodeVisitor& nv )
{
  if ( !((++_count)%60) )
  {
    setValue( 0, !getValue(0) );
    setValue( 1, !getValue(1) );
  }
  osg::Switch::traverse( nv );
}

/************************************************************************/
/*jerrryzfc*/
/*8/25/2018 16:28:02*/
/*  animating the switch node      
/************************************************************************/

osg::ref_ptr<osg::Node> CreateSceneSwitchNode()
{
  osg::ref_ptr<osg::Node> model1= osgDB::readNodeFile("cessna.osg");
  osg::ref_ptr<osg::Node> model2= osgDB::readNodeFile("cessnafire.osg");
    osg::ref_ptr<AnimatingSwitch> root = new AnimatingSwitch;
  root->addChild( model1.get(), true );
  root->addChild( model2.get(), false );

return root; 
}



class InfoVisitor:public osg::NodeVisitor
{

public:
  InfoVisitor():_level(0){
    setTraversalMode(NodeVisitor::TRAVERSE_ALL_CHILDREN);
  }
  std::string spaces()
  {
    return std::string(_level*2,'*');
  }
  virtual void apply(osg::Node & node);
  virtual void apply(osg::Geode& geode);

private:
  unsigned int _level;






};

void InfoVisitor::apply(osg::Node & node)
{
  std::cout << spaces() << node.libraryName() << "::"
    << node.className() << std::endl;
  _level++;
  traverse( node );
  _level--;

}

void InfoVisitor::apply(osg::Geode& geode)
{
  std::cout << spaces() << geode.libraryName() << "::"
    << geode.className() << std::endl;
  _level++;
  for ( unsigned int i=0; i<geode.getNumDrawables(); ++i )
  {
    osg::Drawable* drawable = geode.getDrawable(i);
    std::cout << spaces() << drawable->libraryName() << "::"
      << drawable->className() << std::endl;
  }
  traverse( geode );
  _level--;
}




/************************************************************************/
/*jerrryzfc*/
/*8/27/2018 13:20:24*/
/* 分析场景模型组成                       analyzing the Cessna structure  */
/************************************************************************/

osg::ref_ptr<osg::Node> CreateSceneTraverse()
{
//osg::ref_ptr<osg::PagedLOD> root=new osg::PagedLOD;
//
//root->setFileName(0,"cessnafire.osg");

 osg::ref_ptr<osg::Node> root = osgDB::readNodeFile( "cessnafire.osg" );

InfoVisitor infoVisitor;
root->accept(infoVisitor);



return root; 
}


osg::ref_ptr<osg::Node> CreateScene()
{
osg::ref_ptr<osg::PagedLOD> root=new osg::PagedLOD;

root->setFileName(0,"cow.osg");


return root; 
}



/*

osg::ref_ptr<osg::Node> CreateScene()
{
osg::ref_ptr<osg::PagedLOD> root=new osg::PagedLOD;

root->setFileName(0,"cow.osg");


return root; 
}


*/















int main(_In_ int _Argc, _In_count_(_Argc) _Pre_z_ char ** _Argv, _In_z_ char ** _Env )
{ 

  osg::ref_ptr<osgViewer::Viewer> viewer=new osgViewer::Viewer; 
  viewer->setSceneData(CreateScene().get ());

  viewer->realize();
  osgViewer::GraphicsWindow* gwindow= static_cast<osgViewer::GraphicsWindow*> ( viewer->getCamera()->getGraphicsContext());

  if (gwindow)
  {
    gwindow->setWindowRectangle(15,15,800,600);
  } 
  viewer->run();



  return 0;

}



