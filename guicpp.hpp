// Version 0.01

#define GTK_VERSION 2

#include <string>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <vector>
#include <stdarg.h>
#include <signal.h>

#define OMIT
namespace guicpp
{
// Global Data Types
enum Event
{
	clicked, 
	pressed, 
	released,
	closeEvent, 		// When the GUI system tells the window to close
	destroyEvent,	// Click on close button
	activate,	// When a menu item is clicked
	change,		// When a text field is edited
	none,
	selectedRow,   //The following enum names need to be changed
	unselectedRow,
	expanded,
	collapsed,
	switched,
	toggled
};	

enum MenuAction
{
};

/*enum MenuItemType
{
	none,
	title,
	item,
	checkItem,
	toggleItem,
	radioItem,
	radioAttach,
	separator,
	branch,
	right
};*/

// Class declarations	
class Widget;
class Window;
class Box; class HBox; class VBox;
class Button;
class Separator; class HSeparator; class VSeparator;
class Label;
class MenuBar; 

class Adjustment; 
class ScrollBar;
class Font;
//modified
class Menu;
class MenuItem;
class Tooltip;
class CTree;
class TreeNode;
class Frame;
class Pane;
class NoteBook;

// Added in Version 0.3
class List;
class Frame;
class FileSelection;
class Table;
class ToolbarButton;
class Toolbar;
class Calendar;
class SpinButton;
class Scale;
class ProgressBar;


typedef void (*CallBack)(Widget*,Event,void*);

// class definitions
class Widget
{
public:
	virtual GtkWidget* getWidget() = 0;
	virtual void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info) = 0;
	virtual void setSize(int width, int height)=0;
	virtual void drawPoint(int x, int y);
	virtual void drawLine(int x1, int y1, int x2, int y2);
	virtual void drawRectangle(int x, int y, int width, int height, bool filled=false);
	virtual void drawArc(int x, int y, int width, int height, bool filled=false, int start=0, int size=64*360);
	virtual void drawText(char *str, int x, int y, Font &font);
//	virtual void setTip(Tooltip &tooltip);//modified
};

class GuiCppApp
{
public:
	static void initialize(int *argc, char ***argv);
	static void run();
	static void terminate();
	virtual void abstractClass() = 0;
};

class Window: public Widget
{
	string *title;
	int width, height;
	int borderWidth;
	GtkWidget *window;

	void (*handlerClose)(Widget*,Event,void*);
	void (*handlerDestroy)(Widget*,Event,void*);

	void *infoClose;
	void *infoDestroy;

	static void onDestroy(GtkButton*,gpointer);
public:
	Window();
	Window(const char *title, int width, int height);
	Window(const char *title);
	virtual ~Window();
	GtkWidget* getWidget();
	void setTitle(const char *title);
	void setSize(int width, int height);
	void setModal(bool modal);
	void setBorderWidth(int n);
	void add(Widget &widget);
	void show();
	void hide();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	
};

class FixedWidget: public Widget
{
	GtkWidget *fixedWidget;
public:
	FixedWidget();
	virtual ~FixedWidget();
	GtkWidget* getWidget();
	void add(Widget &widget, int x, int y);
	void move(Widget &widget, int x, int y);
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
};

class DrawingArea: public Widget
{
	GtkWidget *drawingArea;
	void (*handlerPress)(Widget*,Event,void*);
	void (*handlerRelease)(Widget*,Event,void*);

	void *infoPress;
	void *infoRelease;

	static void onPress(GtkWidget*,GdkEventButton*,gpointer);
	static void onRelease(GtkWidget*,GdkEventButton*,gpointer);

public:
	int mousePressX, mousePressY;
	int mouseReleaseX, mouseReleaseY;

	DrawingArea();
	virtual ~DrawingArea();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
	GtkWidget *getWidget();
};

class Box: public Widget
{
protected:
	GtkWidget *box;
public:
	enum BoxType {horizontal, vertical};
	Box(BoxType type=vertical, bool homogeneous=FALSE, int spacing=0);
	virtual ~Box();
	GtkWidget* getWidget();
	void add(Widget &widget);
	void addStart(Widget &widget, bool expand=true, bool fill=true, int padding=10);
	void addEnd(Widget &widget, bool expand=true, bool fill=true, int padding=10);
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
};

