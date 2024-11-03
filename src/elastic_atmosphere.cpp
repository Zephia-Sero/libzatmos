#include "elastic_atmosphere.hpp"
#include "atmosphere.hpp"

ElasticAtmosphere::ElasticAtmosphere(double initialVolume)
	: Atmosphere(initialVolume)
{}
void ElasticAtmosphere::tick(double dt)
{
	Atmosphere::tick(dt);
	// PV = nRT
	// P = external pressure
	// n = get_moles()
	// V = what we need to find (more specifically, dV)
	// R = gas constant
	// T = temp
	// Vt = nRT / P
	// dV = Vt - V0
	double Vt = get_moles() * gasConstant * get_temperature() / externalPressure;
	double dV = Vt - volume;
	add_volume(dV);
}
