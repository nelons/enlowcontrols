#include "pch.h"
#include "SimpleDataGridRow.h"
#include "SimpleDataGridColumn.h"
#include "SimpleDataGridCell.h"

using namespace Enlow::Controls;

SimpleDataGridRow::SimpleDataGridRow() {
	Source = nullptr;
	IsSelected = false;
	_highlighted = false;
}

void 
SimpleDataGridRow::HighlightRow() {
	if (!_highlighted) {
		_highlighted = true;
		UpdateRowCellsBackground();
	}
}

void 
SimpleDataGridRow::ResetRowHighlight() {
	if (_highlighted) {
		_highlighted = false;
		UpdateRowCellsBackground();
	}
}

void 
SimpleDataGridRow::UpdateRowCellsBackground() {
	for (auto cell : CellData) {
		cell.second->UpdateCellBackground();
	}
}