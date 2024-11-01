#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "atmosphere.hpp"
#include <cstdio>
#include <cstdlib>

struct Device {
	virtual inline void update(double dt)
	{
		fprintf(stderr, "Called update on abstract device!");
		exit(1);
	}
};

struct AtmosphericsDevice : public Device {};

struct AtmosphericsPassiveVent : public AtmosphericsDevice {
	Atmosphere &source, &destination;
	AtmosphericsPassiveVent(Atmosphere &source, Atmosphere &destination);
	virtual void update(double dt) override;
};

struct AtmosphericsValve : public AtmosphericsDevice {
	Atmosphere &source, &destination;
	bool active = false;
	AtmosphericsValve(Atmosphere &source, Atmosphere &destination);
	void toggle();
	void set(bool active);
	virtual void update(double dt) override;
};


#endif
