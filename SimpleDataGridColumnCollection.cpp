#include "pch.h"
#include "SimpleDataGridColumnCollection.h"
#include "SimpleDataGridColumn.h"

using namespace Enlow::Controls;
using namespace Windows::Foundation::Collections;
using namespace Platform::Collections;

SimpleDataGridColumnCollection::SimpleDataGridColumnCollection() {
	_columns = ref new Vector<SimpleDataGridColumn^>();
}

Windows::Foundation::Collections::IIterator<SimpleDataGridColumn^>^ 
SimpleDataGridColumnCollection::First() {
	return _columns->First();
}

SimpleDataGridColumn^ 
SimpleDataGridColumnCollection::GetAt(unsigned int index) {
	return _columns->GetAt(index);
}

Windows::Foundation::Collections::IVectorView<SimpleDataGridColumn^>^ 
SimpleDataGridColumnCollection::GetView() {
	return _columns->GetView();
}

bool 
SimpleDataGridColumnCollection::IndexOf(SimpleDataGridColumn^ value, unsigned int* index) {
	return _columns->IndexOf(value, index);
}

void 
SimpleDataGridColumnCollection::SetAt(unsigned int index, SimpleDataGridColumn^ value) {
	_columns->SetAt(index, value);
}

void
SimpleDataGridColumnCollection::InsertAt(unsigned int index, SimpleDataGridColumn^ value) {
	_columns->InsertAt(index, value);
}

void
SimpleDataGridColumnCollection::RemoveAt(unsigned int index) {
	_columns->RemoveAt(index);
}

void
SimpleDataGridColumnCollection::Append(SimpleDataGridColumn^ value) {
	_columns->Append(value);
}

void
SimpleDataGridColumnCollection::RemoveAtEnd() {
	_columns->RemoveAtEnd();
}

void
SimpleDataGridColumnCollection::Clear() {
	_columns->Clear();
}

unsigned int 
SimpleDataGridColumnCollection::GetMany(unsigned int startIndex, Platform::WriteOnlyArray<SimpleDataGridColumn^>^ items) {
	return _columns->GetMany(startIndex, items);
}

void 
SimpleDataGridColumnCollection::ReplaceAll(const Platform::Array<SimpleDataGridColumn^>^ items) {
	_columns->ReplaceAll(items);
}

unsigned int
SimpleDataGridColumnCollection::Size::get() {
	return _columns->Size;
}

