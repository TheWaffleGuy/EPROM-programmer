#include <stdint.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdarg.h>
#include <algorithm> 
#include <conio.h>
#include <math.h>

#define INPUT 0x0
#define OUTPUT 0x1

#define DEC 10
#define HEX 16
#define OCT 8
#ifdef BIN // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
#undef BIN
#endif
#define BIN 2

//typedef std::string String;

//Registers
uint8_t SREG = 0;
uint8_t DDRA = 0;
uint8_t DDRB = 0;
uint8_t DDRC = 0;
uint8_t DDRD = 0;
uint8_t PORTA = 0;
uint8_t PORTB = 0;
uint8_t PORTC = 0;
uint8_t PORTD = 0;
uint8_t PINA = 0;
uint8_t PINB = 0;
uint8_t PINC = 0;
uint8_t PIND = 0;

const uint8_t* port_to_mode[] =
{
  NULL,
  &DDRA,
  &DDRB,
  &DDRC,
  &DDRD,
};

uint8_t* port_to_output[] =
{
  NULL,
  &PORTA,
  &PORTB,
  &PORTC,
  &PORTD,
};

uint8_t* port_to_input[] =
{
  NULL,
  &PINA,
  &PINB,
  &PINC,
  &PIND,
};

//Functions


void portMode(uint8_t PORT, uint8_t MODE){};
void cli() {};
int isDigit(int c)
{
  return ( isdigit (c) == 0 ? false : true);
}
void portWrite(uint8_t PORT, uint8_t VALUE)
{
  PORT++;
  *(port_to_output[PORT]) = VALUE;
}

uint8_t portRead(uint8_t PORT)
{
  PORT++;
  return *(port_to_input[PORT]);
}

// String class
namespace {
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}
}

class String : public std::string {
  public:
    String() {}
    String(const String& string) : String(string.c_str()) {}
    String(const std::string& string) : std::string(string) {}
    String(const char* string) : std::string(string) {}

    String substring(int start) {
      return substr(start);
    }
    String substring(int start, int end) {
      return substr(start, end - start);
    }

    String remove(int start) {
      return erase(start);
    }

    void replace(const String& from, const String& to) {
      ReplaceAll(*this, from, to);
    }

    int indexOf(const char needle) {
      return find(needle);
    }

    void trim() {
      erase(begin(), std::find_if(begin(), end(), [](unsigned char ch) {
        return !std::isspace(ch);
      }));
      erase(std::find_if(rbegin(), rend(), [](unsigned char ch) {
        return !std::isspace(ch);
      }).base(), end());
    }

    long toInt(void) const
    {
      return atol(c_str());
    }
};

//Serial class

class FakeSerial {
public:
  void begin(unsigned long) {}
  void end() {}

  size_t write(char c) {
    std::cout << c;
    return 1;
  }
  size_t write(const unsigned char buf[], size_t size) {
    std::cout << std::string((char*)buf,size);
    return size;
  }

  size_t write(const char *str) {
    if (str == NULL) return 0;
    return write((const uint8_t *)str, strlen(str));
  }

  size_t write(const char *buffer, size_t size) {
    return write((const uint8_t *)buffer, size);
  }

  size_t print(const String &s)
  {
    return write(s.c_str(), s.length());
  }

  size_t print(const char str[])
  {
    return write(str);
  }

  size_t print(char c)
  {
    return write(c);
  }

  size_t print(unsigned char b, int base)
  {
    return print((unsigned long) b, base);
  }

  size_t print(int n, int base)
  {
    return print((long) n, base);
  }

  size_t print(unsigned int n, int base)
  {
    return print((unsigned long) n, base);
  }

  size_t print(long n, int base)
  {
    if (base == 0) {
      return write(n);
    } else if (base == 10) {
      if (n < 0) {
        int t = print('-');
        n = -n;
        return printNumber(n, 10) + t;
      }
      return printNumber(n, 10);
    } else {
      return printNumber(n, base);
    }
  }

  size_t print(unsigned long n, int base)
  {
    if (base == 0) return write(n);
    else return printNumber(n, base);
  }

  size_t println(void)
  {
    return write("\r\n");
  }

  size_t println(const String &s)
  {
    size_t n = print(s);
    n += println();
    return n;
  }

  size_t println(const char c[])
  {
    size_t n = print(c);
    n += println();
    return n;
  }

  size_t println(char c)
  {
    size_t n = print(c);
    n += println();
    return n;
  }

  size_t println(unsigned char b, int base)
  {
    size_t n = print(b, base);
    n += println();
    return n;
  }

  size_t println(int num, int base)
  {
    size_t n = print(num, base);
    n += println();
    return n;
  }

  size_t println(unsigned int num, int base)
  {
    size_t n = print(num, base);
    n += println();
    return n;
  }

  size_t println(long num, int base)
  {
    size_t n = print(num, base);
    n += println();
    return n;
  }

  size_t println(unsigned long num, int base)
  {
    size_t n = print(num, base);
    n += println();
    return n;
  }

  int16_t printf(const char *format, ...)
  {
    va_list ap;
    va_start(ap, format);
    return vprintf(format, ap);
  }

  int available()
  {
    return true;
  }

  String readStringUntil(char terminator)
  {
    using namespace std;
    String ret;
    std::getline (cin, ret, terminator);
    return ret;
  }


private:
size_t printNumber(unsigned long n, uint8_t base)
  {
    char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
    char *str = &buf[sizeof(buf) - 1];

    *str = '\0';

    // prevent crash if called with base == 1
    if (base < 2) base = 10;

    do {
      char c = n % base;
      n /= base;

      *--str = c < 10 ? c + '0' : c + 'A' - 10;
    } while(n);

    return write(str);
  }
};

FakeSerial Serial;