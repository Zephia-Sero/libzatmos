#include "atmosphere.hpp"
#include "elastic_atmosphere.hpp"
#include <cmath>
#include <cstdio>
#include <raylib.h>
#include <stdexcept>
#include <string>
#include "atmospherics_reactions.hpp"
#include "atmospherics_element.hpp"



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
		if (!atmosphericsElements.has_key(entry.first))
			throw std::invalid_argument("Unknown element " + entry.first);
		auto element = atmosphericsElements[entry.first];
		double chemPerc = atmosphere.get_percent_pressure(entry.first) * 100;
		DrawText(TextFormat("%s: %lf%%", element->get_short_name().c_str(), chemPerc), x, ty, 32, BLACK);
		ty += 32;
	}
}

#define CELSIUS(x) (x + 273.15)
int main()
{
	register_atmospherics_builtins();

	AtmosphericsReaction hydrogenCombustion(CELSIUS(550), 241920);
	hydrogenCombustion.add_reactant("hydrogen", 2); // 2H2 + O2 = 2H2O
	hydrogenCombustion.add_reactant("oxygen", 1);
	hydrogenCombustion.add_product("water", 2);
	hydrogenCombustion.reactionSpeed = 1.0;
	atmosphericsReactions.push_back(hydrogenCombustion);

	AtmosphericsReaction hydrogenDisplacement(CELSIUS(800), 711204);
	hydrogenDisplacement.add_reactant("hydrogen", 4); // 4H2 + 2CO2 = C2 + 4H2O
	hydrogenDisplacement.add_reactant("carbon-dioxide", 2);
	hydrogenDisplacement.add_product("nitrogen", 1);
	hydrogenDisplacement.add_product("water", 4);
	hydrogenDisplacement.reactionSpeed = 1.0;
	atmosphericsReactions.push_back(hydrogenDisplacement);

	Atmosphere room1(ROOM, 1000);
	room1.add_moles_temp("hydrogen", 80, CELSIUS(20));
	room1.add_moles_temp("oxygen", 40, CELSIUS(20));
	room1.add_moles_temp("carbon-dioxide", 10, CELSIUS(20));
	double maxPressure = 6000;
	double maxTemp = 34000;

	InitWindow(640, 640, "zatmos");
	//SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(WHITE);
		draw_atmosphere(room1, maxTemp, maxPressure, 64, 64);
		EndDrawing();
		double dt = GetFrameTime();
		double t = GetTime();
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			room1.ignite();
		}
		DrawFPS(0, 0);
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
