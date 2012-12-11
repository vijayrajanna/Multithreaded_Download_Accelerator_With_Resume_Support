#include <iostream>
using namespace std;
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include "guicpp.hpp"
#include "common.hpp"

using namespace guicpp;

#define PORT 5000
#define D printf("[%d]\n",__LINE__);
#define JOBS_FILE	"/home/project/mtda/jobs.dat"

class Job;
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
                static void onWindowClose(Widget *widget,Event event,void *info);
};


class ConnectScreen:public Screen
{
	Label *ipAddressLabel;
	TextField *ipAddressTextField;
	Button *okButton;


	public:
			ConnectScreen();
			~ConnectScreen();
			void create();
			static void onOkButtonClicked(Widget *widget,Event event,void *info);
};


class DataTransferScreen:public Screen
{
	Label *connectLabel;
	Label *localSystemLabel;
	Label *remoteSystemLabel;
	Label *statusLabel;
	TextField *connectTextField;
	TextField *localSystemTextField;
	TextField *remoteSystemTextField;
	TextField *statusTextField;
	Button *disconnectButton;
	Button *fetchButton;
	Button *sendButton;
	Button *localBrowseButton;
	Button *remoteBrowseButton;
	Button *detailsButton;
	ScrollableWindow *localSystemScrollableWindow;
	ScrollableWindow *remoteSystemScrollableWindow;
	CList *localSystemCList;
	CList *remoteSystemCList;

public:
	vector<Job*> jobVector;
	
	DataTransferScreen();
	~DataTransferScreen();
	void create();
	void show();
	static void onDisconnectButtonClicked(Widget *widget,Event event,void *info);
	static void onFetchButtonClicked(Widget *widget,Event event,void *info);
	static void onSendButtonClicked(Widget *widget,Event event,void *info);
	static void onLocalBrowseButtonClicked(Widget *widget,Event event,void *info);
	static void onRemoteBrowseButtonClicked(Widget *widget,Event event,void *info);
	static void onDetailsButtonClicked(Widget *widget,Event event,void *info);
	vector<string> localfile;
	vector<string> remotefile;
};


class ProgressScreen:public Screen
{
	Button *pauseButton;
	Button *pauseAllButton;
	Button *resumeButton;
	Button *resumeAllButton;
	Button *cancelButton;
	Button *cancelAllButton;
	Button *clearButton;
	Button *clearAllButton; 
	Button *returnButton;
	ScrollableWindow *progressScreenScrollableWindow;

	public:
		CList *progressCList;
			ProgressScreen();
			~ProgressScreen();
			void create();
			static void onPauseButtonClicked(Widget *widget,Event event,void *info);
			static void onPauseAllButtonClicked(Widget *widget,Event event,void *info);
			static void onResumeButtonClicked(Widget *widget,Event event,void *info);
			static void onResumeAllButtonClicked(Widget *widget,Event event,void *info);
			static void onCancelButtonClicked(Widget *widget,Event event,void *info);
			static void onCancelAllButtonClicked(Widget *widget,Event event,void *info);
			static void onClearButtonClicked(Widget *widget,Event event,void *info);
			static void onClearAllButtonClicked(Widget *widget,Event event,void *info);
			static void onReturnButtonClicked(Widget *widget,Event event,void *info);
};



class Application
{
	public:
			char apparr[40];
			char patharr[256];
			int sockfd,clientsock;
			struct sockaddr_in serv_addr;
			ConnectScreen *connectScreen;
			DataTransferScreen *dataTransferScreen;
			ProgressScreen *progressScreen;
            Application();
			~Application();
			void create();
			void show();
}*app;

enum
{
	JOB_RUNNING,
	JOB_COMPLETED,
	JOB_PAUSED,
	JOB_RESUME,
	JOB_CANCELLED
};

class Job
{
	char remoteDirectory[256];
	char localDirectory[256];
	char filename[256];
	char ipAddress[50];
	bool download;
	int start;
	int size;
	int transferred;
	int slot;
	int status;
	pthread_t threadID;
	int sockfd;

	static int monitorJobCount;
	
	Job *jobs[MAX_JOBS];
	int jobCount;
	void splitIntoJobs();

	static void* startThread(void *arg);
	//static void* monitorThread(void *arg);
	static gboolean monitorThread(void *arg);
	static void addJob();
	static void removeJob();
	unsigned short getCRC(int sockfd, char filename[], int start, int size);
	unsigned short getCRC(char filename[], int start, int size);
	static bool jobReslottingInProgress;

	bool discarded;
	bool discardedAck;
	bool parent;
public:
	Job(char remoteDirectory[], char localDirectory[], char filename[], bool download, char ipAddress[], int status, int slot);
	Job(char remoteDirectory[], char localDirectory[], char filename[], bool download, char ipAddress[], int status, int start, int length);
	Job(int fd);
	~Job();

	void pause();
	void resume();
	void cancel();
	int getStatus();
	void setStatus(int status);
	static void scheduleReslotting();
	static void endReslotting();
	void setSlot(int slot);
	int getSlot() { return slot; }
	void saveState();

	static void saveState(int);
	static void resumeState();

	static char saveFile[256];	
	friend void ProgressScreen::onClearButtonClicked(Widget*widget,Event E,void* info);
};

int Job::monitorJobCount;
bool Job::jobReslottingInProgress = false;
char Job::saveFile[256]="/home/temp/kiran/multidown/save.dat";

int getCRC(int fd, int start, int size)
{
	return 0;
}

int main(int argc,char **argv)
{
    GuiCppApp::initialize(&argc,&argv);
    app = new Application();
    app->create();
    app->show();
    GuiCppApp::run();
    exit(0);
}

