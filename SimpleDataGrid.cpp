//
// SimpleDataGrid.cpp
// Implementation of the SimpleDataGrid class.
//

#include "pch.h"
#include "SimpleDataGrid.h"

using namespace Enlow::Controls;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

using namespace Windows::Storage;

// The Templated Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234235

DependencyProperty^ SimpleDataGrid::_itemsSource = DependencyProperty::Register(L"ItemsSource",
	IObservableVector<Platform::Object^>::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(OnItemsSourceChanged)));

DependencyProperty^ SimpleDataGrid::_columnDefinitions = DependencyProperty::Register(L"ColumnDefinitions",
	SimpleDataGridColumnCollection::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(ref new SimpleDataGridColumnCollection(), ref new PropertyChangedCallback(OnColumnDefinitionsChanged)));

DependencyProperty^ SimpleDataGrid::_alternateRowBackground = DependencyProperty::Register(L"AlternateRowBackground",
	Windows::UI::Xaml::Media::Brush::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Transparent), ref new PropertyChangedCallback(OnAlternateRowBackgroundChanged)));

DependencyProperty^ SimpleDataGrid::_alternateRowBackgroundOpacity = DependencyProperty::Register(L"AlternateRowBackgroundOpacity",
	double::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(1.0, ref new PropertyChangedCallback(OnAlternateRowBackgroundOpacityChanged)));

DependencyProperty^ SimpleDataGrid::_backgroundOpacity = DependencyProperty::Register(L"BackgroundOpacity",
	double::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(1.0, ref new PropertyChangedCallback(OnBackgroundOpacityChanged)));

DependencyProperty^ SimpleDataGrid::_showRowNumbers = DependencyProperty::Register(L"ShowRowNumbers",
	bool::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(false, ref new PropertyChangedCallback(OnShowRowNumbersChanged)));

DependencyProperty^ SimpleDataGrid::_dividerBrush = DependencyProperty::Register(L"DividerBrush",
	Windows::UI::Xaml::Media::Brush::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Blue), ref new PropertyChangedCallback(OnDividerBrushChanged)));

DependencyProperty^ SimpleDataGrid::_linesVisibility = DependencyProperty::Register(L"LinesVisibility",
	SimpleDataGridLinesVisibilityEnum::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(SimpleDataGridLinesVisibilityEnum::Vertical, ref new PropertyChangedCallback(OnLinesVisibilityChanged)));

DependencyProperty^ SimpleDataGrid::_selectedItem = DependencyProperty::Register(L"SelectedItem",
	Platform::Object::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(OnSelectedItemChanged)));

DependencyProperty^ SimpleDataGrid::_highlightRowBackground = DependencyProperty::Register(L"HighlightRowBackground",
	Windows::UI::Xaml::Media::Brush::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(ref new SolidColorBrush(Windows::UI::Colors::Green)));

DependencyProperty^ SimpleDataGrid::_selectedRowBackground = DependencyProperty::Register(L"SelectedRowBackground",
	Windows::UI::Xaml::Media::Brush::typeid,
	SimpleDataGrid::typeid,
	ref new PropertyMetadata(ref new SolidColorBrush(Windows::UI::Colors::Red)));

SimpleDataGrid::SimpleDataGrid()
{
	DefaultStyleKey = "Enlow.Controls.SimpleDataGrid";

	Background = ref new SolidColorBrush(Windows::UI::Colors::Transparent);

	auto altresource = Application::Current->Resources->Lookup("SystemBaseLowColor");
	if (altresource) {
		auto altcolor = dynamic_cast<IBox<Windows::UI::Color>^>(altresource);
		if (altcolor) {
			HighlightRowBackground = ref new SolidColorBrush(altcolor->Value);
		}
	}

	auto selresource = Application::Current->Resources->Lookup("SystemAccentColor");
	if (selresource) {
		auto selcolor = dynamic_cast<IBox<Windows::UI::Color>^>(selresource);
		if (selcolor) {
			SelectedRowBackground = ref new SolidColorBrush(selcolor->Value);
		}
	}

	ResizeStartPoint = nullptr;
	_oldItemsSource = nullptr;
	_cellData = ref new Platform::Collections::Vector<Platform::Object^>();
	_rows = ref new Platform::Collections::Vector<SimpleDataGridRow^>();
	_columns = ref new Platform::Collections::Vector<SimpleDataGridColumn^>();

	// Watch for changes on the column definitions default static value
	_columnDefinitionsChangedToken = ColumnDefinitions->VectorChanged::add(ref new Windows::Foundation::Collections::VectorChangedEventHandler<Enlow::Controls::SimpleDataGridColumn^>(this, &Enlow::Controls::SimpleDataGrid::OnColumnsVectorChanged));
	
	// TODO: Convert these to dependency properties.
	SettingsPrefix = "";
	SettingsSaveLocation = SettingsSaveLocationEnum::Roaming;

	SelectionMode = SimpleGridSelectionModeEnum::None;
	SelectedItem = nullptr;
	SelectedItems = ref new Platform::Collections::Vector<Platform::Object^>();
}

