#include "atmospherics_reactions.hpp"
#include "atmosphere.hpp"
#include "atmospherics_element.hpp"
#include <utility>


AtmosphericsReaction::AtmosphericsReaction(double autoignitionPoint, double energyReleased, bool ignitable)
	: autoignitionPoint(autoignitionPoint), energyReleased(energyReleased), ignitable(ignitable)
{}
void AtmosphericsReaction::add_reactant(std::string const &chemicalId, double portion)
{
	reactants.push_back(std::pair(chemicalId, portion));
}
void AtmosphericsReaction::add_product(std::string const &chemicalId, double portion)
{
	products.push_back(std::pair(chemicalId, portion));
}
void AtmosphericsReaction::do_once(Atmosphere &atmosphere, double dt) const
{
	double amountPossible = 1.0;
	double speedScale = reactionSpeed;
	speedScale *= atmosphere.tempKelvin / autoignitionPoint; // faster the hotter the reaction is, maybe change later
	for (auto const &reactant : reactants) {
		double amountPossibleSingle = atmosphere.get_moles(reactant.first) / (reactant.second * speedScale);
		amountPossible = std::min(amountPossible, amountPossibleSingle);
	}
	for (auto const &reactant : reactants) {
		double molesRemoved = reactant.second * speedScale * dt;
		atmosphere.remove_without_heat(reactant.first, molesRemoved);
	}
	for (auto const &product : products) {
		double molesAdded = product.second * speedScale * dt;
		atmosphere.add_moles_heat(product.first, molesAdded, 0);
	}
	atmosphere.add_heat(energyReleased * speedScale * dt);
}
std::vector<AtmosphericsReaction> atmosphericsReactions;
