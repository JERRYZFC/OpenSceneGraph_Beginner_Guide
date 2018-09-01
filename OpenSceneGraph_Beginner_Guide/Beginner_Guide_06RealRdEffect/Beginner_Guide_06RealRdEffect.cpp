
#include <Windows.h>


#include "osgViewer/GraphicsWindow"
#include "osgViewer/Viewer"
#include "osg/Node"
#include "osg/Geode"
#include "osg/StateSet"
#include "osg/Group"
#include "osg/Fog"
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include "osg/Light"
#include "osg/LightSource"

#include <osg/LineWidth>
#include <osg/Program>

#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>

/************************************************************************/
/*jerrryzfc*/
/*8/27/2018 19:02:57*/
/* 
The osg::StateSet class divides attributes and modes into two groups: texture and
non-texture. It has several member methods to add non-texture attributes and modes
to the state set itself:
1. The public method setAttribute() adds an osg::StateAttribute derived
object to the state set. Attributes of the same type cannot coexist in one state set.
The previous set one will just be overwritten by the new one.
2. The public method setMode() attaches a mode enumeration to the state set and
sets its value to osg::StateAttribute::ON or osg::StateAttribute::OFF ,
which means to enable or disable the mode.
3. The public method setAttributeAndModes() attaches a rendering attribute and
its associated mode to the state set, and sets the switch value ( ON by default) at the
same time. Be aware that not every attribute has a corresponding mode, but you
can always make use of this method without being indecisive.



*/
/************************************************************************/


/************************************************************************/
/*jerrryzfc*/
/*8/27/2018 18:48:26*/
/* CreatingRealisticRenderingEffects */
/************************************************************************/

osg::ref_ptr<osg::Node> CreateScenePolyLineMode()
{
  osg::ref_ptr<osg::Node> model1=osgDB::readNodeFile("cow.osg");

  osg::ref_ptr<osg::MatrixTransform> trans1=new osg::MatrixTransform;

  trans1->setMatrix(osg::Matrix::translate(-25.0f,0.0f,0.0f));
  trans1->addChild(model1.get());


  osg::ref_ptr<osg::MatrixTransform> tr2=new osg::MatrixTransform;
  tr2->setMatrix(osg::Matrix::translate(25.0f,0.0f,0.0f));
  tr2->addChild(model1.get());

  osg::ref_ptr<osg::PolygonMode>pm=new osg::PolygonMode;
  pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);

  trans1->getOrCreateStateSet()->setAttribute(pm.get());

  osg::ref_ptr<osg::Group> gp=new osg::Group;
  gp->addChild(trans1.get());
  gp ->addChild(tr2.get());


  return gp ; 

   
}




//lighting the glider or not
/************************************************************************/
/*jerrryzfc*/
/*8/28/2018 19:35:06*/
/* 
osg::PolygonMode* pm = dynamic_cast<osg::PolygonMode*>(
stateset->getAttribute(osg::StateAttribute::POLYGONMODE) );
osg::StateAttribute::GLModeValue value =
stateset->getMode( GL_LIGHTING );


*/
/************************************************************************/
osg::ref_ptr<osg::Node> CreateSceneLIGHTING()
{
  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(
    "glider.osg" );
  osg::ref_ptr<osg::MatrixTransform> tr1=new osg::MatrixTransform;

  tr1->setMatrix( osg::Matrix::translate(0.0f,0.0,10.5f));
  tr1->addChild(model.get());

  osg::ref_ptr<osg::MatrixTransform> transformation2 = new
    osg::MatrixTransform;
  transformation2->setMatrix(osg::Matrix::translate(
    0.5f, 0.0f, 0.0f));
  transformation2->addChild( model.get() );

  tr1->getOrCreateStateSet()->setMode( GL_LIGHTING,
    osg::StateAttribute::OFF );
  transformation2->getOrCreateStateSet()->setMode( GL_LIGHTING,
    osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);

/*  The node transformation1 , as shown in the previous image, remains lighted in spite of
    its own setting. However, transformation2 uses a PROTECTED flag to prevent itself from
    being affected by the root. It becomes a little brighter as a result of "turning off" the light on
    its surfaces. This is simply because the geometries are now directly colored according to the
    original color arrays, without any more reaction to the lights.*/
  

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild( tr1.get() );
  root->addChild( transformation2.get() );

  root->getOrCreateStateSet()->setMode( GL_LIGHTING,
    osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE );




return root; 
}