void 
SimpleDataGrid::RestorePointer() {
	if (ArrowCursor != nullptr)
		Window::Current->CoreWindow->PointerCursor = ArrowCursor;

	ResizeStartPoint = nullptr;
}

void
SimpleDataGrid::OnApplyTemplate() {
	ArrowCursor = Window::Current->CoreWindow->PointerCursor;

	_columnHeaderGrid = dynamic_cast<Grid^>(GetTemplateChild("gridColumnHeaders"));
	_rowGrid = dynamic_cast<Grid^>(GetTemplateChild("gridRows"));

	/*auto dc = dynamic_cast<Windows::UI::Xaml::Media::GradientStop^>(GetTemplateChild("dividercolour"));
	if (dc) {
		dc->Color = DividerColour;
	}*/

	auto bdr = dynamic_cast<Border^>(GetTemplateChild("divider"));
	if (bdr)
		bdr->BorderBrush = DividerBrush;

#ifdef _DEBUG
	{
		wchar_t fmt[512];
		swprintf_s(fmt, 512, L"Object %p, Grid %s, OnApplyTemplate() called.\n", this, Name->Data());
		OutputDebugStringW(fmt);

		swprintf_s(fmt, 512, L"-> _columnHeaderGrid is at %p.\n", _columnHeaderGrid);
		OutputDebugStringW(fmt);

		swprintf_s(fmt, 512, L"-> _rowGrid is at %p.\n", _rowGrid);
		OutputDebugStringW(fmt);
	}
#endif

	DrawControl();
}

void 
SimpleDataGrid::RaiseRowClicked(SimpleDataGridRow^ row) {
	OnRowClicked(this, row->Source);

	switch (SelectionMode) {	
	case SimpleGridSelectionModeEnum::Single:
		{
			if (SelectedItem != nullptr && SelectedItem != row) {
				// de-select the row.
				for (SimpleDataGridRow^ current_row : _rows) {
					if (current_row->Source == SelectedItem) {
						/* Do we raise a selected changed for this row ? */
						auto args = ref new SimpleDataGridSelectionChangedArgs();
						args->ItemRemoved = current_row->Source;
						OnSelectionChanged(this, args);

						current_row->IsSelected = false;
						current_row->UpdateRowCellsBackground();
						break;
					}
				}
			}

			auto args = ref new SimpleDataGridSelectionChangedArgs();
			if (row->IsSelected) {
				SelectedItem = nullptr;
				args->ItemRemoved = row->Source;

			} else {
				SelectedItem = row->Source;
				args->ItemAdded = row->Source;

			}

			OnSelectionChanged(this, args);
			row->IsSelected = !row->IsSelected;
			row->UpdateRowCellsBackground();
		}
		break;

	case SimpleGridSelectionModeEnum::Multiple:
		{
			if (row->IsSelected) {
				// remove
				SelectedItem = nullptr;
				unsigned int index = 0;
				if (SelectedItems->IndexOf(row->Source, &index)) {
					SelectedItems->RemoveAt(index);
					row->IsSelected = false;

					auto args = ref new SimpleDataGridSelectionChangedArgs();
					args->ItemRemoved = row->Source;
					OnSelectionChanged(this, args);
				}
			}
			else {
				SelectedItem = row->Source;
				SelectedItems->Append(row->Source);
				row->IsSelected = true;

				auto args = ref new SimpleDataGridSelectionChangedArgs();
				args->ItemAdded = row->Source;
				OnSelectionChanged(this, args);
			}

			row->UpdateRowCellsBackground();
		}
		break;

	default:
		break;
	}

	OnPropertiesChanged({ L"SelectedItem", L"SelectedItems"});
}

