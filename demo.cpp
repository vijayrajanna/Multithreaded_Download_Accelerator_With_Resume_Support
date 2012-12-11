#include<iostream>
using namespace std;
#include "guicpp.hpp"
using namespace guicpp;

int main(int argc,char **argv)
{
  GuiCppApp::initialize(&argc,&argv);
	Window w("ss",222,333);
	w.show();
	GuiCppApp::run();
	return 0;
}

