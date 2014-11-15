#ifndef MLOGLEVEL_H
#define MLOGLEVEL_H

namespace Mara
{
	namespace LogLevel
	{
		enum Level
		{
			Error = 1,
			AppStatus = 2,
			Memory = 4,
			Event = 8,
			Debug = 16
		};
	};
};

#endif
