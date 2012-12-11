#include <iostream>
using namespace std;
#include <string.h>
#include <stdlib.h>

#include "guicpp.hpp"
using namespace guicpp;

class Screen
{
	protected:
				Window *w;
				FixedWidget *f;
	public:
				Screen(char *title,int width,int height);
				virtual ~Screen();
				virtual void create()=0;
				void show();
				void hide();
				static void OnWindowClose(Widget *widget,Event event,void *info);
};


class LoginScreen: public Screen
{
	Label *usernameLabel;
	Label *passwordLabel;
	TextField *usernameTextField;
	TextField *passwordTextField;
	Button *OkButton;
	Button *CancelButton;
	
	public:
			LoginScreen();
			~LoginScreen();
			void create();
			static void OnOkButtonClicked(Widget *widget,Event event,void *info);
			static void OnCancelButtonClicked(Widget *widget,Event event,void *info);
};

class StudentFormScreen: public Screen
{
	Label *nameLabel;
	TextField *nameTextField;
	Label *sexLabel;
	RadioButton *MaleRadioButton;
	RadioButton *FemaleRadioButton;
	Label *preferedTimeLabel;
	ToggleButton *morningToggleButton;
	ToggleButton *afternoonToggleButton;
	ToggleButton *eveningToggleButton;
	Label *questionLabel;	
	CheckButton *friendsCheckButton;
	CheckButton *advertismentCheckButton;
	CheckButton *pampletsCheckButton;
	Label *coursesofferedLabel;
	ScrollableWindow *coursesScrollableWindow;
	CList *coursesCList;
	Label *summaryLabel;
	TextArea *summaryTextArea;
	Button *printButton;
	Button *backButton;

	public:
			StudentFormScreen();
			~StudentFormScreen();
			void create();
			
			static void onPrintButtonClicked(Widget *widget,Event event,void *info);	
			static void onBackButtonClicked(Widget *widget,Event event,void *info);
};

class Application 
{
	public:
			LoginScreen *loginscreen;
		    StudentFormScreen *studentscreen;	
			Application();
			~Application();
			void create();
			void show();
}*app;	

int main(int argc,char **argv)
{
    GuiCppApp::initialize(&argc,&argv);
    app=new Application();
    app->create();
    app->show();
    GuiCppApp::run();
    exit(0);
}

StudentFormScreen::StudentFormScreen():Screen("STUDENT INFO SCREEN",600,300)
{
	nameLabel=new Label("name:");
	sexLabel=new Label("sex:");
	nameTextField=new TextField();
	MaleRadioButton=new RadioButton("male");
	FemaleRadioButton=new RadioButton("female",*MaleRadioButton);
	preferedTimeLabel=new Label("preferedTime:"); 
	morningToggleButton=new ToggleButton("morning");
	afternoonToggleButton=new ToggleButton("afternoon");
	eveningToggleButton=new ToggleButton("evening");
	questionLabel =new Label("Heard abt us frm:");
	friendsCheckButton=new CheckButton("friends");
    advertismentCheckButton=new CheckButton("advertisment");
    pampletsCheckButton=new CheckButton("pamplets");
	coursesofferedLabel=new Label("courses offered:");
	coursesScrollableWindow=new ScrollableWindow();
    char *titles[4]= {"  ID  ","  name  ","  Cost  "};
	coursesCList=new CList(3,titles);
    {
	char *titles[4]= {"1","c","2500"};
	coursesCList->append(titles);
	}
	{
    char *titles[4]= {"2","c++","3000"};
    coursesCList->append(titles);
    }
	{
    char *titles[4]= {"3","java","10000"};
    coursesCList->append(titles);
    }

	
	summaryLabel=new Label("summary");
    summaryTextArea=new TextArea();
    printButton=new Button("print");
    backButton=new Button("back");
}

StudentFormScreen::~StudentFormScreen()
{
	delete nameLabel;
    delete sexLabel;
    delete nameTextField;
    delete MaleRadioButton;
    delete FemaleRadioButton;
    delete preferedTimeLabel;
    delete morningToggleButton;
    delete afternoonToggleButton;
    delete eveningToggleButton;
    delete questionLabel ;
    delete friendsCheckButton;
    delete advertismentCheckButton;
    delete pampletsCheckButton;
    delete coursesofferedLabel;
    delete coursesScrollableWindow;
    delete coursesCList;
    delete summaryLabel;
    delete summaryTextArea;
    delete printButton;
    delete backButton;
}

void StudentFormScreen::create()
{
	
	nameTextField->setSize(60,30);
	summaryTextArea->setSize(400,100);
	printButton->setSize(60,30);
	backButton->setSize(60,30);
	coursesScrollableWindow->setSize(200,100);
	MaleRadioButton->getState();
	FemaleRadioButton->getState();
	morningToggleButton->getState();
	afternoonToggleButton->getState();
	eveningToggleButton->getState();
	friendsCheckButton->getState();
	advertismentCheckButton->getState();
	pampletsCheckButton->getState();
	f->add(*nameLabel,50,50);
	f->add(*nameTextField,120,50);
	f->add(*sexLabel,50,100);
	f->add(*MaleRadioButton,150,100);
	f->add(*FemaleRadioButton,230,100);
	f->add(*preferedTimeLabel,50,140);
    f->add(*morningToggleButton,150,140);
	f->add(*afternoonToggleButton,250,140);
	f->add(*eveningToggleButton,370,140);
	f->add(*questionLabel,50,180);
	f->add(*friendsCheckButton,170,180);
	f->add(*advertismentCheckButton,280,180);
	f->add(*pampletsCheckButton,390,180);
	f->add(*coursesofferedLabel,50,220);
	coursesCList->setSelectionMode(coursesCList->MULTISELECT_MODE);
	coursesScrollableWindow->add(*coursesCList);
	f->add(*coursesScrollableWindow,50,260);
	f->add(*summaryLabel,50,370);
	f->add(*summaryTextArea,120,380);
	f->add(*printButton,50,500);
	f->add(*backButton,200,500);
	w->add(*f);
	printButton->setHandler(Event(clicked),onPrintButtonClicked,NULL);
	backButton->setHandler(Event(clicked),onBackButtonClicked,NULL);

}	

