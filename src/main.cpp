#include "atmosphere.hpp"
#include "elastic_atmosphere.hpp"
#include "chemical_types.hpp"
#include "device.hpp"
#include <cmath>
#include <cstdio>
#include <raylib.h>
#include <string>
#include "atmospherics_reactions.hpp"



void draw_atmosphere(Atmosphere const &atmosphere, double maxTemp, double maxPressure, int x, int y)
{
	double temp = atmosphere.get_temperature();
	double pressure = atmosphere.get_pressure();
	Color color = {
		static_cast<unsigned char>(255 * temp / maxTemp),
		0,
		static_cast<unsigned char>(255 * pressure / maxPressure),
		255
	};
	color = GREEN;
	double size = 10 * std::cbrt(atmosphere.volume); // dm
	DrawRectangle(x, y, size, size, color);
	DrawText(TextFormat("%lfkPa\n\n%lfK", pressure, temp), x + 4, y + 4, 12, BLACK);
	int ty = y + 32 + 100;
	for (auto const &entry : atmosphere.contents) {
		std::string chemName = "UNKNOWN";
		switch (entry.first) {
			case Hydrogen:
				chemName = "H2";
				break;
			case Nitrogen:
				chemName = "N2";
				break;
			case Oxygen:
				chemName = "O2";
				break;
			case Water:
				chemName = "H2O";
				break;
			case Carbon:
				chemName = "C";
				break;
			case CarbonMonoxide:
				chemName = "CO";
				break;
			case CarbonDioxide:
				chemName = "CO2";
				break;
			case TNT:
				chemName = "TNT";
				break;
		}
		double chemPerc = atmosphere.get_percent_pressure(entry.first) * 100;
		DrawText(TextFormat("%s: %lf%%", chemName.c_str(), chemPerc), x, ty, 32, BLACK);
		ty += 32;
	}
}

#define CELSIUS(x) (x + 273.15)
int main()
{
	AtmosphericsReaction hydrogenCombustion(CELSIUS(550), 241920);
	hydrogenCombustion.add_reactant(Hydrogen, 2); // 2H2 + O2 = 2H2O
	hydrogenCombustion.add_reactant(Oxygen, 1);
	hydrogenCombustion.add_product(Water, 2);
	hydrogenCombustion.reactionSpeed = 0.1;
	atmosphericsReactions.push_back(hydrogenCombustion);

	AtmosphericsReaction boudouard(300, -172500, false);
	boudouard.add_reactant(CarbonMonoxide, 2);
	boudouard.add_product(CarbonDioxide, 1);
	boudouard.add_product(Carbon, 1);
	boudouard.reactionSpeed = 0.1;
	atmosphericsReactions.push_back(boudouard);

	AtmosphericsReaction boudouardReverse(300, 172500, false);
	boudouardReverse.add_product(CarbonMonoxide, 2);
	boudouardReverse.add_reactant(CarbonDioxide, 1);
	boudouardReverse.add_reactant(Carbon, 1);
	boudouardReverse.reactionSpeed = 0.1;
	atmosphericsReactions.push_back(boudouardReverse);

	AtmosphericsReaction hydrogenCarbonMonoxideDisplacement(CELSIUS(550), 131300, false);
	hydrogenCarbonMonoxideDisplacement.add_reactant(Hydrogen, 1);
	hydrogenCarbonMonoxideDisplacement.add_reactant(CarbonMonoxide, 1);
	hydrogenCarbonMonoxideDisplacement.add_product(Water, 1);
	hydrogenCarbonMonoxideDisplacement.add_product(Carbon, 1);
	atmosphericsReactions.push_back(hydrogenCarbonMonoxideDisplacement);

	AtmosphericsReaction tntDetonation(CELSIUS(1000), -1475000);
	tntDetonation.add_reactant(TNT, 2);
	tntDetonation.add_product(Nitrogen, 3);
	tntDetonation.add_product(Hydrogen, 5);
	tntDetonation.add_product(CarbonMonoxide, 12);
	tntDetonation.add_product(Carbon, 2);
	atmosphericsReactions.push_back(tntDetonation);

	AtmosphericsReaction carbonCombustion(CELSIUS(700), 501316);
	carbonCombustion.add_reactant(Carbon, 1);
	carbonCombustion.add_reactant(Oxygen, 1);
	carbonCombustion.add_product(CarbonDioxide, 1);
	atmosphericsReactions.push_back(carbonCombustion);

	AtmosphericsReaction carbonMonoxideResolution(CELSIUS(500), -749000 * 2, false);
	carbonMonoxideResolution.add_reactant(CarbonMonoxide, 2);
	carbonMonoxideResolution.add_reactant(Oxygen, 1);
	carbonMonoxideResolution.add_product(CarbonDioxide, 2);
	atmosphericsReactions.push_back(carbonMonoxideResolution);

	ElasticAtmosphere room1(1000);
	room1.add_moles_temp(TNT, 10, CELSIUS(999));
	room1.add_moles_temp(Oxygen, 40, CELSIUS(999));
	double maxPressure = 6000;
	double maxTemp = 34000;

	InitWindow(640, 640, "zatmos");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		double totalMoles = room1.get_moles();
		double totalEnergy = room1.heatEnergy;
		for (auto const &entry : room1.contents) {
			double mass = entry.second * Chemical::get_molar_mass(entry.first);
			static double const c = 299792458; // m/s^2
			static double const c2 = c*c; //m^2/s^4
			totalEnergy += c2 * mass;
		}
		BeginDrawing();
		ClearBackground(WHITE);
		DrawText(TextFormat("Total moles in system: %lf\n", totalMoles), 0, 0, 16, BLACK);
		DrawText(TextFormat("Total energy in system: %lf\n", totalEnergy), 0, 32, 16, BLACK);
		draw_atmosphere(room1, maxTemp, maxPressure, 64, 64);
		EndDrawing();
		double dt = GetFrameTime();
		double t = GetTime();
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			room1.ignite();
		}
		room1.tick(dt);
	}
}