class Label: public Widget
{
	GtkWidget *label;
	string *text;
public:
	Label();
	Label(const char *text);
	virtual ~Label();
	GtkWidget* getWidget(); 
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
};

class Separator: public Widget
{
	GtkWidget *separator;
public:
	enum SeparatorType {horizontal, vertical};
	Separator(SeparatorType type = horizontal);
	virtual ~Separator();
	GtkWidget* getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
};

class Button: public Widget
{
	string *label;
	GtkWidget *button;

	void (*handlerClicked)(Widget*,Event,void*);
	void *infoClicked;

	static void onClick(GtkButton *GTKbutton, gpointer info);
public:
	Button();
	Button(const char *label);
	virtual ~Button();
	GtkWidget* getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
};

class ToggleButton: public Widget
{
	string *label;
	GtkWidget *button;

	void (*handlerClicked)(Widget*,Event,void*);
	void *infoClicked;

	static void onClick(GtkToggleButton *GTKbutton, gpointer info);
public:
	ToggleButton();
	ToggleButton(const char *label);
	virtual ~ToggleButton();
	GtkWidget* getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
	bool getState();
	void setState(bool state);
};

class RadioButton: public Widget
{
	string *label;
	GtkWidget *button;

	void (*handlerClicked)(Widget*,Event,void*);
	void *infoClicked;

	static void onClick(GtkRadioButton *GTKbutton, gpointer info);
public:
	RadioButton();
	RadioButton(const char *label);
	RadioButton(const char *label, RadioButton &attachTo);
	virtual ~RadioButton();
	GtkWidget* getWidget();
	const char *getLabel();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
	bool getState();
	void setState(bool state);
};

class CheckButton: public Widget
{
	string *label;
	GtkWidget *button;

	void (*handlerClicked)(Widget*,Event,void*);
	void *infoClicked;

	static void onClick(GtkCheckButton *GTKbutton, gpointer info);
public:
	CheckButton();
	CheckButton(const char *label);
	virtual ~CheckButton();
	GtkWidget* getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
	bool getState();
	void setState(bool state);
};

class TextField: public Widget
{
	GtkWidget *textField;

	void (*handlerChanged)(Widget*,Event,void*);
	void (*handlerActivated)(Widget*,Event,void*);
	void *infoChanged;
	void *infoActivated;

	bool editable;
	bool visibility;
	static void onChange(GtkEditable *GTKbutton, gpointer info);
	static void onActivate(GtkEditable *GTKbutton, gpointer info);
public:
	TextField();
	TextField(const char *text);
	virtual ~TextField();
	GtkWidget* getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
	void setEditable(bool editable);
	bool isEditable();
	void setVisibility(bool visibility);
	bool isVisible();
 	void setText(const char *text);
	int getText(char *text, int from=0, int to=-1);
};

class TextArea: public Widget
{
	GtkWidget *textArea;
	#if GTK_VERSION == 2
	GtkTextBuffer *textBuffer;
	#endif
public:
	TextArea(Adjustment *hAdjustment=NULL, Adjustment *vAdjustment=NULL);
	virtual ~TextArea();
	GtkWidget *getWidget();
	void setSize(int width, int height);
	void setLineWrap(bool wrap=true);
	void setWordWrap(bool wrap=true);
	void setEditable(bool editable=true);
	void lock();
	void unlock();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setPosition(unsigned int pos);
	unsigned int getPosition();
	void deleteBack(unsigned int n=1);
	void deleteFront(unsigned int n=1);
	unsigned int getLength();
	void insert(char *str, unsigned int pos);
	void insert(char *str);
	void getText(char *str);
};

class Adjustment
{
	GtkObject *adjustment;
public:
	Adjustment(float value, float lower, float upper, float stepInc, float pageInc, float pageSize);
	virtual ~Adjustment();
	virtual GtkObject* getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
};

