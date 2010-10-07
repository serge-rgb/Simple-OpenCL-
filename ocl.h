#include <iostream>
#include <fstream>
#include <cstdio>
#include<cstring>
#include <sstream>
#include <CL/opencl.h>
using namespace std;

#define MAIN

//quick error check
#define CL_CHK(err) if (CL_SUCCESS!=err){\
  cout << "opencl error at " << __FILE__ << ": " <<__LINE__ << endl;\
  exit (-1);							    \
  }

//OpenCL helper class
class OCL
{
public:
  cl_platform_id platform;
  cl_uint        num_devices;
  cl_device_id   *devices;
  cl_context     context;
  
  OCL();
  cl_program createProgram (string fname);
  void buildProgram (cl_program prgrm);
  cl_kernel createKernel (cl_program prgrm,string name);
  //kernel,index,data
  void setKernelArg (cl_kernel,int,size_t,void*);
  virtual ~OCL();

  //---helpers
  void _print_profile_info (cl_platform_id pf, string msg,cl_platform_info id);
};
