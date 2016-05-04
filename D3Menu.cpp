#include "D3Menu.h"

D3Menu::D3Menu()
{
	// EMPTY
}

bool D3Menu::Initilize(rManager * rMGR, DWORD hotKey )
{
	if (rMGR)
	{
		this->rMGR = rMGR;
		return true;
	}

	return false;
}

VOID D3Menu::render()
{
	processItems();
	// MENU BACKGROUND COLOR r=40; g=40; b=39;
	if( !isOpen )
		return VOID();

	SIZE_T posY			= 0;
	SIZE_T posX			= 3;
	SIZE_T loopStep		= 0;

	SIZE_T backgroundBox = 0;
	if (currentEntry)
	{
		backgroundBox = currentEntry->items.size() * 16;
	} else {
		backgroundBox = ( items.size() + menus.size() ) * 16;
	}
	rMGR->FillRGB( posX - 3, posY, 200, backgroundBox, D3DCOLOR_ARGB(255, 40, 40, 39 ) );

	if (currentEntry) // MENU IS OPEN, SO DRAW ONLY IT'S CONTAINERS
	{
		
		for (PMENUITEM item : currentEntry->items) // DRAW ITEMS
		{
			DWORD color = item->color;

			if (item->isEnabled && loopStep == menuStep) {
				color = D3DCOLOR_ARGB(fade, 122, 219, 31);
			} else if (item->isEnabled) {
				color = D3DCOLOR_ARGB(255, 122, 219, 31);
			} else if (loopStep == menuStep) {
				color = D3DCOLOR_ARGB(fade, RED(item->color), GREEN(item->color), BLUE(item->color));
			}
				

			string text = item->itemName;

			if (item->value != nullptr) {
				switch (item->type)
				{
					case VAR_TYPE::_INT:
					{
						text += "<" + to_string(*(INT*)item->value) + ">";
						break;
					}
					case VAR_TYPE::_FLOAT:
					{
						text += "<" + to_string(*(FLOAT*)item->value) + ">";
						break;
					}
					case VAR_TYPE::_STRING:
					{
						text += "<" + *(string*)item->value + ">";
						break;
					}
					case VAR_TYPE::_BOOL:
					{
						BOOL value = *(BOOL*)item->value;
						string final;

						if (value)
							final = "ENABLED";
						else
							final = "DISABLED";

						text += "<" + final + ">";
						break;
					}
					default:
					{
						continue;
					}
				}
			} else {
				text += "<>";
			}
			

			rMGR->drawText( text.c_str(), posX, posY, color );
			
			posY += 14;
			loopStep++;
		}

	} else { // NO MENU IS OPEN, DRAW EVERYTHING

		for (PMENUENTRY entry : menus) // DRAW MENUS
		{
			DWORD color = entry->color;
			if (loopStep == step) {
				color = D3DCOLOR_ARGB(fade, RED(color), GREEN(color), BLUE(color));
			}

			rMGR->drawText( entry->entryName.c_str(), posX, posY, color );
			posY += 14;

			loopStep++;
		}

		for (PMENUITEM item : items) // DRAW ITEMS
		{
			DWORD color = item->color;

			if (item->isEnabled && loopStep == step) {
				color = D3DCOLOR_ARGB(fade, 122, 219, 31);
			} else if (item->isEnabled) {
				color = D3DCOLOR_ARGB(255, 122, 219, 31);
			}else if (loopStep == step) {
				color = D3DCOLOR_ARGB(fade, RED(color), GREEN(color), BLUE(color) );
			}

			string text = item->itemName;

			if (item->value != nullptr) {
				switch (item->type)
				{
					case VAR_TYPE::_INT:
					{
						text += "<" + to_string(*(INT*)item->value) + ">";
						break;
					}
					case VAR_TYPE::_FLOAT:
					{
						text += "<" + to_string(*(FLOAT*)item->value) + ">";
						break;
					}
					case VAR_TYPE::_STRING:
					{
						text += "<" + *(string*)item->value + ">";
						break;
					}
					case VAR_TYPE::_BOOL:
					{
						BOOL value = *(BOOL*)item->value;
						string final;

						if( value )
							final = "ENABLED";
						else
							final = "DISABLED";

						text += "<" + final + ">";
						break;
					}
					default:
					{
						continue;
					}
				}
			} else {
				text += "<>";
			}


			rMGR->drawText(text.c_str(), posX, posY, color);

			posY += 14;
			loopStep++;
		}
	}


	/* FADE */
	if ((fade + 5) > 255)
	{
		fadeOut = TRUE;
	} else if ( (fade - 5) <= 0 ) {
		fadeOut = FALSE;
	}

	if (fadeOut)
	{
		fade -= 5;
	} else {
		fade += 5;
	}

	return VOID();
}