class ScrollBar: public Widget
{
	GtkWidget *scrollbar;
public:
//	static const int horizontal;
//	static const int vertical;	
	enum Type { horizontal, vertical };
	ScrollBar(Type type, Adjustment *adjustment);
	virtual ~ScrollBar();
	GtkWidget *getWidget();
	void setSize(int width, int height);
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
};

class MenuBar: public Widget
{
	GtkWidget *menuBar;
	//GtkAccelGroup *accelGroup;
	//GtkItemFactoryEntry *entries;
public:
	MenuBar();
	virtual ~MenuBar();
	GtkWidget* getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
	void append(MenuItem &item);
	void prepend(MenuItem &item);
	void insert(MenuItem &item,int pos);
};

enum ScrollPolicy
{
	always, never, automatic
};

class ScrollableWindow: public Widget
{
	GtkWidget *window;
	ScrollPolicy horzPolicy, vertPolicy;	
	string *title;
	int width, height;
	int borderWidth;

	void (*handlerClose)(Widget*,Event,void*);
	void (*handlerDestroy)(Widget*,Event,void*);

	void *infoClose;
	void *infoDestroy;

	static void onDestroy(GtkButton*,gpointer);

public:
	ScrollableWindow(Adjustment *horz, Adjustment *vert);
	ScrollableWindow();
	virtual ~ScrollableWindow();
	virtual GtkWidget *getWidget();
	void addWithViewport(Widget &widget);
	void setScrollPolicy(ScrollPolicy horzPolicy, ScrollPolicy vertPolicy);
	void setHorizontalScrollPolicy(ScrollPolicy policy);
	void setVerticalScrollPolicy(ScrollPolicy policy);
	ScrollPolicy getHorizontalScrollPolicy();
	ScrollPolicy getVerticalScrollPolicy();
	void setPlacementTopLeft();
	void setPlacementTopRight();
	void setPlacementBottomLeft();
	void setPlacementBottomRight();	
	void setSize(int width, int height);
	void setModal(bool modal);
	void setBorderWidth(int n);
	void add(Widget &widget);
	void show();
	void hide();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);

};

class CList: public Widget
{
	GtkWidget *cList;
	 int clickedColumn;
	 int clickedRow;
	 bool ascendingOrder;
	 int lastClickedColumn;       // needs to be set by static onTitleClicked 
	void (*handlerClicked)(Widget*,Event,void*);
	void (*handlerSelectRow)(Widget*,Event,void*);
	void (*handlerUnselectRow)(Widget*,Event,void*);

	void *infoClicked;
	void *infoSelectRow;
	void *infoUnselectRow;

	static void onTitleClicked(GtkCList *GTKClist,int column,void*);
	static void onSelectRow(GtkCList *GTKClist,int row,int column,GdkEventButton *eventbutton,void*);
	static void onUnselectRow(GtkCList *GTKClist,int row,int column,GdkEventButton *eventbutton,void*);
public:
	enum 
	{ 
		SINGLE_SELECTION_MODE,
		BROWSABLE_MODE,
		MULTISELECT_MODE,
		EXTENDED_MODE
	};
	enum 
	{
		LEFT, RIGHT, CENTER, FILL
	};	
	CList(int columns);
	CList(int columns, char *titles[]);
	virtual ~CList();
	GtkWidget* getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int width, int height);
	void setSelectionMode(int mode);
	static void defaultHandlerClicked(Widget*,Event,void*);
	int append(char *text[]);
	int prepend(char *text[]);
	int insert(int row, char *text[]);
	void setContents(int row, int col, char *text);
	void remove(int row);
	void clear();
	int getFocusRow();
	int getSelectionCount();
	int getSelection(int n);
	void selectAll();
	void unselectAll();
	void selectRow(int row, int col);
	void unselectRow(int row, int col);
	void moveTo(int row, int col, float rowAlign, float colAlign);
	void lock();
	void unlock();
	void moveRow(int source, int dest);
	void setColumnTitle(int col, string title);
	void justifyColumn(int col, int justification);
	void showTitle();
	void hideTitle();
	void setColumnWidth(int col, int width);
	int getOptimalColumnWidth(int col);
	void setAutoColumnResize(int col, bool flag);
	void setSortColumn(int col);
	void setSortAscending(bool flag);
	void setComparisonFunction();
	void sort();
	void sort(int col);
	int getSortColumn();
	bool getSortAscending();//modified
	void setAutoSort(bool flag);
	void setHorizontalAdjustment(Adjustment *adjustment);
	void setVerticalAdjustment(Adjustment *adjustment);
	//modified
	int getColumn();
	int getRow();

};

