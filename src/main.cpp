#include "atmosphere.hpp"
#include <cmath>
#include <cstdio>
#include <raylib.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "atmospherics_reactions.hpp"
#include "atmospherics_element.hpp"
#include "atmospherics_device.hpp"


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
		if (!atmosphericsElements.has_key(entry.chemicalId))
			throw std::invalid_argument("Unknown element " + entry.chemicalId);
		auto element = atmosphericsElements[entry.chemicalId];
		double chemPerc = atmosphere.get_percent_pressure(entry.chemicalId) * 100;
		double moles = atmosphere.get_moles(entry.chemicalId);
		DrawText(TextFormat("%s: %lf%%\n\n%lfmol", element->get_short_name().c_str(), chemPerc, moles), x, ty, 32, BLACK);
		ty += 64;
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

	Atmosphere hydrogenTank(ROOM, 1000);
	hydrogenTank.add_moles_temp("hydrogen", 80, CELSIUS(20));
	Atmosphere oxygenTank(ROOM, 2000);
	oxygenTank.add_moles_temp("oxygen", 80, CELSIUS(20));
	Atmosphere reactionChamber(ROOM, 1000);
	Atmosphere waterTank(ROOM, 1000);
	double maxPressure = 6000;
	double maxTemp = 34000;

	InitWindow(640, 640, "zatmos");
	//SetTargetFPS(60);

	ZAtmos::AtmosphericsDevices::MolarMixer mixer(oxygenTank, hydrogenTank, reactionChamber, 0.667, 10);
	std::vector<std::string> waterFilter;
	waterFilter.push_back("water");
	ZAtmos::AtmosphericsDevices::FilteredVolumePump h2oFilter(reactionChamber, waterTank, waterFilter, 100);
	ZAtmos::AtmosphericsDevices::TemperatureController cooler(waterTank, -10000);
	ZAtmos::AtmosphericsDevices::TemperatureController heater(reactionChamber, 10000);
	cooler.minTemperature = CELSIUS(20);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(WHITE);
		draw_atmosphere(hydrogenTank, maxTemp, maxPressure, 64, 64);
		draw_atmosphere(oxygenTank, maxTemp, maxPressure, 64, 256);
		draw_atmosphere(reactionChamber, maxTemp, maxPressure, 320, (64+256)/2);
		draw_atmosphere(waterTank, maxTemp, maxPressure, 320, (64+256) / 2 + 256 + 96);
		if (mixer.active) {
			DrawLine(64+100, 64+50, 320, (64+256)/2 + 50, BLACK);
			DrawLine(64+100, 256+50, 320, (64+256)/2 + 50, BLACK);
		}
		if (h2oFilter.active)
			DrawLine(320+50, (64+256)/2 + 100, 320+50, (64+256) / 2 + 256 + 96, BLACK);
		if (cooler.active)
			DrawRectangle(320+50 + 16, (64+256) / 2 + 256 + 96, 8, 8, BLUE);
		EndDrawing();
		double dt = GetFrameTime();
		//double t = GetTime();
		if (IsKeyPressed(KEY_Q))
			mixer.toggle();
		if (IsKeyPressed(KEY_F))
			h2oFilter.toggle();
		if (IsKeyPressed(KEY_C))
			cooler.toggle();
		if (IsKeyPressed(KEY_E))
			heater.toggle();

		DrawFPS(0, 0);
		hydrogenTank.tick(dt);
		oxygenTank.tick(dt);
		reactionChamber.tick(dt);
		waterTank.tick(dt);
		mixer.update(dt);
		h2oFilter.update(dt);
		cooler.update(dt);
		heater.update(dt);
		//valve.update(dt);
	}
}