Screen::Screen(char *title,int width,int height)
{
    w=new Window(title,width,height);
    f=new FixedWidget();
    w->setHandler(Event(destroyEvent),onWindowClose,NULL);
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

void Screen::onWindowClose(Widget *widget,Event event,void *info)
{
    GuiCppApp::terminate();
}


ConnectScreen::ConnectScreen():Screen("Connect",250,200)
{
	ipAddressLabel = new Label("IP Address:");
	ipAddressTextField = new TextField();
	okButton = new Button("Ok");
}

ConnectScreen::~ConnectScreen()
{
	delete ipAddressLabel;
	delete ipAddressTextField;
	delete okButton;
}

void ConnectScreen::create()
{
	ipAddressTextField->setSize(100,30);
	okButton->setSize(100,30);
	
	f->add(*ipAddressLabel,30,50);
	f->add(*ipAddressTextField,110,50);
	f->add(*okButton,85,100);
	w->add(*f);
	okButton->setHandler(Event(clicked),onOkButtonClicked,NULL);
}

void ConnectScreen::onOkButtonClicked(Widget *widget,Event event,void *info)
{	
		
	app->connectScreen->ipAddressTextField->getText(app->apparr,0);
	cout << "Connecting to " << app->apparr << endl;
	
		app->sockfd=socket(AF_INET,SOCK_STREAM,0);

		app->serv_addr.sin_family=AF_INET;
		app->serv_addr.sin_addr.s_addr=inet_addr(app->apparr);
		app->serv_addr.sin_port=htons(PORT);
	
		if((app->sockfd)<0)
		{
			cout<<"error\n";
		}
		if(connect(app->sockfd,(struct sockaddr *)&app->serv_addr,sizeof(app->serv_addr))<0)
		{
			perror("Connect");
			return;
		}
		app->connectScreen->hide();	
		app->dataTransferScreen->show();	
	
}

DataTransferScreen::DataTransferScreen():Screen("Enhanced Data Transfer",700,550)
{
	connectLabel = new Label("Connected To::");
	localSystemLabel = new Label("Local System");
	remoteSystemLabel = new Label("Remote System");
	statusLabel = new Label("Status");
	disconnectButton = new Button("Disconnect");
	fetchButton = new Button("Fetch");
	sendButton = new Button("Send");
	localBrowseButton = new Button("Browse");
	remoteBrowseButton = new Button("Browse");
	detailsButton = new Button("Details");
	localSystemScrollableWindow = new ScrollableWindow();
	remoteSystemScrollableWindow = new ScrollableWindow();
	char *titles[7]= {" Name            "," Type         "," Size "," perm           ","Owner","Group","time"};
    localSystemCList = new CList(7,titles);
	char *titles1[7]={" Name            "," Type "," Size "," perm ","Owner","Group","time"}; 
    remoteSystemCList = new CList(7,titles1);
	connectTextField = new TextField();
	localSystemTextField = new TextField();
	remoteSystemTextField = new TextField();
	statusTextField = new TextField();
}

DataTransferScreen::~DataTransferScreen()
{
	delete connectLabel;
	delete localSystemLabel;
	delete remoteSystemLabel;
	delete statusLabel;
	delete connectTextField;
	delete localSystemTextField;
	delete remoteSystemTextField;
	delete statusTextField;
	delete disconnectButton;
	delete fetchButton;
	delete sendButton;
	delete localBrowseButton;
	delete remoteBrowseButton;
	delete detailsButton;
	delete localSystemScrollableWindow;
	delete remoteSystemScrollableWindow;
	delete localSystemCList;
	delete remoteSystemCList;
}

void DataTransferScreen::create()
{
	connectTextField->setSize(100,30);
	localSystemTextField->setSize(100,30);
	remoteSystemTextField->setSize(100,30);
	statusTextField->setSize(80,30);
	disconnectButton->setSize(100,30);
	fetchButton->setSize(80,30);
	sendButton->setSize(80,30);
	localBrowseButton->setSize(80,30);
	remoteBrowseButton->setSize(80,30);
	detailsButton->setSize(80,30);
	localSystemScrollableWindow->setSize(230,200);
	remoteSystemScrollableWindow->setSize(230,200);
	
	localSystemScrollableWindow->add(*localSystemCList);
	localSystemCList->setSelectionMode(CList::MULTISELECT_MODE);
	localSystemCList->setColumnWidth(0,150);
	localSystemCList->setColumnWidth(1,120);
	localSystemCList->setColumnWidth(2,70);	
	localSystemCList->setColumnWidth(3,120);
	localSystemCList->setColumnWidth(4,50);
	localSystemCList->setColumnWidth(5,50);
	localSystemCList->setColumnWidth(6,200);
	
 	remoteSystemScrollableWindow->add(*remoteSystemCList);
	remoteSystemCList->setSelectionMode(CList::MULTISELECT_MODE);
	remoteSystemCList->setColumnWidth(0,150);
	remoteSystemCList->setColumnWidth(1,120);
	remoteSystemCList->setColumnWidth(2,70);
	remoteSystemCList->setColumnWidth(3,120);
	remoteSystemCList->setColumnWidth(4,50);
	remoteSystemCList->setColumnWidth(5,50);
	remoteSystemCList->setColumnWidth(6,200);
	
	f->add(*connectLabel,40,50);
	f->add(*connectTextField,150,50);
	connectTextField->setEditable(false);
	f->add(*localSystemLabel,40,100);
	f->add(*localSystemTextField,150,100);
	localSystemTextField->setEditable(false);
	f->add(*disconnectButton,450,50);
	f->add(*remoteSystemLabel,330,100);
	f->add(*remoteSystemTextField,450,100);
	f->add(*localSystemScrollableWindow,40,200);
	f->add(*fetchButton,320,250);
	f->add(*sendButton,320,300);
	f->add(*remoteSystemScrollableWindow,450,200);
	f->add(*statusLabel,40,450);
	f->add(*statusTextField,100,450);
    f->add(*localBrowseButton,100,490);
    f->add(*remoteBrowseButton,450,490);
    f->add(*detailsButton,450,450);
	w->add(*f);

	disconnectButton->setHandler(Event(clicked),onDisconnectButtonClicked,NULL);
	fetchButton->setHandler(Event(clicked),onFetchButtonClicked,NULL);
	sendButton->setHandler(Event(clicked),onSendButtonClicked,NULL);
	localBrowseButton->setHandler(Event(clicked),onLocalBrowseButtonClicked,NULL);
	remoteBrowseButton->setHandler(Event(clicked),onRemoteBrowseButtonClicked,NULL);
	detailsButton->setHandler(Event(clicked),onDetailsButtonClicked,NULL);
}

void DataTransferScreen::onDisconnectButtonClicked(Widget *widget,Event event,void *info)
{
	app->dataTransferScreen->hide();
	app->connectScreen->show();
}

void DataTransferScreen::onFetchButtonClicked(Widget *widget,Event event,void *info)
{
	char remoteDirectory[256], localDirectory[256], *filename, ipAddress[256];
	int start,size,status;
	bool download;
	int count = app->dataTransferScreen->remoteSystemCList->getSelectionCount();

	int fd = open(JOBS_FILE,O_RDONLY);
	if (fd != -1)
	{
		char c;
		while (read(fd, &c, 1) == 1)
		{
			app->dataTransferScreen->jobVector.push_back(new Job(fd));
		}
		close(fd);
	}
	
	int i;
	for (i=0; i<count; i++)
	{
		int row = app->dataTransferScreen->remoteSystemCList->getSelection(i);

		app->dataTransferScreen->localSystemTextField->getText(localDirectory,0);

		int reply=htonl(GET_CWD);

		write(app->sockfd,&reply,sizeof(reply));

		read(app->sockfd,&reply,sizeof(reply));
		read(app->sockfd,remoteDirectory,256);

		filename=const_cast<char*>(app->dataTransferScreen->remotefile.at(row).c_str());

		app->dataTransferScreen->connectTextField->getText(ipAddress,0);
		
		{
			char *row[5] = {filename,"","0","0%","Unknown"};
			app->progressScreen->progressCList->append(row);
		}
		app->dataTransferScreen->jobVector.push_back(new Job(remoteDirectory, localDirectory, filename, true, ipAddress, JOB_RUNNING, app->dataTransferScreen->jobVector.size()));
	}

}

void DataTransferScreen::onSendButtonClicked(Widget *widget,Event event,void *info)
{
	int count = app->dataTransferScreen->localSystemCList->getSelectionCount();
	
	int fd = open(JOBS_FILE,O_RDONLY);
	if (fd != -1)
	{
		char c;
		while (read(fd, &c, 1) == 1)
		{
			app->dataTransferScreen->jobVector.push_back(new Job(fd));
		}
		close(fd);
	}
	

	int i;
	for (i=0; i<count; i++)
	{
		int row = app->dataTransferScreen->localSystemCList->getSelection(i);

		char remoteDirectory[256], localDirectory[256], *filename, ipAddress[256];
			
		app->dataTransferScreen->localSystemTextField->getText(localDirectory,0);

		int reply=htonl(GET_CWD);

		write(app->sockfd,&reply,sizeof(reply));

		read(app->sockfd,&reply,sizeof(reply));
		read(app->sockfd,remoteDirectory,256);

		filename=const_cast<char*>(app->dataTransferScreen->localfile.at(row).c_str());

		app->dataTransferScreen->connectTextField->getText(ipAddress,0);
		
		{
			char *row[5] = {filename,"","0","0%","Unknown"};
			app->progressScreen->progressCList->append(row);
		}
		app->dataTransferScreen->jobVector.push_back(new Job(remoteDirectory, localDirectory, filename, false, ipAddress, JOB_RUNNING, app->dataTransferScreen->jobVector.size()));
	}

}


void DataTransferScreen::onLocalBrowseButtonClicked(Widget *widget,Event event,void *info)
{
	int a,b;
	char arr[100]=" ";
	a=app->dataTransferScreen->localSystemCList->getSelectionCount();
	if(a==1)
		{
			b=app->dataTransferScreen->localSystemCList->getSelection(0);
			chdir(app->dataTransferScreen->localfile.at(b).c_str());
		}

	app->dataTransferScreen->show();
}

void DataTransferScreen::onRemoteBrowseButtonClicked(Widget *widget,Event event,void *info)
{
	if (app->dataTransferScreen->remoteSystemCList->getSelectionCount() == 0) return;
	int n = app->dataTransferScreen->remoteSystemCList->getSelection(0);
	int cmd = htonl(CHDIR);
	write(app->sockfd, &cmd, sizeof(cmd));
	write(app->sockfd,app->dataTransferScreen->remotefile.at(n).c_str(),256);

	app->dataTransferScreen->show();
}

void DataTransferScreen::onDetailsButtonClicked(Widget *widget,Event event,void *info)
{
	app->dataTransferScreen->hide();
	app->progressScreen->show();
}

void DataTransferScreen::show()
{
/*	DIR *dp;
	struct dirent *dirptr;

	ConnectTextField->setText(app->apparr); 
	getcwd(app->patharr,256);
	LocalSystemTextField->setText(app->patharr);
	LocalSystemCList->clear();

	dp=opendir(".");
	while (dirptr = readdir(dp))
	{
		char *row[] = {dirptr->d_name,"","","","","",""};
		LocalSystemCList->append(row);
	}	
	
	closedir(dp);
	Screen::show();*/

	FILE *fp = popen("ls -a","r");
	char filename[256];
	struct details d;
	int b;
	struct stat st;
	char type[100];
	char perm[90];
	char owner[100];
	char group[100];		
	char size[100];
    char time[100];
   
	
	connectTextField->setText(app->apparr); 
	getcwd(app->patharr,256);
	localSystemTextField->setText(app->patharr);
	localSystemCList->clear();
	localfile.clear();
	
	 while (fgets(filename,256,fp))
    {
        filename[strlen(filename)-1] = '\0';
		
		lstat(filename,&st);
		if(S_ISDIR(st.st_mode))
		strcpy(type,"Directory File");

		if(S_ISCHR(st.st_mode))
		strcpy(type,"Chararcter Device File");

		if(S_ISBLK(st.st_mode))
		strcpy(type,"Block Device File");

		if(S_ISREG(st.st_mode))
		strcpy(type,"Regular File");

		if(S_ISLNK(st.st_mode))
		strcpy(type,"Symbolic Link File");

		if(S_ISFIFO(st.st_mode))
		strcpy(type,"FIFO File");
		strcpy(perm,"");

		if(st.st_mode & S_IRUSR)
		strcat(perm,"r");
		else
		strcat(perm,"-");

		if(st.st_mode & S_IWUSR)
		strcat(perm,"w");
		else
		strcat(perm,"-");

		if(st.st_mode & S_IXUSR)
		strcat(perm,"x");
		else
		strcat(perm,"-");

		if(st.st_mode & S_IRGRP)
		strcat(perm,"r");
		else
		strcat(perm,"-");

		if(st.st_mode & S_IWGRP)
		strcat(perm,"w");
		else
		strcat(perm,"-");

		if(st.st_mode & S_IXGRP)
		strcat(perm,"x");
		else
		strcat(perm,"-");

		if(st.st_mode & S_IROTH)
		strcat(perm,"r");
		else
		strcat(perm,"-");

		if(st.st_mode & S_IWOTH)
		strcat(perm,"w");
		else
		strcat(perm,"-");

		if(st.st_mode & S_IXOTH)
		strcat(perm,"x");
		else
		strcat(perm,"-");
		
		sprintf(size,"%d",st.st_size);
		sprintf(owner,"%d",st.st_uid);
		sprintf(group,"%d",st.st_gid);
		
		strcpy(time,ctime(&st.st_mtime)); 
		time[strlen(time)-1] = '\0';

        char *row[] = {filename,type ,size,perm,owner,group,time};
        localSystemCList->append(row);
		localfile.push_back(filename);
    }
	pclose(fp);           

	char arr[256];
	int x=htonl(GET_CWD);
	write(app->sockfd,&x,sizeof(x));
	read(app->sockfd,&x,sizeof(x));
	read(app->sockfd,arr,256);
	remoteSystemTextField->setText(arr);


	int y=FETCH_CONTENTS;
	y=htonl(FETCH_CONTENTS);
	write(app->sockfd,&y,sizeof(y));

	remotefile.clear();
	remoteSystemCList->clear();D
	while(1)
	{
		read(app->sockfd,&y,sizeof(y));
    	y = ntohl(y);

		if(y==END)
			break;
	
		read(app->sockfd,&d,sizeof(d));
        char *row[] = {d.filename,d.type ,d.size,d.perm,d.owner,d.group,d.time};
		remoteSystemCList->append(row);

		int z=ACK;
		z=htonl(ACK);
		write(app->sockfd,&z,sizeof(z));

		remotefile.push_back(d.filename);
	}
	Screen::show();
}

ProgressScreen::ProgressScreen():Screen("Progress Screen",700,500)
{
	pauseButton = new Button("Pause");
    pauseAllButton = new Button("Pause All");
    resumeButton=new Button("Resume");
    resumeAllButton=new Button("Resume All");
    cancelButton=new Button("Cancel");
    cancelAllButton=new Button("Cancel All");
    clearButton=new Button("Clear");
    clearAllButton=new Button("Clear All");
    returnButton=new Button("Return");
    progressScreenScrollableWindow=new ScrollableWindow();
    char *titles2[5]={"Filename           ","Total Size","Bytes Transferred","Percent Completed","Status           "}; 
	progressCList=new CList(5,titles2);
}

ProgressScreen::~ProgressScreen()
{
	delete pauseButton;
    delete pauseAllButton;
    delete resumeButton;
    delete resumeAllButton;
    delete cancelButton;
    delete cancelAllButton;
    delete clearButton;
    delete clearAllButton;
    delete returnButton;
    delete progressScreenScrollableWindow;
    delete progressCList;
}

void ProgressScreen::create()
{
	progressScreenScrollableWindow->setSize(600,300);
	pauseButton->setSize(100,30);
    pauseAllButton->setSize(100,30);
    resumeButton->setSize(100,30);
    resumeAllButton->setSize(100,30);
    cancelButton->setSize(100,30);
    cancelAllButton->setSize(100,30);
    clearButton->setSize(100,30);
    clearAllButton->setSize(100,30);
    returnButton->setSize(100,30);
	
    
	f->add(*pauseButton,75,375);
	f->add(*pauseAllButton,75,420);
	f->add(*resumeButton,200,375);
	f->add(*resumeAllButton,200,420);
	f->add(*cancelButton,330,375);
	f->add(*cancelAllButton,330,420);
	f->add(*clearButton,465,375);
	f->add(*clearAllButton,465,420);
	f->add(*returnButton,265,460);
	f->add(*progressScreenScrollableWindow,50,50);
	progressScreenScrollableWindow->add(*progressCList);
	w->add(*f);

	pauseButton->setHandler(Event(clicked),onPauseButtonClicked,NULL);
	pauseAllButton->setHandler(Event(clicked),onPauseAllButtonClicked,NULL);
	resumeButton->setHandler(Event(clicked),onResumeButtonClicked,NULL);
	resumeAllButton->setHandler(Event(clicked),onResumeAllButtonClicked,NULL);
	cancelButton->setHandler(Event(clicked),onCancelButtonClicked,NULL);
	cancelAllButton->setHandler(Event(clicked),onCancelAllButtonClicked,NULL);
	clearButton->setHandler(Event(clicked),onClearButtonClicked,NULL);
	clearAllButton->setHandler(Event(clicked),onClearAllButtonClicked,NULL);
	returnButton->setHandler(Event(clicked),onReturnButtonClicked,NULL);
}

void ProgressScreen::onPauseButtonClicked(Widget*widget,Event E,void* info)
{
	int rows = app->progressScreen->progressCList->getSelectionCount();

	for (int i=0; i<rows; i++)
	{
		int row = app->progressScreen->progressCList->getSelection(i);
		app->dataTransferScreen->jobVector[row]->setStatus(JOB_PAUSED);
	}
}

void ProgressScreen::onPauseAllButtonClicked(Widget*widget,Event E,void* info)
{
	int rows = app->dataTransferScreen->jobVector.size();

	for (int i=0; i<rows; i++)
	{
		app->dataTransferScreen->jobVector[i]->setStatus(JOB_PAUSED);
	}
}

void ProgressScreen::onResumeButtonClicked(Widget*widget,Event E,void* info)
{
	int rows = app->progressScreen->progressCList->getSelectionCount();

	for (int i=0; i<rows; i++)
	{
		int row = app->progressScreen->progressCList->getSelection(i);
		app->dataTransferScreen->jobVector[row]->setStatus(JOB_RESUME);
	}
}

void ProgressScreen::onResumeAllButtonClicked(Widget*widget,Event E,void* info)
{
	int rows = app->dataTransferScreen->jobVector.size();

	for (int i=0; i<rows; i++)
	{
		app->dataTransferScreen->jobVector[i]->setStatus(JOB_RESUME);
	}
}

void ProgressScreen::onCancelButtonClicked(Widget*widget,Event E,void* info)
{
	int rows = app->progressScreen->progressCList->getSelectionCount();

	for (int i=0; i<rows; i++)
	{
		int row = app->progressScreen->progressCList->getSelection(i);
		if (app->dataTransferScreen->jobVector[row]->getStatus() != JOB_COMPLETED)
			app->dataTransferScreen->jobVector[row]->setStatus(JOB_CANCELLED);
	}
}

void ProgressScreen::onCancelAllButtonClicked(Widget*widget,Event E,void* info)
{
	int rows = app->dataTransferScreen->jobVector.size();

	for (int i=0; i<rows; i++)
	{
		if (app->dataTransferScreen->jobVector[i]->getStatus() != JOB_COMPLETED)
			app->dataTransferScreen->jobVector[i]->setStatus(JOB_CANCELLED);
	}
}

void ProgressScreen::onClearButtonClicked(Widget*widget,Event E,void* info)
{
	int rows = app->progressScreen->progressCList->getSelectionCount();

	int i;
	int count=0;
	Job::scheduleReslotting();
	app->progressScreen->progressCList->lock();
	
	for (i=0; i<rows; i++)
	{
		int row = app->progressScreen->progressCList->getSelection(i);
		printf("i=%d, Row=%d, Count=%d, Size=%d\n",i,row,count,app->dataTransferScreen->jobVector.size());
		int status = app->dataTransferScreen->jobVector[row-count]->getStatus();
		if (status != JOB_COMPLETED && status != JOB_CANCELLED) continue;
	
//		app->progressScreen->progressCList->remove(row);
		delete app->dataTransferScreen->jobVector[row-count];
		app->dataTransferScreen->jobVector.erase(app->dataTransferScreen->jobVector.begin()+(row-count));
		count++;
	}

	app->progressScreen->progressCList->clear();
	for (i=0; i<app->dataTransferScreen->jobVector.size(); i++)
	{
		app->dataTransferScreen->jobVector[i]->setSlot(i);
		Job *job = app->dataTransferScreen->jobVector[i];
		char *rowList[] = { "", "", "", "", "" };
		app->progressScreen->progressCList->append(rowList);
		
		job->transferred = 0;
		for (int j=0; j<job->jobCount; j++)
		{
			job->transferred += job->jobs[j]->transferred;
		}
			
		char sizeStr[100], bytesTransferredStr[100], percentCompletedStr[100];
		sprintf(sizeStr, "%d", job->size);
		sprintf(bytesTransferredStr, "%d", job->transferred);
		sprintf(percentCompletedStr, "%.2f%%", (float)job->transferred/job->size*100);
			
		app->progressScreen->progressCList->setContents(i,0,job->filename);
		app->progressScreen->progressCList->setContents(i,1,sizeStr);
		app->progressScreen->progressCList->setContents(i,2,bytesTransferredStr);
		app->progressScreen->progressCList->setContents(i,3,percentCompletedStr);
		
		char *statusStr[] = {"Running","Completed","Paused","Resumed","Cancelled"};
		app->progressScreen->progressCList->setContents(job->slot,4,statusStr[job->getStatus()]);
	}

	printf("New job count=%d\n",app->dataTransferScreen->jobVector.size());
	Job::endReslotting();
	app->progressScreen->progressCList->unlock();
}

void ProgressScreen::onClearAllButtonClicked(Widget*widget,Event E,void* info)
{
	int rows = app->dataTransferScreen->jobVector.size();

	int i;
	int count=0;
	Job::scheduleReslotting();
	app->progressScreen->progressCList->lock();
	
	for (i=0; i<rows; i++)
	{
		int status = app->dataTransferScreen->jobVector[i-count]->getStatus();
		if (status != JOB_COMPLETED && status != JOB_CANCELLED) continue;
		
		app->progressScreen->progressCList->remove(i-count);
		delete app->dataTransferScreen->jobVector[i-count];
		app->dataTransferScreen->jobVector.erase(app->dataTransferScreen->jobVector.begin()+(i-count));
		count++;
	}

	for (i=0; i<app->dataTransferScreen->jobVector.size(); i++)
	{
		app->dataTransferScreen->jobVector[i]->setSlot(i);
	}
	
	Job::endReslotting();
	app->progressScreen->progressCList->unlock();
	printf("New Job count=%d\n",app->dataTransferScreen->jobVector.size());
}

void ProgressScreen::onReturnButtonClicked(Widget*widget,Event E,void* info)
{
	app->progressScreen->Screen::hide();
	app->dataTransferScreen->Screen::show();
}

Job::Job(char remoteDirectory[], char localDirectory[], char filename[], bool download, char ipAddress[], int status, int slot)
{
	// Parent
	parent = true;
	discarded = false;
	discardedAck = false;
	addJob();
	D
	printf("***Job created!***\n");
	strcpy(this->remoteDirectory, remoteDirectory);
	strcpy(this->localDirectory, localDirectory);
	strcpy(this->filename,filename);
	strcpy(this->ipAddress,ipAddress);
	D	
	this->download = download;
	this->slot = slot;
	this->status = status;
	transferred = 0;
	D
	printf("Remote Directory in monitor:%s\n",remoteDirectory);
	printf("local Directory in monitor:%s\n",localDirectory);
	printf("filename in monitor:%s\n",filename);
	printf("IpAddress in monitor:%s\n",ipAddress);
	printf("status in monitor:%d\n",status);

	D	
	
	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(ipAddress);
	D
	if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		setStatus(JOB_CANCELLED);
		removeJob();
		return;
	}
	D
	int n = htonl(CHDIR);
	write(sockfd, &n, sizeof(n));
	write(sockfd, remoteDirectory, 256);

	if ( chdir(localDirectory) < 0)
	{
		setStatus(JOB_CANCELLED);
		close(sockfd);
		removeJob();
		return;
	}
	D	
	if (download)
	{
		n = htonl(GET_SIZE);
		write(sockfd, &n, sizeof(n));
		write(sockfd, filename, 256);
	D
		read(sockfd, &n, sizeof(n)); //SIZE
		read(sockfd, &n, sizeof(n)); //size
		n = ntohl(n);
	D
		if (n == -1)
		{
			setStatus(JOB_CANCELLED);
			close(sockfd);
			removeJob();
			return;
		}
			
		size = n;
	D
	}	
	else //upload
	{D
		struct stat s;
		if (stat(filename, &s) < 0)
		{
			setStatus(JOB_CANCELLED);
			close(sockfd);
			removeJob();
			return;
		}

	D	size = s.st_size;
		printf("Size in stat=%d\n",size);
	}
	D
	jobCount = size / PACKET_CHUNK_SIZE;
	if (size % PACKET_CHUNK_SIZE != 0) jobCount++;

	int i;
	for (i=0; i<jobCount; i++)
	{
		D	int start = i*PACKET_CHUNK_SIZE;
		int length = PACKET_CHUNK_SIZE;
		if ( (i == jobCount-1) && (size % PACKET_CHUNK_SIZE != 0)) length = size % PACKET_CHUNK_SIZE;
D
		jobs[i] = new Job(remoteDirectory, localDirectory, filename, download, ipAddress, status, start, length);
D	}
	D	
	printf("Thread created wiht jobCount=%d\n",jobCount);	
//	pthread_create(&threadID,NULL,monitorThread,this);
	g_timeout_add(500,monitorThread,this);
}