void
SimpleDataGrid :: DrawControl() {
	std::lock_guard<std::mutex> lock(_drawControlLock);

	//if (!_columns || _columns->Size == 0) return;
	if (!_columnHeaderGrid) { /*OutputDebugStringA("ERROR: No columnHeaderGrid set.\n");*/ return; }
	if (!_rowGrid) { /*OutputDebugStringA("ERROR: No rowGrid set.\n");*/ return; }

#ifdef _DEBUG
	{
		wchar_t fmt[512];
		swprintf_s(fmt, 512, L"Object %p, Grid %s, DrawControl() called.\n", this, Name->Data());
		OutputDebugStringW(fmt);
	}
#endif

	{
		const size_t fmt_size = 512;
		wchar_t fmt[fmt_size];
		swprintf_s(fmt, fmt_size, L"ColumnHeaderGrid has %d children.\n", _columnHeaderGrid->Children->Size);
		OutputDebugStringW(fmt);
	}

	_columnHeaderGrid->Children->Clear();
	//_rowGrid->Children->Clear();
	//_rows->Clear();

	/*
		The next commented block is putting in row numbers.
		Though we can't change the columns vector property as
		that is also used internally.

		to do this, need to create a new collection for column defs internally
		and populate that every draw control.
	*/

	/*bool have_rownumber_col = false;
	for (auto def : ColumnDefinitions) {
		if (def->IsRowNumberColumn == true) {
			have_rownumber_col = true;
			break;
		}
	}

	if (!have_rownumber_col) {
		SimpleDataGridColumn^ rowNumberCol = ref new SimpleDataGridColumn();
		rowNumberCol->ColumnName = "Row";
		rowNumberCol->IsSortEnabled = false;
		rowNumberCol->ColumnDefaultWidth = 100;
		rowNumberCol->ColumnMinWidth = 50;
		rowNumberCol->ColumnMaxWidth = 150;
		rowNumberCol->IsRowNumberColumn = true;
		rowNumberCol->Visibility = (!ShowRowNumbers) ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
		ColumnDefinitions->InsertAt(0, rowNumberCol);
	}*/

	// Create the resource 
	auto resourceLoader = Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse();

	unsigned int CurrentPosition = 0;
	for (SimpleDataGridColumn^ def : ColumnDefinitions) {
		def->OwningGrid = this;
		def->ColumnsHeaderGrid = _columnHeaderGrid;
		def->Position = CurrentPosition;

		// Use ResourceKey
		if (!def->ColumnNameResourceKey->IsEmpty()) {
			def->Content = resourceLoader->GetString(def->ColumnNameResourceKey);

		}
		else if (!def->ColumnName->IsEmpty()) {
			def->Content = def->ColumnName;

		}
		else {
			const size_t fmt_size = 512;
			wchar_t fmt[fmt_size];
			swprintf_s(fmt, fmt_size, L"Column %d", CurrentPosition);
			def->Content = ref new String(fmt);

		}

#ifdef _DEBUG
		{
			wchar_t fmt[512];
			swprintf_s(fmt, 512, L"Object %p, Iterating on column definition %p (Name: %s).\n", this, def, def->Content->ToString()->Data());
			OutputDebugStringW(fmt);
		}
#endif

		std::wstring col_prefix = L"";
		if (!Name->IsEmpty())
			col_prefix += Name->Data();

		if (!SettingsPrefix->IsEmpty()) {
			if (col_prefix.size() > 0)
				col_prefix += L"_";

			col_prefix += SettingsPrefix->Data();
		}

		if (col_prefix.size() > 0)
			col_prefix += L"_";

		col_prefix += L"column_";
		const size_t colno_size = 32;
		wchar_t colno[colno_size];
		_itow_s(CurrentPosition, colno, colno_size, 10);
		col_prefix += colno;

		// retrieve whether the column is visible
		std::wstring visible_save = col_prefix;
		visible_save += L"_visible";
		def->VisibleSaveKey = ref new String(visible_save.c_str());

		if (SettingsSaveLocation == SettingsSaveLocationEnum::Roaming) {
			if (ApplicationData::Current->RoamingSettings->Values->HasKey(def->VisibleSaveKey)) {
				auto obj = ApplicationData::Current->RoamingSettings->Values->Lookup(def->VisibleSaveKey);
				auto visibility = dynamic_cast<IBox<bool>^>(obj);
				if (visibility) {
					def->Visibility = (visibility->Value) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
				}
			}

		}
		else if (SettingsSaveLocation == SettingsSaveLocationEnum::Local) {
			if (ApplicationData::Current->LocalSettings->Values->HasKey(def->VisibleSaveKey)) {
				auto obj = ApplicationData::Current->LocalSettings->Values->Lookup(def->VisibleSaveKey);
				auto visibility = dynamic_cast<IBox<bool>^>(obj);
				if (visibility) {
					def->Visibility = (visibility->Value) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
				}
			}
		}

#ifdef _DEBUG
		if (def->Visibility == Windows::UI::Xaml::Visibility::Collapsed) {
			const size_t fmt_size = 512;
			wchar_t fmt[fmt_size];
			swprintf_s(fmt, fmt_size, L"-> column %s is hidden.\n", def->Content->ToString()->Data());
			OutputDebugStringW(fmt);
		}
#endif

		if (!def->TappedEventsRegistered) {
			def->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &Enlow::Controls::SimpleDataGrid::OnColumnHeaderTapped);
			def->RightTapped += ref new Windows::UI::Xaml::Input::RightTappedEventHandler(this, &Enlow::Controls::SimpleDataGrid::OnColumnHeadersRightTapped);
			def->TappedEventsRegistered = true;
		}

		// see if we can get the previous width.
		bool loaded_width = false;
		if (SettingsSaveLocation != SettingsSaveLocationEnum::None) {
			// Set Column Name
			std::wstring width_save = col_prefix;
			width_save += L"_width";
			def->WidthSaveKey = ref new String(width_save.c_str());

			// Get previous value			
			if (SettingsSaveLocation == SettingsSaveLocationEnum::Roaming) {
				if (ApplicationData::Current->RoamingSettings->Values->HasKey(def->WidthSaveKey)) {
					auto column_width = ApplicationData::Current->RoamingSettings->Values->Lookup(def->WidthSaveKey);
					auto actualwidth = dynamic_cast<IBox<double>^>(column_width);
					if (actualwidth) {
						def->Width = actualwidth->Value;
						loaded_width = true;
					}
				}
			}
			else {
				if (ApplicationData::Current->LocalSettings->Values->HasKey(def->WidthSaveKey)) {
					auto column_width = ApplicationData::Current->RoamingSettings->Values->Lookup(def->WidthSaveKey);
					auto actualwidth = dynamic_cast<IBox<double>^>(column_width);
					if (actualwidth) {
						def->Width = actualwidth->Value;
						loaded_width = true;
					}
				}
			}
		}
			
		if (loaded_width == false) {
			if (def->ColumnDefaultWidth != 0.0)
				def->Width = def->ColumnDefaultWidth;
			else
				def->Width = def->ColumnMinWidth;

		}

		if (def->Width == 0.0) {
			// Measure the width of the column title.
			Windows::Foundation::Size available = Windows::Foundation::Size(1000.0f, 1000.0f);
			def->Measure(available);

			def->Width = def->DesiredSize.Width;
		}

		// Create the column in the column headers
		auto col = ref new ColumnDefinition();
		col->Width = GridLength(0.0f, GridUnitType::Auto);
		_columnHeaderGrid->ColumnDefinitions->Append(col);

		// Create the column in the rows
		col = ref new ColumnDefinition();
		col->Width = GridLength(0.0f, GridUnitType::Auto);
		_rowGrid->ColumnDefinitions->Append(col);

		_columnHeaderGrid->SetRow(def, 0);
		_columnHeaderGrid->SetColumn(def, def->Position);


		/* See if def is currently a member of _columnHeaderGrid->Children */
		unsigned int index = 0;
		if (!_columnHeaderGrid->Children->IndexOf(def, &index)) {
			if (def->Parent) {
#ifdef _DEBUG
				wchar_t msg[512];
				swprintf_s(msg, 512, L"Object %p, Grid: %s, Column %s definition has parent (%p).\n", this, Name->Data(), def->Content->ToString()->Data(), def->Parent);
				OutputDebugStringW(msg);
#endif
			}

			if (def->Parent && def->Parent != _columnHeaderGrid) {
				// try and remove ?
				Panel^ parent_ui = dynamic_cast<Panel^>(def->Parent);
				if (parent_ui) {
					unsigned int parent_index = 0;
					if (parent_ui->Children->IndexOf(def, &parent_index))
						parent_ui->Children->RemoveAt(parent_index);
				}
			}
			else if (!def->Parent) {
				try {
					_columnHeaderGrid->Children->Append(def);

#ifdef _DEBUG
					{
						wchar_t fmt[512];
						swprintf_s(fmt, 512, L"Adding column %p (Name: %s) as a child of %p.\n", def, def->Content->ToString()->Data(), _columnHeaderGrid);
						OutputDebugStringW(fmt);
					}
#endif
				}
				catch (Exception^) {
					OutputDebugStringW(L"Failed to add column definition to column header grid.\n");

				}

			}
			else {
				OutputDebugStringW(L"column def has parent and is columnheadergrid.\n");

			}
		}

		++CurrentPosition;
	}

	DrawRows();

