#include "pch.h"
#include "SimpleDataGridColumn.h"

using namespace Enlow::Controls;

using namespace Windows::UI::Core;

using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Shapes;
using namespace Windows::Storage;

/*
DependencyProperty^ SimpleDataGridColumn::_dividerColour = DependencyProperty::Register(L"DividerColour",
	Windows::UI::Color::typeid,
	SimpleDataGridColumn::typeid,
	ref new PropertyMetadata(Windows::UI::Colors::Blue, ref new PropertyChangedCallback(OnDividerColourChanged)));
*/

SimpleDataGridColumn::SimpleDataGridColumn() {
	DefaultStyleKey = "Enlow.Controls.SimpleDataGridColumn";
	ColumnDefaultWidth = 0;
	IsRowNumberColumn = false;
	IsSortEnabled = true;
	SortState = SimpleDataGridColumnSortStateEnum::Unsorted;
	TappedEventsRegistered = false;
	ColumnTitleHorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Left;
	SettingsKey = "";
	CanHideColumn = true;
	WidthSaveKey = "";
	//DividerColour = Windows::UI::Colors::Blue;
	//WidthSaveRoaming = true;
}

void
SimpleDataGridColumn::RaiseColumnRowSortComparison(SimpleDataGridRowSortHandlerArgs^ args) {
	OnColumnRowSortComparison(OwningGrid, args);
}

void
SimpleDataGridColumn::OnApplyTemplate() {
	ContentControl::OnApplyTemplate();

	_divider = dynamic_cast<UIElement^>(GetTemplateChild("content"));
	if (_divider) {
		_divider->PointerCanceled += ref new PointerEventHandler(this, &SimpleDataGridColumn::Divider_PointerCanceled);
		_divider->PointerCaptureLost += ref new PointerEventHandler(this, &SimpleDataGridColumn::Divider_PointerCaptureLost);
		_divider->PointerEntered += ref new PointerEventHandler(this, &SimpleDataGridColumn::Divider_PointerEntered);
		_divider->PointerExited += ref new PointerEventHandler(this, &SimpleDataGridColumn::Divider_PointerExited);
		_divider->PointerMoved += ref new PointerEventHandler(this, &SimpleDataGridColumn::Divider_PointerMoved);
		_divider->PointerPressed += ref new PointerEventHandler(this, &SimpleDataGridColumn::Divider_PointerPressed);
		_divider->PointerReleased += ref new PointerEventHandler(this, &SimpleDataGridColumn::Divider_PointerReleased);
		
	}

	auto bdr = dynamic_cast<Border^>(GetTemplateChild("divider"));
	if (bdr)
		bdr->BorderBrush = OwningGrid->DividerBrush;

	auto sorticon = dynamic_cast<FontIcon^>(GetTemplateChild("SortIcon"));
	if (sorticon) {
		sorticon->Visibility = (IsSortEnabled) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;

	}
}

void 
SimpleDataGridColumn::Divider_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) {
	if (!OwningGrid) return;

	OwningGrid->ResizeStartPoint = e->GetCurrentPoint(ColumnsHeaderGrid);
	_divider->CapturePointer(e->Pointer);
	_resizeLastWidth = ActualWidth;
	e->Handled = true;
}

void
SimpleDataGridColumn::Divider_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) {
	_divider->ReleasePointerCapture(e->Pointer);
	_resizeLastWidth = ActualWidth;

	// save width
	if (!WidthSaveKey->IsEmpty()) {
		if (OwningGrid->SettingsSaveLocation == SettingsSaveLocationEnum::Roaming)
			ApplicationData::Current->RoamingSettings->Values->Insert(WidthSaveKey, ActualWidth);
		else if (OwningGrid->SettingsSaveLocation == SettingsSaveLocationEnum::Local)
			ApplicationData::Current->LocalSettings->Values->Insert(WidthSaveKey, ActualWidth);
	}

	e->Handled = true;
}

void
SimpleDataGridColumn::Divider_PointerCanceled(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) {
	if (OwningGrid)
		OwningGrid->RestorePointer();
}

void 
SimpleDataGridColumn::Divider_PointerCaptureLost(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) {
	if (OwningGrid)
		OwningGrid->RestorePointer();
}

void
SimpleDataGridColumn::Divider_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) {
	if (!OwningGrid) return;
	
	Window::Current->CoreWindow->PointerCursor = ref new CoreCursor(CoreCursorType::SizeWestEast, 2);

	if (OwningGrid->ResizeStartPoint == nullptr && e->Pointer->PointerDeviceType != Windows::Devices::Input::PointerDeviceType::Mouse) {
		PointerPoint^ point = e->GetCurrentPoint(ColumnsHeaderGrid);
		OwningGrid->ResizeStartPoint = point;
		_divider->CapturePointer(e->Pointer);
		_resizeLastWidth = ActualWidth;
		e->Handled = true;
	}
}

void 
SimpleDataGridColumn::Divider_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) {
	if (OwningGrid && OwningGrid->ResizeStartPoint == nullptr)
		OwningGrid->RestorePointer();
}

void 
SimpleDataGridColumn::Divider_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) {
	if (!OwningGrid) return;
	if (!OwningGrid->ResizeStartPoint) return;

	PointerPoint^ point = e->GetCurrentPoint(ColumnsHeaderGrid);
	if (e->Pointer->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Mouse && !point->Properties->IsLeftButtonPressed)
		return;

	double delta = point->Position.X - OwningGrid->ResizeStartPoint->Position.X;
	double newWidth = _resizeLastWidth + delta;

	// Clamp.
	if (ColumnMinWidth > 0.0 && newWidth < ColumnMinWidth) {
		if (newWidth == ColumnMinWidth) return;

		newWidth = ColumnMinWidth;
	} else if (ColumnMaxWidth > 0.0 && newWidth > ColumnMaxWidth) {
		if (newWidth == ColumnMaxWidth) return;

		newWidth = ColumnMaxWidth;
	}

	Width = newWidth;
	e->Handled = true;
}