Job::Job(char remoteDirectory[], char localDirectory[], char filename[], bool download, char ipAddress[], int status, int start, int length)
{
	// Child Job
	parent = false;
D printf("Status @%d = %d\n",__LINE__,status);
	strcpy(this->remoteDirectory, remoteDirectory);
	strcpy(this->localDirectory, localDirectory);
	strcpy(this->filename, filename);
	strcpy(this->ipAddress, ipAddress);
	this->download = download;
	this->slot = -1;
	this->status = status;
	this->start = start;
	this->size = length;
	transferred = 0;
D
	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(ipAddress);
D
	if (connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
	{
		setStatus(JOB_CANCELLED);
		return;
	}
D
	int type = htonl(CHDIR);
	write(sockfd, &type, sizeof(type));
	write(sockfd, remoteDirectory, 256);

//*** chdir localdirectory
D	chdir(localDirectory);
D printf("Status @%d = %d\n",__LINE__,status);
	if (status == JOB_PAUSED)
	{
	D	unsigned short remoteCrc = getCRC(sockfd, filename, start, size);
		unsigned short localCrc = getCRC(filename, start, size);

	D	if (remoteCrc == localCrc) 
		{
	D		setStatus(JOB_COMPLETED);
			transferred = size;
			close(sockfd);
			return;
		}
		else transferred = 0;
	D}

	printf("LAunching child thread...\n");
D	pthread_create(&threadID,NULL,startThread,this);
}

Job::Job(int fd)
{
	jobCount = 0;
	
	//char remoteDirectory[256];
	if (read(fd, remoteDirectory, 256) <= 0)
	{
		delete this;
		return;
	}
	
	//char localDirectory[256];
	read(fd, localDirectory, 256);
	
	//char filename[256];
	read(fd, filename, 256);
	
	//char ipAddress[50];
	read(fd, ipAddress, 50);
	
	//bool download;
	read(fd, &download, sizeof(download));
	
	//int start;
	read(fd, &start, sizeof(start));
	
	//int size;
	read(fd, &size, sizeof(size));
	
	//int transferred;
	read(fd, &transferred, sizeof(transferred));
	
	//int slot;
	read(fd, &slot, sizeof(slot));

	setStatus(JOB_PAUSED);


	addJob();
	transferred = 0; // ***
	
	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(ipAddress);

	if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		setStatus(JOB_CANCELLED);
		removeJob();
		return;
	}

	int n = htonl(CHDIR);
	write(sockfd, &n, sizeof(n));
	write(sockfd, remoteDirectory, 256);

	read(sockfd, &n, sizeof(n));
	read(sockfd, &n, sizeof(n));

	n = ntohl(n);
	if (n != SUCCESS)
	{
		setStatus(JOB_CANCELLED);
		close(sockfd);
		removeJob();
		return;
	}
	
	if ( chdir(localDirectory) < 0)
	{
		setStatus(JOB_CANCELLED);
		close(sockfd);
		removeJob();
		return;
	}
	
	if (download)
	{
		n = htonl(GET_SIZE);
		write(sockfd, &n, sizeof(n));
		write(sockfd, filename, 256);

		read(sockfd, &n, sizeof(n)); //SIZE
		read(sockfd, &n, sizeof(n)); //size
		n = ntohl(n);

		if (n == -1)
		{
			setStatus(JOB_CANCELLED);
			close(sockfd);
			removeJob();
			return;
		}
		
		size = n;
	}
	else //upload
	{
		struct stat s;
		if (stat(filename, &s) < 0)
		{
			setStatus(JOB_CANCELLED);
			close(sockfd);
			removeJob();
			return;
		}

		size = s.st_size;
	}
	
	jobCount = size / PACKET_CHUNK_SIZE;
	if (size % PACKET_CHUNK_SIZE != 0) jobCount++;

	int i;
	for (i=0; i<jobCount; i++)
	{
		int start = i*PACKET_CHUNK_SIZE;
		int length = PACKET_CHUNK_SIZE;
		if ( (i == jobCount-1) && (size % PACKET_CHUNK_SIZE != 0)) length = size % PACKET_CHUNK_SIZE;

		jobs[i] = new Job(remoteDirectory, localDirectory, filename, download, ipAddress, status, start, length);
	}
	
//	pthread_create(&threadID,NULL,monitorThread,this);
	
}

