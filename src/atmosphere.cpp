#include "atmosphere.hpp"
#include "chemical_types.hpp"
#include "atmospherics_reactions.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <vector>

Atmosphere::Atmosphere(AtmosphereType type, double volume)
	: type(type), volume(volume), tempKelvin(0), contents()
{
	recalculate_dirty();
}
void Atmosphere::add_moles_temp(ChemicalType chemical, double moles, double tempKelvin)
{
	// mix gas in
	for (std::pair<ChemicalType, double> &entry : contents) {
		if (entry.first == chemical) {
			entry.second += moles;
			goto no_add;
		}
	}
	// couldn't find, add to list
	contents.push_back(std::pair(chemical, moles));
no_add:;
	// mix temperatures
	add_heat(tempKelvin * Chemical::get_heat_capacity_moles(chemical, moles));
}
void Atmosphere::add_volume(double amount)
{
	volume += amount;
	volume = std::max(0.0, volume);
	if (volume <= 0)
		contents.clear();
	recalculate_dirty();
}
void Atmosphere::add_moles_heat(ChemicalType chemical, double moles, double heatEnergy)
{
	// mix gas in
	for (std::pair<ChemicalType, double> &entry : contents) {
		if (entry.first == chemical) {
			entry.second += moles;
			goto no_add;
		}
	}
	// couldn't find, add to list
	contents.push_back(std::pair(chemical, moles));
no_add:;
	// mix temperatures
	add_heat(heatEnergy);
}
void Atmosphere::add_mass_temp(ChemicalType chemical, double mass, double tempKelvin)
{
	// a / (a/b) = a * b/a = b
	double moles = mass / Chemical::get_molar_mass(chemical);
	// mix gas in
	for (std::pair<ChemicalType, double> &entry : contents) {
		if (entry.first == chemical) {
			entry.second += moles;
			goto no_add;
		}
	}
	// couldn't find, add to list
	contents.push_back(std::pair(chemical, moles));
no_add:;
	// mix temperatures
	add_heat(tempKelvin * Chemical::get_heat_capacity_moles(chemical, moles));
}
void Atmosphere::add_mass_heat(ChemicalType chemical, double mass, double heatEnergy)
{
	// a / (a/b) = a * b/a = b
	double moles = mass / Chemical::get_molar_mass(chemical);
	// mix gas in
	for (std::pair<ChemicalType, double> &entry : contents) {
		if (entry.first == chemical) {
			entry.second += moles;
			goto no_add;
		}
	}
	// couldn't find, add to list
	contents.push_back(std::pair(chemical, moles));
no_add:;
	// mix temperatures
	add_heat(heatEnergy);
}
void Atmosphere::remove(ChemicalType chemical, double moles)
{
	for (auto v = contents.begin(); v < contents.end(); ++v) {
		if (v->first == chemical) {
			if (moles >= v->second) {
				contents.erase(v);
				add_heat(-(v->second * Chemical::get_specific_heat_moles(v->first) * get_temperature()));
			} else {
				v->second -= moles;
				add_heat(-(moles * Chemical::get_specific_heat_moles(v->first) * get_temperature()));
			}
			return;
		}
	}
}
void Atmosphere::remove_without_heat(ChemicalType chemical, double moles)
{
	for (auto v = contents.begin(); v < contents.end(); ++v) {
		if (v->first == chemical) {
			if (moles >= v->second) {
				contents.erase(v);
			} else {
				v->second -= moles;
			}
			return;
		}
	}
}
void Atmosphere::remove_all(ChemicalType chemical)
{
	for (auto v = contents.cbegin(); v < contents.cend(); ++v) {
		if (v->first == chemical) {
			// mol * J/K·mol * K = J
			contents.erase(v);
			add_heat(-(v->second * Chemical::get_specific_heat_moles(v->first) * get_temperature()));
			return;
		}
	}
}