#ifdef _DEBUG
	{
		wchar_t fmt[512];
		swprintf_s(fmt, 512, L"Object %p, Grid %s, DrawControl() exited.\n", this, Name->Data());
		OutputDebugStringW(fmt);
	}
#endif
}

void 
SimpleDataGrid::DrawRows() {
	if (!_rowGrid || !_rows)
		return;

	_rowGrid->Children->Clear();
	_rows->Clear();

	if (ItemsSource == nullptr || ItemsSource->Size == 0)
		return;

	for (auto src : _cellData)
		AddRow(src);
}

SimpleDataGridRow^ 
SimpleDataGrid :: AddRow(Platform::Object^ src) {
	unsigned int CurrentRow = _rows->Size;

	// Create the Row Definition
	auto row = ref new RowDefinition();
	row->Height = Windows::UI::Xaml::GridLength(0.0, GridUnitType::Auto);
	_rowGrid->RowDefinitions->Append(row);

	SimpleDataGridRow^ rowdata = ref new SimpleDataGridRow();
	rowdata->Source = src;
	_rows->Append(rowdata);

	for (SimpleDataGridColumn^ def : ColumnDefinitions) {
		SimpleDataGridCell^ cell = ref new SimpleDataGridCell();
		cell->OwningGrid = this;
		cell->OwningRow = rowdata;
		cell->Content = src;

		cell->Row = CurrentRow;
		cell->Column = def->Position;
		cell->ColumnDefinition = def;
		cell->HorizontalContentAlignment = def->HorizontalContentAlignment;
		cell->VerticalContentAlignment = def->VerticalContentAlignment;

		if ((CurrentRow + 1) % 2 == 0) {
			cell->CellBackground = AlternateRowBackground;

			// apply opacity
			cell->CellBackgroundOpacity = AlternateRowBackgroundOpacity;
		}
		else {
			cell->CellBackground = Background;

			// apply opacity
			cell->CellBackgroundOpacity = BackgroundOpacity;
		}

		if (def->CellTemplate)
			cell->ContentTemplate = def->CellTemplate;

		if (def->IsRowNumberColumn) {
			cell->HorizontalContentAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
			cell->Content = CurrentRow + 1;

		}
		else if (def->DataBinding) {
			Binding^ binding = ref new Binding();
			binding->Mode = def->DataBinding->Mode;
			if (def->DataBinding->Path)
				binding->Path = def->DataBinding->Path;

			if (def->DataBinding->Converter)
				binding->Converter = def->DataBinding->Converter;

			if (def->DataBinding->ElementName)
				binding->ElementName = def->DataBinding->ElementName;

			binding->Source = src;
			cell->SetBinding(cell->ContentProperty, binding);

		}
		else if (!def->BindingProperty->IsEmpty()) {
			Binding^ binding = ref new Binding();
			binding->Path = ref new PropertyPath(def->BindingProperty);
			binding->Mode = BindingMode::OneWay;
			binding->Source = src;
			cell->SetBinding(cell->ContentProperty, binding);

		}

		Binding^ widthBinding = ref new Binding();
		widthBinding->Path = ref new PropertyPath(L"Width");
		widthBinding->Mode = BindingMode::OneWay;
		widthBinding->Source = def;
		cell->SetBinding(cell->WidthProperty, widthBinding);

		Binding^ visibleBinding = ref new Binding();
		visibleBinding->Path = ref new PropertyPath(L"Visibility");
		visibleBinding->Mode = BindingMode::OneWay;
		visibleBinding->Source = def;
		cell->SetBinding(cell->VisibilityProperty, visibleBinding);

		_rowGrid->SetRow(cell, CurrentRow);
		_rowGrid->SetColumn(cell, def->Position);
		_rowGrid->Children->Append(cell);

		rowdata->CellData[def] = cell;
	}

	return rowdata;
}

