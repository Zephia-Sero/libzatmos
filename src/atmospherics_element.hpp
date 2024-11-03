#ifndef CHEMICAL_TYPES_HPP
#define CHEMICAL_TYPES_HPP

#include "registry.hpp"
#include <string>

namespace ZAtmos {
struct AtmosphericsElement {
private:
	std::string name;
	std::string shortName;
	// J / (K · kg)
	double heatCapacity;
	// kg / mol
	double molarMass;
	// W / (m · K)
	double thermalConductivity;
public:
	AtmosphericsElement(std::string name, std::string shortName, double heatCapacity, double molarMass, double thermalConductivity);
	AtmosphericsElement();
	inline std::string get_name() const { return name; }
	inline std::string get_short_name() const { return shortName; }
	// J / (K · mol)
	inline double get_heat_capacity_moles() const { return heatCapacity * molarMass; };
	// J / (K · kg)
	inline double get_heat_capacity_mass() const { return heatCapacity; };
	// kg / mol
	inline double get_molar_mass() const { return molarMass; };
	// W / (m · K)
	inline double get_thermal_conductivity() const { return thermalConductivity; }
};

extern ImmutableRegistry<AtmosphericsElement> atmosphericsElements;
void register_atmospherics_builtins();

}

#endif