void Atmosphere::recalculate_dirty()
{
	// J / (J / K) = J * K/J = K
	if (heatEnergy <= 0 || get_heat_capacity() <= 0) {
		tempKelvin = minTemperature;
		heatEnergy = 0;
	} else {
		tempKelvin = heatEnergy / get_heat_capacity();
	}
}
// K
double Atmosphere::get_temperature() const
{
	return tempKelvin;
}
void Atmosphere::add_heat(double heatEnergy)
{
	// you cannot go below 0.1 kelvin!
	if (heatEnergy < 0) {
		this->heatEnergy = std::max(get_heat_capacity() * minTemperature, this->heatEnergy + heatEnergy);
	} else {
		this->heatEnergy += heatEnergy;
	}
	recalculate_dirty();
}
double Atmosphere::get_moles() const
{
	double sum = 0;
	for (std::pair<ChemicalType, double> const &entry : contents) {
		sum += entry.second;
	}
	return sum;
}
double Atmosphere::get_moles(ChemicalType chemical) const
{
	for (std::pair<ChemicalType, double> const &entry : contents) {
		if (entry.first == chemical)
			return entry.second;
	}
	return 0;
}
double Atmosphere::get_percent_pressure(ChemicalType chemical) const
{
	// PV = nRT
	// V = nRT/T
	// p = RT/V (p = P/n) pressure per mole
	// ap / (ap + bp + cp)
	// ap / p(a + b + c)
	// a / (a + b + c)
	// molar percent is the same as pressure (volume) percent
	double moles = get_moles();
	return get_moles(chemical) / moles;
}
double Atmosphere::get_mass() const
{
	double sum = 0;
	for (std::pair<ChemicalType, double> const &entry : contents) {
		sum += Chemical::get_molar_mass(entry.first) * entry.second;
	}
	return sum;
}
double Atmosphere::get_mass(ChemicalType chemical) const
{
	for (std::pair<ChemicalType, double> const &entry : contents) {
		if (entry.first == chemical)
			return Chemical::get_molar_mass(chemical) * entry.second;
	}
	return 0;
}
double Atmosphere::get_percent_mass(ChemicalType chemical) const
{
	return get_mass(chemical) / get_mass();
}
double Atmosphere::get_pressure() const
{
	// PV = nRT
	// P = nRT / V
	double energy = get_moles() * gasConstant * tempKelvin; // J
	return energy / volume;
}
double Atmosphere::get_pressure(ChemicalType chemical) const
{
	double energy = get_moles(chemical) * gasConstant * tempKelvin;
	return energy / volume;
}
// J / K·kg
double Atmosphere::get_specific_heat_mass() const
{
	double sum = 0;
	double totalMass = get_mass();
	for (std::pair<ChemicalType, double> const &entry : contents) {
		double mass = get_mass(entry.first);
		double massRatio = mass / totalMass;
		sum += massRatio * Chemical::get_specific_heat_mass(entry.first);
	}
	return sum;
}
// J / K·mol
double Atmosphere::get_specific_heat_moles() const
{
	double sum = 0;
	double totalMass = get_mass();
	for (std::pair<ChemicalType, double> const &entry : contents) {
		double mass = get_mass(entry.first);
		double massRatio = mass / totalMass;
		sum += massRatio * Chemical::get_specific_heat_moles(entry.first);
	}
	return sum;
}
double Atmosphere::get_thermal_conductivity() const
{
	double sum = 0;
	double totalMass = get_mass();
	for (std::pair<ChemicalType, double> const &entry : contents) {
		double mass = get_mass(entry.first);
		double massRatio = mass / totalMass;
		sum += massRatio * Chemical::get_thermal_conductivity(entry.first);
	}
	return sum;
}
// J/K
double Atmosphere::get_heat_capacity() const
{
	return get_specific_heat_moles() * get_moles();
}

void Atmosphere::mix_with(Atmosphere &other, double dt, bool temperatureMix)
{
	// keep portion for self, so it doesn't "slosh" back and forth
	double pressureGradient = 0.1 * (get_pressure() - other.get_pressure());
	// + means flow towards other, - means flow towards this
	// L/kPa·s
	double flowMult = maxPressure / (maxPressure + std::abs(pressureGradient));
	flowMult *= flowMult;
	double dN = mixRate * flowMult * pressureGradient * dt;
	if (pressureGradient > 0) {
		move_gas_moles(other, dN);
	} else {
		other.move_gas_moles(*this, -dN);
	}
	if (temperatureMix)
		mix_temperatures(other, dt);
}
void Atmosphere::mix_temperatures(Atmosphere &other, double dt)
{
	// keep half for self, so it doesn't "slosh" back and forth
	// + means flow towards other, - means flow towards this
	double temperatureGradient = (get_temperature() - other.get_temperature());
	double distance = 0.01; // arbitrary 1cm distance to "conduct across"
	double flow = get_thermal_conductivity() * temperatureGradient / distance;
	double area = 1; // arbitrary 1m^2 conduction area
	double dT = flow * area * dt * tempMixRate;
	add_heat(-dT);
	other.add_heat(dT);
}
void Atmosphere::move_gas_moles(Atmosphere &other, double moles)
{
	// PV = nRT
	// V = nRT/P
	// V/n = RT/P
	// n/V = P/RT
	std::vector<std::pair<ChemicalType, double>> moved;
	for (std::pair<ChemicalType, double> &entry : contents) {
		double molarPercent = get_percent_mass(entry.first);
		double entryPortion = molarPercent * moles;
		moved.push_back(std::pair(entry.first, entryPortion));
	}

	double temp = get_temperature();

	for (std::pair<ChemicalType, double> &entry : moved) {
		other.add_moles_temp(entry.first, entry.second, temp);
		remove(entry.first, entry.second);
	}
}
void Atmosphere::move_gas_volume(Atmosphere &other, double volume)
{
	// PV = nRT
	// V = nRT/P
	// V/n = RT/P
	// n/V = P/RT
	std::vector<std::pair<ChemicalType, double>> moved;
	for (std::pair<ChemicalType, double> &entry : contents) {
		double molesPerVolume = entry.second / this->volume;
		double moles = molesPerVolume * volume;
		moved.push_back(std::pair(entry.first, moles));
	}

	double temp = get_temperature();

	for (std::pair<ChemicalType, double> &entry : moved) {
		other.add_moles_temp(entry.first, entry.second, temp);
		remove(entry.first, entry.second);
	}
}
bool Atmosphere::has(ChemicalType chemical, double atLeastMoles) const
{
	for (auto const &entry : contents)
		if (entry.first == chemical)
			return entry.second > atLeastMoles;
	return false;
}

// used for in-atmosphere reactions, like autoignition and such
void Atmosphere::tick(double dt)
{
	double temp = get_temperature();
	for (auto reaction : atmosphericsReactions) {
		for (auto const &reactant : reaction.reactants) {
			if (!has(reactant.first))
				goto next;
		}
		if (get_temperature() < reaction.autoignitionPoint)
next:			continue;
		reaction.do_once(*this);
	}
}
// forcefully burn the atmosphere if possible
void Atmosphere::ignite()
{
	for (auto reaction : atmosphericsReactions) {
		if (!reaction.ignitable) continue;
		for (auto const &reactant : reaction.reactants) {
			if (!has(reactant.first))
				goto next;
		}
		reaction.do_once(*this);
next:		continue;
	}
}
