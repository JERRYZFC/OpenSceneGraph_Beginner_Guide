
#include <Windows.h>

#include <osg/Geometry>
#include <osg/Geode>

#include <osg/AnimationPath>
#include <osg/MatrixTransform>

#include <osg/ImageSequence>
#include <osg/Texture2D> 

#include <osg/BlendFunc>
#include <osg/Material>
#include <osgAnimation/EaseMotion>
 
 
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/UpdateMatrixTransform>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedQuaternionElement>

#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

class SwitchingCallback : public osg::NodeCallback
{
public:
  SwitchingCallback() : _count(0) {}
  virtual void operator()( osg::Node* node,osg::NodeVisitor* nv );
protected:
  unsigned int _count;
};

void SwitchingCallback::operator()( osg::Node* node,
  osg::NodeVisitor* nv )
{
  osg::Switch* switchNode = static_cast<osg::Switch*>( node );
  if ( !((++_count)%60) && switchNode )
  {
    switchNode->setValue( 0, !switchNode->getValue(0) );
    switchNode->setValue( 1, !switchNode->getValue(1) );
  }
  traverse( node, nv );
}

void SwitchShow()
{
  osg::ref_ptr<osg::Node> model1 = osgDB::readNodeFile(
    "cessna.osg" );
  osg::ref_ptr<osg::Node> model2= osgDB::readNodeFile("cessnafire.osg");
    osg::ref_ptr<osg::Switch> root = new osg::Switch;
  root->addChild( model1.get(), false );
  root->addChild( model2.get(), true );


  root->setUpdateCallback( new SwitchingCallback );


  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );

  viewer.setUpViewInWindow(10,10,800,600);
  
  viewer.run();

    

}




class DynamicQuadCallback : public osg::Drawable::UpdateCallback
{
public:
  virtual void update( osg::NodeVisitor*, osg::Drawable* drawable );
};
void DynamicQuadCallback::update( osg::NodeVisitor*, osg::Drawable* drawable )
{
  osg::Geometry* quad = static_cast<osg::Geometry*>( drawable );
  if ( !quad ) return;
  osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(
    quad->getVertexArray() );
  if ( !vertices ) return;
  osg::Quat quat(osg::PI*0.01, osg::X_AXIS);
  vertices->back() = quat * vertices->back();
  quad->dirtyDisplayList();
  quad->dirtyBound();
}


osg::Geometry* createQuad()
{
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
  vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
  vertices->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
  vertices->push_back( osg::Vec3(1.0f, 0.0f, 1.0f) );
  vertices->push_back( osg::Vec3(0.0f, 0.0f, 1.0f) );
  osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
  normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
  colors->push_back( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );
  colors->push_back( osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) );
  colors->push_back( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );
  colors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );
  osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
  quad->setVertexArray( vertices.get() );
  quad->setNormalArray( normals.get() );
  quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
  quad->setColorArray( colors.get() );
  quad->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
  quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );
  return quad.release();
}

void DynamicDraw()
{
  osg::Geometry* quad = createQuad();
  quad->setDataVariance( osg::Object::DYNAMIC );
  quad->setUpdateCallback( new DynamicQuadCallback );


  osg::ref_ptr<osg::Geode> root = new osg::Geode;
  root->addDrawable( quad );
  osgViewer::Viewer viewer;
  
  viewer.setSceneData( root.get() );

  viewer.setUpViewInWindow(10,10,800,600);

    viewer.run();






}





osg::AnimationPath* createAnimationPath( float radius, float time   )
{
  osg::ref_ptr<osg::AnimationPath> path = new
    osg::AnimationPath;
  path->setLoopMode( osg::AnimationPath::LOOP );
  unsigned int numSamples = 32;
  float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
  float delta_time = time / (float)numSamples;
  for ( unsigned int i=0; i<numSamples; ++i )
  {
    float yaw = delta_yaw * (float)i;
    osg::Vec3 pos( sinf(yaw)*radius, cosf(yaw)*radius, 0.0f );
    osg::Quat rot( -yaw, osg::Z_AXIS );
    path->insert( delta_time * (float)i,
      osg::AnimationPath::ControlPoint(pos, rot)
      );
  }
  return path.release();
}

