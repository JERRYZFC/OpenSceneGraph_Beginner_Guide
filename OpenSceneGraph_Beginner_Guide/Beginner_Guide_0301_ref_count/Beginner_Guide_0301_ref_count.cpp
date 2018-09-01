#include <Windows.h>

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <iostream>

class MonitoringTarget : public osg::Referenced
{
public:
  MonitoringTarget( int id ) : _id(id)
  { std::cout << "Constructing target " << _id << std::endl; }

protected:
  virtual ~MonitoringTarget()
  { std::cout << "Destroying target " << _id << std::endl; }

  int _id;
};

int main( int argc, char** argv )
{
  if (true)
  {
     
  osg::ref_ptr<MonitoringTarget> target = new MonitoringTarget(0);
  std::cout << "Referenced count before referring: " << target->referenceCount() << std::endl;
  /************************************************************************/
  /*jerrryzfc*/
  /*8/7/2018 17:52:20*/
  /* 此处赋值会造成内存泄露                                                                     */
  /************************************************************************/
  osg::ref_ptr<MonitoringTarget> anotherTarget = target;
  //osg::ref_ptr<MonitoringTarget> anotherTarget = new MonitoringTarget(1);
  std::cout << "Referenced count after referring: " << target->referenceCount() << std::endl;
 
   }

  for ( unsigned int i=2; i<5; ++i )
  {
    osg::ref_ptr<MonitoringTarget> subTarget = new MonitoringTarget(i);
  }
   
  system("pause");
  return 0;
 
}
