//
// MyUserControl.xaml.cpp
// Implementation of the MyUserControl class
//

#include "pch.h"
#include "MyUserControl.xaml.h"
#include <iostream>
#include <regex>

using namespace VMultiUWPApp_WRC;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

MyUserControl::MyUserControl()
{
	InitializeComponent();
}

void VMultiUWPApp_WRC::MyUserControl::slider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	std::wcout << L"Slider Value Changed: " << e->NewValue.ToString()->Data() << std::endl;
}

void VMultiUWPApp_WRC::MyUserControl::box_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	std::wcout << L"Text Value Changed: " << this->box->Text->ToString()->Data() << std::endl;
}

void VMultiUWPApp_WRC::MyUserControl::button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Windows::Globalization::Calendar^ cal = ref new Windows::Globalization::Calendar();
	Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ formatter = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter(L"longdate longtime");
	Platform::String^ str = formatter->Format(cal->GetDateTime());
	std::wcout << L"Button was clicked on " << std::regex_replace(static_cast<std::wstring>(str->ToString()->Data()), std::wregex(L"\\u200E"), L"") << std::endl; // If you don't remove \u200E then wcout will stop working...
}