VOID D3Menu::handleInput()
{
	if (GetAsyncKeyState(VK_OEM_3) & 1)
	{
		isOpen = !isOpen;
	}

	if( !isOpen )
		return VOID();

	if (GetAsyncKeyState(VK_DOWN) & 1) // STEP DOWN
	{
		if (currentEntry != nullptr)
		{
			if (menuStep == (currentEntry->items.size() - 1)) {
				menuStep = 0;
			} else {
				menuStep++;
			}

			return;
		}

		if( step == ( menus.size() + items.size() - 1) )
			step = 0;
		else { step++; }

	} else if (GetAsyncKeyState(VK_UP) & 1) // STEP UP
	{
		if (currentEntry != nullptr)
		{
			if (menuStep == 0) 
			{
				menuStep = (currentEntry->items.size()-1);
			} else {
				menuStep--;
			}

			return;
		}

		if( step == 0 )
			step = ( menus.size() + items.size() - 1);
		else { step--; }
	}

	if (GetAsyncKeyState(VK_RIGHT) & 1)
	{
		changeValue( VK_RIGHT );
	} else if (GetAsyncKeyState(VK_LEFT) & 1)
	{
		changeValue( VK_LEFT );
	}

	if (GetAsyncKeyState(VK_BACK) & 1)
	{
		if (currentEntry != nullptr)
		{
			PMENUENTRY entry = currentEntry;

			entry->isOpen = false;
			currentEntry = nullptr;

			menuStep = 0;
		}
	}

	if (GetAsyncKeyState(VK_RETURN) & 1)
	{
		if (currentEntry != nullptr)
		{
			PMENUITEM item = items[menuStep];

			if (item->type == VAR_TYPE::_BOOL && item->value != nullptr)
			{
				bool* value = (bool*)item->value;
				*value = !*value;
			}
			item->isEnabled = true;
		}

		if (step < menus.size())
		{
			PMENUENTRY entry = menus[step];

			entry->isOpen = true;
			currentEntry = entry;
		} else {
			PMENUITEM item = items[step - menus.size()];

			if (item->type == VAR_TYPE::_BOOL && item->value != nullptr)
			{
				bool* value = (bool*)item->value;
				*value = !*value;
			}
			item->isEnabled = true;
		}
	}

	return VOID();
}

PMENUITEM D3Menu::createItem( string itemName, LPVOID value, DWORD color, SIZE_T increment, VAR_TYPE type, LPVOID callback, LPVOID stepCallback, bool threaded )
{
	PMENUITEM item		= new MENUITEM;

	item->itemName	= itemName;
	item->value		= value;

	if (color == NULL) {
			item->color	= D3DCOLOR_ARGB(255,242,37,5);
	} else { item->color = color; }

	item->increment	= increment;
	item->type		= type;
	item->callback	= callback;
	item->stepCallback = stepCallback;
	item->isEnabled	= FALSE;
	item->threaded	= threaded;
	item->itemValue	= 0;

	items.push_back( item );
	return item;
}

PMENUITEM D3Menu::createItem(string menuName, string itemName, LPVOID value, DWORD color,SIZE_T increment, VAR_TYPE type, LPVOID callback, LPVOID stepCallback, bool threaded)
{
	PMENUITEM item		= new MENUITEM;

	item->itemName	= itemName;
	item->value		= value;

	if (color == NULL) {
		item->color = D3DCOLOR_ARGB(255, 242, 37, 5);
	} else { item->color = color; }

	item->increment	= increment;
	item->type		= type;
	item->callback	= callback;
	item->stepCallback = stepCallback;
	item->isEnabled	= FALSE;
	item->threaded	= threaded;
	item->itemValue	= 0;

	for (PMENUENTRY entry : menus)
	{
		if (!menuName.compare(entry->entryName))
		{
			entry->items.push_back( item );
			return item;
		}
	}

	return nullptr;
}

