#include "LJM_Utilities.h"
// Source

const char * NumberToDebugLogMode(int mode)
{
	static const char * LJM_DEBUG_LOG_MODE_NEVER_STRING =
		"LJM_DEBUG_LOG_MODE_NEVER";
	static const char * LJM_DEBUG_LOG_MODE_CONTINUOUS_STRING =
		"LJM_DEBUG_LOG_MODE_CONTINUOUS";
	static const char * LJM_DEBUG_LOG_MODE_ON_ERROR_STRING =
		"LJM_DEBUG_LOG_MODE_ON_ERROR";
	static const char * unknown_string = "Unknown LJM_DEBUG_LOG_MODE";

	if (mode == LJM_DEBUG_LOG_MODE_NEVER)
		return LJM_DEBUG_LOG_MODE_NEVER_STRING;
	if (mode == LJM_DEBUG_LOG_MODE_CONTINUOUS)
		return LJM_DEBUG_LOG_MODE_CONTINUOUS_STRING;
	if (mode == LJM_DEBUG_LOG_MODE_ON_ERROR)
		return LJM_DEBUG_LOG_MODE_ON_ERROR_STRING;

	return unknown_string;
}

const char * NumberToConnectionType(int connectionType)
{
	switch (connectionType) {
	case LJM_ctANY:          return "LJM_ctANY";
	case LJM_ctUSB:          return "LJM_ctUSB";
	case LJM_ctTCP:          return "LJM_ctTCP";
	case LJM_ctETHERNET:     return "LJM_ctETHERNET";
	case LJM_ctWIFI:         return "LJM_ctWIFI";
	case LJM_ctNETWORK_UDP:  return "LJM_ctNETWORK_UDP";
	case LJM_ctETHERNET_UDP: return "LJM_ctETHERNET_UDP";
	case LJM_ctWIFI_UDP:     return "LJM_ctWIFI_UDP";
	case LJM_ctNETWORK_ANY:  return "LJM_ctNETWORK_ANY";
	case LJM_ctETHERNET_ANY: return "LJM_ctETHERNET_ANY";
	case LJM_ctWIFI_ANY:     return "LJM_ctWIFI_ANY";
	default:                 return "Unknown connection type";
	}
}

const char * NumberToDeviceType(int deviceType)
{
	switch (deviceType) {
	case LJM_dtANY:     return "LJM_dtANY";
	case 4:             return "LJM_dtT4";
	case LJM_dtT7:      return "LJM_dtT7";
	case 84:            return "LJM_dtTSERIES";
	case LJM_dtDIGIT:   return "LJM_dtDIGIT";
	case -4:            return "Demo fake usb";
	default:
		printf(
			"%s:%d NumberToDeviceType: Unknown device type: %d\n",
			__FILE__,
			__LINE__,
			deviceType
		);
		return "Unknown device type";
	}
}

int GetDeviceType(int handle)
{
	int err, deviceType;
	err = LJM_GetHandleInfo(handle, &deviceType, NULL, NULL, NULL, NULL, NULL);
	ErrorCheck(err, "LJM_GetHandleInfo in GetDeviceType");
	return deviceType;
}

void PrintDeviceInfoFromHandle(int handle)
{
	int DeviceType, ConnectionType, SerialNumber, IPAddress, Port,
		MaxBytesPerMB;

	int err = LJM_GetHandleInfo(handle, &DeviceType, &ConnectionType,
		&SerialNumber, &IPAddress, &Port, &MaxBytesPerMB);

	ErrorCheckWithAddress(err, INITIAL_ERR_ADDRESS,
		"PrintDeviceInfoFromHandle (LJM_GetHandleInfo)");

	PrintDeviceInfo(DeviceType, ConnectionType, SerialNumber, IPAddress, Port,
		MaxBytesPerMB);
}

