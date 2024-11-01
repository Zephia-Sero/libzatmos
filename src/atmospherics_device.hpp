#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "atmosphere.hpp"
#include "atmospherics_element.hpp"
#include "atmospherics_mixture.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

struct GenericDevice {
	bool active = false;
	GenericDevice();
	virtual void update(double dt);
	virtual void toggle();
	virtual void set(bool active);
	virtual bool is_on();
	virtual bool is_running();
};

namespace ZAtmos::AtmosphericsDevices {
struct Device : public GenericDevice {
	Device();
	// Minimum pressure:(destination side) required for device to run.
	double minPressure = 0.0;
	// Maximum pressure (destination side) required for device to run.
	double maxPressure = 10132.50;
	// Minimum temperature required for device to run.
	double minTemperature = 0.0;
	// Maximum temperature required for device to run.
	double maxTemperature = 1000000.00;
};

struct Sink : public Device {
	Atmosphere &source;
	Sink(Atmosphere &source);
	virtual bool is_running() override;
};

struct Source : public Device {
	Atmosphere &destination;
	Source(Atmosphere &destination);
	virtual bool is_running() override;
};

struct BinaryDevice : public Device {
	Atmosphere &source, &destination;
	inline BinaryDevice(Atmosphere &source, Atmosphere &destination)
		: source(source), destination(destination)
	{}
	// Minimum pressure differential required for device to run.
	double minPressureDifferential = -10132.50;
	// Maximum pressure differential required for device to run.
	double maxPressureDifferential = 10132.50;
	// Minimum temperature differential required for device to run.
	double minTemperatureDifferential = -1000000.0;
	// Maximum temperature differential required for device to run.
	double maxTemperatureDifferential = 1000000.00;
	virtual bool is_running() override;
};

struct OneWayValve : public BinaryDevice {
	inline OneWayValve(Atmosphere &source, Atmosphere &destination)
		: BinaryDevice(source, destination)
	{}
	virtual void update(double dt) override;
};

struct Valve : public BinaryDevice {
	Valve(Atmosphere &source, Atmosphere &destination)
		: BinaryDevice(source, destination)
	{}
	virtual void update(double dt) override;
};

struct Spawner : public Source {
	AtmosphericsMixture mixture;
	double temperature;
	inline Spawner(Atmosphere &destination, AtmosphericsMixture mixture, double temperature)
		: Source(destination), mixture(mixture), temperature(temperature)
	{}
	virtual void update(double dt) override;
};

struct Void : public Sink {
	// moles / second
	double removalRate;
	// removalRate is in moles/second
	inline Void(Atmosphere &source, double removalRate)
		: Sink(source), removalRate(removalRate)
	{}
	virtual void update(double dt) override;
};

struct FilteredVoid : public Sink {
	std::vector<std::string> filter;
	// moles / second
	double removalRate;
	// removalRate is in moles/second
	inline FilteredVoid(Atmosphere &source, std::vector<std::string> filter, double removalRate)
		: Sink(source), filter(filter), removalRate(removalRate)
	{}
	virtual void update(double dt) override;
};

struct TemperatureController : public Source {
	double energyRate;
	// energyRate is in J/s of heat energy
	inline TemperatureController(Atmosphere &destination, double energyRate)
		: Source(destination), energyRate(energyRate)
	{}
	virtual void update(double dt) override;
};

struct TemperatureConductor : public BinaryDevice {
	// J / (s · K)
	double conductivity;
	// conductivity is in J / (s · K)
	inline TemperatureConductor(Atmosphere &source, Atmosphere &destination, double conductivity)
		: BinaryDevice(source, destination), conductivity(conductivity)
	{}
	virtual void update(double dt) override;
};

struct FilteredVolumePump : public BinaryDevice {
	std::vector<std::string> filter;
	// liters / second
	double pumpRate;
	// pumpRate is in liters / second
	inline FilteredVolumePump(Atmosphere &source, Atmosphere &destination, std::vector<std::string> filter, double pumpRate)
		: BinaryDevice(source, destination), filter(filter), pumpRate(pumpRate)
	{}
	virtual void update(double dt) override;
};

struct VolumePump : public BinaryDevice {
	// liters / second
	double pumpRate;
	// pumpRate is in liters / second
	inline VolumePump(Atmosphere &source, Atmosphere &destination, double pumpRate)
		: BinaryDevice(source, destination), pumpRate(pumpRate)
	{}