Platform::Object^ 
SimpleDataGrid::GetCellData(Platform::Object^ item, SimpleDataGridColumn^ column) {
	// Need to get the content for the item/column binding def.
	for (auto row : _rows) {
		if (row->Source == item) {
			if (row->CellData.find(column) != row->CellData.end())
				return row->CellData[column]->Content;
			else
				return nullptr;
		}
	}

	return nullptr;
}

void
SimpleDataGrid::OnItemsSourceChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	auto datagrid = dynamic_cast<SimpleDataGrid^>(d);
	if (!datagrid)
		return;

	datagrid->UpdateItemsSource();
}

void 
SimpleDataGrid::UpdateItemsSource() {
	if (_oldItemsSource) {
		_oldItemsSource->VectorChanged::remove(_itemsSourceChangedToken);
		_oldItemsSource = nullptr;
	}

	// Update cell data.
	_cellData->Clear();
	for (auto item : ItemsSource)
		_cellData->Append(item);

	ItemsSource->VectorChanged::add(ref new VectorChangedEventHandler<Platform::Object^>(this, &SimpleDataGrid::OnVectorChanged));
	_oldItemsSource = ItemsSource;

	// Redraw the control.
	//DrawControl();
	DrawRows();
}


void
SimpleDataGrid::OnColumnDefinitionsChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	/*auto datagrid = dynamic_cast<SimpleDataGrid^>(d);
	if (!datagrid)
		return;

	// Unsubscribe from the previous collection.
	auto old_cols = dynamic_cast<Windows::Foundation::Collections::IObservableVector<SimpleDataGridColumn^>^>(e->OldValue);
	old_cols->VectorChanged::remove(datagrid->_columnDefinitionsChangedToken);

	auto cols = dynamic_cast<Windows::Foundation::Collections::IObservableVector<SimpleDataGridColumn^>^>(e->NewValue);
	if (cols) {
		{
			wchar_t fmt[512];
			swprintf_s(fmt, 512, L"Object %p, updating columns value.\n", datagrid);
			OutputDebugStringW(fmt);
		}

		// Subscribe to changes on this collection.
		datagrid->_columnDefinitionsChangedToken = cols->VectorChanged::add(ref new Windows::Foundation::Collections::VectorChangedEventHandler<Enlow::Controls::SimpleDataGridColumn^>(datagrid, &Enlow::Controls::SimpleDataGrid::OnColumnsVectorChanged));

		datagrid->_columns = cols;
		datagrid->DrawControl();
	}
	else {
		wchar_t fmt[512];
		swprintf_s(fmt, 512, L"Object %p, new column definition type is %s.\n", datagrid, e->NewValue->GetType()->FullName->Data());
		OutputDebugStringW(fmt);
	}*/
}

