#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<fstream>
#include<time.h>
#include<vector>
#include<queue>
#include<random>
#include"Segment.h"
#include"Event.h"



using namespace std;

double normal(const double &mean, const double &std)
{
	static const double pii = 3.1415927;
	static const double r_max = RAND_MAX + 1;
	return std*sqrt(-2 * log((rand() + 1) / r_max))*sin(2 * pii*rand() / r_max) + mean;
}

double wykladniczy(double lambda)
{
	double random_01 = (rand()%10);
	return (-1 / lambda)*log(1-random_01/10);		
}


double losuj_pasmo(int czas)
{
	
	return abs(wykladniczy(0.05)) + czas;
}

double losuj_wartosc_pasma(double mean,double var)
{
	return abs(normal(mean, var));
}


bool sortuj(Event x, Event y)
{
	return x.czas < y.czas;
}


int main()
{
	fstream pasmo_dane;
	fstream bufor_dane;
	fstream jakosc_dane;
	vector<Event> zdarzenia;
	float bufor = 0;
	double pasmo = 2;
	double odchylenie_pasma = 2;
	double pasmo_zapasowe = 3;
	float stosunek;
	float roznica;
	

	pasmo_dane.open("pasmo.txt", ios::in | ios::out | ios::trunc);
	bufor_dane.open("bufor.txt", ios::in | ios::out | ios::trunc);
	jakosc_dane.open("jakosc.txt", ios::in | ios::out | ios::trunc);
	srand(time(NULL));
	float czas_rozpoczecia = 0;
	float czas_chwilowy = 0;
	float czas_nowy = 0;
	float czas_poprzedni = 0;
	float czas_testowy = 0;
	float quality[8] = { 0.35, 0.47, 0.63, 0.845, 1.13, 1.52, 2.04, 2.75 };
	float packet_size = quality[7];
	float playback_time = 1;

	Event zmiana_pasma(losuj_pasmo(czas_chwilowy), "zmiana pasma");
	zdarzenia.push_back(zmiana_pasma);
	
	Event bufor_push(czas_chwilowy + (packet_size / pasmo), "bufor push");
	zdarzenia.push_back(bufor_push);

	Event bufor_pull(czas_chwilowy + playback_time, "bufor pull");
	zdarzenia.push_back(bufor_pull);

	Event smooth_streaming(czas_chwilowy, "smooth streaming");
	zdarzenia.push_back(smooth_streaming);
	
	bufor_dane << czas_chwilowy << "	" << bufor  << "\n";
	pasmo_dane << czas_chwilowy << "	" << pasmo << "\n";
	jakosc_dane << czas_chwilowy << "	" << packet_size << "\n";


	while (czas_chwilowy < 500)
	{
		sort(zdarzenia.begin(), zdarzenia.end(), sortuj);
		Event event = zdarzenia.front();
		czas_chwilowy = event.czas;

		if (czas_chwilowy - czas_poprzedni < 0)
		{
			//cout << "error" << "\n";
			czas_chwilowy = czas_poprzedni;
		}

		czas_poprzedni = czas_chwilowy;

		if (event.typ == "zmiana pasma")
		{
			for (int i = 0; i < zdarzenia.size(); i++)
			{
				if (zdarzenia[i].typ == "bufor push")
				{
					Event zmiana_pobierania(czas_chwilowy, "zmiana pobierania");
					zdarzenia.push_back(zmiana_pobierania);
				}
			}
			pasmo = losuj_wartosc_pasma(pasmo,odchylenie_pasma);
			Event zmiana_pasma(losuj_pasmo(czas_chwilowy), "zmiana pasma");
			Event smooth_streaming(czas_chwilowy, "smooth streaming");
			zdarzenia.push_back(zmiana_pasma);
			zdarzenia.push_back(smooth_streaming);
		}


		if (event.typ == "bufor push")
		{
			Event bufor_push(czas_chwilowy + (packet_size / pasmo), "bufor push");
			
			bufor_push.pckt_size = packet_size;
			zdarzenia.push_back(bufor_push);
			
			if(bufor < 30)		bufor++;
				
		}

		if (event.typ == "zmiana pobierania")
		{
			
			for (int i = 0; i < zdarzenia.size(); i++)
			{
				if (zdarzenia[i].typ == "bufor push")
				{
					
					stosunek = czas_chwilowy / zdarzenia[i].czas;
					zdarzenia[i].czas = czas_chwilowy + (stosunek*zdarzenia[i].pckt_size) / pasmo;		
				}
			}
		}

		if (event.typ == "smooth streaming")
		{
			packet_size = quality[7];

			for (int i = 0; i < 8; i++)
			{
				
				if (playback_time <= packet_size/pasmo)
				{
					packet_size = quality[7 - i];
				}
			
				
			}
			
		}

		bufor_dane << czas_chwilowy << "	" << bufor << "\n";
		pasmo_dane << czas_chwilowy << "	" << pasmo << "\n";
		jakosc_dane << czas_chwilowy << "	" << packet_size << "\n";
		//cout << czas_chwilowy << "	" << packet_size << "\n";

		if (event.typ == "bufor pull")
		{
			Event bufor_pull(czas_chwilowy + playback_time, "bufor pull");
			zdarzenia.push_back(bufor_pull);
			if (bufor > 0) bufor = bufor - 1;
			
		}

		
		
		zdarzenia.erase(zdarzenia.begin());
		
	}

	bufor_dane.close();
	pasmo_dane.close();
	jakosc_dane.close();

	system("gnuplot -p -e \"plot 'bufor.txt' with lines ls 9 title 'Bufor[s]','pasmo.txt' with lines ls 7 title 'Pasmo[Mbps]','jakosc.txt' with lines ls 3 title 'Quality[Mbit]'\"");
	
	return 0;
}