class Font
{
	GdkFont *font;
public:
	Font(const char *fontname, int size);
	virtual ~Font();
	operator void* ();
	GdkFont *getFont();
};

class CTree : public Widget
{
	GtkWidget *cTree;
	int lineStyle;
	int expanderStyle;
	TreeNode *rootNode;
	TreeNode *selectedNode,*unSelectedNode,*expandedNode,*collapsedNode;	

	void (*handlerSelectRow)(Widget*,Event,void*);
	void (*handlerUnselectRow)(Widget*,Event,void*);
	void (*handlerExpanded)(Widget*,Event,void*);
	void (*handlerCollapsed)(Widget*,Event,void*);


	void *infoSelectRow;
	void *infoUnselectRow;
	void *infoExpanded;
	void *infoCollapsed;


	static void onSelectRow(GtkCTree *GTKCTree,GtkCTreeNode *node,int column,void*);
	static void onUnselectRow(GtkCTree *GTKCTree,GtkCTreeNode *node,int column,void* );
	static void onExpand(GtkCTree *GTKCTree,GtkCTreeNode *node,void* );
	static void onCollapse(GtkCTree *GTKCTree,GtkCTreeNode *node,void* );
	
	TreeNode* getTreeNode(TreeNode *rootNode,GtkCTreeNode *node);
public:
	enum
	{
		LEFT,RIGHT,CENTER,FILL
	};
	
	enum
	{
		SINGLE_SELECTION_MODE,
		MULTISELECT_MODE,
		BROWSABLE_MODE,
		EXTENDED_MODE
	};

	enum 
	{
		SOLID,
		DOTTED,
		TABBED,
		NONE
	
	};
	enum
	{
		SQUARE,
		TRIANGLE,
		CIRCULAR
	
	
	};	
	CTree(int columns, int activeColumn=0);
	CTree(int columns, char *titles[],int activeColumn=0);
	virtual ~CTree();
	GtkWidget *getWidget();
	
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int rows,int cols);
	void setColumnTitle(int column, string title);
	void justifyColumn(int column, int justification);
	void showTitle();
	void hideTitle();
	void setSelectionMode(int mode);
	void setLineStyle(int style);
	int getLineStyle();
	void setExpanderStyle(int style);
	int getExpanderStyle();
	void lock();
	void unlock();
	void setColumnWidth(int col,int width);
	void setAutoColumnResize(int col,bool flag);
	
	void setRootNode(TreeNode *node);
	TreeNode* getRootNode();
	
	void insertNode(TreeNode *node, TreeNode *parent, TreeNode *sibling);
	void appendNode(TreeNode *node, TreeNode *parent, TreeNode *sibling);
	void removeNode(TreeNode *node);
	TreeNode* getNode(int n);
	
	int getSelectionCount();
	int getSelection(int n);
	
	void select(int n);
	void select(TreeNode *node);
	void unselect(int n);
	void unselect(TreeNode *node);
	
	void selectRecursive(int n);
	void selectRecursive(TreeNode *node);
    	void unSelectRecursive(int n);
	void unSelectRecursive(TreeNode *node);
	
	void collapse(int n);
	void collapse(TreeNode *node);
	void collapseRecursive(int n);
	void collapseRecursive(TreeNode *node);
	
	void expand(int n);
	void expand(TreeNode *node);
	void expandRecursive(int n);
	void expandRecursive(TreeNode *node);
	
	void expandToDepth(int n, int depth);
	void expandToDepth(TreeNode *node, int depth);
	void collapseToDepth(int n, int depth);
	void collapseToDepth(TreeNode *node, int depth);
	int getOptimalColumnWidth(int col);
};	