	virtual void update(double dt) override;
};

struct FilteredMolarPump : public BinaryDevice {
	std::vector<std::string> filter;
	// moles / second
	double pumpRate;
	// pumpRate is in moles / second
	inline FilteredMolarPump(Atmosphere &source, Atmosphere &destination, std::vector<std::string> filter, double pumpRate)
		: BinaryDevice(source, destination), filter(filter), pumpRate(pumpRate)
	{}
	virtual void update(double dt) override;
};

struct MolarPump : public BinaryDevice {
	// moles / second
	double pumpRate;
	// pumpRate is in moles / second
	inline MolarPump(Atmosphere &source, Atmosphere &destination, double pumpRate)
		: BinaryDevice(source, destination), pumpRate(pumpRate)
	{}

	virtual void update(double dt) override;
};


struct VolumeMixer : Device {
	Atmosphere &sourceA, &sourceB, &destination;
	// 0 is all sourceA, 1 is all sourceB
	double ratio;
	// measured in liters / second, scaled by ratio
	double pumpRate;

	// Between sourceA and destination
	// Minimum pressure differential required for device to run.
	double minPressureDifferentialA = -10132.50;
	// Maximum pressure differential required for device to run.
	double maxPressureDifferentialA = 10132.50;
	// Minimum temperature differential required for device to run.
	double minTemperatureDifferentialA = -1000000.0;
	// Maximum temperature differential required for device to run.
	double maxTemperatureDifferentialA = 1000000.00;

	// Between sourceB and destination
	// Minimum pressure differential required for device to run.
	double minPressureDifferentialB = -10132.50;
	// Maximum pressure differential required for device to run.
	double maxPressureDifferentialB = 10132.50;
	// Minimum temperature differential required for device to run.
	double minTemperatureDifferentialB = -1000000.0;
	// Maximum temperature differential required for device to run.
	double maxTemperatureDifferentialB = 1000000.00;

	// ratio is 0 (100% sourceA) to 1 (100% sourceB), pumpRate is measured in liters / second, scaled by ratio
	inline VolumeMixer(Atmosphere &sourceA, Atmosphere &sourceB, Atmosphere &destination, double ratio, double pumpRate)
		: sourceA(sourceA), sourceB(sourceB), destination(destination),
		  ratio(ratio), pumpRate(pumpRate)
	{}

	virtual void update(double dt) override;
	virtual bool is_running() override;
};

struct MolarMixer : Device {
	Atmosphere &sourceA, &sourceB, &destination;
	// 0 is all sourceA, 1 is all sourceB
	double ratio;
	// measured in moles / second, scaled by ratio
	double pumpRate;

	// Between sourceA and destination
	// Minimum pressure differential required for device to run.
	double minPressureDifferentialA = -10132.50;
	// Maximum pressure differential required for device to run.
	double maxPressureDifferentialA = 10132.50;
	// Minimum temperature differential required for device to run.
	double minTemperatureDifferentialA = -1000000.0;
	// Maximum temperature differential required for device to run.
	double maxTemperatureDifferentialA = 1000000.00;

	// Between sourceB and destination
	// Minimum pressure differential required for device to run.
	double minPressureDifferentialB = -10132.50;
	// Maximum pressure differential required for device to run.
	double maxPressureDifferentialB = 10132.50;
	// Minimum temperature differential required for device to run.
	double minTemperatureDifferentialB = -1000000.0;
	// Maximum temperature differential required for device to run.
	double maxTemperatureDifferentialB = 1000000.00;

	// ratio is 0 (100% sourceA) to 1 (100% sourceB), pumpRate is measured in moles / second, scaled by ratio
	inline MolarMixer(Atmosphere &sourceA, Atmosphere &sourceB, Atmosphere &destination, double ratio, double pumpRate)
		: sourceA(sourceA), sourceB(sourceB), destination(destination),
		  ratio(ratio), pumpRate(pumpRate)
	{}

	virtual void update(double dt) override;
	virtual bool is_running() override;
};

}

#endif
