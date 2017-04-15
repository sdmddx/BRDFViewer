#pragma once

namespace Utilities
{
	class COMMON_API MessageProcessor abstract
	{
	public:
		virtual  void ProcessHSCROLL(WPARAM wParam, LPARAM lParam);

		virtual void ProcessCommand(WPARAM wParam, LPARAM lParam);

	};

}
