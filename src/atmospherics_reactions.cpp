#include "atmospherics_reactions.hpp"
#include "atmosphere.hpp"
#include "chemical_types.hpp"
#include <utility>


AtmosphericsReaction::AtmosphericsReaction(double autoignitionPoint, double energyReleased, bool ignitable)
	: autoignitionPoint(autoignitionPoint), energyReleased(energyReleased), ignitable(ignitable)
{}
void AtmosphericsReaction::guess_energy_released()
{
	double reactantEnergy = 0;
	static double const c = 299792458;
	static double const c2 = c*c;
	for (auto const &reactant : reactants)
		reactantEnergy += Chemical::get_molar_mass(reactant.first) * reactant.second * c2;
	double productEnergy = 0;
	for (auto const &product : products)
		productEnergy += Chemical::get_molar_mass(product.first) * product.second * c2;
	energyReleased = reactantEnergy - productEnergy;
}
void AtmosphericsReaction::add_reactant(ChemicalType chemical, double portion)
{
	reactants.push_back(std::pair(chemical, portion));
}
void AtmosphericsReaction::add_product(ChemicalType chemical, double portion)
{
	products.push_back(std::pair(chemical, portion));
}
void AtmosphericsReaction::do_once(Atmosphere &atmosphere) const
{
	double amountPossible = 1.0;
	for (auto const &reactant : reactants) {
		double amountPossibleSingle = atmosphere.contents[reactant.first].second / (reactant.second * reactionSpeed);
		amountPossible = std::min(amountPossible, amountPossibleSingle);
	}
	for (auto const &reactant : reactants) {
		double molesRemoved = reactant.second * reactionSpeed;
		atmosphere.remove_without_heat(reactant.first, molesRemoved);
	}
	for (auto const &product : products) {
		double molesAdded = product.second * reactionSpeed;
		atmosphere.add_moles_heat(product.first, molesAdded, 0);
	}
	atmosphere.add_heat(energyReleased * reactionSpeed);
}
std::vector<AtmosphericsReaction> atmosphericsReactions;
