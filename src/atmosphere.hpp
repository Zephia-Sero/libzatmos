#ifndef ATMOSPHERE_HPP
#define ATMOSPHERE_HPP

#include <string>
#include <utility>
#include <vector>
#include "atmospherics_mixture.hpp"

struct Atmosphere {
private:
	static int currentId;
public:
	int id;
	// J / K·mol
	double gasConstant = 8.31446261815324;
	double minTemperature = 0.001; // K
	double mixRate = 5; // L/kPa·s
	double maxPressure = 1000;
	// note: this is videogamey
	double tempMixRate = 100; // no unit, multiplied against conductivity
	// L
	double volume = 0;
	double tempKelvin = 0;
	double heatEnergy = 0;
	// K
	AtmosphericsMixture contents;

	Atmosphere(double volume);
	bool has(std::string const &chemicalId, double atLeastMoles=0) const;
	// used for in-atmosphere reactions, like autoignition and such
	virtual void tick(double dt);
	// attempt to burn the atmosphere
	void ignite(double dt=1);
	void add_volume(double amount);
	void add_moles_temp(std::string const &chemicalId, double moles, double tempKelvin);
	void add_mass_temp(std::string const &chemicalId, double mass, double tempKelvin);
	virtual void add_moles_heat(std::string const &chemicalId, double moles, double heatEnergy);
	void add_mass_heat(std::string const &chemicalId, double mass, double heatEnergy);
	virtual void remove(std::string const &chemicalId, double moles);
	virtual void remove_without_heat(std::string const &chemicalId, double moles);
	virtual void remove_all(std::string const &chemicalId);

	void mix_with(Atmosphere &other, double dt, bool allowBackflow, bool temperatureMix = true);
	void mix_temperatures(Atmosphere &other, double dt);
	// conductivity is in J / (s · K)
	void mix_temperatures_at(Atmosphere &other, double conductivity, double dt);
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
	double get_moles(std::string const &chemicalId) const;
	// kg
	double get_mass() const;
	// kg
	double get_mass(std::string const &chemicalId) const;
	// %
	double get_percent_pressure(std::string const &chemicalId) const;
	// %
	double get_percent_mass(std::string const &chemicalId) const;
	// kPa
	double get_pressure() const;
	// kPa
	double get_pressure(std::string const &chemicalId) const;
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
