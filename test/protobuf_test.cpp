#include <iostream>

#ifdef _MSC_VER
# ifdef _DEBUG
#  pragma comment(lib,"libprotobufd.lib")
#  pragma comment(lib,"libprotocd.lib")
# else
#  pragma comment(lib,"libprotobuf.lib")
#  pragma comment(lib,"libprotoc.lib")
# endif
#endif
using std::cout;
using std::endl;

int main()
{

	getchar();
	return 0;
}
