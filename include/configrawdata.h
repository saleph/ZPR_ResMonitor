#ifndef INCLUDE_CONFIGRAWDATA_H_
#define INCLUDE_CONFIGRAWDATA_H_


/// Raw byte-structure describing trigger.
/// Used during parsing config file and
/// loading configuration into monitor.
struct TriggerRecordRaw {
	enum Resource 			{CPU, RAM, DISK};
	enum FluctuationType 	{UNDER, OVER};
	enum ResourceUnit		{PERCENT, MB, GB};

	/// Resource type
	Resource resource;
	/// Fluctuation type (under or over some value)
	FluctuationType fluctuationType;
	/// Resource unit - percent, MB or GB
	ResourceUnit unit;
	/// Log duration in seconds
	long duration;
	/// Log resolution in seconds
	long resolution;
};


/// Raw byte-structure describing logging type.
/// Used during parsing config file and
/// loading configuration into monitor.
struct LogRecordRaw {
	enum Resource	{CPU, RAM, DISK};

	/// Resource type
	Resource resource;
	/// Log duration in seconds
	long duration;
	/// Log resolution in seconds
	long resolution;
};

#endif /* INCLUDE_CONFIGRAWDATA_H_ */