int Job::getStatus()
{
	return status;
}

void Job::setStatus(int status)
{
	if (status == JOB_PAUSED || status == JOB_RESUME)
	{
		if (this->status == JOB_COMPLETED || this->status == JOB_CANCELLED)
			return;
	}
	else if (status == JOB_CANCELLED)
	{
		if (this->status == JOB_COMPLETED)
			return;
	}
	else if (status == JOB_COMPLETED)
	{
		if (this->status == JOB_CANCELLED)
			return;
	}
	this->status = status;
}

void Job::scheduleReslotting()
{
	jobReslottingInProgress = true;
	sleep(1);
}

void Job::endReslotting()
{
	jobReslottingInProgress = false;
}

void Job::setSlot(int slot)
{
	this->slot = slot;
}

void* Job::startThread(void *arg)
{
	Job *job = (Job*) arg;
	
	int start = job->start;
	int length = job->size;
	int status;
printf("Start thread launched with start=%d size=%d\n",start,length);

	if (job->download)
	{
		int type = htonl(GET);
		write(job->sockfd, &type, sizeof(type));
		write(job->sockfd, job->filename, 256);
		start = htonl(start);
		length = htonl(length);
		write(job->sockfd, &start, sizeof(start));
		write(job->sockfd, &length, sizeof(length));
		start = ntohl(start);
		length = ntohl(length);
		
		int fd = open(job->filename,O_WRONLY|O_CREAT,0644);
		if (fd < 0)
		{
			job->setStatus(JOB_CANCELLED);
			close(job->sockfd);
			return NULL;
		}
		
		lseek(fd, start, SEEK_SET);
		while (1)
		{
			if (job->getStatus() == JOB_CANCELLED)
			{
				close(fd);
				close(job->sockfd);
				return NULL;
			}
D
			int n,ack;
			read(job->sockfd, &type, sizeof(type));	//DATA
			read(job->sockfd, &n, sizeof(n));	
			n = ntohl(n);
			printf("Type=%d n=%d\n",ntohl(type),n);
		D
			char data[PACKET_SIZE];
			read(job->sockfd, data, n);
			write(fd, data, n);
//			printf("Thread wrote %d bytes to file at offset %d\n",n,start);
			job->transferred += n;
		D
			if (n == 0) break;
			while (job->getStatus() == JOB_PAUSED) {printf("Paused\n");sleep(1);}	//Pause implementation
			if (job->getStatus() == JOB_RESUME) job->setStatus(JOB_RUNNING);
D
			type = htonl(REPLY);
			write(job->sockfd, &type, sizeof(type));
		}
printf("Thread: transferred=%d\n",job->transferred);
		job->setStatus(JOB_COMPLETED);
		close(fd);
	}
	else // upload
	{
		int type=htonl(PUT);D
		write(job->sockfd, &type, sizeof(type));
		write(job->sockfd, job->filename, 256);
		D
		start = htonl(start);
		write(job->sockfd, &start, sizeof(start));
		start = ntohl(start);
		D
		int fd = open(job->filename,O_RDONLY);
		
		if (fd < 0)
		{
			job->setStatus(JOB_CANCELLED);
			close(job->sockfd);
			return NULL;
		}
		
		lseek(fd, start, SEEK_SET);
		
		int n,ack;
		char data[PACKET_SIZE];
		int bytesRemaining = length;
		int bytesToRead = bytesRemaining;
		if (bytesToRead > PACKET_SIZE) bytesToRead = PACKET_SIZE;
printf("Bytes to read=%d\n",bytesToRead);
		while(n=read(fd,data,bytesToRead))
		{
				
			while (job->getStatus() == JOB_PAUSED) {printf("Paused\n");sleep(1);}	//Pause implementation
			if (job->getStatus() == JOB_RESUME) job->setStatus(JOB_RUNNING);

			if (job->getStatus() == JOB_CANCELLED)
			{
				close(fd);
				close(job->sockfd);
				return NULL;
			}

			printf("IN\n");D
			type=htonl(DATA);
			write(job->sockfd,&type,sizeof(type));
			n=htonl(n);
			write(job->sockfd,&n,sizeof(n));
			n=ntohl(n);
			printf("Client: n=%d\n",n);
			write(job->sockfd,data,n);D
			read(job->sockfd,&type,sizeof(type));//read REPLY
			job->transferred += n;D
			bytesRemaining -= n;
			if (bytesRemaining == 0) break;
			bytesToRead = bytesRemaining;
			if (bytesToRead > PACKET_SIZE) bytesToRead = PACKET_SIZE;
			printf("BTR=%d\n",bytesToRead);D
		}
		D
		type=htonl(DATA);
		write(job->sockfd,&type,sizeof(type));
		
		n = 0;
		n=htonl(n);
		write(job->sockfd,&n,sizeof(n));
		
		job->setStatus(JOB_COMPLETED);
		close(job->sockfd);
	}
	return (NULL);
}