void PrintDeviceInfo(int deviceType, int connectionType, int serialNumber,
	int ipAddressInt, int portOrPipe, int packetMaxBytes)
{
	char ipAddressString[LJM_IPv4_STRING_SIZE];

	// Print
	printf("deviceType: %s\n", NumberToDeviceType(deviceType));
	printf("connectionType: %s\n", NumberToConnectionType(connectionType));
	printf("serialNumber: %d\n", serialNumber);

	if (IsNetwork(connectionType)) {
		LJM_NumberToIP(ipAddressInt, ipAddressString);
		printf("IP address: %s\n", ipAddressString);
	}

	if (connectionType == LJM_ctUSB) {
		printf("pipe: %d\n", portOrPipe);
	}
	else {
		printf("port: %d\n", portOrPipe);
	}

	printf("Maximum number of bytes per packet: %d\n",
		packetMaxBytes);
}

void WaitForUserIfWindows()
{
	#ifdef _WIN32
		#ifndef AUTOMATED_TEST
			WaitForUser();
		#endif
	#endif
}

void WaitForUser()
{
	printf("Press enter to continue\n");
	getchar();

	// C++
	// std::cin.ignore();
}

int OpenOrDie(int deviceType, int connectionType, const char * identifier)
{
	int handle, err;
	err = LJM_Open(deviceType, connectionType, identifier, &handle);
	ErrorCheck(err, "LJM_Open(%d, %d, %s, ...)", deviceType, connectionType,
		identifier);
	return handle;
}

int OpenSOrDie(const char *  deviceType, const char *  connectionType,
	const char * identifier)
{
	int handle, err;
	err = LJM_OpenS(deviceType, connectionType, identifier, &handle);
	ErrorCheck(err, "LJM_OpenS(%s, %s, %s, ...)", deviceType, connectionType,
		identifier);
	return handle;
}

void CloseOrDie(int handle)
{
	int err = LJM_Close(handle);
	ErrorCheck(err, "LJM_Close(%d)", handle);
}

void PrintErrorAddressHelper(int errAddress)
{
	if (!(errAddress < 0))
		printf("\terror address: %d\n", errAddress);
}

typedef enum {
	ACTION_PRINT_AND_EXIT,
	ACTION_PRINT
} ErrorAction;

// The "internal" print function for ErrorCheck and ErrorCheckWithAddress
void _ErrorCheckWithAddress(int err, int errAddress, ErrorAction action,
	const char * description, va_list args)
{
	char errName[LJM_MAX_NAME_SIZE];
	if (err >= LJME_WARNINGS_BEGIN && err <= LJME_WARNINGS_END) {
		LJM_ErrorToString(err, errName);
		vfprintf (stdout, description, args);
		printf(" warning: \"%s\" (Warning code: %d)\n", errName, err);
		PrintErrorAddressHelper(errAddress);
	}
	else if (err != LJME_NOERROR)
	{
		LJM_ErrorToString(err, errName);
		vfprintf (stdout, description, args);
		printf(" error: \"%s\" (ErrorCode: %d)\n", errName, err);
		PrintErrorAddressHelper(errAddress);

		if (action == ACTION_PRINT_AND_EXIT) {
			printf("Closing all devices and exiting now\n");
			WaitForUserIfWindows();
			LJM_CloseAll();
			exit(err);
		}
	}
}

void PrintErrorIfError(int err, const char * formattedDescription, ...)
{
	va_list args;

	va_start (args, formattedDescription);
	_ErrorCheckWithAddress(err, INITIAL_ERR_ADDRESS, ACTION_PRINT,
		formattedDescription, args);
	va_end (args);
}

void PrintErrorWithAddressIfError(int err, int errAddress,
	const char * formattedDescription, ...)
{
	va_list args;

	va_start (args, formattedDescription);
	_ErrorCheckWithAddress(err, errAddress, ACTION_PRINT,
		formattedDescription, args);
	va_end (args);
}

void ErrorCheck(int err, const char * formattedDescription, ...)
{
	va_list args;

	va_start (args, formattedDescription);
	_ErrorCheckWithAddress(err, INITIAL_ERR_ADDRESS, ACTION_PRINT_AND_EXIT,
		formattedDescription, args);
	va_end (args);
}