/* int main() */
/* { */
/* 	AtmosphericsReaction hydrogenCombustion(CELSIUS(550), 241920); */
/* 	hydrogenCombustion.add_reactant(Hydrogen, 2); // 2H2 + O2 = 2H2O */
/* 	hydrogenCombustion.add_reactant(Oxygen, 1); */
/* 	hydrogenCombustion.add_product(Water, 2); */
/* 	hydrogenCombustion.reactionSpeed = 0.1; */
/* 	atmosphericsReactions.push_back(hydrogenCombustion); */
/* 	Atmosphere room1(ROOM, 1000); // 1m^3 */
/* 	room1.add_moles_temp(Hydrogen, 20, 293.15); */
/* 	room1.add_moles_temp(Oxygen, 10, 293.15); */
/* 	double maxPressure = 6000; */
/* 	double maxTemp = 34000; */
/*  */
/* 	Atmosphere room2(ROOM, 1000); */
/*  */
/* 	AtmosphericsValve valve(room1, room2); */
/*  */
/* 	InitWindow(640, 640, "zatmos"); */
/* 	SetTargetFPS(10); */
/*  */
/* 	while (!WindowShouldClose()) { */
/* 		double totalMoles = room1.get_moles() + room2.get_moles(); */
/* 		double totalEnergy = room1.heatEnergy + room2.heatEnergy; */
/* 		for (auto const &entry : room1.contents) { */
/* 			double mass = entry.second * Chemical::get_molar_mass(entry.first); */
/* 			static double const c = 299792458; // m/s^2 */
/* 			static double const c2 = c*c; //m^2/s^4 */
/* 			totalEnergy += c2 * mass; */
/* 		} */
/* 		for (auto const &entry : room2.contents) { */
/* 			double mass = entry.second * Chemical::get_molar_mass(entry.first); */
/* 			static double const c = 299792458; // m/s^2 */
/* 			static double const c2 = c*c; //m^2/s^4 */
/* 			totalEnergy += c2 * mass; */
/* 		} */
/* 		BeginDrawing(); */
/* 		ClearBackground(WHITE); */
/* 		DrawText(TextFormat("Total moles in system: %lf\n", totalMoles), 0, 0, 16, BLACK); */
/* 		DrawText(TextFormat("Total energy in system: %lf\n", totalEnergy), 0, 32, 16, BLACK); */
/* 		draw_atmosphere(room1, maxTemp, maxPressure, 64, 64); */
/* 		draw_atmosphere(room2, maxTemp, maxPressure, 64, 320); */
/* 		if (valve.active) */
/* 			DrawRectangle(64+100/2-4, 64+100, 8, 320-64-64, BLACK); */
/* 		EndDrawing(); */
/* 		double dt = GetFrameTime(); */
/* 		double t = GetTime(); */
/* 		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) */
/* 			room1.ignite(); */
/* 		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) */
/* 			valve.toggle(); */
/* 		if (IsKeyDown(KEY_SPACE)) */
/* 			room2.add_volume(dt * 100.0); */
/* 		valve.update(dt / 6); */
/* 		room1.tick(dt / 6); */
/* 		room2.tick(dt / 6); */
/* 	} */
/* } */
