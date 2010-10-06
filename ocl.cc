#include <iostream>
#include <CL/opencl.h>
using namespace std;



class OCL
{
private:
  cl_platform_id platform;
  void _print_profile_info (cl_platform_id pf, string msg,cl_platform_info id);
public:
  OCL();
  virtual ~OCL();
};

OCL::~OCL (){
  
}

//For platform pf, print out msg and then print the result of quering id. (p 32. spec)
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

OCL::OCL (){
  cl_uint num_entries = 10;
  cl_platform_id platforms [10];
  cl_uint num_platforms = -1;
  cl_int err = clGetPlatformIDs (num_entries,
				 platforms,
				 &num_platforms);

  if (err!=CL_SUCCESS){
    cerr << "Error getting platform\n";
    exit (-1);
  }

  cout << "Found " << num_platforms << " platforms" << endl;
  
  for (int i=0;i<num_platforms;i++){

    cout << "\n\n\nPlatform " << i << endl;
    
    cl_platform_id pf = platforms [i];

    _print_profile_info (pf,"Platform profile",CL_PLATFORM_PROFILE);
    _print_profile_info (pf,"Platform version",CL_PLATFORM_VERSION);
    _print_profile_info (pf,"Platform name",CL_PLATFORM_NAME);
    _print_profile_info (pf,"Platform vendor",CL_PLATFORM_VENDOR);
    _print_profile_info (pf,"Platform extensions",CL_PLATFORM_EXTENSIONS);
            
  }
}

int main(int argc, char *argv[])
{
  OCL ocl;
  cout << "meh" << endl;
  return 0;
}
