#pragma once
#include <iostream>
#include<time.h>
#include<math.h>


using namespace std;
class Event
{
public:
	Event(float czas, string typ);
	Event(const Event &z);
	float czas;
	float pckt_size;
	string typ;

};

Event::Event(float czas, string typ)
{
	this->czas = czas;
	this->typ = typ;
}

Event::Event(const Event &z)
{
	czas = z.czas;
	typ = z.typ;
}


	


