#include "device.hpp"
#include "atmosphere.hpp"
AtmosphericsPassiveVent::AtmosphericsPassiveVent(Atmosphere &source, Atmosphere &destination)
	: source(source), destination(destination)
{}

void AtmosphericsPassiveVent::update(double dt)
{
	source.mix_with(destination, dt);
}

AtmosphericsValve::AtmosphericsValve(Atmosphere &source, Atmosphere &destination)
	: source(source), destination(destination)
{}

void AtmosphericsValve::update(double dt)
{
	if (active)
		source.mix_with(destination, dt);
}

void AtmosphericsValve::toggle()
{
	active = !active;
}
void AtmosphericsValve::set(bool active)
{
	this->active = active;
}