gboolean Job::monitorThread(void *arg)
{
	Job *job = (Job*) arg;
printf("JOB %d\n",job->discarded);
	if (job->discarded) 
	{
		job->discardedAck = true;
		return (false);
	}
	if (jobReslottingInProgress) return true;

	bool done = false;
	bool cancelled = false;
	
	char *statusStr[] = {"Running","Completed","Paused","Resumed","Cancelled"};
	app->progressScreen->progressCList->setContents(job->slot,4,statusStr[job->getStatus()]);

	if (job->getStatus() == JOB_PAUSED)
	{printf("[Paused]\n");
		for (int i=0; i<job->jobCount; i++)
		{
			job->jobs[i]->setStatus(JOB_PAUSED);
		}
		return true;
	}

	if (job->getStatus() == JOB_CANCELLED)
	{printf("[Cancelled %d]\n",job->discarded);
		for (int i=0; i<job->jobCount; i++)
		{
			job->jobs[i]->setStatus(JOB_CANCELLED);
		}
		return true;
	}

	if (job->getStatus() == JOB_RESUME)
	{printf("[Resume]\n");
		for (int i=0; i<job->jobCount; i++)
		{
			job->jobs[i]->setStatus(JOB_RESUME);
		}
		job->setStatus(JOB_RUNNING);
		return true;
	}

	job->transferred = 0;
	done = true;
	cancelled = false;
	for (int i=0; i<job->jobCount; i++)
	{
		job->transferred += job->jobs[i]->transferred;
		if (job->jobs[i]->getStatus() != JOB_COMPLETED) done = false;
		if (job->jobs[i]->getStatus() == JOB_CANCELLED) cancelled = true;
	}
			
	int row = app->progressScreen->progressCList->getSelection(0);

	char sizeStr[100], bytesTransferredStr[100], percentCompletedStr[100];
	sprintf(sizeStr, "%d", job->size);
	sprintf(bytesTransferredStr, "%d", job->transferred);
	sprintf(percentCompletedStr, "%.2f%%", (float)job->transferred/job->size*100);
			
	app->progressScreen->progressCList->setContents(job->slot,0,job->filename);
	app->progressScreen->progressCList->setContents(job->slot,1,sizeStr);
	app->progressScreen->progressCList->setContents(job->slot,2,bytesTransferredStr);
	app->progressScreen->progressCList->setContents(job->slot,3,percentCompletedStr);

	if (cancelled) 
	{
		job->setStatus(JOB_CANCELLED);
	}
	else if (done) 
	{
		job->setStatus(JOB_COMPLETED);
		removeJob();
	}
	else return (true);

	app->progressScreen->progressCList->setContents(job->slot,4,statusStr[job->getStatus()]);

	//app->dataTransferScreen->fetchLocalDirContents();
	//app->dataTransferScreen->fetchRemoteDirContents();
	app->dataTransferScreen->show();
	if (job->discarded) job->discardedAck = true;
	return (false);
}

