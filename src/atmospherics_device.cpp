#include "atmospherics_device.hpp"
#include "atmosphere.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>

namespace ZAtmos {
namespace AtmosphericsDevices {
bool Sink::is_running()
{
	double temperature = source.get_temperature();
	double pressure = source.get_pressure();
	return active
	    && (temperature >= minTemperature && temperature <= maxTemperature)
	    && (pressure >= minPressure && pressure <= maxPressure);
}

bool Source::is_running() {
	double temperature = destination.get_temperature();
	double pressure = destination.get_pressure();
	return active
	    && (temperature >= minTemperature && temperature <= maxTemperature)
	    && (pressure >= minPressure && pressure <= maxPressure);
}

bool BinaryDevice::is_running()
{
	double temperatureDest = destination.get_temperature();
	double pressureDest = destination.get_pressure();
	double temperatureDiff = source.get_temperature() - temperatureDest;
	double pressureDiff = source.get_pressure() - pressureDest;
	return active
	    && (temperatureDest >= minTemperature && temperatureDest <= maxTemperature)
	    && (pressureDest >= minPressure && pressureDest <= maxPressure)
	    && (temperatureDiff >= minTemperatureDifferential && temperatureDiff <= maxTemperatureDifferential)
	    && (pressureDiff >= minPressureDifferential && pressureDiff <= maxPressureDifferential);
}

void Valve::update(double dt)
{
	if (!is_running())
		return;
	source.mix_with(destination, dt, true);
}

void OneWayValve::update(double dt)
{
	if (!is_running())
		return;
	source.mix_with(destination, dt, false);
}

void Spawner::update(double dt)
{
	if (!is_running())
		return;
	for (auto &element : mixture)
		destination.add_moles_temp(element.chemicalId, element.moles * dt, temperature);
}

void Void::update(double dt)
{
	if (!is_running())
		return;
	for (auto &element : source.contents)
		source.remove(element.chemicalId, removalRate * source.get_percent_pressure(element.chemicalId) * dt);
}

void FilteredVoid::update(double dt)
{
	if (!is_running())
		return;
	for (auto &element : filter)
		source.remove(element, removalRate * source.get_percent_pressure(element) * dt);
}

void TemperatureController::update(double dt)
{
	if (!is_running())
		return;
	destination.add_heat(energyRate * dt);
}

void TemperatureConductor::update(double dt)
{
	if (!is_running())
		return;
	destination.mix_temperatures_at(source, conductivity, dt);
}

void FilteredVolumePump::update(double dt)
{
	if (!is_running())
		return;
	for (auto &element : filter) {
		double amountPerVolume = source.get_moles(element) / source.volume;
		double amount = amountPerVolume * pumpRate * dt;
		source.remove(element, amount);
		destination.add_moles_temp(element, amount, source.get_temperature());
	}
}

void VolumePump::update(double dt)
{
	if (!is_running())
		return;
	source.move_gas_volume(destination, pumpRate * dt);
}

void FilteredMolarPump::update(double dt)
{
	if (!is_running())
		return;
	for (auto &element : filter) {
		double amount = std::min(source.get_moles(element), pumpRate * dt);
		source.remove(element, amount);
		destination.add_moles_temp(element, amount, source.get_temperature());
	}
}

void MolarPump::update(double dt)
{
	if (!is_running())
		return;
	source.move_gas_moles(destination, pumpRate * dt);
}

void VolumeMixer::update(double dt)
{
	if (!is_running())
		return;
	double amountA = pumpRate * dt * (1.0 - ratio);
	double amountB = pumpRate * dt * ratio;
	sourceA.move_gas_volume(destination, amountA);
	sourceB.move_gas_volume(destination, amountB);
}

void MolarMixer::update(double dt)
{
	if (!is_running())
		return;
	double amountA = pumpRate * dt * (1.0 - ratio);
	double amountB = pumpRate * dt * ratio;
	double cap = std::min(sourceA.get_moles(), sourceB.get_moles());
	if (cap == 0)
		return;
	if (amountA > cap) {
		amountA = cap;
		amountB = amountA / (1.0 - ratio) * ratio;
	}
	if (amountB > cap) {
		amountB = cap;
		amountA = amountB / ratio * (1.0 - ratio);
	}
	sourceA.move_gas_moles(destination, amountA);
	sourceB.move_gas_moles(destination, amountB);
}

bool MolarMixer::is_running()
{
	double temperatureDest = destination.get_temperature();
	double pressureDest = destination.get_pressure();
	double temperatureDiffA = sourceA.get_temperature() - temperatureDest;
	double pressureDiffA = sourceA.get_pressure() - pressureDest;
	double temperatureDiffB = sourceB.get_temperature() - temperatureDest;
	double pressureDiffB = sourceB.get_pressure() - pressureDest;
	return active
	    && (temperatureDest >= minTemperature && temperatureDest <= maxTemperature)
	    && (pressureDest >= minPressure && pressureDest <= maxPressure)
	    && (temperatureDiffA >= minTemperatureDifferentialA && temperatureDiffA <= maxTemperatureDifferentialA)
	    && (pressureDiffA >= minPressureDifferentialA && pressureDiffA <= maxPressureDifferentialA)
	    && (temperatureDiffB >= minTemperatureDifferentialB && temperatureDiffB <= maxTemperatureDifferentialB)
	    && (pressureDiffB >= minPressureDifferentialB && pressureDiffB <= maxPressureDifferentialB);
}



bool VolumeMixer::is_running()
{
	double temperatureDest = destination.get_temperature();
	double pressureDest = destination.get_pressure();
	double temperatureDiffA = sourceA.get_temperature() - temperatureDest;
	double pressureDiffA = sourceA.get_pressure() - pressureDest;
	double temperatureDiffB = sourceB.get_temperature() - temperatureDest;
	double pressureDiffB = sourceB.get_pressure() - pressureDest;
	return active
	    && (temperatureDest >= minTemperature && temperatureDest <= maxTemperature)
	    && (pressureDest >= minPressure && pressureDest <= maxPressure)
	    && (temperatureDiffA >= minTemperatureDifferentialA && temperatureDiffA <= maxTemperatureDifferentialA)
	    && (pressureDiffA >= minPressureDifferentialA && pressureDiffA <= maxPressureDifferentialA)
	    && (temperatureDiffB >= minTemperatureDifferentialB && temperatureDiffB <= maxTemperatureDifferentialB)
	    && (pressureDiffB >= minPressureDifferentialB && pressureDiffB <= maxPressureDifferentialB);
}
}
}
