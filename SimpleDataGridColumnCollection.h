#pragma once

namespace Enlow {
	namespace Controls {
		ref class SimpleDataGridColumn;

		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class SimpleDataGridColumnCollection sealed : public Windows::Foundation::Collections::IObservableVector<SimpleDataGridColumn^> {
		public:
			SimpleDataGridColumnCollection();

			virtual Windows::Foundation::Collections::IIterator<SimpleDataGridColumn^>^ First();
			virtual property unsigned int Size {
				unsigned int get();
			}

			virtual SimpleDataGridColumn^ GetAt(unsigned int index);

			virtual Windows::Foundation::Collections::IVectorView<SimpleDataGridColumn^>^ GetView();

			virtual bool IndexOf(SimpleDataGridColumn^ value, unsigned int* index);
			virtual void SetAt(unsigned int index, SimpleDataGridColumn^ value);
			virtual void InsertAt(unsigned int index, SimpleDataGridColumn^ value);
			virtual void RemoveAt(unsigned int index);
			virtual void Append(SimpleDataGridColumn^ value);
			virtual void RemoveAtEnd();
			virtual void Clear();
			virtual unsigned int GetMany(unsigned int startIndex, Platform::WriteOnlyArray<SimpleDataGridColumn^>^ items);
			virtual void ReplaceAll(const Platform::Array<SimpleDataGridColumn^>^ items);

			virtual event Windows::Foundation::Collections::VectorChangedEventHandler<SimpleDataGridColumn^>^ VectorChanged;

		private:
			Windows::Foundation::Collections::IObservableVector<SimpleDataGridColumn^>^ _columns;
		};

	}
}