void ErrorCheckWithAddress(int err, int errAddress, const char * description,
	...)
{
	va_list args;

	va_start (args, description);
	_ErrorCheckWithAddress(err, errAddress, ACTION_PRINT_AND_EXIT, description,
		args);
	va_end (args);
}

void MillisecondSleep(unsigned int milliseconds)
{
	#ifdef _WIN32
		Sleep(milliseconds);
	#else
		usleep(milliseconds*1000);
	#endif
}

double Get(int handle, const char * valueName)
{
	double value;
	int err;

	err = LJM_eReadName(handle, valueName, &value);
	if (err != LJME_NOERROR) {
		CouldNotRead(err, valueName);
	}
	return value;
}

double GetAndPrint(int handle, const char * valueName)
{
	double value;
	int err;

	err = LJM_eReadName(handle, valueName, &value);
	if (err == LJME_NOERROR) {
		printf("%s: %f\n", valueName, value);
	}
	else {
		CouldNotRead(err, valueName);
	}
	return value;
}

void GetAndPrintIPAddress(int handle, const char * valueName)
{
	double ip;
	char IP_STRING[LJM_IPv4_STRING_SIZE];
	int err;

	err = LJM_eReadName(handle, valueName, &ip);
	if (err == LJME_NOERROR) {
		err = LJM_NumberToIP((unsigned int)ip, IP_STRING);
		ErrorCheck(err, "Converting ip from device to a string");
		printf("%s: %s\n", valueName, IP_STRING);
	}
	else {
		CouldNotRead(err, valueName);
	}
}

void GetAndPrintMACAddressFromValueAddress(int handle, const char * valueName,
	int valueAddress)
{
	int err, byteI;
	enum { NUM_BYTES = 8 };
	enum { NUM_FRAMES = 1 };
	double mac[NUM_BYTES];
	int aAddresses[NUM_FRAMES] = {valueAddress};
	int aTypes[NUM_FRAMES] = {LJM_BYTE};
	int aWrites[NUM_FRAMES] = {LJM_READ};
	int aNumValues[NUM_FRAMES] = {NUM_BYTES};
	int ErrorAddress = INITIAL_ERR_ADDRESS;

	err = LJM_eAddresses(handle, 1, aAddresses, aTypes, aWrites, aNumValues,
		mac, &ErrorAddress);
	if (err != LJME_NOERROR) {
		CouldNotRead(err, valueName);
	}

	printf("%s: ", valueName);
	for (byteI = 0; byteI < NUM_BYTES; byteI++) {
		printf("%02x", (unsigned int)mac[byteI]);
		if (byteI < NUM_BYTES - 1) {
			printf(":");
		}
	}
	printf("\n");
}

void GetAndPrintAddressAndType(int handle, const char * valueDescription,
	int address, int type)
{
	double value;
	int err;

	err = LJM_eReadAddress(handle, address, type, &value);
	if (err == LJME_NOERROR) {
		printf("%s: %f\n", valueDescription, value);
	}
	else {
		CouldNotRead(err, valueDescription);
	}
}

void GetAndPrintConfigValue(const char * configParameter)
{
	double value;
	int err;

	err = LJM_ReadLibraryConfigS(configParameter, &value);
	if (err == LJME_NOERROR) {
		printf("%s: %f\n", configParameter, value);
	}
	else {
		CouldNotRead(err, configParameter);
	}
}

void GetAndPrintConfigString(const char * configParameter)
{
	char string[LJM_MAX_NAME_SIZE];
	int err;

	err = LJM_ReadLibraryConfigStringS(configParameter, string);
	if (err == LJME_NOERROR) {
		printf("%s: %s\n", configParameter, string);
	}
	else {
		CouldNotRead(err, configParameter);
	}
}