osg::ref_ptr<osg::Node> CreateSceneFog()
{
  osg::ref_ptr<osg::Fog> fog = new osg::Fog;
  fog->setMode( osg::Fog::EXP );
  fog->setStart( 200.0f );
  fog->setEnd( 2500.0f );
  fog->setColor( osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f) );

  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile( "lz.osg" );
  model->getOrCreateStateSet()->setAttributeAndModes( fog.get() );
   
  return model; 
}


// light sources
osg::Node* createLightSource( unsigned int num,
  const osg::Vec3& trans,
  const osg::Vec4& color )
{
  osg::ref_ptr<osg::Light> light = new osg::Light;
  light->setLightNum( num );
  light->setDiffuse( color );
  light->setPosition( osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );
  osg::ref_ptr<osg::LightSource> lightSource = new
    osg::LightSource;
  lightSource->setLight( light );

  osg::ref_ptr<osg::MatrixTransform> sourceTrans =
    new osg::MatrixTransform;
  sourceTrans->setMatrix( osg::Matrix::translate(trans) );
  sourceTrans->addChild( lightSource.get() );
  return sourceTrans.release();
}


/************************************************************************/
/*jerrryzfc*/
/*8/28/2018 19:50:21*/
/* The osg::LightSource class is a node of special kind, which affects all nodes that enable
its associated rendering mode, no matter whether these nodes are placed as children of the
light source or not. This is sometimes confusing, but can be explained through the concept of
positional states. That is, rendering states using the current model-view matrix to position
themselves.
Typical positional states in OpenGL include the glLight() function (point light),
glClipPlane() function, and glTexGen() function ( GL_EYE_LINEAR mode). These
states should be anchored during the space transformation; otherwise their appearances
will vary widely according to different model-view matrices applied every time.
OSG uses three osg::Group derived nodes: osg::LightSource , osg::ClipNode , and
osg::TexGenNode , to bind these special states. They all have a setReferenceFrame()
method to use the absolute reference frame, and can be added to the transformation
node to be located in space. The only difference is that osg::LightSource and
osg::TexGenNode have influence over all nodes enabling related modes, but
osg::ClipNode only clips children with specific clipping planes.                                                                     */
/************************************************************************/

osg::ref_ptr<osg::Node> CreateSceneLightSource()
{
 
 
osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(
  "cessna.osg" );
osg::ref_ptr<osg::Group> root = new osg::Group;
root->addChild( model.get() );



osg::Node* light0 = createLightSource(
  0, osg::Vec3(-20.0f,0.0f,0.0f), osg::Vec4(
  1.0f,1.0f,0.0f,1.0f) );
osg::Node* light1 = createLightSource(
  1, osg::Vec3(0.0f,-20.0f,0.0f), osg::Vec4(0.0f,1.0f,1.0f,1.0f)
  );
/************************************************************************/
/*jerrryzfc*/
/*8/28/2018 19:46:55*/
/* The light numbers 0 and 1 are used here. So we will turn on modes GL_LIGHT0
and GL_LIGHT1 of the root node, which means that all nodes in the scene graph
could benefit from the two warm light sources:                                                                     */
/************************************************************************/
root->getOrCreateStateSet()->setMode( GL_LIGHT0,
  osg::StateAttribute::ON );
root->getOrCreateStateSet()->setMode( GL_LIGHT1,
  osg::StateAttribute::ON );
root->addChild( light0 );
root->addChild( light1 );





return root; 
}



/************************************************************************/
/*jerrryzfc*/
/*8/28/2018 19:55:44*/
/* The osg::Texture2D class requires the texture coordinates normalized to [0, 1], otherwise
it uses texture wrapping to handle extra parts. It checks if the dimensions of a texture are
both power-of-two in size, for instance, 64x64 or 256x512, and will automatically scale non
power-of-two images by default, using the OpenGL's gluScaleImage() function internally,
which is convenient for reading arbitrary images, but spends more system time and leads
to larger result size in graphics memory. There is also a setResizeNonPowerOfTwoHint()
method that defines whether we have to force the resizing of images. Note that non
power-of-two images are directly supported by some graphics cards.
The osg::TextureRectangle class supports 2D textures without requiring power-of-two
dimensions. Re-sampling is thus avoided, and less graphics memory will be used to store
image data. However, it doesn't have mipmaps for texture filtering, and texture coordinates
must be dimension-dependent.                                                                     */
/************************************************************************/
osg::ref_ptr<osg::Node> CreateSceneTexture2D()
{
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
  vertices->push_back( osg::Vec3(-0.5f, 0.0f,-0.5f) );
  vertices->push_back( osg::Vec3( 0.5f, 0.0f,-0.5f) );
  vertices->push_back( osg::Vec3( 0.5f, 0.0f, 0.5f) );
  vertices->push_back( osg::Vec3(-0.5f, 0.0f, 0.5f) );
  osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
  normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
  osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
  texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
  texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
  texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
  texcoords->push_back( osg::Vec2(1.0f, 0.0f) );

  osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
  quad->setVertexArray( vertices.get() );
  quad->setNormalArray( normals.get() );
  quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
  quad->setTexCoordArray( 0, texcoords.get() );
  quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );

  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  osg::ref_ptr<osg::Image> image =
    osgDB::readImageFile( "Images/lz.rgb" );
  texture->setImage( image.get() );


  osg::ref_ptr<osg::Geode> root = new osg::Geode;
  root->addDrawable( quad.get() );
  root->getOrCreateStateSet()->setTextureAttributeAndModes(
    0, texture.get() );


 