//making use of the animation path
void AnimationPathTest()
{
  osg::ref_ptr<osg::Node> model =
    osgDB::readNodeFile( "cessna.osg.0,0,90.rot" );/*The string "0,0,90.rot" seems redundant and ambiguous here. It is a kind of pseudo-loader, which is written as part of
    filename but actually does a 90 degrees rotation around the Z axis to the model
    cessna.osg . */
osg::ref_ptr<osg::MatrixTransform> root = new
  osg::MatrixTransform;
root->addChild( model.get() );

osg::ref_ptr<osg::AnimationPathCallback> apcb = new
  osg::AnimationPathCallback;
apcb->setAnimationPath( createAnimationPath(50.0f, 6.0f) );
root->setUpdateCallback( apcb.get() );

osgViewer::Viewer viewer;
viewer.setSceneData( root.get() );

viewer.setUpViewInWindow(10,10,800,600);

 
  viewer.run();

}


class AlphaFadingCallback : public osg::StateAttributeCallback
{
public:
  AlphaFadingCallback()
  { _motion = new osgAnimation::InOutCubicMotion(0.0f, 1.0f); }
  virtual void operator()(osg::StateAttribute*,osg::NodeVisitor*);
protected:
  osg::ref_ptr<osgAnimation::InOutCubicMotion> _motion;
};

void AlphaFadingCallback::operator()( osg::StateAttribute* sa,
  osg::NodeVisitor* nv )
{
  osg::Material* material = static_cast<osg::Material*>( sa );
  if ( material )
  {
    _motion->update( 0.005 );
    float alpha = _motion->getValue();
    material->setDiffuse( osg::Material::FRONT_AND_BACK,
      osg::Vec4(0.0f, 1.0f, 1.0f, alpha)
      );
  }
}


void FadeInTest()
{
  osg::ref_ptr<osg::Drawable> quad = osg::createTexturedQuadGeometry(
    osg::Vec3(-0.5f, 0.0f, -0.5f),
    osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 1.0f)
    );
  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable( quad.get() );
  osg::ref_ptr<osg::Material> material = new osg::Material;
  material->setAmbient( osg::Material::FRONT_AND_BACK,
    osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );
  material->setDiffuse( osg::Material::FRONT_AND_BACK,
    osg::Vec4(0.0f, 1.0f, 1.0f, 0.5f) );
  material->setUpdateCallback( new AlphaFadingCallback );

  geode->getOrCreateStateSet()->setAttributeAndModes(
    material.get() );
  geode->getOrCreateStateSet()->setAttributeAndModes(
    new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
  geode->getOrCreateStateSet()->setRenderingHint(
    osg::StateSet::TRANSPARENT_BIN );

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild( geode.get() );
  root->addChild( osgDB::readNodeFile("glider.osg") );

  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );
  
  viewer.setUpViewInWindow(10,10,800,600);
  viewer.run();


}



//animating in graphics shaders

//Playing movies on textures
/*It is fantastic if we can watch movies or hold video conferences in the 3D world. We can
  place a big quad geometry as the movie screen and attach a dynamic 2D texture to its
  surface. The texture contains a series of images that make up the whole video. It is necessary  that the image sequence can add new images on the fly, which may be transferred from files  or mini cameras.*/

osg::Image* createSpotLight( const osg::Vec4& centerColor,
  const osg::Vec4& bgColor,
  unsigned int size, float power )
{
  osg::ref_ptr<osg::Image> image = new osg::Image;
  image->allocateImage( size, size, 1, GL_RGBA,
    GL_UNSIGNED_BYTE );
  float mid = (float(size)-1) * 0.5f;
  float div = 2.0f / float(size);
  for( unsigned int r=0; r<size; ++r )
  {
    unsigned char* ptr = image->data(0, r);
    for( unsigned int c=0; c<size; ++c )
    {
      float dx = (float(c) - mid)*div;
      float dy = (float(r) - mid)*div;
      float r = powf(1.0f - sqrtf(dx*dx+dy*dy), power);
      if ( r<0.0f ) r = 0.0f;
      osg::Vec4 color = centerColor*r + bgColor*(1.0f - r);
      *ptr++ = (unsigned char)((color[0]) * 255.0f);
      *ptr++ = (unsigned char)((color[1]) * 255.0f);
      *ptr++ = (unsigned char)((color[2]) * 255.0f);
      *ptr++ = (unsigned char)((color[3]) * 255.0f);
    }
  }
  return image.release();
}