void SetConfigValue(const char * configParameter, double value)
{
	int err = LJM_WriteLibraryConfigS(configParameter, value);
	PrintErrorIfError(err, "[LJM_WriteLibraryConfigS(Parameter=%s, Value=%f)]",
		configParameter, value);
}

void SetConfigString(const char * configParameter, const char * string)
{
	int err = LJM_WriteLibraryConfigStringS(configParameter, string);
	PrintErrorIfError(err, "[LJM_WriteLibraryConfigS(Parameter=%s, String=%s)]",
		configParameter, string);
}

void WriteNameOrDie(int handle, const char * name, double value)
{
	int err = WriteName(handle, name, value);
	if (err) {
		WaitForUserIfWindows();
		exit(err);
	}
}

int WriteName(int handle, const char * name, double value)
{
	int err = LJM_eWriteName(handle, name, value);
	PrintErrorIfError(err, "LJM_eWriteName(Handle=%d, Name=%s, Value=%f)",
		handle, name, value);
	return err;
}

void WriteNameAltTypeOrDie(int handle, const char * name, int type,
	double value)
{
	int address;
	int err = LJM_NameToAddress(name, &address, NULL);
	ErrorCheck(err, "WriteNameAltTypeOrDie: LJM_NameToAddress(Name=%s, ...)",
		name);
	err = LJM_eWriteAddress(handle, address, type, value);
	ErrorCheck(err,
		"WriteNameAltTypeOrDie: LJM_eWriteAddress(Handle=%d, Address=%d, Type=%d, Value=%f)",
		handle, address, type, value);
}

void WriteNamesOrDie(int handle, int NumFrames, const char ** aNames,
	const double * aValues)
{
	int errorAddress = INITIAL_ERR_ADDRESS;
	int err = WriteNames(handle, NumFrames, aNames, aValues, &errorAddress);
	if (err) {
		WaitForUserIfWindows();
		exit(err);
	}
}

int WriteNames(int handle, int NumFrames, const char ** aNames,
	const double * aValues, int * errorAddress)
{
	int err = LJM_eWriteNames(handle, NumFrames, aNames, aValues, errorAddress);
	PrintErrorWithAddressIfError(err, *errorAddress,
		"LJM_eWriteNames(Handle=%d, NumFrames=%d, aNames=[%s, ...], aValues=[%f, ...], ...)",
		handle, NumFrames, aNames[0], aValues[0]);
	return err;
}

void WriteNameArrayOrDie(int handle, const char * name, int numValues,
	const double * aValues)
{
	int errorAddress = INITIAL_ERR_ADDRESS;
	int err = LJM_eWriteNameArray(handle, name, numValues, aValues,
		&errorAddress);
	ErrorCheckWithAddress(err, errorAddress,
		"Error during LJM_eWriteNameArray(%d, %s, %d, ...)", handle, name,
		numValues);
}

void WriteNameByteArrayOrDie(int handle, const char * name, int numBytes,
	const char * aBytes)
{
	int errorAddress = INITIAL_ERR_ADDRESS;
	int err = LJM_eWriteNameByteArray(handle, name, numBytes, aBytes,
		&errorAddress);
	ErrorCheckWithAddress(err, errorAddress,
		"Error during LJM_eWriteNameByteArray(%d, %s, %d, ...)", handle, name,
		numBytes);
}

void ReadNameArrayOrDie(int handle, const char * name, int numValues,
	double * aValues)
{
	int errorAddress = INITIAL_ERR_ADDRESS;
	int err = LJM_eReadNameArray(handle, name, numValues, aValues,
		&errorAddress);
	ErrorCheckWithAddress(err, errorAddress,
		"Error during LJM_eReadNameArray(%d, %s, %d, ...)", handle, name,
		numValues);
}

void ReadNameByteArrayOrDie(int handle, const char * name, int numBytes,
	char * aBytes)
{
	int errorAddress = INITIAL_ERR_ADDRESS;
	int err = LJM_eReadNameByteArray(handle, name, numBytes, aBytes,
		&errorAddress);
	ErrorCheckWithAddress(err, errorAddress,
		"Error during LJM_eReadNameByteArray(%d, %s, %d, ...)", handle, name,
		numBytes);
}

