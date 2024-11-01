#include "atmosphere.hpp"
#include "atmospherics_element.hpp"
#include "atmospherics_reactions.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

int Atmosphere::currentId = 0;

Atmosphere::Atmosphere(AtmosphereType type, double volume)
	: type(type), volume(volume), tempKelvin(0), contents()
{
	id = Atmosphere::currentId++;
	recalculate_dirty();
}
void Atmosphere::add_moles_temp(std::string const &chemicalId, double moles, double tempKelvin)
{
	if (!atmosphericsElements.has_key(chemicalId))
		throw std::invalid_argument("Atmospherics Element '" + chemicalId + "' not found when adding to atmosphere " + std::to_string(id));
	// mix gas in
	for (std::pair<std::string, double> &entry : contents) {
		if (entry.first == chemicalId) {
			entry.second += moles;
			goto no_add;
		}
	}
	// couldn't find, add to list
	contents.push_back(std::pair(chemicalId, moles));
no_add:;
	auto element = atmosphericsElements[chemicalId];
	// mix temperatures
	add_heat(tempKelvin * element->get_heat_capacity_moles() * moles);
}
void Atmosphere::add_volume(double amount)
{
	volume += amount;
	volume = std::max(0.0, volume);
	if (volume <= 0)
		contents.clear();
	recalculate_dirty();
}
void Atmosphere::add_moles_heat(std::string const &chemicalId, double moles, double heatEnergy)
{
	if (!atmosphericsElements.has_key(chemicalId))
		throw std::invalid_argument("Atmospherics Element '" + chemicalId + "' not found when adding to atmosphere " + std::to_string(id));
	// mix gas in
	for (std::pair<std::string, double> &entry : contents) {
		if (entry.first == chemicalId) {
			entry.second += moles;
			goto no_add;
		}
	}
	// couldn't find, add to list
	contents.push_back(std::pair(chemicalId, moles));
no_add:;
	// mix temperatures
	add_heat(heatEnergy);
}
void Atmosphere::add_mass_temp(std::string const &chemicalId, double mass, double tempKelvin)
{
	if (!atmosphericsElements.has_key(chemicalId))
		throw std::invalid_argument("Atmospherics Element '" + chemicalId + "' not found when adding to atmosphere " + std::to_string(id));
	auto element = atmosphericsElements[chemicalId];
	// a / (a/b) = a * b/a = b
	double moles = mass / element->get_molar_mass();
	// mix gas in
	for (std::pair<std::string, double> &entry : contents) {
		if (entry.first == chemicalId) {
			entry.second += moles;
			goto no_add;
		}
	}
	// couldn't find, add to list
	contents.push_back(std::pair(chemicalId, moles));
no_add:;
	// mix temperatures
	add_heat(tempKelvin * element->get_heat_capacity_mass() * mass);
}
void Atmosphere::add_mass_heat(std::string const &chemicalId, double mass, double heatEnergy)
{
	if (!atmosphericsElements.has_key(chemicalId))
		throw std::invalid_argument("Atmospherics Element '" + chemicalId + "' not found when adding to atmosphere " + std::to_string(id));
	auto element = atmosphericsElements[chemicalId];
	// a / (a/b) = a * b/a = b
	double moles = mass / element->get_molar_mass();
	// mix gas in
	for (std::pair<std::string, double> &entry : contents) {
		if (entry.first == chemicalId) {
			entry.second += moles;
			goto no_add;
		}
	}
	// couldn't find, add to list
	contents.push_back(std::pair(chemicalId, moles));
no_add:;
	// mix temperatures
	add_heat(heatEnergy);
}
void Atmosphere::remove(std::string const &chemicalId, double moles)
{
	if (!atmosphericsElements.has_key(chemicalId))
		throw std::invalid_argument("Atmospherics Element '" + chemicalId + "' not found when removing from atmosphere " + std::to_string(id));
	auto element = atmosphericsElements[chemicalId];
	for (auto v = contents.begin(); v < contents.end(); ++v) {
		if (v->first == chemicalId) {
			if (moles >= v->second) {
				contents.erase(v);
				add_heat(-(v->second * element->get_heat_capacity_moles() * get_temperature()));
			} else {
				v->second -= moles;
				add_heat(-(moles * element->get_heat_capacity_moles() * get_temperature()));
			}
			return;
		}
	}
}
void Atmosphere::remove_without_heat(std::string const &chemicalId, double moles)
{
	for (auto v = contents.begin(); v < contents.end(); ++v) {
		if (v->first == chemicalId) {
			if (moles >= v->second) {
				contents.erase(v);
			} else {
				v->second -= moles;
			}
			return;
		}
	}
}
void Atmosphere::remove_all(std::string const &chemicalId)
{
	if (!atmosphericsElements.has_key(chemicalId))
		throw std::invalid_argument("Atmospherics Element '" + chemicalId + "' not found when removing from atmosphere " + std::to_string(id));
	auto element = atmosphericsElements[chemicalId];
	for (auto v = contents.cbegin(); v < contents.cend(); ++v) {
		if (v->first == chemicalId) {
			// mol * J/K·mol * K = J
			contents.erase(v);
			add_heat(-(v->second * element->get_heat_capacity_moles() * get_temperature()));
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
	for (std::pair<std::string, double> const &entry : contents) {
		sum += entry.second;
	}
	return sum;
}
double Atmosphere::get_moles(std::string const &chemicalId) const
{
	for (std::pair<std::string, double> const &entry : contents) {
		if (entry.first == chemicalId)
			return entry.second;
	}
	return 0;
}
double Atmosphere::get_percent_pressure(std::string const &chemicalId) const
{
	// PV = nRT
	// V = nRT/T
	// p = RT/V (p = P/n) pressure per mole
	// ap / (ap + bp + cp)
	// ap / p(a + b + c)
	// a / (a + b + c)
	// molar percent is the same as pressure (volume) percent
	double moles = get_moles();
	return get_moles(chemicalId) / moles;
}
double Atmosphere::get_mass() const
{
	double sum = 0;
	for (std::pair<std::string, double> const &entry : contents)
		sum += get_mass(entry.first);
	return sum;
}
double Atmosphere::get_mass(std::string const &chemicalId) const
{
	for (std::pair<std::string, double> const &entry : contents) {
		if (entry.first == chemicalId) {
			if (!atmosphericsElements.has_key(chemicalId))
				throw std::invalid_argument("Atmospherics Element '" + chemicalId + "' not found when calculating mass from atmosphere " + std::to_string(id));
			auto element = atmosphericsElements[chemicalId];
			return element->get_molar_mass() * entry.second;
		}
	}
	return 0;
}
double Atmosphere::get_percent_mass(std::string const &chemicalId) const
{
	return get_mass(chemicalId) / get_mass();
}
double Atmosphere::get_pressure() const
{
	// PV = nRT
	// P = nRT / V
	double energy = get_moles() * gasConstant * tempKelvin; // J
	return energy / volume;
}
double Atmosphere::get_pressure(std::string const &chemicalId) const
{
	double energy = get_moles(chemicalId) * gasConstant * tempKelvin;
	return energy / volume;
}
// J / K·kg
double Atmosphere::get_specific_heat_mass() const
{
	double sum = 0;
	double totalMass = get_mass();
	for (std::pair<std::string, double> const &entry : contents) {
		if (!atmosphericsElements.has_key(entry.first))
			throw std::invalid_argument("Atmospherics Element '" + entry.first + "' not found when calculating heat capacity from atmosphere " + std::to_string(id));
		auto element = atmosphericsElements[entry.first];
		double mass = get_mass(entry.first);
		double massRatio = mass / totalMass;
		sum += massRatio * element->get_heat_capacity_mass();
	}
	return sum;
}
// J / K·mol
double Atmosphere::get_specific_heat_moles() const
{
	double sum = 0;
	double totalMass = get_mass();
	for (std::pair<std::string, double> const &entry : contents) {
		if (!atmosphericsElements.has_key(entry.first))
			throw std::invalid_argument("Atmospherics Element '" + entry.first + "' not found when calculating heat capacity from atmosphere " + std::to_string(id));
		auto element = atmosphericsElements[entry.first];
		double mass = get_mass(entry.first);
		double massRatio = mass / totalMass;
		sum += massRatio * element->get_heat_capacity_moles();
	}
	return sum;
}
double Atmosphere::get_thermal_conductivity() const
{
	double sum = 0;
	double totalMass = get_mass();
	for (std::pair<std::string, double> const &entry : contents) {
		if (!atmosphericsElements.has_key(entry.first))
			throw std::invalid_argument("Atmospherics Element '" + entry.first + "' not found when calculating thermal conductivity from atmosphere " + std::to_string(id));
		auto element = atmosphericsElements[entry.first];
		double mass = get_mass(entry.first);
		double massRatio = mass / totalMass;
		sum += massRatio * element->get_thermal_conductivity();
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
	std::vector<std::pair<std::string, double>> moved;
	for (std::pair<std::string, double> &entry : contents) {
		double molarPercent = get_percent_mass(entry.first);
		double entryPortion = molarPercent * moles;
		moved.push_back(std::pair(entry.first, entryPortion));
	}

	double temp = get_temperature();

	for (std::pair<std::string, double> &entry : moved) {
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
	std::vector<std::pair<std::string, double>> moved;
	for (std::pair<std::string, double> &entry : contents) {
		double molesPerVolume = entry.second / this->volume;
		double moles = molesPerVolume * volume;
		moved.push_back(std::pair(entry.first, moles));
	}

	double temp = get_temperature();

	for (std::pair<std::string, double> &entry : moved) {
		other.add_moles_temp(entry.first, entry.second, temp);
		remove(entry.first, entry.second);
	}
}
bool Atmosphere::has(std::string const &chemicalId, double atLeastMoles) const
{
	for (auto const &entry : contents)
		if (entry.first == chemicalId)
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
		reaction.do_once(*this, dt);
	}
}
// forcefully burn the atmosphere if possible
void Atmosphere::ignite(double dt)
{
	for (auto reaction : atmosphericsReactions) {
		if (!reaction.ignitable) continue;
		for (auto const &reactant : reaction.reactants) {
			if (!has(reactant.first))
				goto next;
		}
		reaction.do_once(*this, dt);
next:		continue;
	}
}