class TreeNode
{
	friend class CTree; 
	vector<TreeNode*> children;
	TreeNode *parent;
	vector<string*> rowInfo;
	int columns;
	int spacing;
	bool expanded;
	GtkCTreeNode *node;
	int columnsPerRow;
public:
	TreeNode();
	TreeNode(int columns);
	TreeNode(int columns, char *info[]);
	~TreeNode();

	void setColumns(int columns);
	int getColumns();
	void setText(char *info[]);
	void setSpacing(int spacing);
	int getSpacing();
	bool getExpanded();
	void setExpanded(bool flag);
	void setContents(int column, char *info);
	char* getContents(int column);

	void appendChild(TreeNode *node);
	void prependChild(TreeNode *node);
	void insertChild( TreeNode *node,int rowNumber);
	TreeNode* getChild(int n);
	int getChildCount();
	int getChildPosition(TreeNode *node);	

	void appendSibling(TreeNode *node);
	void prependSibling(TreeNode *node);
	void insertSibling( TreeNode *node,int rownumber);
	TreeNode* getSibling(int n);
	int getSiblingCount();
	int getSiblingPosition(TreeNode *node);

	TreeNode* getParent();
	void setParent(TreeNode *node);
	
	void removeChild(int n);
	void removeChild(TreeNode *node);
	void removeSibling(int n);
	void removeSibling(TreeNode *node);

	void reorderChild(int childNo, int newPosition);
	void reorderChild(TreeNode *node, int newPosition);
	void reorderSibling(int siblingNo, int newPosition);
	void reorderSibling(TreeNode *node, int newPosition);

	
};

class MenuItem: public Widget
{
	GtkWidget *menuItem;
	string *name;
	
	void (*handlerActivate)(Widget*,Event,void *);
	void (*handlerToggled)(Widget*,Event,void *);
	
	void* infoActivate;
	void* infoToggled;
	
	static void onActivate(GtkButton *GTKButton,gpointer info);	
	static void onToggled(GtkCheckMenuItem *GTKCheckMenuItem,gpointer info);	
public:
	MenuItem(const char *name,bool checkMenu=false);
	~MenuItem();
	GtkWidget* getWidget();

	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info=NULL);
	void setSize(int rows,int cols);

	const char* getName();
	void attachMenu(Menu &menu);

	
};


class Menu: public MenuItem
{
	friend class MenuBar;
	GtkWidget *menu;
	
	bool tearOff;
public:
	Menu(const char *name, bool tearoff=false);
	virtual ~Menu();
	GtkWidget* getWidget();

	void append(MenuItem &item);
	void prepend(MenuItem &item);
	void insert(MenuItem &item,int pos);

	void setTearOff(bool flag);
	bool getTearOff();	
	
};
		
class Tooltip
{
	string  *label;
	GtkTooltips *tooltip;
public:
	Tooltip();
	Tooltip(char *str);
	virtual ~Tooltip();
	GtkTooltips* getWidget();
	char* getLabel();
};

class Pane: public Widget
{

	GtkWidget *pane;
	
public:
	enum Orientation
	{	
		VERTICAL,HORIZONTAL
	};

	Pane(Orientation);
	virtual ~Pane();
	GtkWidget* getWidget();
	
	void setHandler(Event event,void (*f)(Widget*,Event,void*),void *info=NULL);
	void setSize(int rows, int cols);
	
	void setGutterSize(int gutterSize);
	int getGutterSize();
	
	void setHandleSize(int handleSize);
	int getHandleSize();
	
	void add(Widget &widget,int paneNumber);
	void add(Widget &widget,int paneNumber,bool resize,bool shrink);

	void setPosition(int pos);

		
};

class NoteBook: public Widget
{
	GtkWidget *noteBook;

	vector<Widget*> childWidget;
	vector<Widget*> labelWidget;
	vector<Widget*> menuWidget;
	
	void (*handlerSwitched)(Widget*, Event,void *);
	
	void * infoSwitched;

	static void onSwitched(GtkNotebook  *GTKNoteBook,GtkNotebookPage *page,int pagenumber,void*);
public:
	
	enum	Position
	{
		BOTTOM,TOP,RIGHT,LEFT
	};

