#ifndef LJHEADER_H
#define LJHEADER_H

// The version of this driver.  Call GetDriverVersion() to determine the version of the DLL you have.
// It should match this number, otherwise your .h and DLL's are from different versions.
#define DRIVER_VERSION 3.50

#define LJ_HANDLE long
#define LJ_ERROR long


#ifdef  __cplusplus
extern "C"
{
#endif

// **************************************************
// Functions
// **************************************************

// The S form of these functions (the ones with S at the end) take strings instead of numerics for places where constants
// would normally be used.  This allows languages that can't include this header file to still use defined constants rather
// than hard coded numbers.

// The Ptr form of these functions (the ones with Ptr at the end) take a void * instead of a long for the x1 parameter.
// This allows the x1 parameter to be 64-bit pointer address safe, and is required in 64-bit applications when passing an
// pointer/array to x1.

void _stdcall Close();
// Closes all LabJack device handles.

LJ_ERROR _stdcall ListAll(long DeviceType, long ConnectionType, long *pNumFound, long *pSerialNumbers, long *pIDs, double *pAddresses);
LJ_ERROR _stdcall ListAllS(const char *pDeviceType, const char *pConnectionType, long *pNumFound, long *pSerialNumbers, long *pIDs, double *pAddresses);
// Returns all the devices found of a given device type and connection type.  pSerialNumbers, pIDs and
// pAddresses must be arrays of the given type at least 128 elements in size.  pNumFound is a single element and will tell you how many
// of those 128 elements have valid data (and therefore the number of units found.  With pAddresses you'll probably want
// to use the DoubleToStringAddress() function below to convert.

LJ_ERROR _stdcall OpenLabJack(long DeviceType, long ConnectionType, const char *pAddress, long FirstFound, LJ_HANDLE *pHandle);
LJ_ERROR _stdcall OpenLabJackS(const char *pDeviceType, const char *pConnectionType, const char *pAddress, long FirstFound, LJ_HANDLE *pHandle);
// Must be called before working with a device.
// DeviceType = The type of LabJack device to open (see device types constants).
// ConnectionType = How to connect to the device, USB or Ethernet presently.  
//     Ethernet only currently supported on the UE9.
// Address = Either the ID or serial number (if USB), or the IP address.  Note this is a string for either.
// FirstFound = If true, then ignore Address and find the first available LabJack.
// pHandle = The handle to use for subsequent functions on this device, or 0 if failed.
//
// This function returns 0 if success and a handle to the open labjack, or an errorcode if failed.

LJ_ERROR _stdcall AddRequest(LJ_HANDLE Handle, long IOType, long Channel, double Value, long x1, double UserData);
LJ_ERROR _stdcall AddRequestS(LJ_HANDLE Handle, const char *pIOType, long Channel, double Value, long x1, double UserData);
LJ_ERROR _stdcall AddRequestSS(LJ_HANDLE Handle, const char *pIOType, const char *pChannel, double Value, long x1, double UserData);
LJ_ERROR _stdcall AddRequestPtr(LJ_HANDLE Handle, long IOType, long Channel, double Value, void *x1, double UserData);
// Function to add to the list of things to do with the next call to Go/GoOne().
// Handle: A handle returned by OpenLabJack().
// IOType: The type of data request (see IO type constants).
// Channel: The channel # on the particular IOType.
// Value: For output channels, the value to set to.
// X1: Optional parameters used by some IOTypes.
// UserData: Data that is kept with the request and returned with the GetFirst and GetNextResult() functions.
// Returns 0 if success, errorcode if not (most likely handle not found).
// NOTE: When you call AddRequest on a particular Handle, all previous data is erased and cannot be retrieved
// by GetResult() until read again.  This is on a device by device basis, so you can call AddRequest() with a different
// handle while a device is busy performing its I/O.
// The long version is used by languages that can't cast a pointer into a double and should only be used in such situations.

LJ_ERROR _stdcall Go(void);
// Called once AddRequest has specified all the items to do.  This function actually does all those things on all devices.
// takes no parameters.  Returns 0 if success, errorcode if not (currently never returns an error, as errors in reading
// individual items are returned when GetResult() is called.

LJ_ERROR _stdcall GoOne(LJ_HANDLE Handle);
// Same as above, but only does requests on the given handle.

LJ_ERROR _stdcall eGet(LJ_HANDLE Handle, long IOType, long Channel, double *pValue, long x1);
LJ_ERROR _stdcall eGetPtr(LJ_HANDLE Handle, long IOType, long Channel, double *pValue, void *x1);
LJ_ERROR _stdcall eGetS(LJ_HANDLE Handle, const char *pIOType, long Channel, double *pValue, long x1);
LJ_ERROR _stdcall eGetSS(LJ_HANDLE Handle, const char *pIOType, const char *pChannel, double *pValue, long x1);
LJ_ERROR _stdcall ePut(LJ_HANDLE Handle, long IOType, long Channel, double Value, long x1);
LJ_ERROR _stdcall ePutS(LJ_HANDLE Handle, const char *pIOType, long Channel, double Value, long x1);
LJ_ERROR _stdcall ePutSS(LJ_HANDLE Handle, const char *pIOType, const char *pChannel, double Value, long x1);

LJ_ERROR _stdcall eGet_DblArray(LJ_HANDLE Handle, long IOType, long Channel, double *pValue, double *x1);
LJ_ERROR _stdcall eGet_U8Array(LJ_HANDLE Handle, long IOType, long Channel, double *pValue, unsigned char *x1);
LJ_ERROR _stdcall eGetS_DblArray(LJ_HANDLE Handle, const char *pIOType, long Channel, double *pValue, double *x1);
LJ_ERROR _stdcall eGetS_U8Array(LJ_HANDLE Handle, const char *pIOType, long Channel, double *pValue, unsigned char *x1);
LJ_ERROR _stdcall eGetSS_DblArray(LJ_HANDLE Handle, const char *pIOType, const char *pChannel, double *pValue, double *x1);
LJ_ERROR _stdcall eGetSS_U8Array(LJ_HANDLE Handle, const char *pIOType, const char *pChannel, double *pValue, unsigned char *x1);

// These functions do AddRequest, Go, and GetResult in one step.  The Get versions are designed for inputs or retrieving parameters
// as it takes a pointer to a double where the result is placed, but can be used for outputs if pValue is preset to the
// desired value.  This is also useful for things like StreamRead where a value is required to be input and a value is
// returned.  The Put versions are designed for outputs or setting configuration parameters and won't return anything but the error code.
// Internally, all the put versions do is call the get function with a pointer set for you.

// You can repetitively call Go() and GoOne() along with GetResult() to repeat the same requests.  Once you call AddRequest()
// once on a particular device, it will clear out the requests on that particular device.

// NOTE: Be careful when using multiple devices and Go(): AddRequest() only clears out the request list on the device handle
// provided.  If, for example, you perform run two requests, one on each of two different devices, and then add a new request on
// one device but not the other and then call Go(), the original request on the second device will be performed again.

LJ_ERROR _stdcall GetResult(LJ_HANDLE Handle, long IOType, long Channel, double *pValue);
LJ_ERROR _stdcall GetResultS(LJ_HANDLE Handle, const char *pIOType, long Channel, double *pValue);
LJ_ERROR _stdcall GetResultSS(LJ_HANDLE Handle, const char *pIOType, const char *pChannel, double *pValue);
// Called to retrieve data and error codes for the things done in Go().  Typically this should be called
// for each element passed with AddRequest, but outputs can be skipped if errorcodes not needed. 
// Handle, IOType, Channel: these should match the parameters passed in AddRequest to retrieve the result for
//     that particular item.
// pValue = Value retrieved for that item.
// Returns 0 if success, LJE_NODATA if no data available for the particular Handle, IOType, and Channel specified,
// or the error code for the particular action performed.
// The long version is used by languages that can't cast a pointer into a double and should only be used in such situations.

LJ_ERROR _stdcall GetFirstResult(LJ_HANDLE Handle, long *pIOType, long *pChannel, double *pValue, long *px1, double *pUserData);
LJ_ERROR _stdcall GetNextResult(LJ_HANDLE Handle, long *pIOType, long *pChannel, double *pValue, long *px1, double *pUserData);
// These GetResult() type functions are designed for situations when you want to get all the results in order.  Call GetFirstResult()
// first to get the first result avialable for the given handle and the error code.  Then call GetNextResult() repeditively for
// subsequent requests. When either function returns LJE_NO_MORE_DATA_AVAILABLE, you're done.  Note that x1 and UserData are always
// returned as well.  You can therefore use UserData as space for your own tracking information, or whatever else you may need.
// Here's a sample of how a loop might work:
// err = GetFirstResult(..)
// while (!err)
// {
//    process result...
//    err = GetNextResult(..)
// }

LJ_ERROR _stdcall eAIN(LJ_HANDLE Handle, long ChannelP, long ChannelN, double *Voltage, long Range, long Resolution, long Settling, long Binary,  long Reserved1, long Reserved2);
LJ_ERROR _stdcall eDAC(LJ_HANDLE Handle, long Channel, double Voltage, long Binary, long Reserved1, long Reserved2);
LJ_ERROR _stdcall eDI(LJ_HANDLE Handle, long Channel, long *State);
LJ_ERROR _stdcall eDO(LJ_HANDLE Handle, long Channel, long State);
LJ_ERROR _stdcall eAddGoGet(LJ_HANDLE Handle, long NumRequests, long *aIOTypes, long *aChannels, double *aValues, long *ax1s, long *aRequestErrors, long *GoError, long *aResultErrors);
LJ_ERROR _stdcall eTCConfig(LJ_HANDLE Handle, long *aEnableTimers, long *aEnableCounters, long TCPinOffset, long TimerClockBaseIndex, long TimerClockDivisor, long *aTimerModes, double *aTimerValues, long Reserved1, long Reserved2);
LJ_ERROR _stdcall eTCValues(LJ_HANDLE Handle, long *aReadTimers, long *aUpdateResetTimers, long *aReadCounters, long *aResetCounters, double *aTimerValues, double *aCounterValues, long Reserved1, long Reserved2);

LJ_ERROR _stdcall eModbus(LJ_HANDLE Handle, long readwrite, long addr, long size, unsigned char *value);

// Prototypes for beta version of easy functions to make simple tasks easier.

// *************************************************
// NOTE: This driver is completely thread safe.  With some very minor exceptions, you can call all these functions from
// multiple threads at the same time and the driver will keep everything straight.  Because of this, you must call Add...(),
// Go...() and Get...() from the same thread for a particular set of requests.  Internally the list of things to do and
// results are split by thread.  This allows you to use multiple threads to make requests without accidently getting
// data from one thread into another.  If you are Adding requests and then getting NO_DATA_AVAILABlE or similar error when
// Getting the results, then chances are you are running in different threads and don't realize this.
// SubNote: The driver tracks which thread a request is made in by the thread ID.  If you kill a thread and then create a new one
// it is possible for the new thread to have the same ID.  Its not really a problem if you call AddRequest first, but if you
// did GetFirstResult on a new thread you may actually get data from the thread that already ended.
// SubNote: As mentioned, the list of requests and results is kept on a thread by thread basis.  Since the driver can't tell
// when a thread has ended, the results are kept in memory for that thread even though the thread is done (thus the reason for the
// above mentioned subnote).  This is not a problem in general as the driver will clean it all up when its unloaded.  When it can
// be a problem is in situations where you are creating and destorying threads continuously.  This will result in the slow consumption
// of memory as requests on old threads are left behind.  Since each request only uses 44 bytes and as mentioned the ID's will
// eventually get recycled, it won't be a huge memory loss, but it will be there.  In general, even without this issue, we strongly
// recommend against creating and destroying a lot of threads.  Its terribly slow and inefficient.  Use thread pools and other
// techniques to keep new thread creation to a minimum.  This is what is done internally,
// **************************************************
// NOTE: Continuing on the thread safety issue, the one big exception to the thread safety of this driver is in the use of
// the windows TerminateThread() function.  As it warns in the MSDN documentation, using TerminateThread() will kill the thread
// without releasing any resources, and more importantly, releasing any synchronization objects.  If you TerminateThread() on a
// thread that is currently in the middle of a call to this driver, you will more than likely leave a synchronization object open
// on the particular device and you will no longer be able to access the device from you application until you restart.  On some
// devices, it can be worse.  On devices that have interprocess synchonization, such as the U12, calling TerminateThread() may
// kill all access to the device through this driver no matter which process is using it and even if you restart your application.
// Just avoid using TerminateThread()!  All device calls have a timeout, which defaults to 1 second, but is changeable.  Make sure
// when you are waiting for the driver that you wait at least as long as the timeout for it to finish.
// **************************************************

LJ_ERROR _stdcall ResetLabJack(LJ_HANDLE Handle);


// Error functions:

LJ_ERROR _stdcall GetNextError(LJ_HANDLE Handle, long *pIOType, long *pChannel);
// Instead of having to check every GetResult for errors, you can instead repetitively call GetNextError to retrieve all the errors
// from a particular Go.  This includes eGet, ePut, etc if you want to use the same error handling routine.  This function will
// return an error with each call along with the IOType and channel that caused the error.  If there are no errors or no errors left
// it will return LJE_NOERROR.

LJ_ERROR _stdcall GetStreamError(LJ_HANDLE Handle);
// This function allows you to quickly determine if there is a stream error on a particular device. It only applies when
// stream is running.  This is especially useful if you are using a StreamCallback function, which will pass -1 in as scansavailable
// if there is an error.


// Useful functions:

// Some config functions require IP address in a double instead of a string.  Here are some helpful functions:
LJ_ERROR _stdcall DoubleToStringAddress(double Number, char *pString, long HexDot);
// Takes the given IP address in number notation (32 bit), and puts it into the String.  String should be
// preallocated to at least 16 bytes (255.255.255.255\0).

LJ_ERROR _stdcall StringToDoubleAddress(const char *pString, double *pNumber, long HexDot);
// Does the opposite, takes an IP address in dot notation and puts it into the given double.

long _stdcall StringToConstant(const char *pString);
// Converts the given string to the appropriate constant.  Used internally by the S functions, but could be useful if
// you wanted to use the GetFirst/Next functions and couldn't use this header.  Then you could do a comparison on the
// returned values:
//
// if (IOType == StringToConstant("LJ_ioGET_AIN"))
//
// This function returns LJ_INVALID_CONSTANT if an invalid constant.

void _stdcall ErrorToString(LJ_ERROR ErrorCode, char *pString);
// Returns a string describing the given error code or an empty string if not found.  pString must be at least 256 chars in length.

double _stdcall GetDriverVersion(void);
// Returns the version number of this driver, the resultant number should match the number at the top of this header file.

unsigned long _stdcall GetThreadID(void);
// Returns the ID of the current thread.


LJ_ERROR _stdcall TCVoltsToTemp( long TCType, double TCVolts, double CJTempK, double *pTCTempK);
// Utility function to convert voltage readings from thermocouples to temperatures.  Use the LJ_tt constants
// to specify a thermocouple type.  Types B, E, J, K, N, R, S and T are supported.  TC voltes is the
// measured voltage.  CJTempK is the cold junction temperature in kelvin and the output is placed into pTCTempK.


// **************************************************
// Constants
// **************************************************

// Device types:
const long LJ_dtUE9 = 9;
const long LJ_dtU3 = 3;
const long LJ_dtU6 = 6;
const long LJ_dtSMB = 1000;


// Connection types:
const long LJ_ctUSB = 1; // UE9 + U3 + U6
const long LJ_ctETHERNET = 2; // UE9 only
const long LJ_ctETHERNET_MB = 3; // Modbus over Ethernet.  UE9 only.
const long LJ_ctETHERNET_DATA_ONLY = 4; // Opens data port but not stream port.  UE9 only.


// Raw connection types are used to open a device but not communicate with it
// should only be used if the normal connection types fail and for testing.
// If a device is opened with the raw connection types, only LJ_ioRAW_OUT
// and LJ_ioRAW_IN io types should be used.
const long LJ_ctUSB_RAW = 101; // UE9 + U3 + U6
const long LJ_ctETHERNET_RAW = 102; // UE9 only


// IO types:
const long LJ_ioGET_AIN = 10; // UE9 + U3 + U6.  This is single ended version.
const long LJ_ioGET_AIN_DIFF = 15; // U3/U6 only.  Put negative channel in x1.  If 32 is passed as x1, Vref will be added to the result.
const long LJ_ioGET_AIN_ADVANCED = 16; // For testing purposes. 

const long LJ_ioPUT_AIN_RANGE = 2000; // UE9 + U6
const long LJ_ioGET_AIN_RANGE = 2001; // UE9 only

// Sets or reads the analog or digital mode of the FIO and EIO pins.  FIO is Channel 0-7, EIO 8-15.
const long LJ_ioPUT_ANALOG_ENABLE_BIT = 2013; // U3 only
const long LJ_ioGET_ANALOG_ENABLE_BIT = 2014; // U3 only
// Sets or reads the analog or digital mode of the FIO and EIO pins. Channel is starting
// bit #, x1 is number of bits to read. The pins are set by passing a bitmask as a double
// for the value.  The first bit of the int that the double represents will be the setting
// for the pin number sent into the channel variable. 
const long LJ_ioPUT_ANALOG_ENABLE_PORT = 2015; // U3 only
const long LJ_ioGET_ANALOG_ENABLE_PORT = 2016; // U3 only

const long LJ_ioPUT_DAC = 20; // UE9 + U3 + U6
const long LJ_ioPUT_DAC_ENABLE = 2002; // UE9 + U3 (U3 on Channel 1 only)
const long LJ_ioGET_DAC_ENABLE = 2003; // UE9 + U3 (U3 on Channel 1 only)

const long LJ_ioGET_DIGITAL_BIT = 30; // UE9 + U3 + U6.  Changes direction of bit to input as well.
const long LJ_ioGET_DIGITAL_BIT_DIR = 31; // UE9 + U3 + U6
const long LJ_ioGET_DIGITAL_BIT_STATE = 32; // UE9 + U3 + U6.  Does not change direction of bit, allowing readback of output.

// Channel is starting bit #, x1 is number of bits to read.
const long LJ_ioGET_DIGITAL_PORT = 35; // UE9 + U3 + U6  // changes direction of bits to input as well.
const long LJ_ioGET_DIGITAL_PORT_DIR = 36; // UE9 + U3 + U6
const long LJ_ioGET_DIGITAL_PORT_STATE = 37; //UE9 + U3 + U6 // does not change direction of bits, allowing readback of output.

// Digital put commands will set the specified digital line(s) to output.
const long LJ_ioPUT_DIGITAL_BIT = 40; // UE9 + U3 + U6
// Channel is starting bit #, value is output value, x1 is bits to write.
const long LJ_ioPUT_DIGITAL_PORT = 45; // UE9 + U3 + U6

// Used to create a pause between two events in a U3 low-level feedback
// command.  For example, to create a 100 ms positive pulse on FIO0, add a
// request to set FIO0 high, add a request for a wait of 100000, add a
// request to set FIO0 low, then Go.  Channel is ignored.  Value is
// microseconds to wait and should range from 0 to 8388480.  The actual
// resolution of the wait is 128 microseconds.  On U3 hardware version
// 1.20 the resolution and delay times are doubled.
const long LJ_ioPUT_WAIT = 70; // U3 + U6

// Counter.  Input only.
const long LJ_ioGET_COUNTER = 50; // UE9 + U3 + U6

const long LJ_ioPUT_COUNTER_ENABLE = 2008; // UE9 + U3 + U6
const long LJ_ioGET_COUNTER_ENABLE = 2009; // UE9 + U3 + U6

// This will cause the designated counter to reset.  If you want to reset the counter with
// every read, you have to use this command every time.
const long LJ_ioPUT_COUNTER_RESET = 2012;  // UE9 + U3 + U6

// On UE9: timer only used for input. Output Timers don't use these.  Only Channel used.
// On U3/U6: Channel used (0 or 1).
const long LJ_ioGET_TIMER = 60; // UE9 + U3 + U6

const long LJ_ioPUT_TIMER_VALUE = 2006; // UE9 + U3 + U6.  Value gets new value.
const long LJ_ioPUT_TIMER_MODE = 2004; // UE9 + U3 + U6.  On both Value gets new mode.
const long LJ_ioGET_TIMER_MODE = 2005; // UE9 + U3 + U6

// IOType for use with SHT sensor.  For LJ_ioSHT_GET_READING, a channel of LJ_chSHT_TEMP (5000) will 
// read temperature, and LJ_chSHT_RH (5001) will read humidity.
const long LJ_ioSHT_GET_READING = 500; // UE9 + U3 + U6

// Uses settings from LJ_chSPI special channels (set with LJ_ioPUT_CONFIG) to communcaite with
// something using an SPI interface.  The value parameter is the number of bytes to transfer
// and x1 is the address of the buffer.  The data from the buffer will be sent, then overwritten
// with the data read.  The channel parameter is ignored.
const long LJ_ioSPI_COMMUNICATION = 503; // UE9 + U3 + U6

const long LJ_ioI2C_COMMUNICATION = 504; // UE9 + U3 + U6
const long LJ_ioASYNCH_COMMUNICATION = 505; // UE9 + U3 + U6
const long LJ_ioTDAC_COMMUNICATION = 506; // UE9 + U3 + U6

// Sets the original configuration.
// On U3: This means sending the following to the ConfigIO and TimerClockConfig low level
// functions.
//
// ConfigIO
// Byte #
// 6       WriteMask       15      Write all parameters.
// 8       TimerCounterConfig      0       No timers/counters.  Offset=0.
// 9       DAC1Enable      0       DAC1 disabled.
// 10      FIOAnalog       0       FIO all digital.
// 11      EIOAnalog       0       EIO all digital.
//
//
// TimerClockConfig
// Byte #
// 8       TimerClockConfig        130     Set clock to 48 MHz. (24 MHz for U3 hardware version 1.20 or less)
// 9       TimerClockDivisor       0       Divisor = 0.
//
// On UE9/U6: This means disabling all timers and counters, setting the TimerClockConfig to
// default (750 kHZ for UE9, 48 MHz for U6) and the offset to 0 (U6).
//
const long LJ_ioPIN_CONFIGURATION_RESET = 2017; // UE9 + U3 + U6

// The raw in/out are unusual, channel number corresponds to the particular comm port, which 
// depends on the device.  For example, on the UE9, 0 is main comm port, and 1 is the streaming comm.
// Make sure and pass a porter to a char buffer in x1, and the number of bytes desired in value.  A call 
// to GetResult will return the number of bytes actually read/written.  The max you can send out in one call
// is 512 bytes to the UE9 and 16384 bytes to the U3.
const long LJ_ioRAW_OUT = 100; // UE9 + U3 + U6
const long LJ_ioRAW_IN = 101; // UE9 + U3 + U6

const long LJ_ioRAWMB_OUT = 104; // UE9 Only.  Used with LJ_ctETHERNET_MB to send raw modbus commands to the modbus TCP/IP Socket.
const long LJ_ioRAWMB_IN = 105;  // UE9 only.  Used with LJ_ctETHERNET_MB to receive raw modbus responses from the modbus TCP/IP Socket.

// Sets the default power up settings based on the current settings of the device AS THIS DLL KNOWS.  This last part
// basically means that you should set all parameters directly through this driver before calling this.  This writes
// to flash which has a limited lifetime, so do not do this too often.  Rated endurance is 20,000 writes.
const long LJ_ioSET_DEFAULTS = 103; // U3 + U6

// Requests to create the list of channels to stream.  Usually you will use the CLEAR_STREAM_CHANNELS request first, which
// will clear any existing channels, then use ADD_STREAM_CHANNEL multiple times to add your desired channels.  Note that
// you can do CLEAR, and then all your ADDs in a single Go() as long as you add the requests in order.
const long LJ_ioADD_STREAM_CHANNEL = 200; // UE9 + U3 + U6
// Put negative channel in x1.  If 32 is passed as x1, Vref will be added to the result.
const long LJ_ioADD_STREAM_CHANNEL_DIFF = 206; // U3 + U6

const long LJ_ioCLEAR_STREAM_CHANNELS = 201; // UE9 + U3 + U6
const long LJ_ioSTART_STREAM = 202; // UE9 + U3 + U6
const long LJ_ioSTOP_STREAM = 203; // UE9 + U3 + U6
 
const long LJ_ioADD_STREAM_DAC = 207; //UE9 only

// Get stream data has several options.  If you just want to get a single channel's data (if streaming multiple channels), you
// can pass in the desired channel #, then the number of data points desired in Value, and a pointer to an array to put the
// data into as X1.  This array needs to be an array of doubles. Therefore, the array needs to be 8 * number of
// requested data points in byte length. What is returned depends on the StreamWaitMode.  If None, this function will only return
// data available at the time of the call.  You therefore must call GetResult() for this function to retrieve the actually number
// of points retreived.  If Pump or Sleep, it will return only when the appropriate number of points have been read or no
// new points arrive within 100ms.  Since there is this timeout, you still need to use GetResult() to determine if the timeout
// occured.  If AllOrNone, you again need to check GetResult.
//
// You can also retreive the entire scan by passing LJ_chALL_CHANNELS.  In this case, the Value determines the number of SCANS
// returned, and therefore, the array must be 8 * number of scans requested * number of channels in each scan.  Likewise
// GetResult() will return the number of scans, not the number of data points returned.
//
// Note: data is stored interleaved across all streaming channels.  In other words, if you are streaming two channels, 0 and 1,
// and you request LJ_chALL_CHANNELS, you will get, Channel0, Channel1, Channel0, Channel1, etc.  Once you have requested the
// data, any data returned is removed from the internal buffer, and the next request will give new data.
//
// Note: if reading the data channel by channel and not using LJ_chALL_CHANNELS, the data is not removed from the internal buffer
// until the data from the last channel in the scan is requested.  This means that if you are streaming three channels, 0, 1 and 2,
// and you request data from channel 0, then channel 1, then channel 0 again, the request for channel 0 the second time will
// return the exact same data.  Also note, that the amount of data that will be returned for each channel request will be
// the same until you've read the last channel in the scan, at which point your next block may be a different size.
//
// Note: although more convenient, requesting individual channels is slightly slower then using LJ_chALL_CHANNELS.  Since you
// are probably going to have to split the data out anyway, we have saved you the trouble with this option.
//
// Note: if you are only scanning one channel, the Channel parameter is ignored.

const long LJ_ioGET_STREAM_DATA = 204; // UE9 + U3 + U6

// Since this driver has to start a thread to poll the hardware in stream mode, we give you the option of having this thread
// call a user defined function after a block of data has been read.  To use, pass a pointer to the following function type as
// Channel.  Pass 0 to turn this feature off.  X1 is a user definable value that is passed when the callback function is called.
// Value is the number of scans before the callback occurs.  If 0, then the default is used, which varies depending on the
// device and scan rate but typically results in the callback being called about every 10ms.

/* example:
void StreamCallback(long ScansAvailable, long UserValue)
{
   ... do stuff when a callback occurs (retrieve data for example)
}

...
   tStreamCallback pCallback = StreamCallback;
   AddRequest(Handle,LJ_ioSET_STREAM_CALLBACK,(long)pCallback,0,0,0);
...
*/
// NOTE: the callback function is called from a secondary worker thread that has no message pump.  Do not directly update any
// windows controls from your callback as there is no message pump to do so.  On the same note, the driver stream buffer is locked
// during the callback function.  This means that while your callback function is running, you cannot do a GetStreamData or any
// other stream function from a DIFFERENT thread.  You can of course do getstreamdata from the callback function.  You don't really
// have to worry about this much as any other thread trying to do GetStreamData or other function will simply stall until your
// callback is complete and the lock released.  Where you will run into trouble is if your callback function waits on another thread
// that is trying to do a stream function.  Note this applies on a device by device basis.

typedef void (*tStreamCallback)(long ScansAvailable, long UserValue);
const long LJ_ioSET_STREAM_CALLBACK = 205; // UE9 + U3 + U6
const long LJ_ioSET_STREAM_CALLBACK_PTR = 260; // UE9 + U3 + U6. This is for 64-bit compatibility.  The tStreamCallback function is passed as X1 and channel is the User Variable.

// Channel = 0 buzz for a count, Channel = 1 buzz continuous.
// Value is the Period.
// X1 is the toggle count when channel = 0.
const long LJ_ioBUZZER = 300; // U3 only

// There are a number of things that happen asynchronously inside the UD driver and so don't directly associate with a Add, Go, or
// Get function.  To allow the UD to tell your code when these events occur, you can pass it a callback function that will
// be called whenever this occurs.  For an example of how to use the callback, look at the StreamCallback above.  Like the
// streamcallback, the function may be called from any thread, maybe one with a message pump, maybe not, so you should make
// your function fast and don't do anything with windows or on screen controls.
typedef void (*tEventCallback)(long EventCode, long Data1, long Data2, long Data3, long UserValue);
// Pass pointer to function as Channel, and userdata as x1.  Value not used, but should be set to 0 for forward
// compatibility.  If more events are required with future LabJacks, we may use this to allow you to specify which
// events you wish to receive notifcation of and 0 will mean All.
const long LJ_ioSET_EVENT_CALLBACK = 400; // UE9 + U3 + U6

// These are the possible EventCodes that will be passed:
const long LJ_ecDISCONNECT = 1; // Called when the device is unplugged from USB.  No Data is passed.
const long LJ_ecRECONNECT = 2; // Called when the device is reconnected to USB.  No Data is passed.
const long LJ_ecSTREAMERROR = 4; // Called when a stream error occurs. Data1 is errorcode, Data2 and Data3 are not used.
// Future events will be power of 2.

// Config iotypes:
const long LJ_ioPUT_CONFIG = 1000; // UE9 + U3 + U6
const long LJ_ioGET_CONFIG = 1001; // UE9 + U3 + U6


// Channel numbers used for CONFIG types:
const long LJ_chLOCALID = 0; // UE9 + U3 + U6
const long LJ_chHARDWARE_VERSION = 10; // UE9 + U3 + U6 (Read Only)
const long LJ_chSERIAL_NUMBER = 12; // UE9 + U3 + U6 (Read Only)
const long LJ_chFIRMWARE_VERSION = 11; // UE9 + U3 + U6 (Read Only)
const long LJ_chBOOTLOADER_VERSION = 15; // UE9 + U3 + U6 (Read Only)
const long LJ_chPRODUCTID = 8; // UE9 + U3 + U6 (Read Only)

// UE9 specific:
const long LJ_chCOMM_POWER_LEVEL = 1; // UE9
const long LJ_chIP_ADDRESS = 2; // UE9
const long LJ_chGATEWAY = 3; // UE9
const long LJ_chSUBNET = 4; // UE9
const long LJ_chPORTA = 5; // UE9
const long LJ_chPORTB = 6; // UE9
const long LJ_chDHCP = 7; // UE9
const long LJ_chMACADDRESS = 9; // UE9
const long LJ_chCOMM_FIRMWARE_VERSION = 11; // UE9
const long LJ_chCONTROL_POWER_LEVEL = 13; // UE9
const long LJ_chCONTROL_FIRMWARE_VERSION = 14; // UE9 (Read Only)
const long LJ_chCONTROL_BOOTLOADER_VERSION = 15; // UE9 (Read Only)
const long LJ_chCONTROL_RESET_SOURCE = 16; // UE9 (Read Only)
const long LJ_chUE9_PRO = 19; // UE9 (Read Only)

const long LJ_chLED_STATE = 17; // U3 + U6.  Sets the state of the LED.  Value = LED state.
const long LJ_chSDA_SCL = 18; // U3 + U6.  Enable/disable SDA/SCL as digital I/O.

const long LJ_chU3HV = 22; // U3 only (Read Only).  Value will be 1 for a U3-HV and 0 for a U3-LV or a U3 with hardware version < 1.30.

const long LJ_chU6_PRO = 23; // U6 only.

// Driver related:
// Number of milliseconds that the driver will wait for communication to complete.
const long LJ_chCOMMUNICATION_TIMEOUT = 20; // For all command-response calls, this is the time after a command that the driver will wait for a response.  USB Default = 1500 ms.  Ethernet Default = 500 ms.
const long LJ_chSTREAM_COMMUNICATION_TIMEOUT = 21; // This affects the background stream read thread in the driver.  This is the maximum time that can pass between 2 stream data packets from the device.  Default = 500 ms.

// Used to access calibration and user data.  The address of an array is passed in as x1.
// For the UE9, a 1024-element buffer of bytes is passed for user data and a 128-element
// buffer of doubles is passed for cal constants.
// For the U3/U3-LV, a 256-element buffer of bytes is passed for user data and a 12-element
// buffer of doubles is passed for cal constants.
// For the U3-HV, a 256-element buffer of bytes is passed for user data and a 20-element
// buffer of doubles is passed for cal constants.
// For the U6, a 256-element buffer of bytes is passed for user data and a 64-element
// buffer of doubles is passed for cal constants.
// The layout of cal constants are defined in the users guide for each device.
// When the LJ_chCAL_CONSTANTS special channel is used with PUT_CONFIG, a
// special value (0x4C6C) must be passed in to the Value parameter. This makes it
// more difficult to accidently erase the cal constants.  In all other cases the Value
// parameter is ignored.
const long LJ_chCAL_CONSTANTS = 400; // UE9 + U3 + U6
const long LJ_chUSER_MEM = 402; // UE9 + U3 + U6

// Used to write and read the USB descriptor strings.  This is generally for OEMs
// who wish to change the strings.
// Pass the address of an array in x1.  Value parameter is ignored.
// The array should be 128 elements of bytes.  The first 64 bytes are for the
// iManufacturer string, and the 2nd 64 bytes are for the iProduct string.
// The first byte of each 64 byte block (bytes 0 and 64) contains the number
// of bytes in the string.  The second byte (bytes 1 and 65) is the USB spec
// value for a string descriptor (0x03).  Bytes 2-63 and 66-127 contain unicode
// encoded strings (up to 31 characters each).
const long LJ_chUSB_STRINGS = 404; // U3 + U6

// Timer/counter related:
const long LJ_chNUMBER_TIMERS_ENABLED = 1000; // UE9 + U3 + U6
const long LJ_chTIMER_CLOCK_BASE = 1001; // UE9 + U3 + U6
const long LJ_chTIMER_CLOCK_DIVISOR = 1002; // UE9 + U3 + U6
const long LJ_chTIMER_COUNTER_PIN_OFFSET = 1003; // U3 + U6

// AIn related:
const long LJ_chAIN_RESOLUTION = 2000; // UE9 + U3 + U6
const long LJ_chAIN_SETTLING_TIME = 2001; // UE9 + U3 + U6
const long LJ_chAIN_BINARY = 2002; // UE9 + U3 + U6

// DAC related:
const long LJ_chDAC_BINARY = 3000; // UE9 + U3

// SHT related: 
// LJ_chSHT_TEMP and LJ_chSHT_RH are used with LJ_ioSHT_GET_READING to read those values.
// The LJ_chSHT_DATA_CHANNEL and LJ_chSHT_SCK_CHANNEL constants use the passed value 
// to set the appropriate channel for the data and SCK lines for the SHT sensor. 
// Default digital channels are FIO0 for the data channel and FIO1 for the clock channel. 
const long LJ_chSHT_TEMP = 5000; // UE9 + U3 + U6
const long LJ_chSHT_RH = 5001; // UE9 + U3 + U6
const long LJ_chSHT_DATA_CHANNEL = 5002; // UE9 + U3 + U6. Default is FIO0
const long LJ_chSHT_CLOCK_CHANNEL = 5003; // UE9 + U3 + U6. Default is FIO1

// SPI related:
const long LJ_chSPI_AUTO_CS = 5100; // UE9 + U3 + U6
const long LJ_chSPI_DISABLE_DIR_CONFIG = 5101; // UE9 + U3 + U6
const long LJ_chSPI_MODE = 5102; // UE9 + U3 + U6
const long LJ_chSPI_CLOCK_FACTOR = 5103; // UE9 + U3 + U6
const long LJ_chSPI_MOSI_PIN_NUM = 5104; // UE9 + U3 + U6
const long LJ_chSPI_MISO_PIN_NUM = 5105; // UE9 + U3 + U6
const long LJ_chSPI_CLK_PIN_NUM = 5106; // UE9 + U3 + U6
const long LJ_chSPI_CS_PIN_NUM = 5107; // UE9 + U3 + U6

// I2C related:
// Used with LJ_ioPUT_CONFIG
const long LJ_chI2C_ADDRESS_BYTE = 5108; // UE9 + U3 + U6
const long LJ_chI2C_SCL_PIN_NUM = 5109; // UE9 + U3 + U6
const long LJ_chI2C_SDA_PIN_NUM = 5110; // UE9 + U3 + U6
const long LJ_chI2C_OPTIONS = 5111; // UE9 + U3 + U6
const long LJ_chI2C_SPEED_ADJUST = 5112; // UE9 + U3 + U6

// Used with LJ_ioI2C_COMMUNICATION:
const long LJ_chI2C_READ = 5113; // UE9 + U3 + U6
const long LJ_chI2C_WRITE = 5114; // UE9 + U3 + U6
const long LJ_chI2C_GET_ACKS = 5115; // UE9 + U3 + U6
const long LJ_chI2C_WRITE_READ = 5130; // UE9 + U3 + U6

// ASYNCH related:
// Used with LJ_ioASYNCH_COMMUNICATION
const long LJ_chASYNCH_RX = 5117; // UE9 + U3 + U6
const long LJ_chASYNCH_TX = 5118; // UE9 + U3 + U6
const long LJ_chASYNCH_FLUSH = 5128; // UE9 + U3 + U6
const long LJ_chASYNCH_ENABLE = 5129; // UE9 + U3 + U6

// Used with LJ_ioPUT_CONFIG and LJ_ioGET_CONFIG
const long LJ_chASYNCH_BAUDFACTOR = 5127; // UE9 + U3 + U6

// LJ TickDAC related:
const long LJ_chTDAC_SCL_PIN_NUM = 5119; // UE9 + U3 + U6: Used with LJ_ioPUT_CONFIG.
// Used with LJ_ioTDAC_COMMUNICATION
const long LJ_chTDAC_SERIAL_NUMBER = 5120; // UE9 + U3 + U6: Read only.
const long LJ_chTDAC_READ_USER_MEM = 5121; // UE9 + U3 + U6
const long LJ_chTDAC_WRITE_USER_MEM = 5122; // UE9 + U3 + U6
const long LJ_chTDAC_READ_CAL_CONSTANTS = 5123; // UE9 + U3 + U6
const long LJ_chTDAC_WRITE_CAL_CONSTANTS = 5124; // UE9 + U3 + U6
const long LJ_chTDAC_UPDATE_DACA = 5125; // UE9 + U3 + U6
const long LJ_chTDAC_UPDATE_DACB = 5126; // UE9 + U3 + U6

// Stream related.  Note: Putting to any of these values will stop any running streams.
const long LJ_chSTREAM_SCAN_FREQUENCY = 4000; // UE9 + U3 + U6
const long LJ_chSTREAM_BUFFER_SIZE = 4001;
const long LJ_chSTREAM_CLOCK_OUTPUT = 4002; // UE9 only
const long LJ_chSTREAM_EXTERNAL_TRIGGER = 4003; // UE9 only
const long LJ_chSTREAM_WAIT_MODE = 4004;
const long LJ_chSTREAM_DISABLE_AUTORECOVERY = 4005; // U3 + U6
const long LJ_chSTREAM_SAMPLES_PER_PACKET = 4108; // UE9 + U3 + U6. Read only for UE9.
const long LJ_chSTREAM_READS_PER_SECOND = 4109;
const long LJ_chAIN_STREAM_SETTLING_TIME = 4110; // U6 only

// Read only stream related
const long LJ_chSTREAM_BACKLOG_COMM = 4105; // UE9 + U3 + U6
const long LJ_chSTREAM_BACKLOG_CONTROL = 4106; // UE9 only
const long LJ_chSTREAM_BACKLOG_UD = 4107;

// Special channel numbers
const long LJ_chALL_CHANNELS = -1;
const long LJ_INVALID_CONSTANT = -999;


//Thermocouple Type constants.
const long LJ_ttB = 6001;
const long LJ_ttE = 6002;
const long LJ_ttJ = 6003;
const long LJ_ttK = 6004;
const long LJ_ttN = 6005;
const long LJ_ttR = 6006;
const long LJ_ttS = 6007;
const long LJ_ttT = 6008;


// Other constants:

// Ranges (not all are supported by all devices):
const long LJ_rgBIP20V = 1;   // -20V to +20V
const long LJ_rgBIP10V = 2;   // -10V to +10V
const long LJ_rgBIP5V = 3;    // -5V to +5V
const long LJ_rgBIP4V = 4;    // -4V to +4V
const long LJ_rgBIP2P5V = 5;  // -2.5V to +2.5V
const long LJ_rgBIP2V = 6;    // -2V to +2V
const long LJ_rgBIP1P25V = 7; // -1.25V to +1.25V
const long LJ_rgBIP1V = 8;    // -1V to +1V
const long LJ_rgBIPP625V = 9; // -0.625V to +0.625V
const long LJ_rgBIPP1V = 10;  // -0.1V to +0.1V
const long LJ_rgBIPP01V = 11; // -0.01V to +0.01V

const long LJ_rgUNI20V = 101;    // 0V to +20V
const long LJ_rgUNI10V = 102;    // 0V to +10V
const long LJ_rgUNI5V = 103;     // 0V to +5V
const long LJ_rgUNI4V = 104;     // 0V to +4V
const long LJ_rgUNI2P5V = 105;   // 0V to +2.5V
const long LJ_rgUNI2V = 106;     // 0V to +2V
const long LJ_rgUNI1P25V = 107;  // 0V to +1.25V
const long LJ_rgUNI1V = 108;     // 0V to +1V
const long LJ_rgUNIP625V = 109;  // 0V to +0.625V
const long LJ_rgUNIP5V = 110;    // 0V to +0.500V
const long LJ_rgUNIP25V = 112;   // 0V to +0.25V
const long LJ_rgUNIP3125V = 111; // 0V to +0.3125V
const long LJ_rgUNIP025V = 113;  // 0V to +0.025V
const long LJ_rgUNIP0025V = 114; // 0V to +0.0025V


// Timer modes:
const long LJ_tmPWM16 = 0; // 16 bit PWM
const long LJ_tmPWM8 = 1; // 8 bit PWM
const long LJ_tmRISINGEDGES32 = 2; // 32-bit rising to rising edge measurement
const long LJ_tmFALLINGEDGES32 = 3; // 32-bit falling to falling edge measurement
const long LJ_tmDUTYCYCLE = 4; // Duty cycle measurement
const long LJ_tmFIRMCOUNTER = 5; // Firmware based rising edge counter
const long LJ_tmFIRMCOUNTERDEBOUNCE = 6; // Firmware counter with debounce
const long LJ_tmFREQOUT = 7; // Frequency output
const long LJ_tmQUAD = 8; // Quadrature
const long LJ_tmTIMERSTOP = 9; // Stops another timer after n pulses
const long LJ_tmSYSTIMERLOW = 10; // Read lower 32-bits of system timer
const long LJ_tmSYSTIMERHIGH = 11; // Read upper 32-bits of system timer
const long LJ_tmRISINGEDGES16 = 12; // 16-bit rising to rising edge measurement
const long LJ_tmFALLINGEDGES16 = 13; // 16-bit falling to falling edge measurement
const long LJ_tmLINETOLINE = 14; // Line to Line measurement


// Timer clocks:
const long LJ_tc750KHZ = 0; // UE9: 750 kHz 
const long LJ_tcSYS = 1;    // UE9 + U3 + U6: System clock

const long LJ_tc2MHZ = 10;       // U3: Hardware Version 1.20 or lower
const long LJ_tc6MHZ = 11;       // U3: Hardware Version 1.20 or lower
const long LJ_tc24MHZ = 12;      // U3: Hardware Version 1.20 or lower
const long LJ_tc500KHZ_DIV = 13; // U3: Hardware Version 1.20 or lower
const long LJ_tc2MHZ_DIV = 14;   // U3: Hardware Version 1.20 or lower
const long LJ_tc6MHZ_DIV = 15;   // U3: Hardware Version 1.20 or lower
const long LJ_tc24MHZ_DIV = 16;  // U3: Hardware Version 1.20 or lower

const long LJ_tc4MHZ = 20;      // U3: Hardware Version 1.21 or higher, + U6
const long LJ_tc12MHZ = 21;     // U3: Hardware Version 1.21 or higher, + U6
const long LJ_tc48MHZ = 22;     // U3: Hardware Version 1.21 or higher, + U6
const long LJ_tc1MHZ_DIV = 23;  // U3: Hardware Version 1.21 or higher, + U6
const long LJ_tc4MHZ_DIV = 24;  // U3: Hardware Version 1.21 or higher, + U6
const long LJ_tc12MHZ_DIV = 25; // U3: Hardware Version 1.21 or higher, + U6
const long LJ_tc48MHZ_DIV = 26; // U3: Hardware Version 1.21 or higher, + U6


// Stream wait modes:
const long LJ_swNONE = 1; // No wait, return whatever is available.
const long LJ_swALL_OR_NONE = 2; // No wait, but if all points requested aren't available, return none.
const long LJ_swPUMP = 11; // Wait and pump the message pump.  Preferred when called from primary thread (if you don't know
                           // if you are in the primary thread of your app then you probably are.  Do not use in worker
                           // secondary threads (i.e. ones without a message pump).
const long LJ_swSLEEP = 12; // Wait by sleeping (don't do this in the primary thread of your app, or it will temporarily 
                            // hang).  This is usually used in worker secondary threads.


// BETA CONSTANTS:
// Please note that specific usage of these constants and their values might change.

// SWDT:
// Sets parameters used to control the software watchdog option.  The device is only
// communicated with and updated when LJ_ioSWDT_CONFIG is used with LJ_chSWDT_ENABLE
// or LJ_chSWDT_DISABLE.  Thus, to change a value, you must use LJ_io_PUT_CONFIG
// with the appropriate channel constant so set the value inside the driver, then call
// LJ_ioSWDT_CONFIG to enable that change. 
const long LJ_ioSWDT_CONFIG = 507; // UE9 + U3 + U6: Use with LJ_chSWDT_ENABLE or LJ_chSWDT_DISABLE.
const long LJ_ioSWDT_STROKE = 508; // UE9 only: Used when SWDT_STRICT_ENABLE is turned on to renew the watchdog. 

const long LJ_chSWDT_ENABLE = 5200; // UE9 + U3 + U6: Used with LJ_ioSWDT_CONFIG to enable watchdog.  Value paramter is number of seconds to trigger.
const long LJ_chSWDT_DISABLE = 5201; // UE9 + U3 + U6: Used with LJ_ioSWDT_CONFIG to disable watchdog.

// Used with LJ_io_PUT_CONFIG
const long LJ_chSWDT_RESET_DEVICE = 5202; // U3 + U6: Reset U3 on watchdog reset.  Write only.
const long LJ_chSWDT_RESET_COMM = 5203; // UE9 only: Reset Comm on watchdog reset.  Write only.
const long LJ_chSWDT_RESET_CONTROL = 5204; // UE9 only: Reset Control on watchdog trigger.  Write only.
const long LJ_chSWDT_UPDATE_DIOA = 5205; // UE9 + U3 + U6: Update DIO0 settings after reset.  Write only.
const long LJ_chSWDT_UPDATE_DIOB = 5206; // UE9 only: Update DIO1 settings after reset.  Write only.
const long LJ_chSWDT_DIOA_CHANNEL = 5207; // UE9 + U3 + U6: DIO0 channel to be set after reset.  Write only.
const long LJ_chSWDT_DIOA_STATE = 5208; // UE9 + U3 + U6: DIO0 state to be set after reset.  Write only.
const long LJ_chSWDT_DIOB_CHANNEL = 5209; // UE9: DIO1 channel to be set after reset.  Write only.
const long LJ_chSWDT_DIOB_STATE = 5210; // UE9: DIO1 state to be set after reset.  Write only.
const long LJ_chSWDT_UPDATE_DAC0 = 5211; // UE9 only: Update DAC0 settings after reset.  Write only.
const long LJ_chSWDT_UPDATE_DAC1 = 5212; // UE9 only: Update DAC1 settings after reset.  Write only.
const long LJ_chSWDT_DAC0 = 5213; // UE9 only: Voltage to set DAC0 at on watchdog reset.  Write only.
const long LJ_chSWDT_DAC1 = 5214; // UE9 only: Voltage to set DAC1 at on watchdog reset.  Write only.
const long LJ_chSWDT_DAC_ENABLE = 5215; // UE9 only: Enable DACs on watchdog reset.  Default is true.  Both DACs are enabled or disabled togeather.  Write only.
const long LJ_chSWDT_STRICT_ENABLE = 5216; // UE9 only: Watchdog will only renew with LJ_ioSWDT_STROKE command.
const long LJ_chSWDT_INITIAL_ROLL_TIME = 5217; // UE9 only: Watchdog timer for the first cycle when powered on, after watchdog triggers a reset the normal value is used.  Set to 0 to disable.

// END BETA CONSTANTS


// Error codes:  These will always be in the range of -1000 to 3999 for labView compatibility (+6000).
const LJ_ERROR LJE_NOERROR = 0;
const LJ_ERROR LJE_COMMAND_LIST_ERROR = 1; 
const LJ_ERROR LJE_INVALID_CHANNEL_NUMBER = 2; // Cccurs when a channel that doesn't exist is specified (i.e. DAC #2 on a UE9), or data from streaming is requested on a channel that isn't streaming.
const LJ_ERROR LJE_INVALID_RAW_INOUT_PARAMETER = 3;
const LJ_ERROR LJE_UNABLE_TO_START_STREAM = 4;
const LJ_ERROR LJE_UNABLE_TO_STOP_STREAM = 5;
const LJ_ERROR LJE_NOTHING_TO_STREAM = 6;
const LJ_ERROR LJE_UNABLE_TO_CONFIG_STREAM = 7;
const LJ_ERROR LJE_BUFFER_OVERRUN = 8; // Cccurs when stream buffer overruns (this is the driver buffer not the hardware buffer).  Stream is stopped.
const LJ_ERROR LJE_STREAM_NOT_RUNNING = 9;
const LJ_ERROR LJE_INVALID_PARAMETER = 10;
const LJ_ERROR LJE_INVALID_STREAM_FREQUENCY = 11;
const LJ_ERROR LJE_INVALID_AIN_RANGE = 12;
const LJ_ERROR LJE_STREAM_CHECKSUM_ERROR = 13; // Occurs when a stream packet fails checksum.  Stream is stopped.
const LJ_ERROR LJE_STREAM_COMMAND_ERROR = 14; // Occurs when a stream packet has invalid command values.  Stream is stopped.
const LJ_ERROR LJE_STREAM_ORDER_ERROR = 15; // Occurs when a stream packet is received out of order (typically one is missing).  Stream is stopped.
const LJ_ERROR LJE_AD_PIN_CONFIGURATION_ERROR = 16; // Occurs when an analog or digital request was made on a pin that isn't configured for that type of request.
const LJ_ERROR LJE_REQUEST_NOT_PROCESSED = 17; // When a LJE_AD_PIN_CONFIGURATION_ERROR occurs, all other IO requests after the request that caused the error won't be processed. Those requests will return this error.


// U3 & U6 Specific Errors
const LJ_ERROR LJE_SCRATCH_ERROR = 19;
const LJ_ERROR LJE_DATA_BUFFER_OVERFLOW = 20;
const LJ_ERROR LJE_ADC0_BUFFER_OVERFLOW = 21;
const LJ_ERROR LJE_FUNCTION_INVALID = 22;
const LJ_ERROR LJE_SWDT_TIME_INVALID = 23;
const LJ_ERROR LJE_FLASH_ERROR = 24;
const LJ_ERROR LJE_STREAM_IS_ACTIVE = 25;
const LJ_ERROR LJE_STREAM_TABLE_INVALID = 26;
const LJ_ERROR LJE_STREAM_CONFIG_INVALID = 27;
const LJ_ERROR LJE_STREAM_BAD_TRIGGER_SOURCE = 28;
const LJ_ERROR LJE_STREAM_INVALID_TRIGGER = 30;
const LJ_ERROR LJE_STREAM_ADC0_BUFFER_OVERFLOW = 31;
const LJ_ERROR LJE_STREAM_SAMPLE_NUM_INVALID = 33;
const LJ_ERROR LJE_STREAM_BIPOLAR_GAIN_INVALID = 34;
const LJ_ERROR LJE_STREAM_SCAN_RATE_INVALID = 35;
const LJ_ERROR LJE_TIMER_INVALID_MODE = 36;
const LJ_ERROR LJE_TIMER_QUADRATURE_AB_ERROR = 37;
const LJ_ERROR LJE_TIMER_QUAD_PULSE_SEQUENCE = 38;
const LJ_ERROR LJE_TIMER_BAD_CLOCK_SOURCE = 39;
const LJ_ERROR LJE_TIMER_STREAM_ACTIVE = 40;
const LJ_ERROR LJE_TIMER_PWMSTOP_MODULE_ERROR = 41;
const LJ_ERROR LJE_TIMER_SEQUENCE_ERROR = 42;
const LJ_ERROR LJE_TIMER_SHARING_ERROR = 43;
const LJ_ERROR LJE_TIMER_LINE_SEQUENCE_ERROR = 44;
const LJ_ERROR LJE_EXT_OSC_NOT_STABLE = 45;
const LJ_ERROR LJE_INVALID_POWER_SETTING = 46;
const LJ_ERROR LJE_PLL_NOT_LOCKED = 47;
const LJ_ERROR LJE_INVALID_PIN = 48;
const LJ_ERROR LJE_IOTYPE_SYNCH_ERROR = 49;
const LJ_ERROR LJE_INVALID_OFFSET = 50;
const LJ_ERROR LJE_FEEDBACK_IOTYPE_NOT_VALID = 51;
const LJ_ERROR LJE_CANT_CONFIGURE_PIN_FOR_ANALOG = 67;
const LJ_ERROR LJE_CANT_CONFIGURE_PIN_FOR_DIGITAL = 68;
const LJ_ERROR LJE_TC_PIN_OFFSET_MUST_BE_4_TO_8 = 70;
const LJ_ERROR LJE_INVALID_DIFFERENTIAL_CHANNEL = 71;
const LJ_ERROR LJE_DSP_SIGNAL_OUT_OF_RANGE = 72;

// Other errors
const LJ_ERROR LJE_SHT_CRC = 52;
const LJ_ERROR LJE_SHT_MEASREADY = 53;
const LJ_ERROR LJE_SHT_ACK = 54;
const LJ_ERROR LJE_SHT_SERIAL_RESET = 55;
const LJ_ERROR LJE_SHT_COMMUNICATION = 56;

const LJ_ERROR LJE_AIN_WHILE_STREAMING = 57;

const LJ_ERROR LJE_STREAM_TIMEOUT = 58;
const LJ_ERROR LJE_STREAM_CONTROL_BUFFER_OVERFLOW = 59;
const LJ_ERROR LJE_STREAM_SCAN_OVERLAP = 60;
const LJ_ERROR LJE_FIRMWARE_VERSION_IOTYPE = 61;
const LJ_ERROR LJE_FIRMWARE_VERSION_CHANNEL = 62;
const LJ_ERROR LJE_FIRMWARE_VERSION_VALUE = 63;
const LJ_ERROR LJE_HARDWARE_VERSION_IOTYPE = 64;
const LJ_ERROR LJE_HARDWARE_VERSION_CHANNEL = 65;
const LJ_ERROR LJE_HARDWARE_VERSION_VALUE = 66;

const LJ_ERROR LJE_LJTDAC_ACK_ERROR = 69;

const LJ_ERROR LJE_STREAM_INVALID_CONNECTION = 73;

const LJ_ERROR LJE_MIN_GROUP_ERROR = 1000; // All errors above this number will stop all requests, below this number are request level errors.

const LJ_ERROR LJE_UNKNOWN_ERROR = 1001; // Occurs when an unknown error occurs that is caught, but still unknown.
const LJ_ERROR LJE_INVALID_DEVICE_TYPE = 1002; // Occurs when devicetype is not a valid device type.
const LJ_ERROR LJE_INVALID_HANDLE = 1003; // Occurs when invalid handle used.
const LJ_ERROR LJE_DEVICE_NOT_OPEN = 1004;  // Occurs when Open() fails and AppendRead called despite.
const LJ_ERROR LJE_NO_DATA_AVAILABLE = 1005; // This is cause when GetResult() called without calling Go/GoOne(), or when GetResult() passed a channel that wasn't read.
const LJ_ERROR LJE_NO_MORE_DATA_AVAILABLE = 1006;
const LJ_ERROR LJE_LABJACK_NOT_FOUND = 1007; // Occurs when the LabJack is not found at the given id or address.
const LJ_ERROR LJE_COMM_FAILURE = 1008; // Occurs when unable to send or receive the correct number of bytes.
const LJ_ERROR LJE_CHECKSUM_ERROR = 1009;
const LJ_ERROR LJE_DEVICE_ALREADY_OPEN = 1010; // Occurs when LabJack is already open via USB in another program or process.
const LJ_ERROR LJE_COMM_TIMEOUT = 1011;
const LJ_ERROR LJE_USB_DRIVER_NOT_FOUND = 1012;
const LJ_ERROR LJE_INVALID_CONNECTION_TYPE = 1013;
const LJ_ERROR LJE_INVALID_MODE = 1014;
const LJ_ERROR LJE_DEVICE_NOT_CONNECTED = 1015; // Occurs when a LabJack that was opened is no longer connected to the system.

// These errors aren't actually generated by the UD, but could be handy in your code to indicate an event as an error code without
// conflicting with LabJack error codes.
const LJ_ERROR LJE_DISCONNECT = 2000; 
const LJ_ERROR LJE_RECONNECT = 2001;

// and an area for your own codes.  This area won't ever be used for LabJack codes.
const LJ_ERROR LJE_MIN_USER_ERROR = 3000;
const LJ_ERROR LJE_MAX_USER_ERROR = 3999;


// Warning are negative
const LJ_ERROR LJE_DEVICE_NOT_CALIBRATED = -1; // Defaults used instead
const LJ_ERROR LJE_UNABLE_TO_READ_CALDATA = -2; // Defaults used instead




/* Version History:
2.02: ain_binary fixed for non-streaming.
      Adjusted for new streaming usb firmware (64-byte packets).
      New streaming errors- stop the stream and error is returned with next GetData.
      Fixed resolution setting while streaming (was fixed to 12, now follows poll setting).
2.03: Added callback option for streaming.
2.04: Changed warnings to be negative.
      Renamed POWER_LEVEL to COMM_POWER_LEVEL.
2.05: Updated timer/counter stuff.  Added CounterReset, TimerClockDivisor.
2.06: Fixed problem when unplugging USB UE9 and plugging it into a different USB port.
      Renamed LJ_chCOUNTERTIMER_CLOCK to LJ_chTIMER_CLOCK_CONFIG.
2.08: Fixed two UE9 USB unplug issue.
      Driver now uses high temp calibration for Control power level zero.
      Added new timer modes to header.
      Fixed LJ_tcSYS constant in header.
2.09: New timer constants for new modes.
      Timer/counter update bits auto-setting updated.
      put_counter_reset will execute with the next go, not only with the next read.
2.10: Timer/counter update bits auto-setting updated again.
      Fixed MIO bits as outputs.
      listall().
      Fixed control power level and reset source.
      Added ioDIGITAL_PORT_IN and ioDIGITAL_PORT_OUT.
2.11: Fixed problem with ListAll when performed without prior opening of devices.
2.12: Added initial raw U3 support.
2.13: Fixed issues with streaming backlog and applying cals to extended channels.
2.14: Fixed issue with U3 raw support.
2.15: Fixed driver issue with stream clock output and stream external triggering.
2.16: Fixed problem with listall after changing local id.
2.17: Fixed issues with streaming backlog and applying cals to extended channels.
      Fixed problem with usb reset.
      Added timer mode 6 to header file.
2.18: Fixed issue with rollover on counter/timer.
      Fixed reset issues.
2.21: UE9 changed to use feedbackalt instead of feedback.  Allows for multiple internal
      channels to be called from same call to feedback.
      Fixed internal channel numbers 14/128 and 15/136 and extended channels to return
      proper values.
2.22: Fixed problem with ioGET_TIMER when passed as a string.
      Added support for make unlimited number of requests for analog input channels for a
      single call to a go function.
2.23: 
2.24: Fixed bug sometimes causing errors when a pointer was passed into the Raw io
      functions that caused a communication error.
2.25: Improved U3 support.
2.26: Fixed U3 bugs with timer/counter values, DAC rounding and set analog enabled port functions.
2.46: Various U3 fixes and support added.
2.47: Fixed threading issue.
2.48: Added LJ_ioPUT_WAIT.
2.56: Fixed bug with eAIN.
2.57: Added Thermocouple conversion functions.
2.58: Added SWDT functionality for UE9 (BETA).
2.59: Fixed bug causing some U3 timer values to report incorrectly.
      Added support for OEMs to change USB string descriptors on the U3.
2.60: Added beta support for timer values for U3s with hardware version >= 1.21.
2.61: Fixed bug causing U3 streaming to sometimes hang.
2.62: Added ability to open devices over USB using the serial number.
2.63: Added new stream error codes and fixed bug with opening multiple U3s.
2.64: Fixed bug when streaming with differential channels on U3.
2.65: Improved eAIN to work properly with special all special channels.
2.66: LJ_chSTREAM_ENABLE_AUTORECOVER renamed to LJ_chSTREAM_DISABLE_AUTORECOVERY.
2.67: Fixed bug with eTCConfig and eTCValues on U3.
2.68: Added internal function for factory use.
2.69: Added Ethernet support for ListAll() for UE9.
2.70: Added I2C support for U3 and UE9.
2.72: Fixed problem with using trigger mode when streaming.
2.73: Added detection for reads from Timers/Counters that weren't enabled on UE9.
2.74: Fixed bug that sometimes occurs when requesting version numbers.
2.75: Fixed issue with reopening U3 after a device reset.
2.76: Fixed bug with the proper range not always being used when streaming on a UE9.
      Fixed bug with UE9 Ethernet ListAll() returning wrong values for IP addresses.
2.77: Fixed bug with special channel readings while streaming.
      Added Asynch support for U3.
2.78: Fixed bug when doing SPI communication with an odd number of bytes.
2.79: LJ_chI2C_WRITE_READ support.
3.00: Added support for U3-HV and U3-LV.
3.01: Fixed bug with U3-HV applying wrong calibration constants when streaming.
3.02: Fixed bugs with U3-HV and U3-LV and a delay with StopStream.
3.03: Added initial support for U6.
3.04: Fixed bug with Asynch communication on U3-HVs and U3-LVs.
3.05: Fixed calibration bug with eAIN on the U3-HV and U3-LV.
3.06: Fixed bug with SWDT functionality for U3.
3.10: Added support for new USB driver that works with 64-bit OS versions.
3.11: Fixed a memory leak when devices were kept open via USB.
3.12: Fixed various U6 bugs.
3.13: Added LJE_DEVICE_ALREADY_OPEN error to occur when another program/process has the
      LabJack open.
3.14: Various minor bug fixes.
3.15: Added full support for Asynch communication on the U6.
3.16: Fixed bug causing a delay when the LabJackUD driver was unloaded in a program.
3.17: Added support for SWDT settings for initial roll time and strict mode.
3.18: Fixed a bug with negative timer values when using ETCConfig with LJ_tmQUAD on the
      U3 and U6.
      Fixed bug causing a delay when stream on U3/U6 was stopped.
3.19: Fixed bug that caused LJ_ioPUT_TIMER_VALUE to corrupt other results when done in
      the same Go or GoOne call.
3.22: Fixed bug in the U6 and U3 that caused excessive delays when com buffer started to fill.
3.23: Added GetThreadID function.
3.26: Fixed bug with U6 reading channels 200-224 not returning proper values.
3.27: Fixed bug with setting calibration constants for LJTickDAC.
3.28: Fixed bug with U3s reading internal temperature using LJ_ioGET_AIN_DIFF.
      Added LJ_ctETHERNET_DATA_ONLY support.
3.32: Various bug fixes.
3.38: Added AddRequestPtr, ePutPtr and LJ_ioSET_STREAM_CALLBACK_PTR for better
      64-bit compatibility.
3.40: Added LJ_chSTREAM_COMMUNICATION_TIMEOUT support for U3/U6.
3.41: Fixed compatibility for Ptr functions and raw read/write support.
3.42: Fixed error in values of streaming extended channels on U6.
3.43: Fixed bug with eAIN high resolution readings on Pro devices.
      Fixed bug with IC2 and *Ptr functions.
3.44: Changed eAddGoGet() to return LJE_COMMAND_LIST_ERROR if an error is detected.
      Error arrays are also defaulted to zero. 
3.45: Fixed a bug which would sometimes cause a critcal error if the LabJackUD.dll was
      unloaded after streaming.
3.46: Updated internal reconnect routine to help prevent deadlocks.
      Fixed a bug when disabling UE9 DACs.
      Added the ability to set multiple timer values in a single Go/GoOne call for the U6.
      Updated LJ_chASYNCH_TX's result so the Value is the number of bytes in the RX buffer.
      Fixed LJ_chCAL_CONSTANTS, LJ_chUSER_MEM, LJ_chUSB_STRINGS and LJ_ioSPI_COMMUNICATION
      functionality when using Array and Ptr functions.
      Fixed writing calibration constants, LJ_chCAL_CONSTANTS, for the U3 and U6.
      Fixed eTCValues U6 bug.
3.47: Fixed bug when streaming with resolution index 0 on a U6-Pro that caused incorrect
      calibration constants to be applied.
      Added LJ_chSTREAM_COMMUNICATION_TIMEOUT read support.
3.48: Fixed LJ_ioADD_STREAM_DAC when using Array and Ptr functions.
3.49: Fixed a U3 and U6 bug with stream mode where sometimes invalid samples were being
      added to the UD stream buffer after auto-recovery ended.
3.50: Fixed a 64-bit access violation bug with eAIN.
*/


// Depreciated constants:
const long LJ_ioANALOG_INPUT = 10;
const long LJ_ioANALOG_OUTPUT = 20; // UE9 + U3
const long LJ_ioDIGITAL_BIT_IN = 30; // UE9 + U3
const long LJ_ioDIGITAL_PORT_IN = 35; // UE9 + U3
const long LJ_ioDIGITAL_BIT_OUT = 40; // UE9 + U3
const long LJ_ioDIGITAL_PORT_OUT = 45; // UE9 + U3
const long LJ_ioCOUNTER = 50; // UE9 + U3
const long LJ_ioTIMER = 60; // UE9 + U3
const long LJ_ioPUT_COUNTER_MODE = 2010; // UE9
const long LJ_ioGET_COUNTER_MODE = 2011; // UE9
const long LJ_ioGET_TIMER_VALUE = 2007; // UE9
const long LJ_ioCYCLE_PORT = 102;  // UE9 
const long LJ_chTIMER_CLOCK_CONFIG = 1001; // UE9 + U3
const long LJ_ioPUT_CAL_CONSTANTS = 400;
const long LJ_ioGET_CAL_CONSTANTS = 401;
const long LJ_ioPUT_USER_MEM = 402;
const long LJ_ioGET_USER_MEM = 403;
const long LJ_ioPUT_USB_STRINGS = 404;
const long LJ_ioGET_USB_STRINGS = 405;
const long LJ_ioSHT_DATA_CHANNEL = 501; // UE9 + U3
const long LJ_ioSHT_CLOCK_CHANNEL = 502; // UE9 + U3
const long LJ_chI2C_ADDRESS = 5108; // UE9 + U3
const long LJ_chASYNCH_CONFIG = 5116; // UE9 + U3
const long LJ_rgUNIP500V = 110; // 0V to +0.500V
const long LJ_ioENABLE_POS_PULLDOWN = 2018; // U6
const long LJ_ioENABLE_NEG_PULLDOWN = 2019; // U6
const long LJ_rgAUTO = 0;
const long LJ_chSWDT_UDPATE_DIOA = 5205;


#ifdef  __cplusplus
} // extern C
#endif
 
#endif