PMENUITEM D3Menu::createItem(PMENUENTRY menu, string itemName, LPVOID value, DWORD color,SIZE_T increment, VAR_TYPE type, LPVOID callback, LPVOID stepCallback, bool threaded )
{
	PMENUITEM item		= new MENUITEM;

	item->itemName	= itemName;
	item->value		= value;

	if (color == NULL) {
		item->color = D3DCOLOR_ARGB(255, 242, 37, 5);
	} else { item->color = color; }

	item->increment	= increment;
	item->type		= type;
	item->callback	= callback;
	item->stepCallback = stepCallback;
	item->isEnabled	= FALSE;
	item->threaded	= threaded;
	item->itemValue	= 0;

	if (menu == nullptr)
		return nullptr;

	menu->items.push_back( item );
	return item;
}

PMENUITEM D3Menu::createToggle(string itemName, LPVOID value, DWORD color, LPVOID callback, bool threaded)
{
	PMENUITEM toggle = new MENUITEM;

	toggle->itemName		= itemName;
	toggle->value			= value;

	if (color == NULL) {
		toggle->color = D3DCOLOR_ARGB(255,0,153,204);
	} else { toggle->color = color; }

	toggle->callback		= callback;

	if (value != nullptr) {
		toggle->isEnabled = *(bool*)value;
	} else {
		toggle->isEnabled = FALSE;
	}

	toggle->threaded		= threaded;
	toggle->type			= VAR_TYPE::_BOOL;

	items.push_back( toggle );
	return toggle;
}

PMENUITEM D3Menu::createToggle(string menuName, string itemName, LPVOID value, DWORD color, LPVOID callback, bool threaded)
{
	PMENUITEM item = new MENUITEM;

	item->itemName = itemName;
	item->value = value;

	if (color == NULL) {
		item->color = D3DCOLOR_ARGB(255, 0, 153, 204);
	} else { item->color = color; }

	item->type = VAR_TYPE::_BOOL;
	item->callback	= callback;

	if (value != nullptr) {
		item->isEnabled = *(bool*)value;
	} else {
		item->isEnabled = FALSE;
	}

	item->threaded	= threaded;

	for (PMENUENTRY entry : menus)
	{
		if (!menuName.compare(entry->entryName))
		{
			entry->items.push_back(item);
			return item;
		}
	}

	return nullptr;
}

PMENUITEM D3Menu::createToggle(PMENUENTRY menu, string itemName, LPVOID value, DWORD color, LPVOID callback, bool threaded)
{
	PMENUITEM item = new MENUITEM;

	item->itemName = itemName;
	item->value = value;

	if (color == NULL) {
		item->color = D3DCOLOR_ARGB(255, 0, 153, 204);
	} else { item->color = color; }

	item->type = VAR_TYPE::_BOOL;
	item->callback = callback;

	if (value != nullptr) {
		item->isEnabled = *(bool*)value;
	} else {
		item->isEnabled = FALSE;
	}

	item->threaded = threaded;

	if (menu == nullptr)
		return nullptr;

	menu->items.push_back(item);
	return item;
}

PMENUENTRY D3Menu::createMenu(string menuName, DWORD color )
{
	PMENUENTRY entry	= new MENUENTRY;

	entry->entryName		= menuName;

	if (color == NULL) {
		entry->color = D3DCOLOR_ARGB(255,152,222,42);
	} else { entry->color = color; }

	entry->isOpen		= false;

	menus.push_back( entry );
	return entry;
}

PMENUITEM D3Menu::getItem(string itemName)
{
	for (PMENUITEM item : items)
	{
		if (!itemName.compare(item->itemName))
		{
			return item;
		}
	}

	// IF ITEM WASN'T FOUND IN STANDART ITEMS, LOOK FOR IT IN ENTRIES
	for (PMENUENTRY entry : menus)
	{
		for (PMENUITEM item : entry->items)
		{
			if (!itemName.compare(item->itemName))
			{
				return item;
			}
		}
	}

	return nullptr;
}

