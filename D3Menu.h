#pragma once

using namespace std;

// STANDART INCLUDES
#include <string>
#include <vector>
#include <thread>

// MY INCLUDES
#include "renderManager.h"

#define RED(x) ((x>>16)&255)
#define GREEN(x) ((x>>12)&255)
#define BLUE(x) (x&255)

enum VAR_TYPE { T_INT, T_FLOAT, T_STRING, T_BOOL };

typedef struct _MENUITEM {
	string	itemName;
	LPVOID	value;
	LPVOID	stepCallback;
	LPVOID	callback;
	DWORD	color;
	bool	isEnabled;
	bool	threaded;
	SIZE_T	increment;
	INT		itemValue; // A NUMERICAL REPRESENTATION FOR STRING VALUES IF REQUIRED
	VAR_TYPE type; 
} MENUITEM, *PMENUITEM;

typedef struct _MENUENTRY {
	string				entryName;
	vector<PMENUITEM>	items;
	bool				isOpen;
	DWORD				color;
} MENUENTRY, *PMENUENTRY;

typedef VOID(*callback)( LPVOID value, bool* enabled );
typedef VOID(*stepCallback)( LPVOID value, INT step );// -1 for down +1 for UP
typedef VOID(*callbackString)(INT* value, string* name, INT step ); // VALUE IS A NUMBER THAT REPRESENTS STEPS THAT HAVE ALREADY BEEN INCREASED BY

class D3Menu {

public:
	D3Menu();

	bool		Initilize( rManager* rMGR );

	VOID		render();
	VOID		handleInput();

	PMENUITEM	createItem( string itemName, LPVOID value, DWORD color, SIZE_T increment, VAR_TYPE type, LPVOID callback, LPVOID stepCallback, bool threaded );
	PMENUITEM	createItem( string menuName, string itemName, LPVOID value, DWORD color,SIZE_T increment, VAR_TYPE type, LPVOID callback, LPVOID stepCallback, bool threaded );
	PMENUITEM	createItem( PMENUENTRY menu, string itemName, LPVOID value, DWORD color,SIZE_T increment, VAR_TYPE type, LPVOID callback, LPVOID stepCallback, bool threaded );

	PMENUITEM	createToggle(string itemName, LPVOID value, DWORD color, LPVOID callback, bool threaded );
	PMENUITEM	createToggle(string menuName, string itemName, LPVOID value, DWORD color, LPVOID callback, bool threaded );
	PMENUITEM	createToggle(PMENUENTRY menu, string itemName, LPVOID value, DWORD color, LPVOID callback, bool threaded );

	PMENUENTRY	createMenu( string menuName, DWORD color  );

	PMENUITEM	getItem( string itemName );
	PMENUITEM	getItem( string menuName, string itemName );
	PMENUITEM	getItem( PMENUENTRY menu, string itemName );

	bool		isItemEnabled( string itemName );
	bool		isItemEnabled( string menuName, string itemName  );
	bool		isItemEnabled( PMENUENTRY menu, string itemName  );

	LPVOID		getItemValue( string itemName );
	LPVOID		getItemValue( string menuName, string itemName );
	LPVOID		getItemValue( PMENUENTRY menu, string itemName );

	VOID		setItemValue( string itemName, LPVOID value );
	VOID		setItemValue( string menuName, string itemName, LPVOID value );
	VOID		setItemValue( PMENUENTRY menu, string itemName, LPVOID value );

private:

	VOID		changeValue( DWORD vkCode);

	DWORD		fontColor	= D3DCOLOR_ARGB( 255, 0, 0, 180 );
	rManager*	rMGR		= nullptr;

	//IX FONT!
	SIZE_T		step		= 0;
	SIZE_T		menuStep	= 0;
	BOOL		isOpen		= false;

	SIZE_T		fade		= 255;
	BOOL		fadeOut		= false;

	vector<PMENUENTRY>	menus;
	vector<PMENUITEM>	items;

	PMENUENTRY			currentEntry	= nullptr;
};