void AnimatingInGraphicsShaders()
{
  osg::Vec4 centerColor( 1.0f, 1.0f, 0.0f, 1.0f );
  osg::Vec4 bgColor( 0.0f, 0.0f, 0.0f, 1.0f );
  osg::ref_ptr<osg::ImageSequence> imageSequence = new
    osg::ImageSequence;
  imageSequence->addImage( createSpotLight(centerColor, bgColor, 64,
    3.0f) );
  imageSequence->addImage( createSpotLight(centerColor, bgColor, 64,
    3.5f) );
  imageSequence->addImage( createSpotLight(centerColor, bgColor, 64,
    4.0f) );
  imageSequence->addImage( createSpotLight(centerColor, bgColor, 64,
    3.5f) );

  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  texture->setImage(  imageSequence.get() );


  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable( osg::createTexturedQuadGeometry(
    osg::Vec3(), osg::Vec3(1.0,0.0,0.0), osg::Vec3(0.0,0.0,1.0))
    );
  geode->getOrCreateStateSet()->setTextureAttributeAndModes(
    0, texture.get(), osg::StateAttribute::ON );

  imageSequence->setLength( 0.5 );
  imageSequence->play();


  osgViewer::Viewer viewer;
  viewer.setSceneData( geode.get() );
  viewer.setUpViewInWindow(10,10,800,600);
    viewer.run();



}


void createAnimationPath( float radius, float time,
  osgAnimation::Vec3KeyframeContainer* container1,
  osgAnimation::QuatKeyframeContainer* container2 )
{
  unsigned int numSamples = 32;
  float delta_yaw = 2.0f * osg::PI/((float)numSamples - 1.0f);
  float delta_time = time / (float)numSamples;
  for ( unsigned int i=0; i<numSamples; ++i )
  {
    float yaw = delta_yaw * (float)i;
    osg::Vec3 pos( sinf(yaw)*radius, cosf(yaw)*radius, 0.0f );
    osg::Quat rot( -yaw, osg::Z_AXIS );
    container1->push_back(
      osgAnimation::Vec3Keyframe(delta_time * (float)i, pos)
      );
    container2->push_back(
      osgAnimation::QuatKeyframe(delta_time * (float)i, rot)
      );
  }
}

//Creating complex key-frame animations
void CreatingComplexKeyFrameAnimations()
{

  osg::ref_ptr<osgAnimation::Vec3LinearChannel> ch1 =
    new osgAnimation::Vec3LinearChannel;
  ch1->setName( "position" );
  ch1->setTargetName( "PathCallback" );

  osg::ref_ptr<osgAnimation::QuatSphericalLinearChannel> ch2 =
    new osgAnimation::QuatSphericalLinearChannel;
  ch2->setName( "quat" );
  ch2->setTargetName( "PathCallback" );

  createAnimationPath( 50.0f, 6.0f,
    ch1->getOrCreateSampler()->getOrCreateKeyframeContainer(),
    ch2->getOrCreateSampler()->getOrCreateKeyframeContainer() );

  osg::ref_ptr<osgAnimation::Animation> animation = new
    osgAnimation::Animation;
  animation->setPlayMode( osgAnimation::Animation::LOOP );
  animation->addChannel( ch1.get() );
  animation->addChannel( ch2.get() );

  osg::ref_ptr<osgAnimation::UpdateMatrixTransform> updater =
    new osgAnimation::UpdateMatrixTransform("PathCallback");
  updater->getStackedTransforms().push_back(
    new osgAnimation::StackedTranslateElement("position") );
  updater->getStackedTransforms().push_back(
    new osgAnimation::StackedQuaternionElement("quat") );

  osg::ref_ptr<osg::MatrixTransform> animRoot= new
    osg::MatrixTransform;
  animRoot->addChild( osgDB::readNodeFile("cessna.osg.0,0,90.rot")
    );
  animRoot->setDataVariance( osg::Object::DYNAMIC );
  animRoot->setUpdateCallback( updater.get() );


  osg::ref_ptr<osgAnimation::BasicAnimationManager> manager =
    new osgAnimation::BasicAnimationManager;
  manager->registerAnimation( animation.get() );



  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild( animRoot.get() );
  root->setUpdateCallback( manager.get() );

  manager->playAnimation( animation.get() );
  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );
   viewer.setUpViewInWindow(10,10,800,600);
  viewer.run();
}


int  creatingAndDrivingCharacterSystem()
{





}


int main(_In_ int _Argc, _In_count_(_Argc) _Pre_z_ char ** _Argv, _In_z_ char ** _Env)
{

  //SwitchShow();
  //DynamicDraw();
  //AnimationPathTest();
   //FadeInTest();
   //AnimatingInGraphicsShaders();
   //CreatingComplexKeyFrameAnimations();
  return 0;
}