return root; 
}



//filters and wrapping modes
osg::ref_ptr<osg::Node> CreateSceneBlendFunc()
{
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
  vertices->push_back( osg::Vec3(-0.5f, 0.0f,-0.5f) );
  vertices->push_back( osg::Vec3( 0.5f, 0.0f,-0.5f) );
  vertices->push_back( osg::Vec3( 0.5f, 0.0f, 0.5f) );
  vertices->push_back( osg::Vec3(-0.5f, 0.0f, 0.5f) );
  osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
  normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
  osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
  texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
  texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
  texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
  texcoords->push_back( osg::Vec2(1.0f, 0.0f) );

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
  colors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 0.5f) );


  osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
  quad->setVertexArray( vertices.get() );
  quad->setNormalArray( normals.get() );
  quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
  quad->setColorArray( colors.get() );
  quad->setColorBinding( osg::Geometry::BIND_OVERALL );

  quad->setTexCoordArray( 0, texcoords.get() );
  quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );
  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable( quad.get() );

  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  osg::ref_ptr<osg::Image> image =
    osgDB::readImageFile( "Images/lz.rgb" );
  texture->setImage( image.get() );


  /************************************************************************/
  /*jerrryzfc*/
  /*8/28/2018 20:01:43*/
  /* During the drawing traversal, the GL_SRC_ALPHA and GL_ONE_MINUS_SRC_ALPHA
  enumerations determine the blended color values by using the following equations:
  R = srcR * srcA + dstR * (1 - srcA)
  G = srcG * srcA + dstG * (1 - srcA)
  B = srcB * srcA + dstB * (1 - srcA)
  Here, [srcR, srcG, srcB] is a color value of the translucent quad being rendered, and
  [dstR, dstG, dstB] is the screen color value to be overridden, which was actually filled
  by the opaque glider model just now. The resultant color [R, G, B] is computed according
  to the alpha factor srcA of the quad's color vector, and hence mixed by the incoming and
  previous color values to generate the translucent effect.                                                                     */
  /************************************************************************/

  osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
  blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  osg::StateSet* stateset = geode->getOrCreateStateSet();
  stateset->setTextureAttributeAndModes( 0, texture.get() );
  stateset->setAttributeAndModes( blendFunc );
  
  stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild( geode.get() );
  root->addChild( osgDB::readNodeFile("glider.osg") );




return root; 
}



//cartoon
osg::ref_ptr<osg::Node> CreateSceneShaderProgram()
{
  static const char* vertSource = {
    "varying vec3 normal;\n"
    "void main()\n"
    "{\n"
    " normal = normalize(gl_NormalMatrix * gl_Normal);\n"
    " gl_Position = ftransform();\n"
    "}\n"
  };

  static const char* fragSource = {
    "uniform vec4 color1;\n"
    "uniform vec4 color2;\n"
    "uniform vec4 color3;\n"
    "uniform vec4 color4;\n"
    "varying vec3 normal;\n"
    "void main()\n"
    "{\n"
    " float intensity = dot(vec3(gl_LightSource[0].position),normal);\n"
    " if (intensity > 0.95) gl_FragColor = color1;\n"
    " else if (intensity > 0.5) gl_FragColor = color2;\n"
    " else if (intensity > 0.25) gl_FragColor = color3;\n"
    " else gl_FragColor = color4;\n"
    "}\n"
  };

  osg::ref_ptr<osg::Shader> vertShader =
    new osg::Shader( osg::Shader::VERTEX, vertSource );
  osg::ref_ptr<osg::Shader> fragShader =
    new osg::Shader( osg::Shader::FRAGMENT, fragSource );
  osg::ref_ptr<osg::Program> program = new osg::Program;
  program->addShader( vertShader.get() );
  program->addShader( fragShader.get() );


  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile( "cow.osg" );
  osg::StateSet* stateset = model->getOrCreateStateSet();
  stateset->setAttributeAndModes( program.get() );
  stateset->addUniform(
    new osg::Uniform("color1", osg::Vec4(
    1.0f, 0.5f, 0.5f, 1.0f)) );
  stateset->addUniform(
    new osg::Uniform("color2", osg::Vec4(
    0.5f, 0.2f, 0.2f, 1.0f)) );
  stateset->addUniform(
    new osg::Uniform("color3", osg::Vec4(
    0.2f, 0.1f, 0.1f, 1.0f)) );
  stateset->addUniform(
    new osg::Uniform("color4", osg::Vec4(
    0.1f, 0.05f, 0.05f, 1.0f)) );


return model; 
}

