#pragma once

namespace Enlow {
	namespace Controls {
		ref class SimpleDataGrid;
		ref class SimpleDataGridColumn;

		public enum class SimpleDataGridColumnSortStateEnum { Unsorted, Ascending, Descending };

		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class SimpleDataGridRowSortHandlerArgs sealed {
		public:
			SimpleDataGridRowSortHandlerArgs() {
				lhs = nullptr;
				rhs = nullptr;
				Column = nullptr;
				SortDirection = SimpleDataGridColumnSortStateEnum::Unsorted;
				//Handled = false;
				LeftLessThan = false;
			}

			property Platform::Object^ lhs;
			property Platform::Object^ rhs;
			property SimpleDataGridColumn^ Column;
			property SimpleDataGridColumnSortStateEnum SortDirection;
			//property bool Handled;
			property bool LeftLessThan;
		};

		public delegate void SimpleDataGridRowSortHandler(SimpleDataGrid^ sender, SimpleDataGridRowSortHandlerArgs^ args);
	}
}