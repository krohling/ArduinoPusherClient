#ifndef LOGGER_H
#define LOGGER_H

#define LOGGER_ENABLED 1

#if LOGGER_ENABLED
inline void LogPrint(const char** table, int index)
{
	int len = strlen(table[index]);
	char buffer[len+1];
	strcpy(buffer, table[index]);
	Serial.print(buffer); 
}

inline void LogPrintLn(const char** table, int index)
{
    int len = strlen(table[index]);
	char buffer[len+1];
	strcpy(buffer, table[index]);
    
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

