#ifndef DRAWER_HEADER
#define DRAWER_HEADER

#include "main.h"
#include "Engine.h"

void render( LPVOID value, bool* enabled );
void setRange( LPVOID value, bool* enabled );
void teleport( LPVOID value, bool* enabled );
void setDamage( LPVOID value, bool* enabled );
void unlockCar( LPVOID value, bool* enabled );
void listWeapon( string data );
void resetStats( LPVOID value, bool* enabled );
void findWeapon( string data );
void setIndirect( LPVOID value, bool* enabled );
void displayScripts( LPVOID value, bool* enabled );
void findConsumable( string data );
void listConsumable( string data );
void teleportUnsafe( LPVOID value, bool* enabled );
void spawnWeaponConsole( int id, int amount );
void spawnConsumableConsole( int id, int amount );

#endif