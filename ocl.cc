#include <iostream>
#include <fstream>
#include <sstream>
#include <CL/opencl.h>

using namespace std;

//called by ocl when something blows up
void ocl_callback (const char* errinfo,
		   const void* privinfo,
		   size_t cb, void *user_data){
  cout << "something blew up"<< endl;
}

class OCL
{
private:
  cl_platform_id platform;
  cl_uint        num_devices;
  cl_device_id   *devices;
  cl_context     context;
  void _print_profile_info (cl_platform_id pf, string msg,cl_platform_info id);
public:
  OCL();
  cl_program createProgram (string fname);
  virtual ~OCL();
};


int main(int argc, char *argv[])
{
  OCL ocl;
  return 0;
}

OCL::OCL (){
  cl_uint num_entries = 10;
  cl_platform_id platforms [10];
  cl_uint num_platforms = -1;
  cl_int err; //Store return value for various funcs.


  //============Platform

  
  err = clGetPlatformIDs (num_entries,
				 platforms,
				 &num_platforms);

  if (err!=CL_SUCCESS){
    cerr << "Error getting platform\n";
    exit (-1);
  }

  cout << "\n\n\nFound " << num_platforms << " platforms" << endl;

  
  for (int i=0;i<num_platforms;i++){

    cout << "\nPlatform " << i << endl;
    
    cl_platform_id pf = platforms [i];

    _print_profile_info (pf,"Platform profile",CL_PLATFORM_PROFILE);
    _print_profile_info (pf,"Platform version",CL_PLATFORM_VERSION);
    _print_profile_info (pf,"Platform name",CL_PLATFORM_NAME);
    _print_profile_info (pf,"Platform vendor",CL_PLATFORM_VENDOR);
    _print_profile_info (pf,"Platform extensions",CL_PLATFORM_EXTENSIONS);

    //Select our platform with no particular criteria
    this->platform = pf;
  }

  
  //==============Device

  
  
  err = clGetDeviceIDs (this->platform,
			CL_DEVICE_TYPE_GPU,
			0,NULL,&num_devices); //Get number of gpus
  
  if (err!=CL_SUCCESS){
    cout << "error getting device info\n";
    exit (-1);
  }
  this->devices = (cl_device_id*) malloc (sizeof (cl_device_id)*num_devices);
  err = clGetDeviceIDs (this->platform,
			CL_DEVICE_TYPE_GPU,
			num_devices,devices,NULL);
  if (err!=CL_SUCCESS){
    cout << "fuck you..\n";
    exit (-1);
  }

  cout << "\n\n\nFound " << num_devices << " devices\n";

  //Print devices.
  for (int i = 0; i < num_devices; ++i) {
    cl_device_id device = devices [i];
    size_t strsize;
    clGetDeviceInfo (device,
		     CL_DEVICE_NAME,0,NULL,&strsize);
    char *str = (char*)malloc (strsize);
    clGetDeviceInfo (device,
		     CL_DEVICE_NAME,strsize,str,0);
    cout << "\nDevice " << i << ": " << str << endl;
    free (str);
  }

  //TODO: Make it use multiple CPUs as well..

  //===========Create context
  const cl_context_properties plist [] = {
    CL_CONTEXT_PLATFORM,
    (cl_context_properties) this->platform,
    0}; //END WITH 0..
  this->context = clCreateContext (plist,
				   this->num_devices,
				   this->devices,
				   ocl_callback,
				   NULL,
				   &err);
  if (err!=CL_SUCCESS){
    cout << "Could not create context\n";
    if (err==CL_INVALID_PLATFORM){
      cout << "Invalid Platform"<<endl;
    }
    if (err==CL_INVALID_VALUE){
      cout << "Invalid value\n";
      cout << "num_devices is " << num_devices << endl;
      cout << "devices is null? " <<   (devices==NULL) << endl;
    }
    exit (-1);
  }

  //   COMMAND QUEUE
  cl_command_queue q = clCreateCommandQueue (this->context,
					     this->devices [0], //This is just a test.. use gpu
					     0,
					     &err);
  if (err!=CL_SUCCESS){
    cout << "Cant create command queue\n";
    exit (-1);
  }

  //do something
  
  //read source
  cl_program prgrm = createProgram("ocl.cl");

  
  clReleaseProgram (prgrm);

  if (err!=CL_SUCCESS){
    cout << " ...\n";
  }
  
  
  clReleaseCommandQueue (q);
  //   END COMMAND QUEUE
}

cl_program OCL::createProgram (string fname){
  cl_int err;
  ifstream file ("ocl.cl");
  stringstream ss;
  
  int num_lines = 1;
  while (file.good ()){
    char c;
    if (c=='\n') num_lines++;
    c = file.get();
    ss.put (c);
  }
  
  char *src [num_lines];
  size_t lengths [num_lines];
  int i=0;
  
  while (!ss.eof ()){
    char *str=(char*)malloc (128*sizeof (char));
    ss.getline(str,128);  
    string s = str;
    lengths [i]=s.length ();
    
    src[i]=str;
    i++;
  }
  
  cl_program prgrm = clCreateProgramWithSource (this->context,
						num_lines,
						(const char**) src,
						lengths,
						&err);
  return prgrm;
}

// Helpers
void OCL::_print_profile_info (cl_platform_id pf, string msg,cl_platform_info id){
  size_t str_size;
  cl_uint ret = clGetPlatformInfo (pf,
				   id, 0, NULL, &str_size);
  if (ret!=CL_SUCCESS){
    cout << "Error getting platform info.\n";
    exit (-1);
  }
  void *str = malloc (str_size);
  ret = clGetPlatformInfo (pf,
			   id,
			   str_size,
			   str,
			   NULL);
  if (ret!=CL_SUCCESS){
    cout << "Error getting platform info.\n";
    exit (-1);
  }
  cout << msg << ": " << (char*) str << endl; //Full or embedded
  free (str);
}

OCL::~OCL (){
  if (devices) free(devices);
  clReleaseContext (this->context);
}

