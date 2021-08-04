//
// MyUserControl.xaml.h
// Declaration of the MyUserControl class
//

#pragma once

#include "MyUserControl.g.h"

namespace VMultiUWPApp_WRC
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class MyUserControl sealed
	{
	public:
		MyUserControl();
	private:
		void slider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
		void box_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
