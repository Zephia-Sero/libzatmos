#include "atmospherics_element.hpp"
#include "registry.hpp"

ImmutableRegistry<AtmosphericsElement> atmosphericsElements;

void register_atmospherics_builtins()
{
	atmosphericsElements.add("hydrogen",
		AtmosphericsElement("Hydrogen", "H2",
			14295.63492,
			2.016 / 1000.0,
			0.1819));
	atmosphericsElements.add("nitrogen",
		AtmosphericsElement("Nitrogen", "N2",
			1039.502524,
			28.01340 / 1000.0,
			0.026));
	atmosphericsElements.add("oxygen",
		AtmosphericsElement("Oxygen", "O2",
			918.1594310,
			31.9988 / 1000.0,
			0.0238));
	atmosphericsElements.add("carbon-dioxide",
		AtmosphericsElement("Carbon Dioxide", "CO2",
			871,
			44.009 / 1000.0,
			0.0872));
	atmosphericsElements.add("water",
		AtmosphericsElement("Water", "H2O",
			2026.057509,
			18.0152833 / 1000.0,
			0.68));
}
AtmosphericsElement::AtmosphericsElement(std::string name, std::string shortName, double heatCapacity, double molarMass, double thermalConductivity)
	: name(name), shortName(shortName),
	  heatCapacity(heatCapacity), molarMass(molarMass), thermalConductivity(thermalConductivity)
{}
AtmosphericsElement::AtmosphericsElement()
{
	name = "UNDEFINED";
	shortName = "UNDEFINED";
	heatCapacity = 0;
	molarMass = 0;
	thermalConductivity = 0;
}
