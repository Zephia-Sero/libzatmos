#ifndef ATMOSPHERE_HPP
#define ATMOSPHERE_HPP

#include <utility>
#include <vector>
enum AtmosphereType {
	WORLD,
	ROOM,
	PIPE,
};

#include "chemical_types.hpp"

struct Atmosphere {
public:
	AtmosphereType type;
	// J / K·mol
	double gasConstant = 8.31446261815324;
	double tempKelvin = 0;
	double minTemperature = 0.001; // K
	double mixRate = 5; // L/kPa·s
	double maxPressure = 1000;
	// note: this is videogamey
	double tempMixRate = 100; // no unit, multiplied against conductivity
	// L
	double volume = 0;
	double heatEnergy = 0;
	// K
	std::vector<std::pair<ChemicalType, double>> contents;

	Atmosphere(AtmosphereType type, double volume);
	bool has(ChemicalType chemical, double atLeastMoles=0) const;
	// used for in-atmosphere reactions, like autoignition and such
	virtual void tick(double dt);
	// attempt to burn the atmosphere
	void ignite();
	void add_volume(double amount);
	void add_moles_temp(ChemicalType chemical, double moles, double tempKelvin);
	void add_mass_temp(ChemicalType chemical, double mass, double tempKelvin);
	void add_moles_heat(ChemicalType chemical, double moles, double heatEnergy);
	void add_mass_heat(ChemicalType chemical, double mass, double heatEnergy);
	void remove(ChemicalType chemical, double moles);
	void remove_without_heat(ChemicalType chemical, double moles);
	void remove_all(ChemicalType chemical);

	void mix_with(Atmosphere &other, double dt, bool temperatureMix = true);
	void mix_temperatures(Atmosphere &other, double dt);
	void move_gas_volume(Atmosphere &other, double volume);
	void move_gas_moles(Atmosphere &other, double moles);

	void recalculate_dirty();
	// K
	double get_temperature() const;
	// J
	void add_heat(double heatEnergy);
	// mol
	double get_moles() const;
	// mol
	double get_moles(ChemicalType chemical) const;
	// kg
	double get_mass() const;
	// kg
	double get_mass(ChemicalType chemical) const;
	// %
	double get_percent_pressure(ChemicalType chemical) const;
	// %
	double get_percent_mass(ChemicalType chemical) const;
	// kPa
	double get_pressure() const;
	// kPa
	double get_pressure(ChemicalType chemical) const;
	// J / K·kg
	double get_specific_heat_mass() const;
	// J / K·mol
	double get_specific_heat_moles() const;
	// J / K
	double get_heat_capacity() const;
	// W/K·m
	double get_thermal_conductivity() const;
};

#endif