void StudentFormScreen:: onPrintButtonClicked(Widget *widget,Event event,void *info)
{
	int len;
	len = app->studentscreen->summaryTextArea->getLength();
	 app->studentscreen->summaryTextArea->setPosition(len);
	app->studentscreen->summaryTextArea->deleteBack(len);


	char name[20];
	char arr[100]=" ";
	strcat(arr,"name: ");
	app->studentscreen->nameTextField->getText(name,0);
	strcat(arr,name);
	strcat(arr,"\ngender: ");
    if(app->studentscreen->MaleRadioButton->getState())

	strcat(arr,"Male");
	else
	strcat(arr,"Female");

	strcat(arr,"\nPreffered Time: ");
	
    if(app->studentscreen->morningToggleButton->getState())

	strcat(arr,"Morning ");

    if(app->studentscreen->afternoonToggleButton->getState())
	strcat(arr,"Afternoon ");

    if(app->studentscreen->eveningToggleButton->getState())
	strcat(arr,"Evening ");
	
	strcat(arr,"\nHow do u know abt us: ");

	if(app->studentscreen->friendsCheckButton->getState())
	strcat(arr,"Friends ");

	if(	app->studentscreen->advertismentCheckButton->getState())
	strcat(arr,"advertisements ");

	if(	app->studentscreen->pampletsCheckButton->getState())
	strcat(arr,"pamplets ");
	strcat(arr,"\nCourses Offered: ");
	
	int a,b,i=0;
	a = app->studentscreen->coursesCList->getSelectionCount();
	char array[5][20]={"1 c 2000\n","2 c++ 3000\n","3 java 4000"};
	while(a)
	{
	 	b = app->studentscreen->coursesCList->getSelection(i);	
	 	i++;
	 	strcat(arr,array[b]);
		a--;
	}
		app->studentscreen->summaryTextArea->insert(arr);
		
}
void StudentFormScreen:: onBackButtonClicked(Widget *widget,Event event,void *info)
{
	app->studentscreen->hide();
    app->loginscreen->show();   		
}


Screen::Screen(char *title,int width,int height)
{
	w=new Window(title,width,height);
	f=new FixedWidget();
	w->setHandler(Event(destroyEvent),OnWindowClose,NULL);
}

Screen::~Screen()
{
	delete w;
	delete f;
}

void Screen::show()
{
	w->show();
}

void Screen::hide()
{
	w->hide();
}

void Screen::OnWindowClose(Widget *widget,Event event,void *info)
{
	GuiCppApp::terminate();
}

LoginScreen::LoginScreen():Screen("Login Screen",400,300)
{
	usernameLabel=new Label("Username");
	passwordLabel=new Label("Password");
	usernameTextField=new TextField();
	passwordTextField=new TextField();
	OkButton=new Button("Ok");
	CancelButton=new Button("Cancel");
}

LoginScreen::~LoginScreen()
{
	delete usernameLabel;
	delete passwordLabel;
	delete usernameTextField;;
	delete passwordTextField;
	delete OkButton;
	delete CancelButton;
}

void LoginScreen::create()
{
	usernameTextField->setSize(125,30);
	passwordTextField->setSize(125,30);
	passwordTextField->setVisibility(false);
	OkButton->setSize(100,30);
	CancelButton->setSize(100,30);

	f->add(*usernameLabel,50,50);
	f->add(*usernameTextField,150,50);
	f->add(*passwordLabel,50,100);
	f->add(*passwordTextField,150,100);
	f->add(*OkButton,50,175);
	f->add(*CancelButton,200,175);
    w->add(*f);
	OkButton->setHandler(Event(clicked),OnOkButtonClicked,NULL);
    CancelButton->setHandler(Event(clicked),OnCancelButtonClicked,NULL);
}

void LoginScreen::OnOkButtonClicked(Widget *widget,Event event,void *info)
{
	char username[100];
	char password[100];
    app->loginscreen->usernameTextField->getText(username,0);
 	app->loginscreen->passwordTextField->getText(password,0);
    if(strcmp(username,"scott")==0 && strcmp(password,"tiger")==0)
   	{
		app->loginscreen->hide();
		app->studentscreen->show();
	}

    app->loginscreen->passwordTextField->setText("");
}

void LoginScreen::OnCancelButtonClicked(Widget *widget,Event event,void *info)
{
	GuiCppApp::terminate();
}

Application::Application()
{
	loginscreen = new LoginScreen();
	studentscreen = new StudentFormScreen();
}
            
Application::~Application()
{
	delete loginscreen;
	delete studentscreen;
}

void Application::create()
{
	loginscreen->create();	
	studentscreen->create();
}

void Application::show()
{
	loginscreen->show();
	
}