	NoteBook();
	virtual ~NoteBook();

	void showTab( bool flag);
	void setHomogenous( bool flag);
	void setScrollable( bool flag);
	void setTabPosition( Position position);
	void setTabBorder( int borderWidth);
	void setTabBorder( int horizontalBorderWidth,int verticalBorderWidth);
	
	void popupEnable(bool flag); 

	void appendPage(Widget &widget,Widget &tabLabelWidget);
	void appendPage(Widget &widget,char *str);
	void appendPage(Widget &widget,Widget &tabLabelWidget,Widget &menuWidget);

	void prependPage(Widget &widget,Widget &tabLabelWidget);
	void prependPage(Widget &widget,Widget &tabLabelWidget, Widget &menuWidget);

	void insertPage(Widget &widget,Widget &tabLabelWidget, Widget &menuWidget,int position);
	void insertPage(Widget &widget,Widget &tabLabelWidget,int position);
	
	void reorderPage(Widget &childWidget,int newpos);
	void reorderPage(int oldpos,int newpos);

	void removePage(int pageNumber);
	void removePage(Widget &widget);
	
	int getCurrentPage();
	void setCurrentPage(int pageNumber);

	void nextPage();
	void previousPage();
	
	int getPageNumber(Widget &childWidget);
 	
	Widget *getChild(int pageNumber);
	
	Widget *getLabel(int pageNumber);
	Widget *getLabel(Widget &childWidget);

	Widget *getMenuLabel(int pageNumber);
	Widget *getMenuLabel(Widget &childWidget);

	
	
	void setLabel(Widget &childWidget, Widget &labelWidget);
	void setLabel(Widget &childWidget, char *str);
	void setLabel(int position, Widget &labelWidget);
	void setLabel(int position, char *str);

	void setMenuLabel(Widget &childWidget, Widget &labelWidget);
	void setMenuLabel(Widget &childWidget, char *str);
	void setMenuLabel(int position, Widget &labelWidget);
	void setMenuLabel(int position, char *str);
	
	void setHandler(Event event , void (*f)(Widget*,Event,void*),void *info=NULL);
	void setSize(int rows, int cols);
	GtkWidget *getWidget();
};
	
class List
{
	GtkWidget *list;
public:
	List();
	GtkWidget *getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setSize(int width, int height);
	void setSelectionMode(int mode);
	void append(Widget *widget);
	void prepend(Widget *widget);
	void insert(Widget *widget, int row);
	void remove(int row);
	void clear();
	int getSelectionCount();
	int getSelection(int n);
	void selectAll();
	void unselectAll();
};

class Frame : public Widget
{
	GtkWidget *frame;

public:
	enum
	{
		SHADOW_NONE,
		SHADOW_IN,
		SHADOW_OUT,
		SHADOW_ETCHED_IN,
		SHADOW_ETCHED_OUT
	};
		
	Frame();
	Frame(const char *label);
	Frame(const char *label, int align);
	GtkWidget *getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setSize(int width, int height);
	void add(Widget *widget);
	void setLabel(const char *label);
	void setLabelAlignment(int align);
	void setShadowType(int type);
};

/*class FileSelection : public Widget
{
	GtkWidget *fileSelection;
	char *pathnamePtr;
	char *dirPathnamePtr;

	void (*handlerOkClicked)(Widget*,Event,void*);
	void (*handlerCancelClicked)(Widget*,Event,void*);

	void *infoOkClicked;
	void *infoCancelClicked;

	static void onOkButtonClicked(GtkButton *GTKbutton, gpointer info);
	static void onCancelButtonClicked(GtkButton *GTKbutton, gpointer info);

public:
	FileSelection();
	FileSelection(const char *title);
	FileSelection(const char *title, const char *pathname);
	GtkWidget* getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setOkHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setCancelHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setSize(int width, int height);
	void show();
	void hide();
	void showFileOperationButtons();
	void hideFileOperationButtons();
	void setPathname(const char *pathname);
	char* getPathname();
	char* getDirPathname();
};*/

class Table : public Widget
{
	GtkWidget *table;

public:
	enum
	{
		EXPAND = 1 << 0,
		SHRINK = 1 << 1 
	};

