#ifndef ATMOSPHERICS_REACTIONS_HPP
#define ATMOSPHERICS_REACTIONS_HPP

#include <string>
#include <utility>
#include <vector>
#include "atmosphere.hpp"
#include "atmospherics_element.hpp"
struct AtmosphericsReaction {
	std::vector<std::pair<std::string, double>> reactants;
	std::vector<std::pair<std::string, double>> products;
	bool ignitable = true;
	double autoignitionPoint;
	// J/mol
	double energyReleased;
	// mol/s
	double reactionSpeed = 1;
	// K, J/mol
	AtmosphericsReaction(double autoignitionPoint, double energyReleased, bool ignitable = true);
	void add_reactant(std::string const &chemicalId, double portion);
	void add_product(std::string const &chemicalId, double portion);
	void do_once(Atmosphere &atmosphere, double dt) const;
};
extern std::vector<AtmosphericsReaction> atmosphericsReactions;


#endif
