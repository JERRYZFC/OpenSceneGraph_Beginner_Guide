
#include <Windows.h>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <fstream>


class LogFileHandler : public osg::NotifyHandler
{
public:
  LogFileHandler( const std::string& file ) { _log.open( file.c_str() ); }
  virtual ~LogFileHandler() { _log.close(); }

  virtual void notify( osg::NotifySeverity severity, const char* message )
  {
    switch ( severity )
    {
    case osg::ALWAYS: _log << "[ALWAYS]: "; break;
    case osg::FATAL: _log << "[FATAL]: "; break;
    case osg::WARN: _log << "[WARN]: "; break;
    case osg::NOTICE: _log << "[NOTICE]: "; break;
    case osg::INFO: _log << "[INFO]: "; break;
    default: break;
    }
    _log << message;
  }

protected:
  /************************************************************************/
  /*jerrryzfc*/
  /*8/7/2018 18:07:38*/
  /* 	// output stream associated with a C stream                                                                     */
  /************************************************************************/
  std::ofstream _log;
};



class zfcLogFileHandler:public osg::NotifyHandler
{

public:
  zfcLogFileHandler(const std::string &filePath){
     _loger.open(filePath.c_str());
  }

  ~zfcLogFileHandler(){
    _loger.close();
    std::cout<<"析构成功"<<std::endl;
  }

  virtual void notify(osg::NotifySeverity severity, const char *message)
  {
   
    /*switch (severity)
    {
    case  osg::NotifySeverity::INFO:
      _loger<<"[zfcInfo]"<<message;
      break;
    }*/

    _loger<<"[zfc_loger]:"<<severity<<"    "<<message;

  }


protected:
  std::ofstream _loger;

};

int main( int argc, char** argv )
{
  osg::setNotifyLevel( osg::INFO );

  osg::ref_ptr<zfcLogFileHandler> zfcloger=new zfcLogFileHandler("曾小超的日志.txt");

  osg::setNotifyHandler(zfcloger.get() );

 // osg::ArgumentParser arguments( &argc, argv );
  osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("cow.osg");
  if ( !root )
  {
   // OSG_FATAL << arguments.getApplicationName() <<": No data loaded." << std::endl;
    return -1;
  }

  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );
  viewer.run();


   return 0;
}
