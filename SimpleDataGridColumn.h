#pragma once

#pragma warning(disable: 4453)

#include "SimpleDataGridRowSortHandler.h"

namespace Enlow {
	namespace Controls {
		ref class SimpleDataGrid;

		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class SimpleDataGridColumn sealed : public Windows::UI::Xaml::Controls::ContentControl {
		public:
			SimpleDataGridColumn();

			property Platform::String^ ColumnName;
			property Platform::String^ ColumnNameResourceKey;
			property Platform::String^ BindingProperty;
			property double ColumnDefaultWidth;
			property double ColumnMinWidth;
			property double ColumnMaxWidth;
			property bool IsVisible;

			property bool IsSortEnabled;
			property bool CanHideColumn;

			property Windows::UI::Xaml::Data::Binding^ DataBinding;
			property Windows::UI::Xaml::DataTemplate^ CellTemplate;

			property Windows::UI::Xaml::HorizontalAlignment ColumnTitleHorizontalAlignment;

			property Windows::UI::Xaml::Media::Brush^ ColumnTitleBackground;

			/*
				If set, this will save the width to Roaming/Local Settings.
			*/
			property Platform::String^ SettingsKey;

			event SimpleDataGridRowSortHandler^ OnColumnRowSortComparison;

			/*
				This is calculated and is ignored.
			*/
			property Windows::UI::Xaml::GridLength ColumnWidth;

		internal:
			unsigned int Position;
			bool IsRowNumberColumn;
			SimpleDataGridColumnSortStateEnum SortState;
			bool TappedEventsRegistered;

			Platform::String^ VisibleSaveKey;
			Platform::String^ WidthSaveKey;

			SimpleDataGrid^ OwningGrid;
			Windows::UI::Xaml::Controls::Grid^ ColumnsHeaderGrid;
			
			inline bool operator< (const SimpleDataGridColumn^ rhs) {
				return (this->Position < rhs->Position);
			}

			void RaiseColumnRowSortComparison(SimpleDataGridRowSortHandlerArgs^ args);

		protected:
			virtual void OnApplyTemplate() override;

		private:
			Windows::UI::Xaml::UIElement^ _divider;
			double _resizeLastWidth;

			void Divider_PointerCanceled(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
			void Divider_PointerCaptureLost(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
			void Divider_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
			void Divider_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
			void Divider_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
			void Divider_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
			void Divider_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		};
	}
}