Job::~Job()
{
	printf("~Job\n");
	setStatus(JOB_CANCELLED);
	discarded = true;
	if (!parent) return; 
	sleep(1);

	for (int i=0; i<jobCount; i++)
	{
		delete jobs[i];
	}
}

void Job::addJob()
{
	while (monitorJobCount >= MAX_JOBS)
	{
		;
	}

	monitorJobCount++;
}

void Job::removeJob()
{
	monitorJobCount--;
}
		
unsigned short Job::getCRC(int sockfd, char filename[], int start, int size)
{
	int crc;
	
	int type = htonl(GET_CRC);
	write(sockfd, &type, sizeof(type));
	write(sockfd, filename, 256);
	start = htonl(start);
	write(sockfd, &start, sizeof(start));
	size = htonl(size);
	write(sockfd, &size, sizeof(size));
	read(sockfd, &type, sizeof(type)); //CRC
	read(sockfd, &crc, sizeof(crc));
	crc = ntohl(crc);
	return (crc);
}

unsigned short Job::getCRC(char filename[], int start, int size)
{
	int fd;
	if ((fd = open(filename,O_RDONLY)) < 0) return (0);
	return (::getCRC(fd,start,size));
}

void Job::pause()
{
	if (slot == -1)
	{
		// Child
		if (getStatus() == JOB_RUNNING) setStatus(JOB_PAUSED);
	}
	else
	{
		// Parent
		for (int i=0; i<jobCount; i++)
		{
			jobs[i]->setStatus(JOB_PAUSED);
		}
		setStatus(JOB_PAUSED);
	}
}

