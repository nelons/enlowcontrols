//
// SimpleDataGridCell.cpp
// Implementation of the SimpleDataGridCell class.
//

#include "pch.h"
#include "SimpleDataGridCell.h"

using namespace Enlow::Controls;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Shapes;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

// The Templated Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234235

SimpleDataGridCell::SimpleDataGridCell() {
	DefaultStyleKey = "Enlow.Controls.SimpleDataGridCell";
	Row = 0;
	Column = 0;
}

void 
SimpleDataGridCell::OnApplyTemplate() {
	ContentControl::OnApplyTemplate();

	_background = dynamic_cast<Rectangle^>(GetTemplateChild("background"));
	if (!_background) return;

	_background->Fill = CellBackground;
	_background->Opacity = CellBackgroundOpacity;

	_originalBrush = _background->Fill;

	auto content = dynamic_cast<ContentPresenter^>(GetTemplateChild("content"));
	if (!content) return;

	/*auto dc = dynamic_cast<Windows::UI::Xaml::Media::GradientStop^>(GetTemplateChild("dividercolour"));
	if (dc) {
		dc->Color = OwningGrid->DividerColour;
	}*/

	auto bdr = dynamic_cast<Border^>(GetTemplateChild("divider"));
	if (bdr)
		bdr->BorderBrush = OwningGrid->DividerBrush;

	PointerEntered += ref new PointerEventHandler(this, &SimpleDataGridCell::OnPointerEntered);
	PointerExited += ref new PointerEventHandler(this, &SimpleDataGridCell::OnPointerExited);

	Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &Enlow::Controls::SimpleDataGridCell::OnTapped);
}

void 
SimpleDataGridCell::UpdateCellBackground() {
	if (!_background) return;

	if (OwningRow->IsHighlighted) {
		if (_background->Fill != OwningGrid->HighlightRowBackground)
			_background->Fill = OwningGrid->HighlightRowBackground;
		return;
	}

	if (OwningRow->IsSelected) {
		if (_background->Fill != OwningGrid->SelectedRowBackground)
			_background->Fill = OwningGrid->SelectedRowBackground;
		return;
	}

	_background->Fill = _originalBrush;
}

void
SimpleDataGridCell::OnPointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) {
	if (OwningRow)
		OwningRow->HighlightRow();
}

void 
SimpleDataGridCell::OnPointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) {
	if (OwningRow)
		OwningRow->ResetRowHighlight();
}

void 
SimpleDataGridCell::OnTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e) {
	OwningGrid->RaiseRowClicked(OwningRow);
}