PMENUITEM D3Menu::getItem(string menuName, string itemName)
{
	for (PMENUENTRY entry : menus)
	{
		if (!menuName.compare(entry->entryName))
		{
			for (PMENUITEM item : entry->items)
			{
				if (!itemName.compare(item->itemName))
				{
					return item;
				}
			}
		}
	}

	return nullptr;
}

PMENUITEM D3Menu::getItem(PMENUENTRY menu, string itemName)
{
	if (menu == nullptr)
		return nullptr;

	for (PMENUITEM item : menu->items)
	{
		if (!itemName.compare(item->itemName))
		{
			return item;
		}
	}

	return nullptr;
}

bool D3Menu::isItemEnabled(string itemName)
{
	for (PMENUITEM item : items)
	{
		if (!itemName.compare(item->itemName))
		{
			return item->isEnabled;
		}
	}

	// IF ITEM WASN'T FOUND IN STANDART ITEMS, LOOK FOR IT IN ENTRIES
	for (PMENUENTRY entry : menus)
	{
		for (PMENUITEM item : entry->items)
		{
			if (!itemName.compare(item->itemName))
			{
				return item->isEnabled;
			}
		}
	}

	return false;
}

bool D3Menu::isItemEnabled(string menuName, string itemName)
{
	for (PMENUENTRY entry : menus)
	{
		if (!menuName.compare(entry->entryName))
		{
			for (PMENUITEM item : entry->items)
			{
				if (!itemName.compare(item->itemName))
				{
					return item->isEnabled;
				}
			}
		}
	}

	return false;
}

bool D3Menu::isItemEnabled(PMENUENTRY menu, string itemName)
{
	if (menu == nullptr)
		return false;

	for (PMENUITEM item : menu->items)
	{
		if (!itemName.compare(item->itemName))
		{
			return item->isEnabled;
		}
	}

	return false;
}

LPVOID D3Menu::getItemValue(string itemName)
{
	for (PMENUITEM item : items)
	{
		if (!itemName.compare(item->itemName))
		{
			return item->value;
		}
	}

	// IF ITEM WASN'T FOUND IN STANDART ITEMS, LOOK FOR IT IN ENTRIES
	for (PMENUENTRY entry : menus)
	{
		for (PMENUITEM item : entry->items)
		{
			if (!itemName.compare(item->itemName))
			{
				return item->value;
			}
		}
	}

	return nullptr;
}

LPVOID D3Menu::getItemValue(string menuName, string itemName)
{
	for (PMENUENTRY entry : menus)
	{
		if (!menuName.compare(entry->entryName))
		{
			for (PMENUITEM item : entry->items)
			{
				if (!itemName.compare(item->itemName))
				{
					return item->value;
				}
			}
		}
	}

	return nullptr;
}

LPVOID D3Menu::getItemValue(PMENUENTRY menu, string itemName)
{
	if (menu == nullptr)
		return nullptr;

	for (PMENUITEM item : menu->items)
	{
		if (!itemName.compare(item->itemName))
		{
			return item->value;
		}
	}

	return nullptr;
}

VOID D3Menu::setItemValue(string itemName, LPVOID value)
{
	for (PMENUITEM item : items)
	{
		if (!itemName.compare(item->itemName))
		{
			item->value = value;
		}
	}

	// IF ITEM WASN'T FOUND IN STANDART ITEMS, LOOK FOR IT IN ENTRIES
	for (PMENUENTRY entry : menus)
	{
		for (PMENUITEM item : entry->items)
		{
			if (!itemName.compare(item->itemName))
			{
				item->value = value;
			}
		}
	}

}

VOID D3Menu::setItemValue(string menuName, string itemName, LPVOID value)
{
	for (PMENUENTRY entry : menus)
	{
		if (!menuName.compare(entry->entryName))
		{
			for (PMENUITEM item : entry->items)
			{
				if (!itemName.compare(item->itemName))
				{
					item->value = value;
				}
			}
		}
	}
}

VOID D3Menu::setItemValue(PMENUENTRY menu, string itemName, LPVOID value)
{
	if( menu == nullptr )
		return VOID();

	for (PMENUITEM item : menu->items)
	{
		if (!itemName.compare(item->itemName))
		{
			item->value = value;
		}
	}
}