void Job::resume()
{
	if (getStatus() == JOB_PAUSED) setStatus(JOB_RESUME);
}




void Job::saveState(int)
{
	for(int i=0;i<app->dataTransferScreen->jobVector.size();i++)
	{
		printf("Paused: %d\n",i);
		app->dataTransferScreen->jobVector[i]->setStatus(JOB_PAUSED);
	}
	sleep(1);
	for(int i=0;i<app->dataTransferScreen->jobVector.size();i++)
	{
		printf("Saved: %d\n",i);
		app->dataTransferScreen->jobVector[i]->saveState();
	}
	for(int i=0;i<app->dataTransferScreen->jobVector.size();i++)
	{
		printf("Killed: %d\n",i);
		app->dataTransferScreen->jobVector[i]->setStatus(JOB_CANCELLED);
	}
	sleep(1);
	for(int i=0;i<app->dataTransferScreen->jobVector.size();i++)
	{
		printf("Status after killing: %d\n",app->dataTransferScreen->jobVector[i]->getStatus());
	}
}

void Job::saveState()
{
	if(getStatus()==JOB_PAUSED)
	{
	
		int fd=open(saveFile,O_WRONLY|O_CREAT|O_APPEND,0644);

		write(fd,remoteDirectory,256);
		printf("Remote Dir:%s\n",remoteDirectory);
		write(fd,localDirectory,256);
		printf("local Dir:%s\n",localDirectory);
		write(fd,filename,256);
		printf("Filename:%s\n",filename);
		write(fd,&download,sizeof(download));
		write(fd,ipAddress,50);
		printf("IpAddress :%s\n",ipAddress);
		write(fd,&status,sizeof(status));
		write(fd,&size,sizeof(size));
		printf("Size:%d\n",size);

		close(fd);
	}
}
	
