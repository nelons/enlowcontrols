//
// SimpleDataGrid.h
// Declaration of the SimpleDataGrid class.
//

#pragma once

#include "SimpleDataGridColumn.h"
#include "SimpleDataGridColumnCollection.h"
#include <mutex>

namespace Enlow
{
	namespace Controls
	{
		ref class SimpleDataGridCell;
		ref class SimpleDataGridRow;

		public enum class SettingsSaveLocationEnum { None, Local, Roaming };

		public delegate void SimpleGridRowClickedHandler(SimpleDataGrid^ sender, Platform::Object^ row);

		public enum class SimpleDataGridLinesVisibilityEnum { None, Horizontal, Vertical, Both };

		public enum class SimpleGridSelectionModeEnum { None, Single, Multiple };

		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class SimpleDataGridSelectionChangedArgs sealed {
		public:
			SimpleDataGridSelectionChangedArgs() {
				ItemAdded = nullptr;
				ItemRemoved = nullptr;
			}

			property Platform::Object^ ItemAdded;
			property Platform::Object^ ItemRemoved;
		};

		public delegate void SimpleDataGridSelectionChangedHandler(SimpleDataGrid^ sender, SimpleDataGridSelectionChangedArgs^ args);

		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class SimpleDataGrid sealed : public Windows::UI::Xaml::Controls::Control, Windows::UI::Xaml::Data::INotifyPropertyChanged
		{
		public:
			SimpleDataGrid();

			virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

			/*
				These are global handlers for all columns and only called if
				the type cannot automatically be determined or compared
				and the comparison is not handled at the column level.
			*/
			event SimpleDataGridRowSortHandler^ OnRowSortComparison;

			property Platform::String^ SettingsPrefix;
			property SettingsSaveLocationEnum SettingsSaveLocation;

			event SimpleGridRowClickedHandler^ OnRowClicked;

			property SimpleGridSelectionModeEnum SelectionMode;
			property Windows::Foundation::Collections::IVector<Platform::Object^>^ SelectedItems;
			event SimpleDataGridSelectionChangedHandler^ OnSelectionChanged;		

			property Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ ItemsSource {
				Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ get() {
					return (Windows::Foundation::Collections::IObservableVector<Platform::Object^>^)GetValue(ItemsSourceProperty);
				}
				void set(Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ iValue) {
					SetValue(ItemsSourceProperty, iValue);
				}
			}

			static property Windows::UI::Xaml::DependencyProperty^ ItemsSourceProperty {
				Windows::UI::Xaml::DependencyProperty^ get() {
					return _itemsSource;
				}
			}

			property SimpleDataGridColumnCollection^ ColumnDefinitions {
				SimpleDataGridColumnCollection^ get() {
					return (SimpleDataGridColumnCollection^)GetValue(ColumnDefinitionsProperty);
				}
				void set(SimpleDataGridColumnCollection^ iValue) {
					SetValue(ColumnDefinitionsProperty, iValue);
				}
			}

			static property Windows::UI::Xaml::DependencyProperty^ ColumnDefinitionsProperty {
				Windows::UI::Xaml::DependencyProperty^ get() {
					return _columnDefinitions;
				}
			}

			property Windows::UI::Xaml::Media::Brush^ AlternateRowBackground {
				Windows::UI::Xaml::Media::Brush^ get() {
					return (Windows::UI::Xaml::Media::Brush^)GetValue(AlternateRowBackgroundProperty);
				}
				void set(Windows::UI::Xaml::Media::Brush^ value) {
					SetValue(AlternateRowBackgroundProperty, value);
				}
			}
			static property Windows::UI::Xaml::DependencyProperty^ AlternateRowBackgroundProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _alternateRowBackground; } }

			property double BackgroundOpacity {
				double get() { return (double)GetValue(BackgroundOpacityProperty); }
				void set(double value) { SetValue(BackgroundOpacityProperty, value); }
			}

			static property Windows::UI::Xaml::DependencyProperty^ BackgroundOpacityProperty {
				Windows::UI::Xaml::DependencyProperty^ get() { return _backgroundOpacity; }
			}

			property double AlternateRowBackgroundOpacity {
				double get() {
					return (double)GetValue(AlternateRowBackgroundOpacityProperty);
				}
				void set(double value) {
					SetValue(AlternateRowBackgroundOpacityProperty, value);
				}
			}

			static property Windows::UI::Xaml::DependencyProperty^ AlternateRowBackgroundOpacityProperty {
				Windows::UI::Xaml::DependencyProperty^ get() {
					return _alternateRowBackgroundOpacity;
				}
			}