//geometry shader
/************************************************************************/
/*jerrryzfc*/
/*8/28/2018 20:16:09*/
/* SG provides full support of the shading language, even those coming with OpenGL4. It also
supports the NVIDIA Cg through a third-party project named osgNV. Can't wait to have fun
with shaders? Besides your own adventures, there are several great open source projects
that are making good use of shaders for your reference:
 The osgCompute and osgCUDA projects are used for computing with GPU parallel
streaming processors: http://www.cg.informatik.uni-siegen.de/svt/osgcompute/                The osgNV project can support the NVIDIA Cg language: http://osgnv.sourceforge.net/
 The osgOcean project is used for realistic ocean simulation: http://code.google.com/p/osgocean/
 The osgPPU project implements various effects using the post processing technique:
http://projects.tevs.eu/osgppu
*/
/************************************************************************/
osg::ref_ptr<osg::Node> CreateScene()
{
  static const char* vertSource = {
    "#version 120\n"
    "#extension GL_EXT_geometry_shader4 : enable\n"
    "void main()\n"
    "{ gl_Position = ftransform(); }\n"
  };


  static const char* geomSource = {
    "#version 120\n"
    "#extension GL_EXT_geometry_shader4 : enable\n"
    "uniform int segments;\n"
    "void main(void)\n"
    "{\n"
    " float delta = 1.0 / float(segments);\n"
    " vec4 v;\n"
    " for ( int i=0; i<=segments; ++i )\n"
    " {\n"
    " float t = delta * float(i);\n"
    " float t2 = t * t;\n"
    " float one_minus_t = 1.0 - t;\n"
    " float one_minus_t2 = one_minus_t * one_minus_t;\n"
    " v = gl_PositionIn[0] * one_minus_t2 * one_minus_t+\n"
    " gl_PositionIn[1] * 3.0 * t * one_minus_t2 +\n"
    " gl_PositionIn[2] * 3.0 * t2 * one_minus_t +\n"
    " gl_PositionIn[3] * t2 * t;\n"
    " gl_Position = v;\n"
    " EmitVertex();\n"
    " }\n"
    " EndPrimitive();\n"
    "}\n"
  };
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
    vertices->push_back( osg::Vec3(1.0f, 1.0f, 1.0f) );
    vertices->push_back( osg::Vec3(2.0f, 1.0f,-1.0f) );
    vertices->push_back( osg::Vec3(3.0f, 0.0f, 0.0f) );
    osg::ref_ptr<osg::Geometry> controlPoints = new osg::Geometry;

    controlPoints->setVertexArray( vertices.get() );
    controlPoints->addPrimitiveSet(
      new osg::DrawArrays(GL_LINES_ADJACENCY_EXT, 0, 4) );
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( controlPoints.get() );

    int segments = 10;
    osg::ref_ptr<osg::Program> program = new osg::Program;
    program->addShader(
      new osg::Shader(osg::Shader::VERTEX, vertSource) );
    program->addShader(
      new osg::Shader(osg::Shader::GEOMETRY, geomSource) );
    program->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, segments+1 );
    program->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT,
      GL_LINES_ADJACENCY_EXT );
    program->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT,
      GL_LINE_STRIP );


    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
    lineWidth->setWidth( 2.0f );

    osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setAttributeAndModes( program.get() );
    stateset->setAttribute( lineWidth.get() );
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    stateset->addUniform( new osg::Uniform("segments", segments) );



    return geode;
     
 

}













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