void Job::resumeState()
{
	char localDirectory[256],remoteDirectory[256],filename[256],ipAddress[50];
	int size,status;
	bool download;
	int fd=open(saveFile,O_RDONLY);
printf("resume state, fd=%d\n",fd);
	if (fd >= 0)
	{
	
		while (read(fd,remoteDirectory,256))
		{
			read(fd,localDirectory,256);
			read(fd,filename,256);
			read(fd,&download,sizeof(download));
			read(fd,ipAddress,50);
			read(fd,&status,sizeof(status));
			read(fd,&size,sizeof(size));

			char sizeStr[10];
			sprintf(sizeStr,"%d",size);
			char *row[5] = {filename,sizeStr,"0","0%","Unknown"};
			app->progressScreen->progressCList->append(row);
			app->dataTransferScreen->jobVector.push_back(new Job(remoteDirectory,localDirectory,filename,download,ipAddress,JOB_PAUSED,app->dataTransferScreen->jobVector.size()));
			printf("size in resume state=%d\n",size);
		}
	
		close(fd);
		remove(saveFile);
	}
}

Application::Application()
{
    connectScreen = new ConnectScreen();
	dataTransferScreen = new DataTransferScreen();
	progressScreen = new ProgressScreen();
	  
}

Application::~Application()
{
    delete connectScreen;
	delete dataTransferScreen;
	delete progressScreen;
    
}

void Application::create()
{
   connectScreen->create();
   dataTransferScreen->create();
   progressScreen->create();	
}

void Application::show()
{
    connectScreen->show();

}




	