			property bool ShowRowNumbers { 
				bool get() { return (bool) GetValue(ShowRowNumbersProperty); }
				void set(bool value) { SetValue(ShowRowNumbersProperty, value); }
			}
			static property Windows::UI::Xaml::DependencyProperty^ ShowRowNumbersProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _showRowNumbers; } }

			property Windows::UI::Xaml::Media::Brush^ DividerBrush {
				Windows::UI::Xaml::Media::Brush^ get() { return (Windows::UI::Xaml::Media::Brush^)GetValue(DividerBrushProperty); }
				void set(Windows::UI::Xaml::Media::Brush^ value) { SetValue(DividerBrushProperty, value); }
			}
			static property Windows::UI::Xaml::DependencyProperty^ DividerBrushProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _dividerBrush; } }

			property SimpleDataGridLinesVisibilityEnum LinesVisibility {
				SimpleDataGridLinesVisibilityEnum get() { return (SimpleDataGridLinesVisibilityEnum)GetValue(LinesVisibilityProperty); }
				void set(SimpleDataGridLinesVisibilityEnum value) { SetValue(LinesVisibilityProperty, value); }
			}
			static property Windows::UI::Xaml::DependencyProperty^ LinesVisibilityProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _linesVisibility; } }

			property Platform::Object^ SelectedItem {
				Platform::Object^ get() { return (Platform::Object^)GetValue(SelectedItemProperty); }
				void set(Platform::Object^ value) { SetValue(SelectedItemProperty, value); }
			}
			static property Windows::UI::Xaml::DependencyProperty^ SelectedItemProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _selectedItem; } }

			property Windows::UI::Xaml::Media::Brush^ HighlightRowBackground {
				Windows::UI::Xaml::Media::Brush^ get() { return (Windows::UI::Xaml::Media::Brush^)GetValue(HighlightRowBackgroundProperty); }
				void set(Windows::UI::Xaml::Media::Brush^ value) { SetValue(HighlightRowBackgroundProperty, value); }
			}
			static property Windows::UI::Xaml::DependencyProperty^ HighlightRowBackgroundProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _highlightRowBackground; } }

			property Windows::UI::Xaml::Media::Brush^ SelectedRowBackground {
				Windows::UI::Xaml::Media::Brush^ get() { return (Windows::UI::Xaml::Media::Brush^)GetValue(SelectedRowBackgroundProperty); }
				void set(Windows::UI::Xaml::Media::Brush^ value) { SetValue(SelectedRowBackgroundProperty, value); }
			}
			static property Windows::UI::Xaml::DependencyProperty^ SelectedRowBackgroundProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _selectedRowBackground; } }


		internal:
			property Windows::UI::Core::CoreCursor^ ArrowCursor;
			property Windows::UI::Input::PointerPoint^ ResizeStartPoint;

			void RestorePointer();

			void RaiseRowClicked(SimpleDataGridRow^ row);

			void OnPropertyChanged(Platform::String^ propertyName) {
				PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
			}

			void OnPropertiesChanged(std::vector<wchar_t*> properties) {
				for_each(begin(properties), end(properties), [this](wchar_t* prop) {
					OnPropertyChanged(ref new Platform::String(prop));
				});
			}

		protected:
			virtual void OnApplyTemplate() override;

		private:
			void DrawControl();
			void DrawRows();
			std::mutex _drawControlLock;

			SimpleDataGridRow^ AddRow(Platform::Object^ src);

			Platform::Object^ GetCellData(Platform::Object^ item, SimpleDataGridColumn^ column);

			Windows::UI::Xaml::Controls::Grid^ _columnHeaderGrid;
			Windows::UI::Xaml::Controls::Grid^ _rowGrid;

			Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ _cellData;
			Windows::Foundation::Collections::IObservableVector<SimpleDataGridRow^>^ _rows;			

			static Windows::UI::Xaml::DependencyProperty^ _itemsSource;
			static void OnItemsSourceChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);
			void UpdateItemsSource();
			Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ _oldItemsSource;
			Windows::Foundation::EventRegistrationToken _itemsSourceChangedToken;

			Windows::Foundation::Collections::IObservableVector<SimpleDataGridColumn^>^ _columns;
			Windows::Foundation::EventRegistrationToken _columnDefinitionsChangedToken;
			void OnColumnsVectorChanged(Windows::Foundation::Collections::IObservableVector<Enlow::Controls::SimpleDataGridColumn^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ event);			
			static Windows::UI::Xaml::DependencyProperty^ _columnDefinitions;
			static void OnColumnDefinitionsChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

			void OnColumnHeadersRightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e);
			void OnColumnVisibilityClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void OnVectorChanged(Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ event);

			void ClearOtherColumnSortVisualStates(SimpleDataGridColumn^ sortedColumn);
			void OnSortAscendingClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void OnSortDescendingClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

			void SortGrid(SimpleDataGridColumn^ column, bool sortAscending);
			void PlaceItem(Platform::Object^ item, SimpleDataGridColumn^ column, bool sortAscending);

			void OnColumnHeaderTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);

			static Windows::UI::Xaml::DependencyProperty^ _alternateRowBackground;
			static void OnAlternateRowBackgroundChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

			static Windows::UI::Xaml::DependencyProperty^ _alternateRowBackgroundOpacity;
			static void OnAlternateRowBackgroundOpacityChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

			static Windows::UI::Xaml::DependencyProperty^ _backgroundOpacity;
			static void OnBackgroundOpacityChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

			static Windows::UI::Xaml::DependencyProperty^ _showRowNumbers;
			static void OnShowRowNumbersChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

			static Windows::UI::Xaml::DependencyProperty^ _dividerBrush;
			static void OnDividerBrushChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

			static Windows::UI::Xaml::DependencyProperty^ _linesVisibility;
			static void OnLinesVisibilityChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

			static Windows::UI::Xaml::DependencyProperty^ _selectedItem;
			static void OnSelectedItemChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

			static Windows::UI::Xaml::DependencyProperty^ _highlightRowBackground;

			static Windows::UI::Xaml::DependencyProperty^ _selectedRowBackground;

		};
	}
}
