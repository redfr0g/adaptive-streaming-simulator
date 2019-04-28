#pragma once


using namespace std;


class Segment
{
public:
	int quality;
	float czas_trwania;
	float rozmiar;
	int numer;
	float czas_startu;
	
	Segment(int quality, float czas_trwania, float czas_startu, int numer)
	{
		quality = quality;
		czas_trwania = czas_trwania;
		czas_startu = czas_startu;
		numer = numer;
		rozmiar = czas_trwania*quality;
	}

};