VOID D3Menu::changeValue(DWORD vkCode)
{
	if (vkCode == VK_LEFT)
	{
		PMENUITEM item = nullptr;

		if (currentEntry != nullptr)
		{
			try {
				item = currentEntry->items.at(menuStep);
			} catch (std::exception e) {
				UNREFERENCED_PARAMETER( e );
				return;
			}
		} else {
			if (step > menus.size() - 1) {
				try {
					item = items.at(step - menus.size());
				} catch (std::exception e) {
					UNREFERENCED_PARAMETER(e);
					return;
				}
			} else {
				return; // MENU SELECTED
			}
		}

		if (item->value != nullptr)
		{
			switch (item->type)
			{
				case VAR_TYPE::_INT:
				{
					if (item->stepCallback != nullptr)
					{
						stepCallback func = (stepCallback)item->stepCallback;
						func(item->value, -1);
						break;
					}

					*(INT*)item->value -= item->increment;
					break;
				}
				case VAR_TYPE::_FLOAT:
				{
					if (item->stepCallback != nullptr)
					{
						stepCallback func = (stepCallback)item->stepCallback;
						func(item->value, -1);
						break;
					}

					*(FLOAT*)item->value -= item->increment;
					break;
				}
				case VAR_TYPE::_STRING:
				{
					if (item->stepCallback != nullptr)
					{
						callbackString func = (callbackString)item->stepCallback;
						func(&item->itemValue, (string*)item->value, -1);
					}
					break;
				}
				default:
				{

				}
			}
		}
	} else {
		if (step < menus.size())
		{
			PMENUENTRY entry = menus[step];

			entry->isOpen = true;
			currentEntry = entry;

		} else {
			PMENUITEM item = nullptr;

			if (currentEntry != nullptr)
			{
				try {
					item = currentEntry->items.at(menuStep);
				} catch (std::exception e) {
					UNREFERENCED_PARAMETER(e);
					return;
				}
			} else { 
				try {
					item = items.at( step - menus.size() ); 
				} catch (std::exception e) {
					UNREFERENCED_PARAMETER(e);
					return;
				}
			}

			if (item->value != nullptr)
			{
				switch (item->type)
				{
					case VAR_TYPE::_INT:
					{
						if (item->stepCallback != nullptr)
						{
							stepCallback func = (stepCallback)item->stepCallback;
							func(item->value, 1);
							break;
						}

						*(INT*)item->value += item->increment;
						break;
					}
					case VAR_TYPE::_FLOAT:
					{
						if (item->stepCallback != nullptr)
						{
							stepCallback func = (stepCallback)item->stepCallback;
							func(item->value, 1);
							break;
						}

						*(FLOAT*)item->value += item->increment;
						break;
					}
					case VAR_TYPE::_STRING:
					{
						if (item->stepCallback != nullptr)
						{
							callbackString func = (callbackString)item->stepCallback;
							func(&item->itemValue, (string*)item->value, 1);
						}
						break;
					}
					default:
					{

					}
				}
			}
		}
	}
}

VOID D3Menu::processItems()
{
	for(PMENUITEM item : items)
	{
		if ( item->isEnabled && item->callback != nullptr )
		{
			callback func = (callback)item->callback;
			if ( item->threaded  )
			{
				if (item->future.valid()) {
					auto status = item->future.wait_for(std::chrono::milliseconds(0));
					if (status != std::future_status::ready)
						continue;
				}

				item->future = std::async( std::launch::async, func, item->value, &item->isEnabled );
				continue;
			}

			func(item->value, &item->isEnabled);
		}
	}

	for (PMENUENTRY entry : menus)
	{
		for (PMENUITEM item : entry->items)
		{
			if (item->isEnabled && item->callback != nullptr)
			{
				callback func = (callback)item->callback;
				if (item->threaded)
				{
					if (item->future.valid()) {
						auto status = item->future.wait_for(std::chrono::milliseconds(0));
						if (status != std::future_status::ready)
							continue;
					}

					item->future = std::async(std::launch::async, func, item->value, &item->isEnabled);
					continue;
				}

				func(item->value, &item->isEnabled);
			}
		}
	}

	return VOID();
}
