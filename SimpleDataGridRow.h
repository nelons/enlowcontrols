#pragma once

#include <map>

namespace Enlow {
	namespace Controls {
		ref class SimpleDataGrid;
		ref class SimpleDataGridCell;
		ref class SimpleDataGridColumn;

		private enum class SimpleDataGridRowHighlightedStateEnum { Normal, Highlighted };
		private enum class SimpleDataGridRowSelectedStateEnum { Normal, Selected };

		[Windows::Foundation::Metadata::WebHostHidden]
		private ref class SimpleDataGridRow sealed {
		public:
			SimpleDataGridRow();

			property bool IsHighlighted { bool get() { return _highlighted; } }
			property bool IsSelected;

		internal:
			Platform::Object^ Source;
			SimpleDataGrid^ OwningGrid;
			std::map<SimpleDataGridColumn^,SimpleDataGridCell^> CellData;

			void HighlightRow();
			void ResetRowHighlight();

			void UpdateRowCellsBackground();

		private:
			bool _highlighted;
		};
	}
}