void CouldNotRead(int err, const char * valueName)
{
	char errString[LJM_MAX_NAME_SIZE];
	LJM_ErrorToString(err, errString);
	printf("Could not read %s. Error was %s (%d)\n", valueName, errString, err);
}

void EnableLoggingLevel(double logLevel)
{
	// Warning: These calls may change
	ErrorCheck(
		LJM_WriteLibraryConfigS(LJM_DEBUG_LOG_MODE, 2.0),
		"Setting log mode to continuous");
	ErrorCheck(
		LJM_WriteLibraryConfigS(LJM_DEBUG_LOG_LEVEL, logLevel),
		"Setting log level");
	ErrorCheck(
		LJM_WriteLibraryConfigS(LJM_DEBUG_LOG_FILE_MAX_SIZE, 123456789),
		"Setting LJM_DEBUG_LOG_FILE_MAX_SIZE");
}

unsigned int GetCurrentTimeMS()
{
	return LJM_GetHostTick() / 1000;
}

unsigned int IPToNumber(const char * IPv4String)
{
	unsigned int number = 0;
	int err = LJM_IPToNumber(IPv4String, &number);
	ErrorCheck(err, "LJM_IPToNumber - %s", IPv4String);

	return number;
}

int EqualFloats(double v1, double v2, double delta)
{
	if (v1 - v2 < delta && v2 - v1 < delta) {
		return 1; // True
	}

	return 0; // False
}

int IsTCP(int connectionType)
{
	if (connectionType == LJM_ctTCP ||
		connectionType == LJM_ctETHERNET ||
		connectionType == LJM_ctWIFI)
	{
		return 1;
	}
	return 0;
}

int IsUDP(int connectionType)
{
	if (connectionType == LJM_ctNETWORK_UDP ||
		connectionType == LJM_ctETHERNET_UDP ||
		connectionType == LJM_ctWIFI_UDP)
	{
		return 1;
	}
	return 0;
}

int IsNetwork(int connectionType)
{
	if (IsTCP(connectionType) ||
		IsUDP(connectionType) ||
		connectionType == LJM_ctNETWORK_ANY ||
		connectionType == LJM_ctETHERNET_ANY ||
		connectionType == LJM_ctWIFI_ANY
	) {
		return 1;
	}
	return 0;
}

int DoesDeviceHaveWiFi(int handle)
{
	double hardwareInstalled = 0;
	int err = LJM_eReadName(handle, "HARDWARE_INSTALLED", &hardwareInstalled);
	if (err != LJME_NOERROR) {
		return 0;
	}
	if (((int)hardwareInstalled) & 0x2) {
		return 1;
	}
	return 0;
}

void DisplayDebugLoggingConfigurations()
{
	double mode;
	int err = LJM_ReadLibraryConfigS(LJM_DEBUG_LOG_MODE, &mode);
	ErrorCheck(err, "LJM_ReadLibraryConfigS(LJM_DEBUG_LOG_MODE, ...)");
	printf("LJM_DEBUG_LOG_MODE: %s\n", NumberToDebugLogMode((int)mode));

	if ((int)mode != LJM_DEBUG_LOG_MODE_NEVER) {
		// GetAndPrintConfigString and GetAndPrintConfigValue are defined in
		// LJM_Utilities.h
		GetAndPrintConfigString(LJM_DEBUG_LOG_FILE);

		GetAndPrintConfigValue(LJM_DEBUG_LOG_FILE_MAX_SIZE);
		GetAndPrintConfigValue(LJM_DEBUG_LOG_LEVEL);
	}
}

int GetAddressFromNameOrDie(const char * name)
{
	int address = -1;
	int err = LJM_NameToAddress(name, &address, NULL);
	ErrorCheck(err, "GetAddressFromNameOrDie > LJM_NameToAddress(%s, ...)",
		name);
	return address;
}