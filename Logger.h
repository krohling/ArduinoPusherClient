#ifndef LOGGER_H
#define LOGGER_H

#define LOGGER_ENABLED 1

#if LOGGER_ENABLED
inline void LogPrint(PROGMEM const char** table, int index)
{
	char* ptr = (char*)pgm_read_word(&(table[index]));
	int len = strlen_P(ptr);
	char buffer[len+1];
	strcpy_P(buffer, ptr);
    
	Serial.print(buffer); 
}

inline void LogPrintLn(PROGMEM const char** table, int index)
{
    char* ptr = (char*)pgm_read_word(&(table[index]));
	int len = strlen_P(ptr);
	char buffer[len+1];
	strcpy_P(buffer, ptr);
    
	Serial.println(buffer);
	Serial.flush();
}

inline void LogPrint(const String& text)
{
    Serial.print(text); 
}

inline void LogPrintLn(const String& text)
{
    Serial.println(text);
	Serial.flush();
}

inline void LogPrint(const char text[])
{
    Serial.print(text); 
}

inline void LogPrintLn(const char text[])
{
    Serial.println(text);
	Serial.flush();
}

inline void LogPrint(const int value)
{
    Serial.print(value); 
}

inline void LogPrintLn(const int value)
{
    Serial.println(value);
	Serial.flush();
}

#else

inline void LogPrint(...)
{
}

inline void LogPrintLn(...)
{
}

#endif
#endif

