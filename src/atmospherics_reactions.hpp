#ifndef ATMOSPHERICS_REACTIONS_HPP
#define ATMOSPHERICS_REACTIONS_HPP

#include <utility>
#include <vector>
#include "atmosphere.hpp"
#include "chemical_types.hpp"
struct AtmosphericsReaction {
	std::vector<std::pair<ChemicalType, double>> reactants;
	std::vector<std::pair<ChemicalType, double>> products;
	bool ignitable = true;
	double autoignitionPoint;
	// J/mol
	double energyReleased;
	// mol/s
	double reactionSpeed = 1;
	// K, J/mol
	AtmosphericsReaction(double autoignitionPoint, double energyReleased, bool ignitable = true);
	void add_reactant(ChemicalType chemical, double portion);
	void add_product(ChemicalType chemical, double portion);
	void do_once(Atmosphere &atmosphere) const;
	void guess_energy_released();
};
extern std::vector<AtmosphericsReaction> atmosphericsReactions;


#endif
