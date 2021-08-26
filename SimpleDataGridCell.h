//
// SimpleDataGridCell.h
// Declaration of the SimpleDataGridCell class.
//

#pragma once

namespace Enlow {
	namespace Controls {
		ref class SimpleDataGrid;
		ref class SimpleDataGridRow;
		ref class SimpleDataGridColumn;

		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class SimpleDataGridCell sealed : public Windows::UI::Xaml::Controls::ContentControl
		{
		public:
			SimpleDataGridCell();

			property Windows::UI::Xaml::Media::Brush^ CellBackground;
			property double CellBackgroundOpacity;
			property bool IsSelected;

		protected:
			virtual void OnApplyTemplate() override;

		internal:			
			unsigned int Row;
			unsigned int Column;
			SimpleDataGridColumn^ ColumnDefinition;
			SimpleDataGridRow^ OwningRow;
			SimpleDataGrid^ OwningGrid;

			void UpdateCellBackground();

		private:	
			Windows::UI::Xaml::Shapes::Rectangle^ _background;
			Windows::UI::Xaml::Media::Brush^ _originalBrush;

			void OnPointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
			void OnPointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);

			void OnTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		};
	}
}
