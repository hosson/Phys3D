#pragma once

namespace ho
{
	class Float4
	{
	public:
		union uFloat4
		{
			float f[4];
			struct { float a, r, g, b; };
			struct { float r, g, b, a; };
		};
	};
}