void 
SimpleDataGrid::OnColumnHeadersRightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e) {
	auto column = dynamic_cast<SimpleDataGridColumn^>(sender);
	if (!column) return;
	
	MenuFlyout^ flyout = ref new MenuFlyout();

	auto showsubs = ref new MenuFlyoutSubItem();
	showsubs->Text = "Show/Hide Columns";

	for (auto current : ColumnDefinitions) {
		ToggleMenuFlyoutItem^ item = ref new ToggleMenuFlyoutItem();
		item->Text = current->Content->ToString();
		item->Tag = current;
		item->IsEnabled = current->CanHideColumn;
		item->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Enlow::Controls::SimpleDataGrid::OnColumnVisibilityClick);
		item->IsChecked = (current->Visibility == Windows::UI::Xaml::Visibility::Visible);
		showsubs->Items->Append(item);
	}
	flyout->Items->Append(showsubs);

	if (column->IsSortEnabled) {
		auto sep = ref new MenuFlyoutSeparator();
		flyout->Items->Append(sep);

		auto sort = ref new ToggleMenuFlyoutItem();
		sort->Tag = sender;
		sort->IsChecked = (column->SortState == SimpleDataGridColumnSortStateEnum::Ascending);
		sort->Text = "Sort Ascending";
		sort->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Enlow::Controls::SimpleDataGrid::OnSortAscendingClick);
		flyout->Items->Append(sort);

		sort = ref new ToggleMenuFlyoutItem();
		sort->Tag = sender;
		sort->IsChecked = (column->SortState == SimpleDataGridColumnSortStateEnum::Descending);
		sort->Text = "Sort Descending";
		sort->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Enlow::Controls::SimpleDataGrid::OnSortDescendingClick);
		flyout->Items->Append(sort);
	}

	UIElement^ element = dynamic_cast<UIElement^>(sender);
	if (element) {
		auto point = e->GetPosition(element);
		flyout->ShowAt(element, point);
	}
}

void SimpleDataGrid::OnColumnVisibilityClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	MenuFlyoutItem^ source = dynamic_cast<MenuFlyoutItem^>(sender);
	if (!source) return;
	if (!source->Tag) return;

	SimpleDataGridColumn^ def = dynamic_cast<SimpleDataGridColumn^>(source->Tag);
	if (!def) return;

	if (def->Visibility == Windows::UI::Xaml::Visibility::Visible) {
		// Make sure that we're not hiding the last column.
		unsigned int visible_count = 0;
		for (auto item : ColumnDefinitions) {
			if (item->Visibility == Windows::UI::Xaml::Visibility::Visible)
				++visible_count;
		}

		if (visible_count > 1)
			def->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

	} else
		def->Visibility = Windows::UI::Xaml::Visibility::Visible;

	if (SettingsSaveLocation == SettingsSaveLocationEnum::Roaming) {
		ApplicationData::Current->RoamingSettings->Values->Insert(def->VisibleSaveKey, (def->Visibility == Windows::UI::Xaml::Visibility::Visible) ? true : false);

	}
	else if (SettingsSaveLocation == SettingsSaveLocationEnum::Local) {
		ApplicationData::Current->LocalSettings->Values->Insert(def->VisibleSaveKey, (def->Visibility == Windows::UI::Xaml::Visibility::Visible) ? true : false);

	}

	DrawControl();
}

void
SimpleDataGrid::OnVectorChanged(Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ eventArgs) {
	// Update the internal collection ....
	if (eventArgs->CollectionChange == CollectionChange::ItemInserted) {
		SimpleDataGridColumn^ sorted_by = nullptr;
		for (auto def : ColumnDefinitions) {
			if (def->SortState != SimpleDataGridColumnSortStateEnum::Unsorted) {
				sorted_by = def;
				break;
			}
		}

		if (sorted_by) {
			// Find the right place to insert according to how the grid is sorted.
			auto item = ItemsSource->GetAt(eventArgs->Index);
			AddRow(item);
			PlaceItem(item, sorted_by, (sorted_by->SortState == SimpleDataGridColumnSortStateEnum::Ascending) ? true : false);

		} else {
			// Add it to where it's been added in the source collection.
			_cellData->InsertAt(eventArgs->Index, ItemsSource->GetAt(eventArgs->Index));

		}
	}
	else if (eventArgs->CollectionChange == CollectionChange::ItemRemoved) {
		_cellData->RemoveAt(eventArgs->Index);

	}

	DrawControl();
}

