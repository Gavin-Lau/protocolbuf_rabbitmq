#include "pack.h"

using std::cout;
using std::endl;

void pack(std::string& serBuf)
{
	ClassBook::StuList stulist;
	ClassBook::Student* stu = stulist.add_stu();
	stu->mutable_addr()->assign("西祀胡同");
	stu->set_id(1);
	stu->set_gender(ClassBook::Student::Gender::Student_Gender_MALE);
	stu->mutable_name()->assign("隔壁老王");
	ClassBook::Student_Scole* scole = stu->mutable_scole();
	scole->set_chinese(98.1);
	scole->set_math(97.1);
	scole->set_english(87.1);
#ifdef MYDEBUG
	cout << stulist.DebugString() << endl;
#endif 
	stulist.SerializeToString(&serBuf);
}

void unpack(std::string& serBuf)
{
	ClassBook::StuList stulist;
	stulist.ParseFromString(serBuf);
#ifdef MYDEBUG
	cout << stulist.DebugString() << endl;
#endif 
}