	Table();
	Table(int row, int col);
	Table(int row, int col, bool equalSized);
	GtkWidget *getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setSize(int width, int height);

	void add(Widget * widget, int row, int col, int rows = 1, int cols = 1, int xpack = 3, int ypack = 3, int xpadding = 0, int ypadding = 0);
	void setRowSpacing(int row, int spacing);
	void setColSpacing(int col, int spacing);
	void setRowSpacing(int spacing);
	void setColSpacing(int spacing);
};

class Calendar : public Widget
{
	GtkWidget *calendar;

public:
	enum
	{
		SHOW_HEADING 		= 1 << 0,
		SHOW_DAY_NAMES 		= 1 << 1,
		NO_MONTH_CHANGE 	= 1 << 2,
		SHOW_WEEK_NUMBERS 	= 1 << 3,
		WEEK_START_MONDAY 	= 1 << 4
	};

	Calendar();
	GtkWidget *getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setSize(int width, int height);

	void show();
	void hide();

	int selectMonth(int month, int year);
	void selectDay(int day);
	void markDay(int day);
	void unmarkDay(int day);
	void clearMarks();
	void getDate(int *day, int *month, int *year);
	void changeDisplay(int options);
};

class SpinButton : public Widget
{
	GtkWidget *spinButton;
	GtkAdjustment *adjustment;

public:
	enum
	{
		UPDATE_ALWAYS,
		UPDATE_IF_VALID
	};

	SpinButton(float initialValue, float minValue, float maxValue, int precision = 1, float stepIncrement = 1.0, float pageIncrement = 1.0, float pageSize = 1.0);
	GtkWidget *getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setSize(int width, int height);

	void show();
	void hide();

	void setAdjustment(float initialValue, float minValue, float maxValue, float stepIncrement = 1.0, float pageIncrement = 1.0, float pageSize = 1.0);
	void setPrecision(int precision);
	void setInitialValue(float initialValue);
	void setUpdatePolicy(int policy);
	void setWrap(bool state);
	void setNumeric(bool state);
	float getCurrentValueAsFloat();
	int getCurrentValueAsInt();
};

class ProgressBar : public Widget
{
	GtkWidget *progressBar;
	GtkAdjustment *adjustment;

public:
	enum
	{
		CONTINUOUS,
		DESCRETE
	};

	enum
	{
		LEFT_TO_RIGHT,
		RIGHT_TO_LEFT,
		BOTTOM_TO_TOP,
		TOP_TO_BOTTOM
	};

	ProgressBar();
	ProgressBar(float initialValue, float minValue, float maxValue, float stepIncrement = 0.0, float pageIncrement = 0.0, float pageSize = 0.0);
	GtkWidget *getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setSize(int width, int height);

	void setAdjustment(float initialValue, float minValue, float maxValue, float stepIncrement = 0.0, float pageIncrement = 0.0, float pageSize = 0.0);
	void setValue(int initialValue);
	void setActivityMode(bool mode);
	void setActivityStep(int step);
	void setActivityBlocks(int blocks);
	void setProgressPercentage(float percentage);
	float getProgressPercentage();
	void setBarStyle(int style);
	void setOrientation(int orientation);
	void setShowText(bool state);
	void setTextAlignment(float x, float y);
	void setFormatString(const char *format);
	string getCurrentText();
};

class Combo : public Widget
{
	GtkWidget *combo;
	GList *glist;

	bool inputState;
	bool emptyState;

public:
	Combo();
	GtkWidget *getWidget();
	void setHandler(Event event, void (*f)(Widget*,Event,void*), void *info);
	void setSize(int width, int height);
	void show();
	void hide();
	void append(char *text);
	void prepend(char *text);
	void insert(char *text, int row);
	void setRestrictedInput(bool state);
	void setEmptyOk(bool state);
	void enableCursorKeys(bool state);
	void enableCursorKeysAlways(bool state);
	void setCaseSensitive(bool state);
	void setItemString(Widget *widget, const char *text);
	void setComboDisableActivate();
	void getText(char *text);
};

} // End of namespace: guicpp