void 
SimpleDataGrid::ClearOtherColumnSortVisualStates(SimpleDataGridColumn^ sortedColumn) {
	for (auto column : ColumnDefinitions) {
		if (column == sortedColumn) continue;

		column->SortState = SimpleDataGridColumnSortStateEnum::Unsorted;
		VisualStateManager::GoToState(column, "Unsorted", true);
	}
}

void
SimpleDataGrid::OnSortAscendingClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	SimpleDataGridColumn^ column = nullptr;

	MenuFlyoutItem^ menuitem = dynamic_cast<MenuFlyoutItem^>(sender);
	if (menuitem) {
		if (!menuitem->Tag) return;
		column = dynamic_cast<SimpleDataGridColumn^>(menuitem->Tag);

	} else {
		column = dynamic_cast<SimpleDataGridColumn^>(sender);

	}

	if (!column) return;

	SortGrid(column, true);
}

void
SimpleDataGrid :: SortGrid(SimpleDataGridColumn^ column, bool sortAscending) {
	OutputDebugStringW(L"SortGrid() called.\n");

	if (!column->IsSortEnabled) return;
	if (!ItemsSource) return;

	_cellData->Clear();

	for (unsigned int row = 0; row < ItemsSource->Size; ++row) {
		Platform::Object^ item = ItemsSource->GetAt(row);

		PlaceItem(item, column, sortAscending);
	}

	ClearOtherColumnSortVisualStates(column);
	column->SortState = (sortAscending) ? SimpleDataGridColumnSortStateEnum::Ascending : SimpleDataGridColumnSortStateEnum::Descending;
	VisualStateManager::GoToState(column, (sortAscending) ? "SortAscending" : "SortDescending", true);

	DrawControl();
}

void
SimpleDataGrid :: PlaceItem(Platform::Object^ item, SimpleDataGridColumn^ column, bool sortAscending) {
	auto item_data = GetCellData(item, column);
	if (!item_data) {
		_cellData->InsertAt(_cellData->Size, item);
		return;
	}

	bool inserted = false;
	auto item_typecode = Platform::Type::GetTypeCode(item_data->GetType());
	switch (item_typecode) {
	case TypeCode::Object:
		{
			for (unsigned int k = 0; k < _cellData->Size; ++k) {
				auto current = _cellData->GetAt(k);
				
				SimpleDataGridRowSortHandlerArgs^ args = ref new SimpleDataGridRowSortHandlerArgs();
				args->lhs = item;
				args->rhs = current;
				args->Column = column;
				args->LeftLessThan = false;
				args->SortDirection = (sortAscending) ? SimpleDataGridColumnSortStateEnum::Ascending : SimpleDataGridColumnSortStateEnum::Descending;
				
				/* Try column compare first. */
				column->RaiseColumnRowSortComparison(args);

				/* Try global compare */
				if (!args->LeftLessThan) {
					OnRowSortComparison(this, args);
				}

				/* Insert */
				if (args->LeftLessThan) {
					_cellData->InsertAt(k, item);
					inserted = true;
					break;
				}
			}
		}
		break;

	case TypeCode::String:
	{
		for (unsigned int k = 0; k < _cellData->Size; ++k) {
			auto current = _cellData->GetAt(k);
			auto current_data = GetCellData(current, column);
			if (Platform::Type::GetTypeCode(current_data->GetType()) == item_typecode) {
				// This is also a string !
				auto compare_result = wcscmp(item_data->ToString()->Data(), current_data->ToString()->Data());
				if ((sortAscending && compare_result < 0) || (!sortAscending && compare_result > 0)) {
					_cellData->InsertAt(k, item);
					inserted = true;
					break;
				}
			}
			else {
				// Flag error (different types in same column)

			}
		}
	}
	break;

	case TypeCode::Int8:
	case TypeCode::Int16:
	case TypeCode::Int32:
	case TypeCode::Int64:
	{
		int item_value = _wtoi(item_data->ToString()->Data());
		for (unsigned int k = 0; k < _cellData->Size; ++k) {
			auto current = _cellData->GetAt(k);
			auto current_data = GetCellData(current, column);
			if (Platform::Type::GetTypeCode(current_data->GetType()) == item_typecode) {
				// Convert the current_data value.
				int current_value = _wtoi(current_data->ToString()->Data());
				if ((sortAscending && current_value < item_value) || (!sortAscending && current_value > item_value)) {
					_cellData->InsertAt(k, item);
					inserted = true;
					break;
				}
			}
		}
	}
	break;

	case TypeCode::Boolean:

		break;

	case TypeCode::Char16:
		break;

	case TypeCode::DateTime:
		break;

	case TypeCode::Double:

		break;

	case TypeCode::UInt8:
	case TypeCode::UInt16:
	case TypeCode::UInt32:
	case TypeCode::UInt64:
		{
			unsigned int item_value = _wtoi(item_data->ToString()->Data());
			for (unsigned int k = 0; k < _cellData->Size; ++k) {
				auto current = _cellData->GetAt(k);
				auto current_data = GetCellData(current, column);
				if (Platform::Type::GetTypeCode(current_data->GetType()) == item_typecode) {
					// Convert the current_data value.
					unsigned int current_value = _wtoi(current_data->ToString()->Data());
					if ((sortAscending && current_value < item_value) || (!sortAscending && current_value > item_value)) {
						_cellData->InsertAt(k, item);
						inserted = true;
						break;
					}
				}
			}
		}
		break;
	}

	if (!inserted) {
		// Insert at the end.
		_cellData->InsertAt(_cellData->Size, item);
	}
}

