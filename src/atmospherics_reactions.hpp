#ifndef ATMOSPHERICS_REACTIONS_HPP
#define ATMOSPHERICS_REACTIONS_HPP

#include <string>
#include <vector>
#include "atmosphere.hpp"


struct AtmosphericsReaction {
	AtmosphericsMixture reactants;
	AtmosphericsMixture products;
	double autoignitionPoint;
	// J/mol
	double energyReleased;
	// mol/s
	double reactionSpeed = 1;
	bool ignitable = true;
	// K, J/mol
	AtmosphericsReaction(double autoignitionPoint, double energyReleased, bool ignitable = true);
	void add_reactant(std::string const &chemicalId, double portion);
	void add_product(std::string const &chemicalId, double portion);
	void do_once(Atmosphere &atmosphere, double dt) const;
};
extern std::vector<AtmosphericsReaction> atmosphericsReactions;


#endif
