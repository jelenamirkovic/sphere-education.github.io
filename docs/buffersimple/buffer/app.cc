#include<iostream>
#include<string>
#include<string.h>

void sample_function(const char* var)
{
  char buffer[10];
  strcpy(buffer, var);
  std::cout<<"Everything worked fine\n";
  return;
}


int main()
{
  std::string s;
  std::cout<<"Enter a string and press ENTER: ";
  std::cin >> s;
  sample_function(s.c_str());
  return 0;
}