void 
SimpleDataGrid::OnSortDescendingClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	SimpleDataGridColumn^ column = nullptr;

	MenuFlyoutItem^ menuitem = dynamic_cast<MenuFlyoutItem^>(sender);
	if (menuitem) {
		if (!menuitem->Tag) return;
		column = dynamic_cast<SimpleDataGridColumn^>(menuitem->Tag);

	} else {
		column = dynamic_cast<SimpleDataGridColumn^>(sender);

	}

	if (!column) return;

	SortGrid(column, false);
}

void 
SimpleDataGrid::OnColumnHeaderTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e) {
	auto column = dynamic_cast<SimpleDataGridColumn^>(sender);
	if (!column) return;
	
	switch (column->SortState) {
	case SimpleDataGridColumnSortStateEnum::Unsorted:
	case SimpleDataGridColumnSortStateEnum::Descending:
		OnSortAscendingClick(sender, nullptr);
		break;

	case SimpleDataGridColumnSortStateEnum::Ascending:
		OnSortDescendingClick(sender, nullptr);
		break;
	}	
}

void 
SimpleDataGrid::OnAlternateRowBackgroundChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	auto datagrid = dynamic_cast<SimpleDataGrid^>(d);
	if (!datagrid)
		return;

	datagrid->DrawControl();
}

/*void
SimpleDataGrid::OnOddRowBackgroundChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	auto datagrid = dynamic_cast<SimpleDataGrid^>(d);
	if (!datagrid)
		return;

	datagrid->DrawControl();
}*/

void 
SimpleDataGrid::OnBackgroundOpacityChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	auto datagrid = dynamic_cast<SimpleDataGrid^>(d);
	if (!datagrid)
		return;

	datagrid->DrawControl();
}

void 
SimpleDataGrid::OnAlternateRowBackgroundOpacityChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	auto datagrid = dynamic_cast<SimpleDataGrid^>(d);
	if (!datagrid)
		return;

	datagrid->DrawControl();
}

void 
SimpleDataGrid::OnShowRowNumbersChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	auto datagrid = dynamic_cast<SimpleDataGrid^>(d);
	if (!datagrid)
		return;

	datagrid->DrawControl();
}

void 
SimpleDataGrid::OnDividerBrushChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	/* TODO: Update the divider colour in the rows and columns ? */
}

void 
SimpleDataGrid::OnLinesVisibilityChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	/* TODO: Update the lines that are visible */

}

void 
SimpleDataGrid::OnSelectedItemChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e) {
	OutputDebugStringW(L"SelectedItem Changed.\n");
}


void
SimpleDataGrid::OnColumnsVectorChanged(Windows::Foundation::Collections::IObservableVector<Enlow::Controls::SimpleDataGridColumn^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ args) {
	wchar_t fmt[512];
	swprintf_s(fmt, 512, L"Object %p, Columns vector has changed. Is now size %d.\n", this, _columns->Size);
	OutputDebugStringW(fmt);
	
	switch (args->CollectionChange) {
	case Windows::Foundation::Collections::CollectionChange::ItemInserted:
		_columns->Append(ColumnDefinitions->GetAt(args->Index));
		DrawControl();
		break;

	case Windows::Foundation::Collections::CollectionChange::ItemRemoved:
		_columns->RemoveAt(args->Index);
		DrawControl();
		break;

	case Windows::Foundation::Collections::CollectionChange::Reset:
		_columns->Clear();
		DrawControl();
		break;
	}
}