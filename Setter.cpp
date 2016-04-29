#include "Drawer.h"

using namespace A2OA;

int  itemRef		= 0;

std::vector<DWORD> weapons;

void displayScripts( LPVOID value, bool* enabled )
{
	auto			world = World::Singleton();
	if (!world->getCameraOn()->getUnit()->getBase())
		return;

	auto			locPlayer	= world->getCameraOn()->getUnit()->getPlayer();
	auto			locUnit		= world->getCameraOn()->getUnit();

	if (!locPlayer->getBase())
		return;

	auto wpnID = locUnit->getWeaponID();
	cout << wpnID << endl;
}

void teleport( LPVOID value, bool* enabled )
{

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();

	while( *enabled )
	{
		if( GetAsyncKeyState( 0x57 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x + ( *(int*)value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z + ( *(int*)value*locPlayer->getViewY() );
			plyrPos.y = 0;

			locPlayer->setPos( plyrPos );
		} else if( GetAsyncKeyState( 0x53 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x - ( *(int*)value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z - ( *(int*)value*locPlayer->getViewY() );
			plyrPos.y = 0;

			locPlayer->setPos( plyrPos );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
	}
}

void teleportUnsafe( LPVOID value, bool* enabled )
{
	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();

	while( *enabled )
	{
		if( GetAsyncKeyState( 0x57 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x + ( *(int*)value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z + ( *(int*)value*locPlayer->getViewY() );

			locPlayer->setPos( plyrPos );
		} else if( GetAsyncKeyState( 0x53 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x - ( *(int*)value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z - ( *(int*)value*locPlayer->getViewY() );

			locPlayer->setPos( plyrPos );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
	}
}

void unlockCar( LPVOID value, bool* enabled  )
{
	auto			world			= World::Singleton();
	auto			locPlayer		= world->getCameraOn()->getUnit()->getPlayer();
	auto			transData		= TransData::Singleton( );

	auto			entityTablePtr		= world->getEntityTable();
	auto			entityTable			= entityTablePtr->getTable();
	auto			entityTableSize		= entityTablePtr->getTableSize();

	for( DWORD i = 0; i < entityTableSize; i++ ){

		auto UnitInfo = entityTable->getUnitInfoById( i );
		auto Unit = UnitInfo->getUnit();
		auto UnitBase = Unit->getBase();

		if( UnitBase ){
			if( Unit->getID() == 0 ){

				auto VehicleInfo	= UnitInfo->getVehicle();
				auto Vehicle		= VehicleInfo->getVehicle();
				if( transData->Distance3D( locPlayer->getPos(), Vehicle->getPos() ) < 15 ){
					VehicleInfo->unlock();
					break;
				}
			}
		}

	}
}

void setDamage( LPVOID value, bool* enabled ){
	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();
	int				prevItemValue = 0;

	DWORD wID = locPlayer->getWeaponID(), wPtr = 0;
	DWORD offsets[ 3 ] = { ( wID * 0x24 + 0x4 ), 0x8, 0x200 };

	wPtr = m->readPointer<DWORD>( locPlayer->getBase() + 0x694, offsets, 3 );
	prevItemValue = (int) m->read<float>( wPtr + 0x140 );

	m->write( wPtr + 0x140, *(float*)value );
	*(float*)value = prevItemValue;
}

void setIndirect( LPVOID value, bool* enabled ){
	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	DWORD wID = m->read<DWORD>( locPlayer->getBase() + 0x6E0 ), wPtr = 0;
	DWORD offsets[ 3 ] = { ( wID * 0x24 + 0x4 ), 0x8, 0x200 };

	wPtr = m->readPointer<DWORD>( locPlayer->getBase() + 0x694, offsets, 3 );
	m->write( wPtr + 0x144, *(float*)value );
}

void setRange( LPVOID value, bool* enabled ){
	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn( )->getUnit();

	DWORD wID = m->read<int>( locPlayer->getBase() + 0x6E0 ), wPtr = 0;
	DWORD offsets[ 3 ] = { ( wID * 0x24 + 0x4 ), 0x8, 0x200 };

	wPtr = m->readPointer<DWORD>( locPlayer->getBase() + 0x694, offsets, 3 );
	m->write( wPtr + 0x148, *(float*)value );
}

void resetStats( LPVOID value, bool* enabled ){
	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	DWORD wID = m->read<int>( locPlayer->getBase() + 0x6E0 ), wPtr = 0;
	DWORD offsets[ 3 ] = { ( wID * 0x24 + 0x4 ), 0x8, 0x200 };

	wPtr = m->readPointer<DWORD>( locPlayer->getBase() + 0x694, offsets, 3 );
	m->write( wPtr + 0x148, (float) 0 );
	m->write( wPtr + 0x144, (float) 0 );
}

void spawnWeaponConsole( int id, int amount )
{
	if( id <= 0 || amount <= 0 )
		return;

	auto			WeaponTable				= WeaponTable::Singleton();
	auto			WeaponTableSize			= WeaponTable->getTableSize();

	auto			world					= World::Singleton();
	auto			locPlayer				= world->getCameraOn()->getUnit();

	auto			InventoryTable			= locPlayer->getInventoryTable();
	auto			InventoryTableSize		= InventoryTable->getTableSize();
	auto			InventoryTableMaxSize	= InventoryTable->getTableMaxSize();
	auto			Inventory				= InventoryTable->getInventory();

	auto  Weapon = WeaponTable->getEntryById( id );
	if( Weapon->getBase() ){

		for( int i = 0; i < amount; i++ )
		{
			if( InventoryTableSize < InventoryTableMaxSize ){
				DWORD distance = Inventory->calculateDistance( InventoryTableSize );

				m->write( distance, Weapon->getBase() );
				InventoryTable->setTableSize( InventoryTableSize + 1 );
				InventoryTableSize++;

				weapons.push_back( distance );
			}
		}
	}
}

void spawnConsumableConsole( int id, int amount )
{
	if( id <= 0 || amount <= 0 )
		return;

	auto			ConsumableTable = ConsumableTable::Singleton();
	auto			ConsumableTableSize = ConsumableTable->getTableSize();

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	auto			InventoryTable = locPlayer->getInventoryTable();
	auto			InventoryTableSize = InventoryTable->getConsumableTableSize();
	auto			InventoryTableMaxSize = InventoryTable->getConsumableTableMaxSize();
	auto			Inventory = InventoryTable->getConsumableInventory();

	auto  Consumable = ConsumableTable->getEntryById( id );
	if( Consumable->getBase() ){

		for( int i = 0; i < amount; i++ )
		{
			if( InventoryTableSize < InventoryTableMaxSize ){
				DWORD memory = m->allocateMemory( 0x2C );

				DWORD item1 = m->read<DWORD>( m->read<DWORD>( Inventory->getBase() + 0x28 ) );
				DWORD item2 = m->read<DWORD>( m->read<DWORD>( Inventory->getBase() + 0x4 ) );

				m->write( memory, m->read<DWORD>( Inventory->getBase() ) );
				m->write( memory + 0x4, item2 );
				m->write( memory + 0x8, Consumable->getBase() );

				m->write( memory + 0x28, item1 );
				m->write( memory + 0x2C, itemRef );
				itemRef++;

				/* UNCHECKED */
				int ammoXOR = m->read<DWORD>( Consumable->getBase() + 0x2C ) ^ (int) ( 0xBABAC8B6L );
				int ammoVal = ammoXOR << 1;

				m->write( memory + 0xC, int( ammoXOR - ammoVal ) );
				m->write( memory + 0x24, ammoVal );
				/* END UNCHECKED */

				DWORD distance = Inventory->calculateDistance( InventoryTableSize );

				m->write( distance, memory );
				InventoryTable->setConsumableTableSize( InventoryTableSize + 1 );
				InventoryTableSize++;
			}
		}
	}
}

void listWeapon( string data )
{
	auto			WeaponTable = WeaponTable::Singleton();
	auto			WeaponTableSize = WeaponTable->getTableSize();

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	auto			InventoryTable = locPlayer->getInventoryTable();
	auto			InventoryTableSize = InventoryTable->getTableSize();
	auto			InventoryTableMaxSize = InventoryTable->getTableMaxSize();
	auto			Inventory = InventoryTable->getInventory();


	for( size_t i = 0; i < ( WeaponTableSize - 1 ); i++ )
	{
		auto		Consumable = WeaponTable->getEntryById( i );
		if( Consumable->getBase() )
		{
			string typeName			= Consumable->getType()->getString();
			string typeNameLower	= typeName;

			if( typeName.length() > 0 )
			{
				for( size_t l = 0; l < data.length(); l++ )
				{
					data[ l ] = tolower( data[ l ] );
				}
				for( size_t l = 0; l < typeName.length(); l++ )
				{
					typeNameLower[ l ] = tolower( typeName[ l ] );
				}

				if( data == "all" || typeNameLower.find( data, 0 ) != -1 )
				{
					cout << "ID: " << i << " Type: " << typeName << " Name: " << Consumable->getName()->getString() << endl;
				}
			}
		}
	}

}

void listConsumable( string data )
{
	auto			ConsumableTable				= ConsumableTable::Singleton();
	auto			ConsumableTableSize			= ConsumableTable->getTableSize();

	auto			world						= World::Singleton();
	auto			locPlayer					= world->getCameraOn()->getUnit();

	auto			InventoryTable				= locPlayer->getInventoryTable();
	auto			InventoryTableSize			= InventoryTable->getConsumableTableSize();
	auto			InventoryTableMaxSize		= InventoryTable->getConsumableTableMaxSize();
	auto			Inventory					= InventoryTable->getConsumableInventory();


	for( size_t i = 0; i < ( ConsumableTableSize - 1 ); i++ )
	{
		auto		Consumable = ConsumableTable->getEntryById( i );
		if( Consumable->getBase() )
		{
			string typeName = Consumable->getType()->getString();
			string typeNameLower = typeName;

			if( typeName.length() > 0 )
			{
				for( size_t l = 0; l < data.length(); l++ )
				{
					data[ l ] = tolower( data[ l ] );
				}
				for( size_t l = 0; l < typeName.length( ); l++ )
				{
					typeNameLower[ l ] = tolower( typeName[ l ] );
				}

				if( data == "all" || typeNameLower.find( data, 0 ) != -1 )
				{
					cout << "ID: " << i << " Type: " << typeName << " Name: " << Consumable->getName( )->getString( ) << endl;
				}
			}
		}
	}

}

void findConsumable( string data )
{
	auto			ConsumableTable = ConsumableTable::Singleton();
	auto			ConsumableTableSize = ConsumableTable->getTableSize();

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	auto			InventoryTable = locPlayer->getInventoryTable();
	auto			InventoryTableSize = InventoryTable->getConsumableTableSize();
	auto			InventoryTableMaxSize = InventoryTable->getConsumableTableMaxSize();
	auto			Inventory = InventoryTable->getConsumableInventory();


	for( size_t i = 0; i < ( ConsumableTableSize - 1 ); i++ )
	{
		auto		Consumable = ConsumableTable->getEntryById( i );
		if( Consumable->getBase() )
		{
			string itemName			= Consumable->getName()->getString();
			string itemNameLower	= itemName;

			if( itemName.length() > 0 )
			{
				for( size_t l = 0; l < itemName.length(); l++ )
				{
					itemNameLower[ l ] = (char) tolower( itemName[ l ] );
				}
				
				for( size_t l = 0; l < data.length(); l++ )
				{
					data[ l ] = (char)tolower( data[ l ] );
				}

				if( itemNameLower.find( data, 0 ) != -1 )
				{
					cout << "ID: " << i << " Type: " << Consumable->getType()->getString() << " Name: " << itemName << endl;
				}
			}
		}
	}

}

void findWeapon( string data )
{
	auto			WeaponTable = WeaponTable::Singleton();
	auto			WeaponTableSize = WeaponTable->getTableSize();

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	auto			InventoryTable = locPlayer->getInventoryTable();
	auto			InventoryTableSize = InventoryTable->getTableSize();
	auto			InventoryTableMaxSize = InventoryTable->getTableMaxSize();
	auto			Inventory = InventoryTable->getInventory();


	for( size_t i = 0; i < ( WeaponTableSize - 1 ); i++ )
	{
		auto		Consumable = WeaponTable->getEntryById( i );
		if( Consumable->getBase() )
		{
			string itemName = Consumable->getName()->getString();
			string itemNameLower = itemName;

			if( itemName.length() > 0 )
			{
				for( size_t l = 0; l < itemName.length(); l++ )
				{
					itemNameLower[ l ] = tolower( itemName[ l ] );
				}
				for( size_t l = 0; l < data.length(); l++ )
				{
					data[ l ] = tolower( data[ l ] );
				}

				if( itemNameLower.find( data, 0 ) != -1 )
				{
					cout << "ID: " << i << " Type: " << Consumable->getType( )->getString( ) << " Name: " << itemName << endl;
				}
			}
		}
	}

}
