/*
 *	My_Analyzer - Version 3.8
 *
 *	This source file is under General Public License version 3. Please feel free to
 *	distribute it, hack it, or do anything else you like to it. I would ask, however
 *	that is you make any cool improvements, you let me know via any of the websites
 *	on which I have published this.
 *
 *	This is a highly modified version of the code originally written by Jack Purdum, W8TEE.
 *
 *	The contributors to this version are:
 *
 *		John Price       - WA2FZW	(Major upgrades and bug fixes)
 *		Edwin Houwertjes - PE1PWF	(AD8951 DDS, 6 meter & Custom band modifications)
 *		Dick Holst       - K2RH		(Lots of testing help since I only have one real antenna)
 *		Jim Smith        - G3ZQC	(More testing and analysis help)
 *		Glenn Percy		 - VK3PE	(AD8307 Detector and lots of testing)
 *		Bill Blackwell   - AB1XB    (Addition of on-demand debugging output)
 *		Dave Wilde       - M0WID    (Skip the band/frequency request on startup and more)
 *      George Kavanagh  - KB1HFT   (Addition of statistics calculation and its serial output)
 *
 *
 *	In addition to fixing a few bugs in the original code, the main point of writing this
 *	modified version of the code was to add the capability to use the antenna analyzer on
 *	the 6 meter band (50.0 to 54.0 MHz). This required changing the AD9850 DDS to the newer
 *	AD9851 DDS module (the software will still work with the AD9850).
 *
 *	The first problem with adding 6 meters is that Jack defined a lot of the frequency
 *	related variables as type "int", which can only deal with positive numbers up to
 *	32K. Since 50Mhz is well beyond that limit, all the frequency related variable types
 *	needed to be changed to either "unsigned" or "float".
 *
 *	In the process of analyzing the code to figure out all the things related to dealing
 *	with the frequencies, I found a number of bugs and some things that I felt could be
 *	improved upon. This code includes those fixes and improvements. 
 *
 *	In Version 01.0, I fixed a number of bugs, mostly related to items in the "Options"
 *	(later changed to "View/Save") menu.
 *
 *	In Version 01.1, I revised the menu structure. The "View Mins" item is moved from the top
 *	level menu to a choice in the "Options" (later changed to "View/Save") menu, where everything
 *	else having to do with saved data is located. A new top level menu item titled "Maintenance"
 *	is added. Under that heading the choices are "Delete File", which is moved  from the
 *	"Options" menu and new menu items: "Delete All", "Reset Seq #" and "Clear Mins".
 *
 *	I also changed the order in which many of the functions are defined in an attempt to
 *	make the overall flow of the code easier to follow.
 *
 *	I've totally lost track of all the changes I made in Version 01.2. The major
 *	ones include a total overhaul of the encoder operation, allowing the frequency
 *	to be changed while using the single frequency SWR measurement capability,
 *	remembering band and frequency range limits between scans instead of having to
 *	set them each time a new scan is run.
 *
 *	Internally, I streamedlined how text is displayed on the screen and added a lot
 *	of error handling for conditions that were not being checked.
 *
 *	In version 02.0, with much help from Jim Smith (G3ZQC) and Edwin Houwertjes (PE1PWF )
 *	support for the higher frequency AD9851 DDS module, which in turn allowed the optional
 *	addition of the 6 meter band and an optional "Custom" band.
 *
 *	In Version 02.1, support for the "Fine Tune" button was included, which allows the
 *	operator to vary the frequency setting functions.
 *
 *	In Version 02.2, an SWR calibration function was added, allowing the operator to use
 *	a known dummy load to set a calibration factor for each band.
 *
 *	In Version 02.3, the calibration function was removed as it just didn't work. Added in
 *	Version 02.3 is the ability to fine tune MHz for the 6 meter and "Custom" bands. Also,
 *	in the single frequency measurement, the starting fequency will be the mid-point of
 *	the currently set band limits instead of the default band range.
 *
 *	Version 02.4 just cleans up some of the code. There are few outwardly visible changes,
 *	other than changing the "Options" menu title to "View/Save" and adding the "Mount_SD"
 *	selection under the "Maintenance" menu.
 *
 *	Version 02.5 adds support for Glenn's (VK3PE) AD8307 Log detector circuit in place of
 *	the Germanium diodes and some more internal cleanup.
 *
 *	In Version 02.6, we added an analog SWR meter to the single frequency function.
 *
 *	In Version 02.7, I got tired of chasing glitches in the "New Scan", "View Plot" and
 *	particularly the "Overlay" functions, so I redid the whole graphing logic. At the
 *	same time, at Glenn's suggestion, the plot was modified to use a logarithmic SWR
 *	scale which also allowed the maximum plotted SWR to be raised to 10:1.
 *
 *	In Version 02.8, I added a test to the "Overlay" function to detect when the current
 *	scan and the overlay scans have different frequency ranges and display an error message
 *	instead of trying to perform the meaningless overlay.
 *
 *	Also in Version 02.8, we add the capability to repeat live scans.
 *
 *	Version 02.9 involved more major code cleanup and the addition of the "Examine" feature
 *	which allows the operator to use the encoder to scroll a cursor line across a new scan
 *	or viewed old one to see the SWR at each measurement point.
 *
 *	In Version 3.1, we take another crack at SWR calibration, but only if Edwin's (PE1PWF)
 *	hardware modification is installed. The symbol "PE1PWF_MOD" in the header file should be
 *	defined if the hardware mods are installed; commented out if not.
 *
 *	There a number of minor changes in Version 03.2 and one major one. The lower limit of
 *	the "Custom" band is now set to 100KHz. The SWR measurements aren't great down that low,
 *	but Edwin thought it would be a good idea the be able to use the analyzer as a signal
 *	generator for aligning IF chains. Along with that, it the "Fine Tune" button is installed,
 *	the units digit in the frequency adjustments will now change in 1KHz increments instead
 *	of 2KHz.
 *
 *	Someone on the SCHR group thought labelling the scan plots (particularly those done from
 *	saved files) would be a good idea, so we did that. When doing overlays, both curves will
 *	be labelled.
 *
 *	Version 03.3 adds a "Freq Cal" function to the "Maintenance" menu to allow the
 *	DDS frequency to be set more accurately. Please read the "User Manual" for details.
 *
 *	Version 03.3.1 fixes a bug caused by the addition of the "Freq Cal" function that was
 *	keeping the DDSes on some boards (but not all) from working.
 *
 *	In Version 03.4, I did a cleaner fix to the problem of the DDS not starting correctly
 *	which was fixed (but not as neatly as possible) in Version 03.3.1. Also, thanks to Bill
 *	(AB1XB) added the capability to turn the debugging output on or off via an option under
 *	the "Maintenance" menu.
 *
 *	There are a number of changes in Version 03.5.
 *
 *	I fixed the bug that was causing the "EEPROM.Serial" function to not display the DDS
 *	calibration constant correctly and a bug in the "View Mins" function where the bars for
 *	SWRs higher than 10:1 were exceeding the vertical limit of the graph.
 *
 *	People running Version 03.4 and earlier were still having problems with the DDS not
 *	initializing to the correct frequency. After extensive testing, I did find that the
 *	problem would occurr if the DDS types were changed (in the header file, not necessarily
 *	in the actual unit). This has been corrected.
 *
 *	There are also some new features in Version 03.5:
 *
 *	I added the option to display a horizontal cursor line in the "Examine" function. It can be
 *	turned on or off based on whether the symbol "HORIZ_INDEX" is defined or not in the header
 *	file. The line changes colors based on the SWR value at the current plot point.
 *
 *	The current band and scan frequency range limits are now displayed at the bottom of the menu
 *	screens.
 *
 *	By enabling the definition of "SKIP_BAND_SELECT" in the header file, the user can cause
 *	the analyzer to skip over the requests for the band and frequency range on startup UNLESS
 *	there is no valid data saved in the EEPROM (as would be the case if it was erased or this
 *	is the first time the analyzer was used).
 *
 *
 *	Significant changes in Version 03.6!
 *
 *	Again using code provided by Dave (M0WID) as a starting point, I added the optional ability
 *	to display the raw Arduino pin data on the "Frequency" and "SWR Calibrate" displays. This
 *	required changing the layout of both displays a little bit to make room for the additional
 *	data. You can also toggle the DDS on and off from these screens. The explanation of why you
 *	might want to do that is in the "User Manual".
 *
 *	I also added the ability to perform an "SWR Calibrate" on Glenn's (VK3PE) units. You still
 *	have to do the hardware adjustment first, but this function tweaks the results.
 *
 *	SWR values that get computed as less than 1.00 or greater than 20.00 will now be displayed
 *	as "N/A".  This is a side effect of what happens when you turn the DDS off as mentioned
 *	above.
 *
 *  Additional code by KB1HFT and AB1BX implements optional calculation and debug
 *  output of statistics min, max, and standard deviation, on the analog readings.
 *  This is implemented by the inclusion of the C++ class "AAStats" which is defined
 *  by the AAStats.cpp and AAStats.h files.
 *   
 *  Settings in the new "Options" submenu of the Maintenance menu control whether and which of the
 *  statistics are calculated and output to the serial monitor.  See the intro
 *  comments in AAStats.h for details.
 *
 *	The option to enable debug mode has been moved into the "Options" submenu of the Maintenance menu.
 *	
 *	An additional item in the "Options" submenu allows the serial monitor output baud rate to be set.
 *	When the baud rate setting of the Arduino IDE Serial Monitor, which must be the same value,
 *	is changed, the Antenna Analyzer will restart.
 *	
 *	Future options may be added to this screen as desired.
 *
 *
 *	Changes in Version 03.7.2
 *
 *	Only a couple of minor tweaks in this version. I added code to display the "slope"
 *	value to the DEBUG output when using the AD8307 version of the analyzer and added
 *	a few more decimal places to its display everywhere.
 *
 *
 *	Changes in Version 03.8
 *
 *	Fixed a bug in the calculation of the frequency at which to do the SWR calibration.
 *	It was using the average of the "legal" band limits as opposed to the limits set
 *	by the user. When using the "Custom" band, this sometimes caused the calibration
 *	frequency to be outside the limits set by the user.
 *
 *	Also fixed a bug in the setting of the lower frequency limit for the "Custom" band.
 *	If the "FT" button was set to change the MHz value and the current setting was less
 *	then 1 MHz, trying to lower the frequency was causing garbage settings.
 *
 *	Modified the "Repeat Scans" function to display the scan count and maximum count in
 *	place of where the "LIVE" label would normally appear. Note that the counts will be
 *	shown regardless of whether the "LABEL_SCAN" symbol is defined or not.
 *
 *	The code is designed to run on the Arduino Mega Pro Mini and uses a 3.5" MCU Friend
 *	TFT display. Additional components are an AD9850 (Type 2) DDS module or AD9851 DDS
 *	module, and a Mini-360 Buck Converter (Voltage Regulator).
 *
 *	For reference, the datasheets for both DDSes can be found online at:
 *
 *		AD9850:	http://www.analog.com/media/en/technical-documentation/data-sheets/AD9850.pdf
 *		AD9851:	http://www.analog.com/media/en/technical-documentation/data-sheets/AD9851.pdf
 */

/*
 *	The "#include" statements for all the necessary libraries have been moved to
 *	the local header file "My_Analyzer.h", as different ones are needed for the
 *	AD8950 and AD9851 DDS modules.
 *
 *	Other things which can be set in the My_Analyzer.h file include:
 *
 *		Which DDS is being used; AD9850 or AD9851
 *		Whether or not the AD8307 detector modification is installed
 *		Whether or not your board has the PE1PWF modifications installed
 *		Whether or not you wish to add the 6 meter and/or "Custom" bands
 *		The frequency range limits for those additional bands
 *		All of the parameters for the repeating scan capability
 *		Whether or not the "Fine Tune" button is installed
 *		Whether or not to turn the "Examine" feature on automatically
 *		Whether or not to enable the horizontal cursor in the "Examine" feature
 *		Whether or not to display labels on the scan plots
 *		Whether or not to skip the request for band and frequency ranges on startup
 *		Whether or not to display the Arduino pin readings on the "SWR Calibrate" and
 *			"Frequency" displays.
 */

#include "My_Analyzer.h"			// Lots of definitions
#include "AAStats.h"


/*
 * These are  macros:
 */

#define ELEMENTS(x)		( sizeof ( x ) / sizeof ( x[0] ))
#define pulseHigh(pin)	{ digitalWrite ( pin, HIGH ); digitalWrite ( pin, LOW ); delay ( 10 ); }

#ifndef min
	#define min ( a, b ) ((( a ) < ( b )) ? ( a ) : ( b ))
#endif

/*
 *	Options flags
 */

#define OPT_DBG_MODE		0x0001			// Debugging mode is enabled
#define OPT_STATS			0x0002			// Show statistics
#define OPT_STATS_POINT		0x0004			// Show SWR point statistics
#define OPT_STATS_LOG_IND	0x0008			// Show stats individual readings
#define OPT_FLAGS_VALID		0x8000			// Valid options flags indicator

/*
 *  These two macro definitions are what makes it possible to turn the debugging statements on
 *  and off without needing to recompile and reload the software:
 */

#define DEBUG_BEGIN if (optDebugMode) {		// Start debugging output
#define DEBUG_END }							// End debugging output


/*
 *	Use the TFT display in "LANDSCAPE" mode and set lots of other display parameters. If for some
 *	reason you need to use the display upside down, I understand you can change the "LANDSCAPE"
 *	definition to "3". Haven't tried it myself though.
 */

#define PORTRAIT 				 0					// NOTE: These can set the origin to any
#define LANDSCAPE 				 1					// corner of the display with value 0 - 3


/*
 *	These control how the SWR readings are taken and how many readings are taken
 *	in the process of performing a scan. Note that in Version 03.6, I doubled the
 *	number of samples taken when using the VK3PE version. This improved the jitter
 *	problems those boards seem to have.
 */

#define SCAN_INTERVALS			100					// Number of plot scan intervals
#define MAX_PLOT_POINTS			(SCAN_INTERVALS+1)	// Number of actual plot points
#define MAX_POINTS_PER_SAMPLE	75					// Max points read at each frequency


/*
 *	Map of the assigned EEPROM addresses:
 */

#define SWR_MINS_SET		  0					// EEPROM minimum SWRs set if this = 1
#define SWR_MINS_ADDRESS	  2					// Starting EEPROM address for mins

#define	ACTIVE_BAND_SET		 50					// Indicator that active band data is valid
#define	ACTIVE_BAND_INDEX	 52					// Saved active band index setting
#define	ACTIVE_BAND_BOTTOM	 54					// Saved active band low frequeny setting
#define ACTIVE_BAND_TOP		 56					// Saved active band high frequency setting
#define	FREQ_CALIBRATION	 58					// DDS Calibration constant

#define NEXT_SD_FILE_NUMBER	 90					// The next number to be used in a file name
#define SWR_CALIBRATION		 92					// SWR Calibration factor if PE1PWF mods are installed
#define OPTION_FLAGS		 94					// Program option flags (bit mask)
#define DDS_IN_USE			 96					// Last DDS type installed (9850 or 9851)
#define SLOPE_CALIBRATION	 98					// Store slope calibration for AD8307 (float)

#define SERIAL_BAUD_RATE	102					// Serial monitor baud rate

#define EEPROM_NEXT_ADDR	106					// Next EEPROM Address available


/*
 *	Definitions associated with the rotary encoder:
 */

volatile unsigned char 	encoderDirection;		// Which way the encoder was moved
volatile int			clickCount;				// How many clicks was it moved


/*
 *	Define the pins used to control the AD9850 or AD9851 DDS module:
 */

#define 	DDS_FREQ_UPD_PIN	48			// connected to AD9850/51 freq update pin (FQ)
#define 	DDS_RESET_PIN		49			// connected to AD9850/51 reset pin (RST)
#define 	DDS_DATA_PIN		51			// connected to AD9850/51 serial data pin (MOSI)
#define 	DDS_CLOCK_PIN		52			// connected to AD9850/51 module word load clock pin (CLK)

uint32_t	DDS_Calibrate; 					// Frequency calibration value for the DDS


/*
 *	Definitions for DDS type:
 */

#define		DDS_TYPE_9850 0					// These values are needed for the DDS library
#define		DDS_TYPE_9851 1					// initialization function

uint8_t		ddsType = -1;					// Real type to be determined later


/*
 *	Define the pins for measuring the forward and reflected voltages from the SWR bridge:
 */

#define ANALOG_FORWARD		A6			// Forward voltage
#define ANALOG_REFLECTED	A7			// Reflected voltage


/*
 *	Definitions of things related to the SD card.
 *	
 *	In Version 01.2, I added the global variable "useFile", which was defined locally
 *	in a number of functions. There was some redundency between the "ShowFiles" and
 *	"SelectFiles" functions. For operations that require a list of the files to be
 *	displayed and then selecting one, they were both being called, and both were displaying
 *	the list. The only operation that requires displaying the list without making a specific
 *	selection is the "Delete All" function.
 *
 *	The 2 functions are modified to set the global variables "fileCount" and "useFile"
 *	instead of returning the values as a first step into eliminating the redundency.
 */

#define SD_SS			53			// Select pin used for SD card on LCD panel

#define MAX_FILES		20			// Number of SD files that can be opened
#define NAME_LENGTH		13			// The max file name size

bool	SD_Status = false;			// "true" when valid SD card is loaded


/*
 *	Character array to hold SD file names:
 */

char mySDFiles [ MAX_FILES ] [ NAME_LENGTH ];


/*
 *	Definitions related to the "Fine Tune" feature
 */

#define		FINE_TUNE_PIN		21					// Fine tune pin (interrupt enabled, but interrupt not used)

volatile	bool		newFT_Interrupt  = false;	// New short push of the push button
volatile	bool		longFT_Interrupt = false;	// New long push of the push button
volatile	int			FT_pushCount     = 0;		// Ranges from 0 to 3 (but sometimes 4)
volatile	int			calPushCount     = 0;		// Ranges from 0 to 3
volatile unsigned long	FT_Time 		 = 0;		// Time the button was first pushed
bool					FT_installed     = false;	// Assume no "Fine Tune" button


/*
 *	Definitions for things related to the scan frequencies:
 *
 *	"FREQ_INCREMENT" is used in the Set Band/Frequencies functions if the "FineTune" option
 *	is not installed. "FIXED_FREQ_INCR" is used in the single frequency SWR reading function
 *	if the "FineTune" option is not installed. The definitions of both can be changed to
 *	suit your particular needs if desired.
 *
 *	If the "Fine Tune" option is installed, the values in "freqIncrements" are used based
 *	on the past history of pushes on the fine tune button.
 */
 
#define 	FREQ_INCREMENT		100			// Used for upper/lower boundaries of scan adjustment
#define 	FIXED_FREQ_INCR		  5			// Used in frequency adjustment measures

int			freqIncrements[4] = { 100, 10, 1, 1000 };	// The active increment factors
int			activeFreqIncrement;						// The currently selected one

int			calIncrements[4]  = { 1000, 100, 10, 1 };	// Used in calibration function
int			activeCalIncrement;							// The currently selected one



/*
 *	Define the symbolic parameters for "PaintText()" & "EraseText()"
 */

#define		NORMAL		0x01		// Normal text ( Green on Black )
#define		HIGHLIGHT	0x02		// Highighted text ( Blue on White )
#define		WARNING		0x03		// Warning messages ( Yellow on Black )
#define		ERR			0x04		// Error messages ( Red on Black )
#define		HEADING		0x05		// Heading text ( White on Black )
#define		SPECIAL		0x06		// Splash screen ( Magenta on Black)
#define		SCAN_LBL	0x07		// For labelling scan plots ( Yellow on Transparent)
#define		OVLY_LBL	0x08		// For labelling overlay plots ( White on Transparent)
#define		DEBUG		0x09		// Debug label (Red on Transparent)

#define		LEFT		0x01		// Left text alignment
#define		CENTER		0x02		// Centered text
#define		RIGHT		0x03		// Right aligned text


/*
 *	These 3 symbols can be used in the parameter for column to force things to print at 
 *	either side of the full screen, or in the middle. They force the associated text
 *	alignment, of course.
 */

#define		SCREEN_L	-1		// Screen left
#define		SCREEN_C	-2		// Screen center
#define		SCREEN_R	-3		// Screen right


/*
 *	Define computational parameters for converting text column and row locations to
 *	cursor locations in the "PaintText()" and "EraseText()" functions. 
 */

#define		SZ_1_W		6				// Size 1 text is 6 pixels wide
#define		SZ_1_H		8				// And 8 pixels high

#define		NO_PAGE	   -1				// Don't display a page number in headings


/*
 *	"topMargin" and "upperPlotMargin" used to be set by #defines, but they were defined without
 *	regard to the actual size of the header text and where the upper limit of the graphs are
 *	actually located. So we make variables out of them and in "setup()" we will calculate the
 *	values based on the parameters defined in the scan structure.
 *
 *	"topMargin" tells us the Y coordinate of where to start erasing the screen when we want to
 *	leave the header and "upperPlotMargin: is the Y coordinate of the top line of the graphs.
 */

int	topMargin;
int	upperPlotMargin;


/*
 *	Define text strings for common messages used throughout the program. It uses a little
 *	global space, but makes overall life easier. It also insures standardization. Before
 *	doing it this way, there were 3 different strings for the "Press switch" message! Note,
 *	there is only one such message at the present time.
 */

const char	msgPushButton[]	= "Press switch to continue";


/*
 *	Define the colors. Note I defined a bunch that aren't used. Should I ever need them,
 *	they are here! The un-used ones are not inverted in setup() based on the display type.
 */

int BLUE    = 0x001F;
int GREEN   = 0x07E0;
int RED     = 0xF800;
int YELLOW  = 0xFFE0;
int WHITE   = 0xFFFF;
int BLACK   = 0x0000;
int DKGREEN = 0x03E0;
int DKGREY	= 0x8410;		// Used on SWR meter & scan plots
int MAGENTA = 0xF81F;		// Only used on first 2 lines of the startup screen

int ORANGE  = 0xFA60;		// Not used
int LIME    = 0x07FF;		// Not used
int AQUA    = 0x04FF;		// Not used
int LTPINK  = 0xFDDF;		// Not used
int LTGREY  = ~0xE71C;		// Not used
int	PINKY    = 0xF8FF;		// Not used - Compiler does not like the name "PINK"!


/*
 *	Some other miscellaneous stuff:
 */

int		menuIndex;						// Index to currently selected main menu item
int		subMenuIndex;					// Index to the currently selected item in a sub-menu
int		nextSD_FileNumber;				// Next number to use in file naming (stored in EEPROM)

int		fileCount = 0;					// Number of files read from card (limited by MAX_FILES)
int		useFile	  = 0;					// Index of a selected file

int		menuColumns[3] = { 0, 12, 25 };	// Columns for main & sub menus
int		menuRow;						// Row and columns for menu displays 
int		menuCol;						// (text size values not pixels)

int		displayW;						// Width & Height of display
int		displayH;

uint16_t 	g_identifier;				// Identifier of the TFT display type

int			activeBandSet;				// Valid band data indicator
int			activeBandIndex;			// From EEPROM active band for scans
uint16_t	activeBandBottom;			// Lower frequency setting
uint16_t	activeBandTop;				// Upper frequency setting

uint16_t	optionFlags			= OPT_FLAGS_VALID;// Dynamic option bit flags
uint8_t   	optDebugMode		= 0;	// Debugging mode indicator
uint8_t		optStats			= 0;	// Statistics enabled
uint8_t		optStatsPoint		= 0;	// Statistics SWR points enabled
uint8_t		optStatsLogIndiv	= 0;	// Statistics log individual readings enabled
char*		strYes	= " yes ";			// Convenient string constants
char*		strNo	= " no ";


/*
 *	Array and storage for setting serial monitor baud rate
 */

long		baudRates [] = { 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 74880, 115200 };
long		baudRate = 9600;			// Default 9600 baud

/*
 *	These all seem to be used in doing a scan, but I haven't figured them all out yet.
 */

float	hedge;							// Horizontal edge (low frequency for scan)
float	bump;							// Frequency difference between vertical lines


/*
 *	The following variable definitions are only used if using the AD8307 detector circuit 
 *	or Edwin's (PE1PWF) hardware modifications.
 */

#if ( defined ( PE1PWF_MOD ) || defined ( AD8307_SWR ))

	float	returnLoss;					// Return loss
	float	reflectionCoefficient;		// Amount of reflected power
	float	slope = 0.025;          	// AD8307 spec is 0.025mv/dB (range is .023-0.028) - M0WID     

	int		calFactor;					// Fudge factor in Edwin's SWR formula
#endif

bool	SWR_dataHeaderDone;				// Used by "PaintSwrData()"
bool	ddsOn;							// Is the DDS turned on or not?

float	FWD = 0.0;						// Averaged forward voltage
float	REV = 0.0;						// Averaged reverse voltage

float	vRef;							// Analog reference voltage for the AD8307 mods


/*
 *	Strings used in displaying the menu items
 */

const char *mainMenu[]		= { " Analysis ", " View/Save ", " Maintenance" };

#if ( defined ( PE1PWF_MOD ) || defined ( AD8307_SWR ))		// Add "SWR calibrate

	const char *menuAnalysis[]	= { " Single Scan ", " Repeat Scans ", " Frequency ",
									" Change Band ", " Set Limits ", " SWR Calibrate ", " Main Menu " };

#else								// No "SWR Calibrate"

	const char *menuAnalysis[]	= { " Single Scan ", " Repeat Scans ", " Frequency ",
								    " Change Band ", " Set Limits ", " Main Menu " };

#endif

														
const char *menuViewSave[]	= { " Save Scan ", " View Plot ", " Overlay ", " View Table ",
							    " Plot>Serial ", " View Mins ", " Main Menu " };
							   
const char *menuMaint[]		= { " Delete File ", " Delete All ", " Reset Seq # ", " Clear Mins ",
							    " Erase EEPROM ", " EEPROM>Serial ", " Mount SD ",
							    " Freq Cal ", " Options ", " Main Menu " };

const char *menuOptions[]	= { " Enable Debug Mode ", " Log Statistics Summary", "  Log SWR Point Stats ", "  Log Indiv Readings ",
							    " Serial Baud Rate ", " Main Menu " };

/*
 *	Note that the "6M" and "Custom" entries are included in the "menuBands" list regardless
 *	of whether or not they are really being used. If the "Custom" band is added, but not
 *	the 6 meter band, the "Custom" title will be shifted down a slot in the "setup()"
 *	function.
 */
 
int	bandCount = 10;									// 10 Without the optional bands

#define STRING_40M      "  40M "					// Use defines so I can do
#define STRING_6M		"   6M "					// compares later on
#define STRING_CUSTOM	"Custom"

const char *menuBands[]  = { " 160M ", "  80M ", "  60M ", STRING_40M, "  30M ","  20M ",
							  "  17M ", "  15M ", "  12M ", "  10M ", STRING_6M, STRING_CUSTOM };


/*
 *	Define the upper and lower frequencies for each of the standard (US) Ham bands. Note
 *	some tricky stuff here to deal with the optional inclusion of the 6 meter and "Custom"
 *	bands. The last 2 slots are initialized to zeroes here. They will be initialized in
 *	"setup()" to the proper values for 6 meters and/or "Custom" if those are enabled in
 *	the "My_Analyzer.h" header file. The value of "bandCount" will be adjusted
 *	accordingly.
 *	
 *	Modified in Version 01.2 to be a 2 dimensional array. It makes finding things easier!
 */

uint32_t   bandEdges[][2] = { {  1800,  2000 },		// 160 Meters
							  {  3500,  4000 },		//  80 Meters
							  {  5330,  5404 },		//  60 Meters (really 5330.5 to 5403.5)
							  {  7000,  7300 },		//  40 Meters
							  { 10100, 10150 },		//  30 Meters
							  { 14000, 14350 },		//  20 Meters
							  { 18068, 18158 },		//  17 Meters
							  { 21000, 21450 },		//  15 Meters
							  { 24890, 24990 },		//  12 Meters
							  { 28000, 29700 },		//  10 Meters
							  {      0,    0 },		//  Last slots are reserved for 6 meters
							  {      0,	   0 } };	//  and/or "Custom"


#define 	LOWER	1				// Used for get & set lower band edge
#define 	UPPER	2				//                    upper


/*
 *	Array containing the minimum SWR readings for each band (stored in EEPROM). Has space
 *	for the 2 optional bands,
 */

uint16_t	minSWRs[12];			// Each element is the minimum for the bands above


/*
 *	The original code used one stucture for all the graphical outputs (scans & bars). In
 *	Version 02.4, I made 2 different structures; one for scan plots and one strictly for
 *	the bargraph plots. Why? Two reasons: First, in examining the code, a number of the members
 *	get initialized every time a new graph is going to be plotted, and always to the same
 *	values. It is much simpler to type those members as "const" and only set the values one
 *	time; not to mention if the values need to be changed, they need to be changed in only
 *	one place!
 *
 *	Secondly, not all members are used for the two different types of plots, thus the unneeded
 *	ones could be eliminitated completely from the two structures.
 *
 *	There is one common structure (named "scan") which will be used for real scans and data
 *	read from saved files when either the "View Plot" or "View Table" options are used. A
 *	separate version of the structure (named "ovly") is used to hold the data used for the
 *	"Overlay" function and is defined as a local variable in the "PlotOverlay()" function.
 *
 *	These along with a lot of the functions that manipulate them should really be redone in
 *	C++ as classes and implemented as objects.
 */

struct scanStruct						// Graph structure declaration for scans
{
	const int	xOrigin     =  60;		// Upper left horizontal coordinate
	const int	yOrigin     = 290;		// Lower left vertical coordinate
	const int	graphWidth  = 350;		// Width of graph
	const int	graphHeight = 260;		// Height of graph
	float		swr[MAX_PLOT_POINTS];	// SWR readings for current scan
	float		freq[MAX_PLOT_POINTS];	// Frequencies for current scan
	float		minFreq;				// Minimum frequency
	float		maxFreq;				// Maximum frequency
	float		minSWR;					// Minimum SWR
	float		maxSWR;					// Maximum SWR
	int			minIndex;				// Array index of minimum SWR/frequency pair
	float		xPos;					// X coordinate for current plot point (frequency)
	float		yPos;					// Y coordinate for current plot point (SWR value)
	float		lastX;					// X coordinate of last plot point (frequency)
	float		lastY;					// Y coordinate of last plot point (SWR)
	float		deltaX;					// Scale division between lo and hi
	bool		isValid = false;		// Contents of the structure are a valid plot
	char		fileName[10];			// Added in Version 03.2 for labelling the plot
} scan;

struct barStruct						// Graph structure declaration for bar graphs
{
	const int	xOrigin     =  60;		// Upper left horizontal coordinate
	const int	yOrigin     = 290;		// Lower left vertical coordinate
	const int	graphWidth  = 410;		// Width of graph
	const int	graphHeight = 260;		// Height of graph

	const int	Width	    =  24;		// Width of a bar
	const int	Space 	    =  10;		// Space between bars
	int			X;						// Horizontal position of current bar

	float		currentSWR;				// Current SWR value
} bar;


/*
 *	The meter structure contains the coordinates and other data for drawing the analog
 *	SWR meter and maintaining the position of the needle. Again, this should probably
 *	be implemented as a class/object.
 */

struct meterStruct
{
	const int	startX	  = 50;				// Starting X coordinate (upper left hand corner)
	const int	startY	  =  5;				// Starting Y coordinate

	const int	frameW   = 380;				// Width of outer frame
	const int	frameH   = 100;				// Height of outer frame

	const int	scaleX   =  80;				// Left side of scale line
	const int	scaleY   =  startY + 50;	// Vertical position of scale line
	const int	scaleLen = 320;				// Length of scale line

	float		lastPtr  = scaleX;			// Last pointer position
	float		newPtr   = 0;				// New pointer position		
} meter;


MCUFRIEND_kbv	tft;						// Create the display object
File			root;						// Create the file object
Rotary rotary = Rotary ( PIN_A, PIN_B );		// Create the encoder object

AAStats MyStats ( MAX_PLOT_POINTS, MAX_POINTS_PER_SAMPLE );

#ifdef	AD9850_DDS
	AD985XSPI DDS ( DDS_TYPE_9850 );		// Create the DDS object
#else
	AD985XSPI DDS ( DDS_TYPE_9851 );		// Create the DDS object
#endif

bool	exitFlag  = false;					// Used in main menu sub-menus

bool	stopRepeating;						// For "DoNewScan()" - Stop repeating
int		repeatCount = DEFAULT_COUNT;		// How many times to repeat the scan
int		scanCount = 0;						// How many we've done


/*
 *	The "setup()" function is called by the Arduino's bootstrap loader whenever the processor
 *	is rebooted. It is responsible for initializing all of the things needed to run the
 *	analyzer.
 */

void setup ()
{
	int			eepromFlag;					// Flag stored in EPROM indicating whether or
											// not the minimum SWRs have been set

	uint16_t	lastDDS;					// Last DDS type used from EEPROM
	uint16_t	newDDS;						// Current DDS type (9850 or 9851)

	EEPROM.get ( SERIAL_BAUD_RATE, baudRate );	// Get the serial monitor baud rate
	int ndx;
	if ( !validateBaudRate ( baudRate, &ndx ) )	// And validate it
	{
		baudRate = 115200;
		EEPROM.put ( SERIAL_BAUD_RATE, baudRate );
	}

	Serial.begin ( baudRate );					// Start the serial interface
	delay ( 1000 );								// Give it a second to stabilize

	EEPROM.get ( OPTION_FLAGS, optionFlags );	// Get the option flags & set indicators
	if ( ! ( optionFlags & OPT_FLAGS_VALID ) )	// If valid indicator not set
	{
		optionFlags |= OPT_FLAGS_VALID;			// Set option flags valid indicator
		EEPROM.put ( OPTION_FLAGS, optionFlags );
	}
	else if ( optionFlags == 0xFFFF )			// Otherwise if ROM is all ones
	{
		optionFlags = OPT_FLAGS_VALID;			// Initialize option flags
		EEPROM.put ( OPTION_FLAGS, optionFlags );
	}
	
	optDebugMode = optionFlags & OPT_DBG_MODE;
	optStats = optionFlags & OPT_STATS;
	optStatsPoint = optionFlags & OPT_STATS_POINT;
	optStatsLogIndiv = optionFlags & OPT_STATS_LOG_IND;

/*
 *	The first thing we're going to take care of is determining whether or not we need
 *	to add the optional bands to the "menuBands" and "bandEdges" arrays.
 */

#ifdef	ADD_6_METERS								// If Adding 6 meters

	bandEdges[bandCount][0] = LOW_6M_EDGE;			// Add frequency limits to the
	bandEdges[bandCount][1] = HIGH_6M_EDGE;			// "bandEdges" array
	bandCount++;									// Bump the band count

#endif

#ifdef	ADD_CUSTOM									// If adding the "Custom" band

	bandEdges[bandCount][0] = LOW_C_EDGE;			// Add frequency limits to the
	bandEdges[bandCount][1] = HIGH_C_EDGE;			// "bandEdges" array
	bandCount++;									// Bump the band count


/*
 *	This might seem a little weird, but if we added the "Custom" band but not the
 *	6 meter band, we need to move the title for "Custom" down a slot in the menu list.
 */

	#ifndef	ADD_6_METERS							// Custom added, but not 6 meters

		strcpy (( char* ) menuBands[bandCount-1], menuBands[bandCount] );

	#endif

#endif


/*
 *	Now we see if the "Fine Tune" hardware modifications are installed or not. In Version 03.6
 *	the fine tune button no longer uses interrupts. This was done to allow us to discern 
 *	between long pushes and short ones.
 */

	FT_installed = false;								// Assume it is not installed
	
	#ifdef	FT_INSTALLED								// But if it is

		pinMode ( FINE_TUNE_PIN, INPUT_PULLUP );		// Setup the fine tune pin

		FT_pushCount     = 0;							// No button pushes
		newFT_Interrupt  = false;						// No short interrupt
		longFT_Interrupt = false;						// No long interrupt
		FT_Time 		 = 0;							// Clear last interrupt time stamp
		FT_installed     = true;						// Button ia installed
		
		activeFreqIncrement = freqIncrements[0];		// 100KHz

	#endif


/*
 *	Configure the encoder pin options:
 */

	pinMode ( PIN_A,  INPUT_PULLUP );		// Setup the encoder pins
	pinMode ( PIN_B,  INPUT_PULLUP );		// Reverse definitions of A & B if it works backwards
	pinMode ( SWITCH, INPUT_PULLUP );		// And the encoder switch


/*
 *	Attach the encoder interrupt handler to both encoder pins. Note, in Jack's original
 *	code, he used a different form of the attach function that is not recommended and
 *	supposedly not supported on the Arduino Mega.
 */

	attachInterrupt ( digitalPinToInterrupt ( PIN_A ), ReadEncoder, CHANGE );
	attachInterrupt ( digitalPinToInterrupt ( PIN_B ), ReadEncoder, CHANGE );


/*
 *	Disable the pullup resistors on the analog pins used to read the forward and
 *	reflected voltages so they don't affect the readings.
 */

 	pinMode ( ANALOG_FORWARD,   INPUT );
	pinMode ( ANALOG_REFLECTED, INPUT );

	pinMode ( BATT_CHECK_PIN,   INPUT );		// If used

/*
 *	Some experimentation has shown that we can get slightly smoother looking scan plots if
 *	we use the "INTERNAL1V1" reference voltage with Edwin's modifications, but only with
 *	those changes to the hardware (and it's only been tested with the modifications done
 *	exactly as he describes).
 *
 *	Previously, I had tried using the 2.56V reference for the AD8307 board and it didn't seem
 *	to work. However I had forgotten that the voltage is part of the SWR computation and since
 *	I didn't change that, it didn't work. When I changed the formula to match the actual reference
 *	voltage, it actually works a little better (smoother curves).
 *
 *	If you're using the unmodified circuitry (regardless of which DDS module) it needs the
 *	2.56V reference.
 */

	analogReference ( INTERNAL2V56 );			// MEGA 2.56V internal reference
	vRef = 2.56;

	#ifdef	PE1PWF_MOD
		analogReference ( INTERNAL1V1 );		// MEGA 1.1V internal reference
		vRef = 1.0;								// Not used, but set it anyway
	#endif
	
	#ifdef	AD8307_SWR 							// If using the AD8307 detector
		analogReference ( INTERNAL2V56 );		// 2.56V internal reference
		vRef = 2.56;							// Really used in AD8307 computations

	#endif


/*
 *	After changing the analog reference, the first few readings from analogRead() may
 *	not be accurate, so we read and ignore 10 readings.
 */

 	for ( int i = 0; i < 10; i++ )
 	{
		analogRead ( ANALOG_FORWARD );
	 	analogRead ( ANALOG_REFLECTED );
	}


/*
 *	Initialize the opening menu parameters
 */

	menuIndex  	  = 0;					// The highest level; drill down from there
	subMenuIndex  = 0;					// Index of selected item


/*
 *	Read the value of eepromFlag from EPROM address "SWR_MINS_SET". If the value is "1", it
 *	indicates that somewhere along the line, the saved minimum SWR readings were saved. If
 *	the value is anything else, it indicates the minimums have not been initialized, so we
 *	will proceede to do that.
 */

	EEPROM.get ( SWR_MINS_SET, eepromFlag );	// Have the minimum SWRs been set?

	if ( eepromFlag != 1 )						// They have not been set
		SetEEPROMMins ();						// So set them

	else
		ReadEEPROMMins ();						// SetEEPROMMins set the values in memory also


/*
 *	We also get the next number to be used in the file name sequence, which is also stored
 *	in the EPROM. This was originally in "ReadEEPROMMins", but was taken out of there in 
 *	Version 01.1, because having a function doing something totally unrelated to its
 *	primary purpose is confusticating!
 */

	EEPROM.get ( NEXT_SD_FILE_NUMBER, nextSD_FileNumber );	// The file number for the next SD file name


/*
 *	Now we ask the TFT display what type it really is, as theoretically, any size display
 *	could be used, however, some of them use different values to define the colors.
 */

	g_identifier = tft.readID();		// Get TFT ID  3.95" = 0x9486; 3.6" = 0x9488

	if ( g_identifier == 0x9486 || g_identifier == 0x9481 )	// Some displays need colors inverted
	{
		BLUE    = ~BLUE;				// I only invert the ones actually used.
		GREEN   = ~GREEN;				// If more from the earlier list are used,
		RED     = ~RED;					// they need to be added here.
		YELLOW  = ~YELLOW;
		WHITE   = ~WHITE;
		BLACK   = ~BLACK;
		DKGREEN = ~DKGREEN;
		MAGENTA = ~MAGENTA;
		LTGREY  = ~LTGREY;
		DKGREY  = ~DKGREY;
	}


/*
 *	Initialize the display. Originally, there was a bunch of code in here to determine
 *	whether the display should be classified as "LARGE" or "SMALL" based on its identification
 *	code. As that was never used, I ripped it out in Version 02.4.
 */

	tft.begin ( g_identifier );				// Initialize the display

	DEBUG_BEGIN
		Serial.print ( "TFT ID: " );
		Serial.println ( g_identifier, HEX );
	DEBUG_END

	tft.setRotation ( LANDSCAPE );						// Use landscape mode
	displayW = tft.width ();							// Get width
	displayH = tft.height ();							// And height

/*
 *	Calculate the space needed for the header line and the upper limit of the graphs:
 */

	topMargin = ( SZ_1_H * 2 ) + 4;			// Size 2 text + 4 extra pixels
	upperPlotMargin = scan.yOrigin - scan.graphHeight;


/*
 *	Start up the DDS. The frequencies are in Hz. The standard crystal frequency for the
 *	AD9850 is 125MHz; the standard frequency for the AD9851 is 180MHz. The values are
 *	used in the calibrate function call, and can be tweaked if necessary.
 *
 *	Modified in Version 02.0 to allow use of the AD9851.
 *
 *	Modified in Version 03.2.1 to use Farrukh's new DDS library
 *
 *	Modified in Version 03.3 to get FREQ_CALIBRATION from the EEPROM; the calibration
 *	constant for the DDS.
 */

#ifdef	AD9850_DDS
	ddsType = DDS_TYPE_9850;					// Type for DDS library initilization
	newDDS  = 9850;								// Type that gets stored in EEPROM
#endif

#ifdef	AD9851_DDS
	ddsType = DDS_TYPE_9851;					// Type for DDS library initilization
	newDDS  = 9851;								// Type that gets stored in EEPROM
#endif


/*
 *	Get all the saved variables for the DDS. We'll read both the "calFactor" used in the
 *	SWR computations for Edwin's version and the "slope" used in the SWR computations for
 *	Glenn's version unless we're running on an un-modified analyzer.
 */

#if ( defined ( PE1PWF_MOD ) || defined ( AD8307_SWR ))
	EEPROM.get ( SWR_CALIBRATION , calFactor );			// Get previous setting

	if ( calFactor == 0 )								// If not previously set
	{
		calFactor = 135;								// Start with default value
		EEPROM.put ( SWR_CALIBRATION , calFactor );		// Save new calibration factor value
	}

	EEPROM.get ( SLOPE_CALIBRATION, slope );			// Get last slope factor
    
    	if (( slope < 0.02) || (slope > 0.03 ))			// Range check
    	{
    		slope = 0.025;								// Default if invalid
			EEPROM.put ( SLOPE_CALIBRATION, slope );	// Save new value
    	}
#endif

	EEPROM.get ( DDS_IN_USE, lastDDS );						// Last known DDS used
	EEPROM.get ( FREQ_CALIBRATION, DDS_Calibrate );			// And calibration factor

	if (( ddsType != lastDDS ) || ( DDS_Calibrate == 0 ))	// Need to reset the calibration factor
	{
		if ( ddsType == DDS_TYPE_9850 ) 
			DDS_Calibrate = CAL_9850;

		else if ( ddsType == DDS_TYPE_9851 )
			DDS_Calibrate = CAL_9851;					// Some flakey 9851s might need 120000000

		EEPROM.put ( FREQ_CALIBRATION, DDS_Calibrate );	// Save updated DDS info in EEPROM
		EEPROM.put ( DDS_IN_USE, newDDS );
	}

	pinMode	( DDS_FREQ_UPD_PIN, OUTPUT );				// Set all DDS pins for output mode
	pinMode ( DDS_CLOCK_PIN,    OUTPUT );
	pinMode ( DDS_DATA_PIN,     OUTPUT );
	pinMode ( DDS_RESET_PIN,    OUTPUT ); 

	DDS.begin     ( DDS_CLOCK_PIN, DDS_FREQ_UPD_PIN, DDS_RESET_PIN );
	DDS.calibrate ( DDS_Calibrate ); 

	DDS.down ();							// Put the DDS to sleep until we need it

	DEBUG_BEGIN
		Serial.println ("DDS Module Initialized ...");
	DEBUG_END

	Splash ();							// Display the startup screen


/*
 *	Initialize the SD card reader. Note that if the card is not in the slot (or there is
 *	some other issue such as incorrect formatting), simply inserting a card will not bring
 *	it back to life. To restore functionality of the file processing functions, the program
 *	must be restarted or a new card can be inserted and the "Mount SD" option can be selected
 *	in the "Maintenance" menu.
 */

	Mount_SD ();						// Initialize the SD card 


/*
 *	See if the operator wants to change the active band or frequency limits. Modified in Version
 *	03.5 to actually give the user the option to skip getting the active band and frequency
 *	range information if the symbol "SKIP_BAND_SELECT" is defined in the header file.
 *
 *	However, it the band and frequency data that was read from the EEPROM is not valid, then
 *	we ask.
 */

	bool	setBandInfo = true;				// Assume we need to request band/frequency info

	#ifdef	SKIP_BAND_SELECT				// Owner doesn't want to be bothered

		setBandInfo = false;				// Set the flag to false

	#endif

	ReadActiveBandData ();					// Read saved active band data (or initialize it)

	if ( setBandInfo || !activeBandSet )
		SetActiveBand ();					// See if operator wants changes

	SaveActiveBandData ();					// Save new values and set activeBandSet flag


/*
 *	Display the top level menu
 */

	ShowMainMenu ();					// Show top level menu
}


/*
 *	"loop()" is run continuously upon completion of the "setup()" function. Basically, all
 *	we do here is handle processing of the top level menu which was put on the screen as the
 *	last operation in "setup()".
 *
 *	Modified in Version 03.8 to add a "BatteryCheck" capability.
 */

void loop()
{
	int oldIndex;							// Saves & restores the menu item selected

	#if ( defined ( DO_BATT_CK ))			// If enabled
		BatteryCheck ();					// See if we're good still
	#endif

	if ( encoderDirection != DIR_NONE )		// If the encoder rotated
	{
		DEBUG_BEGIN
			Serial.print ( "encoderDirection = " );
			Serial.print ( encoderDirection );
			Serial.print ( ", menuIndex = " );
			Serial.println ( menuIndex );
		DEBUG_END

		AlterMenuOption ( encoderDirection );	// Move selected menu option
		ResetEncoder ();						// Reset encoder movement flags
	}

	if ( digitalRead ( SWITCH ) == LOW )		// Did operator select an option?
	{
		delay	( 50 );							// Debounce

		oldIndex = menuIndex;					// Save index of highlighted selection

		switch ( menuIndex )					// Let's see what we need to do!						
		{
			case 0:								// Analysis?
				DoAnalysis ();					// Go do it
				break;

			case 1:								// View/Save Menu
				DoOptions ();					// Go do it
				break;

			case 2:								// Maintenance Options
				DoMaintenance ();				// Go do it
				break;

			default:
				DEBUG_BEGIN
					Serial.print( "I shouldn't be here: menuIndex = " );
					Serial.println ( menuIndex );
				DEBUG_END
				break;
		}

		menuIndex = oldIndex;						// Restore saved menu selection index

		if ( !exitFlag)								// If the exitFlag is set, don't wait for the switch
		{
			while ( digitalRead ( SWITCH ))	{}		// Force a pause to view results 

			tft.fillScreen ( BLACK );				// Fade to black
		}

		ShowMainMenu ();					// Restore top level menu
		subMenuIndex = 0;					// Back to top level menu
		ResetEncoder ();					// Clear the encoder movement indicators
		exitFlag = false;					// Clear the exit flag
	}
}



/******************************  INITILIZATION FUNCTIONS  ******************************
 *
 *	The following functions are all used in the "setup()" function to initialize various
 *	items (SD card, Display, etc.). They might also be used from other places in the
 *	program. In Version 01.1, I moved them here in an effort to group related things
 *	together to make the overall code easier to follow.
 */

/*
 *	"SetActiveBand()" is called from "setup()" and from the "Set Band" option in the
 *	"Analysis" menu.
 *
 *	It displays the current active band on the display and allows the operator to set
 *	a different band, or simply operate the encoder switch to continue using the current
 *	setting. As the last settings for the information were loaded from the EEPROM in
 *	"setup" (or initialized to the defaults if never saved), there is no need to re-read
 *	them from the EEPROM here.
 */

void SetActiveBand()
{
	int	oldBandIndex = activeBandIndex;				// Remember current band setting

	tft.fillScreen ( BLACK );						// Fade to black

	PaintText ( 3, SCREEN_C,  2, 0, NORMAL,  CENTER, "Select Band" );
	PaintText ( 3, SCREEN_C,  4, 0, HEADING, CENTER, menuBands[activeBandIndex] );
	PaintText ( 2, SCREEN_C, 10, 0, NORMAL,  CENTER, msgPushButton );

	ResetEncoder ();								// Clear the encoder flags

	while ( digitalRead ( SWITCH ))					// While encoder switch not pushed
	{
		if ( encoderDirection != DIR_NONE )			// If the encoder moved
		{
			switch ( encoderDirection )				// Change bands
			{
				case DIR_CW:								// Next higher
					activeBandIndex ++;						// Increment the index
					if ( activeBandIndex >= bandCount )		// Out of bounds
						activeBandIndex = 0;				// Back to the beginning
					break;

				case DIR_CCW:
					activeBandIndex --;						// Decrement the index
					if ( activeBandIndex < 0 )				// Out of bounds
						activeBandIndex = bandCount -1;		// Last one
					break;
			}
			ResetEncoder ();					// Clear encoder flags

			EraseText ( 3, SCREEN_C, 4, 0, BLACK,   CENTER, 10 );
			PaintText ( 3, SCREEN_C, 4, 0, HEADING, CENTER, menuBands[activeBandIndex] );
		}
	}


/*
 *	If we changed bands, get the band limits for the new band. If we didn't change bands,
 *	we use the ones previously set by the operator.
 */

	if ( oldBandIndex != activeBandIndex )
	{
		activeBandBottom = GetBandEdge ( activeBandIndex, LOWER );		// Get lower band edge
		activeBandTop    = GetBandEdge ( activeBandIndex, UPPER );		// And upper band edge
	}

	SetBandEdge ( LOWER );					// Set the frequency limits	
	SetBandEdge ( UPPER );			
	SaveActiveBandData ();					// Save new information
}


/*
 *	"SetBandEdge()" sets the "UPPER" or "LOWER" frequency limit in preparation for doing a
 *	full scan. The only parameter is the "UPPER" or "LOWER" specification.
 *
 *	It sets the minFreq or maxFreq entry in the graphics structure and displays the frequency
 *	on the display.
 *
 *	Modified in Version 01.2 to work with the "activeBand" frequency settings. Also modified
 *	to prevent the lower frequency to be equal to or higher than the upper frequency and
 *	vice-versa.
 */

void SetBandEdge ( int whichOne )
{
	uint32_t 	edge;							// Band limit frequency
	uint32_t	tempLowerBandEdge;				// Used in limit checks
	uint32_t	tempUpperBandEdge;				// Used in limit checks
	char		freqBuff[8];					// Place to build ASCII frequency

	tft.fillScreen ( BLACK );					// Fade to black

	FT_pushCount = 0;							// Default initial setting is 100KHz steps

	activeFreqIncrement = freqIncrements[FT_pushCount];

	if ( whichOne == LOWER )					// Bottom limit requested?
	{
		FormatFrequency ( activeBandBottom, freqBuff, 3 );

		PaintText ( 3, SCREEN_C, 2, 0, NORMAL, CENTER, "Set Lower Scan Limit" );
		DisplayFrequency ( 3, 10, 3, 1, HEADING, LEFT, freqBuff );
		PaintText ( 2, SCREEN_C, 10, 0, NORMAL, CENTER, msgPushButton );

		edge = activeBandBottom;
	}

	else									// Upper limit requested
	{
		FormatFrequency ( activeBandTop, freqBuff, 3 );

		PaintText ( 3, SCREEN_C, 2, 0, NORMAL, CENTER, "Set Upper Scan Limit" );
		DisplayFrequency ( 3, 10, 3, 1, HEADING, LEFT, freqBuff );
		PaintText ( 2, SCREEN_C, 10, 0, NORMAL, CENTER, msgPushButton );

		edge = activeBandTop;
	}

	ResetEncoder ();							// Clear encoder movement flags

	while ( digitalRead ( SWITCH ))				// Wait for encoder switch change
	{
		ReadFT ();								// "Fine Tune" button pushed?
		
		if ( !FT_installed )							// If "Fine Tune" not installed
			activeFreqIncrement = FREQ_INCREMENT;		// Use fixed increment value

		else if ( newFT_Interrupt )						// Need to move the carat?
		{
			FormatFrequency ( edge, freqBuff, 3 );		// Format frequency into "buff"
			DisplayFrequency ( 3, 10, 3, 1, HEADING, LEFT, freqBuff );

			activeFreqIncrement = freqIncrements[FT_pushCount];

			ResetFT ();
		}

		if ( encoderDirection != DIR_NONE )				// If it has rotated...
		{			
			switch ( encoderDirection )					// Direction of rotation
			{
				case DIR_CW:							// Clockwise
					edge += activeFreqIncrement;		// Increased edge value
					break;

				case DIR_CCW:							// Counter-clockwise
					if (( edge < 1000L ) && ( activeFreqIncrement == 1000 ))
						break;
					edge -= activeFreqIncrement;		// Decreased edge value
					break;
			}

/*
 *	Now we need to do some error checking! First of all, we can't let the bottom frequency
 *	become greater than or equal to the top frequency and vice-versa.
 *
 *	In the "LOWER" section, we're not going to let the frequency go below 100KHz.
 *
 *	In the "UPPER" section, we're not going to let the frequency go above 65.5MHz.
 */
//**HERE
			if ( whichOne == LOWER )				// If working on the bottom limit?
			{
				tempLowerBandEdge = GetBandEdge ( activeBandIndex, LOWER );

				if ( edge < 100 )
					edge = 100;

				if ( edge >= activeBandTop )
					edge = edge - activeFreqIncrement;	// Back it up
			}

			if ( whichOne == UPPER )					// If working on the upper limit?
			{
				tempUpperBandEdge = GetBandEdge ( activeBandIndex, UPPER );

				if ( edge <= activeBandBottom )
					edge = edge + activeFreqIncrement;	// Back it up

				if ( edge > 65500 )						// Overflow avoidance!
					edge = 65500;
			}

			FormatFrequency ( edge, freqBuff, 3 );
			DisplayFrequency ( 3, 10, 3, 1, HEADING, LEFT, freqBuff );

			ResetEncoder ();						// Clear encoder moved flags
			ResetFT ();								// And pushbutton
		}
	}

	if ( whichOne == LOWER )
		scan.minFreq = activeBandBottom = edge;		// Set lower frequency limit in graphics structure

	else
		scan.maxFreq = activeBandTop = edge;		// Set upper frequency limit in graphics structure
}


/*
 *	"GetBandEdge()" returns the "UPPER" or "LOWER" frequency limit from the "bandEdges"
 *	array. The two parameter are the index of the band requested in the "menuBands" array	
 *	and either "UPPER" or "LOWER" to specify which limit is being requested.
 */

uint16_t GetBandEdge ( int bandIndex, int whichOne )
{
	if ( whichOne == LOWER )
		return bandEdges[bandIndex][0];				// Send back lower band edge

	else
		return bandEdges[bandIndex][1];				// Send back upper band edge
}


/*
 *	"GetActiveEdge()" is added in Version 03.8.
 *
 *	It returns the "UPPER" or "LOWER" frequency limit from the "activeBandBottom" or
 *	"activeBandTop" variables, which are the band limits set by the user as opposed to
 *	the legal band limits.
 */

uint16_t GetActiveEdge ( int whichOne )
{
	if ( whichOne == LOWER )
		return activeBandBottom;				// Send back lower band edge

	else
		return activeBandTop;					// Send back upper band edge
}


/*
 *	"ReadActiveBandData()" is called in the "setup()" function and from the "Set Band" option
 *	under the "Analysis" menu. It reads the saved active band information from the EPROM.
 *
 *	If the indicator in address "ACTIVE_BAND_SET" is not 1, the data is not valid, and instead
 *	of reading it, we will initialize the saved values to the proper settings for 40 meters
 *	and set the appropriate variables in memory. This should only happen the first time this
 *	version of the software is executed or after the "Erase EEPROM" function has been used.
 *
 *	If the data is valid, we initialize the appropriate variables from the EEPROM data.
 */

void ReadActiveBandData ()
{
	int		i;											// Loop index

	EEPROM.get ( ACTIVE_BAND_SET, activeBandSet);		// See if EEPROM data is valid

	if ( activeBandSet!= 1 )							// If not valid, initialize it
	{
		for ( i = 0; i < bandCount; i++ )
		{
			if ( strcmp ( menuBands[i], STRING_40M ) == 0 )	// 40 Meters is our default
				break;										// Found it										// Next
		}

		activeBandIndex  = i;							// Remember index to "menuBands"

		activeBandBottom = GetBandEdge ( i, LOWER );	// Get 40 meter lower band edge
		activeBandTop    = GetBandEdge ( i, UPPER );	// And upper band edge
	}

	else												// Read saved data
	{
		EEPROM.get ( ACTIVE_BAND_INDEX,  activeBandIndex );	// Index to "menuBands"

		if ( activeBandIndex == bandCount )				// Fixes the band "m" problem caused by
			activeBandIndex--;							// Eliminating 6M while keeping "Custom"

		EEPROM.get ( ACTIVE_BAND_BOTTOM, activeBandBottom );		// Low frequency limit
		EEPROM.get ( ACTIVE_BAND_TOP,    activeBandTop );			// High frequency limit
	}
}


/*
 *	"SaveActiveBandData()" is called whenever the saved data in the EEPROM needs to be updated.
 *	It is also used from the "Set Band" and "Set Range" options under the "Analysis" menu.
 *
 *	It sets the valid data indicator and writes the current band and frequency range to
 *	the EEPROM.
 */

void SaveActiveBandData ()
{
	activeBandSet= 1;										// Data valid indicator

	EEPROM.put ( ACTIVE_BAND_SET,    activeBandSet);		// Show saved data is valid
	EEPROM.put ( ACTIVE_BAND_INDEX,  activeBandIndex );		// Index to band information arrays
	EEPROM.put ( ACTIVE_BAND_BOTTOM, activeBandBottom );	// Active low end frequency
	EEPROM.put ( ACTIVE_BAND_TOP,    activeBandTop );		// Active high end frequency
}


/*
 *	"SetEEPROMMins()" zeros out the EPROM memory locations for saving the minimum
 *	SWR readings and sets a flag in the EPROM showing that it has been initialized.
 *
 *	Modified in Version 01.1 to also set the values in the "minSWRs" array to zero,
 *	to save having to re-read them from the EEPROM.
 */

void SetEEPROMMins ()
{
	EEPROM.put ( SWR_MINS_SET, 1 );							// Says we've been here before
	for ( int i = 0; i < ELEMENTS ( minSWRs ); i++ )		// 2 bytes at a time
	{
		minSWRs[i] = 0;										// Zap the array
		
		EEPROM.put ( SWR_MINS_ADDRESS + i*2 , minSWRs[i] );	// Fill with 2 byte zeroes
	}
}


/*
 *	"ReadEEPROMMins()" reads the saved minimum SWR values for each band from the EPROM.
 *	
 *	In Version 01.1, I removed the read of the next file sequence number from this function
 *	and put it in the "setup()" function proper, as file sequence numbers are totally unrelated
 *	to the minimum SWR values.
 */

void ReadEEPROMMins ()
{
	for ( int i = 0; i < bandCount; i++ )
	{
		EEPROM.get ( SWR_MINS_ADDRESS + ( i * 2 ), minSWRs[i] );

		DEBUG_BEGIN
			Serial.print ( "i = " );
			Serial.print ( i );
			Serial.print ( ", minSWRs[i] = " );
			Serial.println ( minSWRs[i] );
		DEBUG_END
	}
}


/*
 *	"validateBaudRate()" tests that a specified baud rate is in the baud rate table.
 *	If successful the table index is returned in the 2nd parameter and the function returns "true".
 */
 
bool validateBaudRate ( long baud, int *index )
{
	for ( int n = 0; n < ELEMENTS ( baudRates ); n++ )
	{
		if ( baud == baudRates[n] )
		{
			*index = n;
			return true;
		}
	}
	*index = -1;
	return false;
}

/********************************  MENU PROCESSING FUNCTIONS  ********************************
 *
 *	These functions are all used in the above "loop()" function (and also other places in the
 *	program). They include the functions for displaying a menu, dealing with movement of the
 *	encoder switch and the three functions that actually perform the selections available
 *	under each of the main menu options.
 */

/*
 *	"ShowMainMenu()" displays the main menu on the screen. Added in Version 02.4, it
 *	replaces "ShowMenu()" shich appeared to be a general purpose function, but was in
 *	fact only used for displaying the main menu.
 */

void ShowMainMenu ()
{
	int i;										// Loop counter
	int	menuItems = ELEMENTS ( mainMenu );		// Number of entries in the main menu

	tft.fillScreen ( BLACK );					// Fade to black

	for ( i = 0; i < menuItems; i++ )			// Loop on number of menu items
		PaintText ( 2, menuColumns[i], 0, 1, HEADING, LEFT, mainMenu[i] );

	PaintText ( 2, menuColumns[menuIndex], 0, 2, HIGHLIGHT, LEFT, mainMenu[menuIndex] );

	menuRow = 0;								// Remember row
	menuCol = menuColumns[menuIndex];			// and column of selected item
	
	PaintFooter ();								// Display band & frequency limits
	ShowDebugMode();							// Display dynamic debugging indicator					
}


/*
 *	"AlterMenuOption()" changes the selected main menu option based on the value of the
 *	"whichWay" parameter, which is expected to be either "CW" or "CCW" based on which
 *	direction the encoder was last moved.
 *
 *	The previously selected menu item is redisplayed without the highlight, and the newly
 *	selected one is highlighted.
 */

void AlterMenuOption ( int whichWay )
{
	int oldColumn = menuCol;				// Column of current selection
	int oldIndex  = menuIndex;				// Index of currently selected item

	switch ( whichWay )
	{
		case DIR_CW:						// Clockwise rotation
			menuIndex++;					// Increment the index

      		if ( menuIndex == ELEMENTS ( mainMenu ) )		// Wraparound test
				menuIndex = 0;								// Set index back to zero

			break;

		case DIR_CCW:										// Counter-clockwise rotation
			menuIndex--;									// So decrement the index

			if ( menuIndex < 0 )							// If it went negative
				menuIndex = ELEMENTS ( mainMenu ) - 1;		// set it to the last item

			break;

		default:
			break;
 	}

	menuCol = menuColumns[menuIndex];			// Set column of the newly selected menu item

	PaintText ( 2, menuColumns[oldIndex], 0, 2, HEADING,   LEFT, mainMenu[oldIndex] );
	PaintText ( 2, menuCol, 0, 2, HIGHLIGHT, LEFT, mainMenu[menuIndex] );
}


/*
 *	"ShowSubMenu()" displays a sub-menu associated with one of the main menu selections.
 *
 *	The input parameters are:
 *
 *		const char *whichMenu[]		Array of pointers to the menu options
 *		int len						The number of items in the menu
 *		char *values[]				(optional) Array of values for each menu option
 *		int valueCol				(Optional) Column# in which values are displayed
 *
 *	The two new optional parameters allow a value associated with each menu item
 *	to be displayed in a single column to the side of the menu. Used by DoSetOptions().
 *
 *	It returns nothing.
 */

void ShowSubMenu ( const char *menu[], int len, char *values[] = NULL, int valueCol = 0 )
{
	int i;												// Loop index

	tft.fillRect ( 0, topMargin, displayW + 10, displayH, BLACK );	// Erase screen below top menu

	for ( i = i; i < len; i++ )
	{
		PaintText ( 2, menuCol, i + 1, 1, NORMAL, LEFT, menu[i] );	// Display the whole menu
		if (values != NULL)
			PaintText ( 2, valueCol, i + 1, 1, NORMAL, LEFT, values[i] );	// Display the value for this menu item
	}

	PaintText ( 2, menuCol, 1, 1, HIGHLIGHT, LEFT, menu[0] );		// Then highlight the 1st entry

	PaintFooter();									// Display band info
	ShowDebugMode();								// Display dynamic debugging indicator					

	menuRow 	 = 1;								// Top entry is highlighted
	subMenuIndex = 0;								// Index of top entry

	AlterMenuDepth ( encoderDirection, menu, len );	// Wait for operator to make a selection

	ResetEncoder ();								// Clear the encoder flags

	if (values == NULL)
		tft.fillRect ( 0, topMargin, displayW, displayH, BLACK );    // Erase the menu list
}


/*
 *	"AlterMenuDepth()" highlights menu options as user scrolls through the list. Parameters
 *	are:
 *
 *		int whichWay			Are we scrolling up or down
 *		const char *menu[]		The menu that is being scrolled
 *		int len					The number of menu options
 *
 *	It returns nothing
 */

void AlterMenuDepth ( int whichWay, const char *menu[], int itemCount )
{
	int oldSubMenuIndex;						// Previous selection
	int oldMenuRow;

	ResetEncoder ();							// No encoder movement yet

	oldSubMenuIndex = subMenuIndex;				// Remember old index (should be 0)
	oldMenuRow = menuRow;						// And old row

	while ( true )								// Do until we get tired
	{
		if ( digitalRead ( SWITCH ) == LOW )	// If the encoder switch is pushed
      		return;								// Our work here is done

		if ( encoderDirection != DIR_NONE )				// Did the encoder move?
		{
			oldSubMenuIndex = subMenuIndex;				// Remember where we are in the list

			switch ( encoderDirection )					// Which way did the encoder turn?
			{
				case DIR_CW:							// Clockwise
					subMenuIndex++;						// Next item
					if ( subMenuIndex == itemCount	)	// At the bottom?
						subMenuIndex = 0;				// Yes, go back to the top

          			break;

				case DIR_CCW:							// Counter-clockwise rotation
					subMenuIndex--;						// Move up 1

					if ( subMenuIndex < 0 )				// Already at the top item?
						subMenuIndex = itemCount - 1;	// Then move to the bottom item

					break;

				default:
					break;
			}

			ResetEncoder ();							// Clear encoder movement flags

			menuRow = subMenuIndex + 1;					// Set new row number

			PaintText ( 2, menuCol, oldMenuRow,  1, NORMAL,    LEFT, menu[oldSubMenuIndex] );
			PaintText ( 2, menuCol, menuRow,     1, HIGHLIGHT, LEFT, menu[subMenuIndex] );

			oldMenuRow = menuRow;					// Old row now becomes current row
		}
	}												// End of while ( true )
}													// End of AlterMenuDepth ()


/*
 *	"DoAnalysis()" handles processing of the options listed in the "Analysis" sub-menu.
 */

void DoAnalysis ()
{
	ShowSubMenu ( menuAnalysis, ELEMENTS ( menuAnalysis ) );	// Display the "Analysis" sub-menu

	switch ( subMenuIndex )					// What does the operator want to do?
	{
		case 0:								// Single scan
			DoNewScan ( SINGLE_SCAN );		// Do the scan
			Examine ();						// Analyze data?
			break;

/*
 *	Added in Version 02.8, we allow the user to repeat a scan a set number of times. Pressing
 *	the encoder switch will terminate the process upon completion of the current scan.
 */

		case 1:
			RepeatScan ();
			Examine ();						// Analyze data?
			break;


/*
 *	The single frequency functionality was severly modified in Version 01.2. Rather than
 *	making the operator select a frequency at the start of the process and having the
 *	software just monitor that single frequency, we now pick the starting frequency as
 *	the middle of the scan limits set by the operator (within a few KHz), and allow the
 *	operator to change the frequency while the scan is being done.
 *
 *	Starting with Version 01.2, we used the current working band set in "activeBandIndex"
 *	instead of asking for a new one.
 */

		case 2:								// Set specific frequency
			DoSingleFrequency ();			// And read SWR
			break;

		case 3:								// Change bands
			SetActiveBand ();				// Go set it!
			break;

		case 4:								// Change scan frequency limits
			SetBandEdge (LOWER );			// Set starting frequency
			SetBandEdge ( UPPER );			// And end frequency
			SaveActiveBandData ();			// Save new settings
			break;

		#if ( defined ( PE1PWF_MOD ) || defined ( AD8307_SWR ))
		
			case 5:
				DoCalibration ();				// Do SWR calibration
				break;
		#endif

		default:							// Go back to main menu
			exitFlag = true;				// Without waiting for an encoder push
			break;
	}										// End of "switch ( subMenuIndex )"
}											// End of "DoAnalysis()"


/*
 *	"DoOptions()" handles processing of the tasks listed under the "View/Save" main menu item.
 */

void DoOptions()
{
	ShowSubMenu ( menuViewSave, ELEMENTS ( menuViewSave ));		// Display the sub-menu

	switch ( subMenuIndex )						// What does the operator want to do?
	{
		case 0:									// Save Scan
			SaveScan ();
			break;

		case 1:									// View old plot
			ViewOldPlot ();
			Examine ();							// Analyze data?
			break;

		case 2:									// Do overlay
			PlotOverlay ();
			break;

		case 3:									// View file data in table format
			ViewTable ();
			break;

		case 4:									// Serial monitor output
			PlotToSerial ();
			break;

		case 5:									// View Minimums
	        DrawBarChart ();					// Just do it
	        break;

		default:								// Go back to main menu
			exitFlag = true;					// Without waiting for encoder switch push
			break;
	}											// End of menu switch
}												// End of DoOptions


/*
 *	"DoMaintenance()" was added in Version 01.1 along with a new main menu option of "Maintenance".
 *	The "Delete File" function was moved from the "View/Save" menu to this one, and new functions to
 *	"Delete All (files)", "Reset Seq #" and "Clear Mins" were added.
 *
 *	"Reset Seq #" can only be used if there are no files currently on the SD card.
 */

void DoMaintenance()
{
	ShowSubMenu ( menuMaint, ELEMENTS ( menuMaint ));		// Display the menu choices

	switch ( subMenuIndex )						// What does operator want to do?
	{
		case 0:									// Delete single file
			DeleteSingleFile ();
        	break;

		case 1:									// Delete All Files
			DeleteAllFiles ();
			break;

    	case 2:									// Reset File Sequence Number
			ResetFileSeqNumber ();
			break;

/*
 *	Case "3" resets the minimum SWR readings for each band both in the memory array
 *	and in the EPROM.
 */

		case 3:										// Reset minimum SWRs

			SetEEPROMMins ();						// And Poof, it's done

			tft.fillScreen ( BLACK );				// Fade to black

			PaintText ( 2, SCREEN_C, 5, 0, NORMAL, CENTER, "SWR Minimum readings have been reset!" );
			PaintText ( 2, SCREEN_C, 8, 0, NORMAL, CENTER, msgPushButton );
			break;

		case 4:										// Erase entire EEPROM (Except next file number)
			EraseEEPROM ();							// Poof!
			break;

		case 5:										// Send EEPROM data to serial monitor
			ReadEEPROM ();
			break;

		case 6:										// Mount SD card
			Mount_SD ();
			if ( SD_Status )
			{
				PaintText ( 2, SCREEN_C, 7, 0, WARNING, CENTER, "SD Card mounted successfully" );
			
				delay ( 3000 );							// Give operator 3 seconds to read the message
			}

			exitFlag = true;
			break;

		case 7:										// Frequency calibration
			DoFreqCal ();							// Do it
			break;

		case 8:
			DoSetOptions ();						// Set the program options
			break;

		default:								// Go back to main menu
			exitFlag = true;					// Without waiting for encoder switch push
			break;
	}											// End of switch ( subMenuIndex )
}												// End of DoMaintenance


/********************************  COMMAND PROCESSING FUNCTIONS  ********************************
 *
 *	The functions in this section are the main processing functions for the commands listed
 *	in the sub-menus.
 */

/*	"DoNewScan()" sets things up to do a new scan. It was modified in Version 01.2
 *	to no longer ask for the band and frequency limits, but rather to use those set at
 *	startup or via the "Change Band" and/or "Set Limits" menu options.
 *
 *	It underwent a major overhaul in Version 02.7 as part of converting the SWR graphs
 *	from having a linear Y axis to using the more appropriate logarithmic scale.
 *
 *	Modified in Version 02.8 to take a parameter indicating whether a single scan is to
 *	be performed or repeated scans. If doing repeated scans, we look for the encoder switch
 *	to be pushed during the scan, and exit after the current scan.
 *
 *	Modified in Version 03.2 to label the scan plot.
 *
 *	Modified in Version 03.8 to label the scan plot with the counters when doing
 *	repeated scans. The "nnn/nnn" label will be in place of the "LIVE" label and will
 *	appear regardless of whether the "LABEL_SCAN" symbol is defined or not.
 */

void DoNewScan ( bool mode )
{

	if ( optStats )
		MyStats.InitAVGReadingsForScan();      
	int		ix;									// Loop index

	stopRepeating = false;						// If in repeat mode, keep going

	DDS.up ();									// Wake up the DDS
	delay ( 10 );								// Give it a moment

	scan.isValid = false;								// Clear valid plot data indicator

	scan.minFreq = (float) activeBandBottom * 1000.0;	// Get frequency range
	scan.maxFreq = (float) activeBandTop    * 1000.0;

	scan.minSWR = 100.0;								// Off the chart minimum
	scan.maxSWR =  10.0;								// Top of the range is now 10

/*
 *	Compute "deltaX", which is the change in frequency between frequencies to be tested. We
 *	want 100 ("SCAN_INTERVALS") between the endpoints of the frequency range.
 */
	scan.deltaX = ( scan.maxFreq - scan.minFreq ) / SCAN_INTERVALS;

	GraphAxis ();								// Paint the grid


/*
 *	Changes added in Version 03.2 to label the plot if "LABEL_SCAN" is defined:
 */

	if ( mode == REPEAT_SCAN )
	{
		sprintf ( scan.fileName, "%u/%u", scanCount, repeatCount );
 		PaintText ( 2, SCREEN_C, 1, 3, SCAN_LBL, CENTER, scan.fileName );
	}

	else
	{
		#if ( defined ( LABEL_SCAN ))
 			strcpy ( scan.fileName, "LIVE" );
 			PaintText ( 2, SCREEN_C, 1, 3, SCAN_LBL, CENTER, scan.fileName );
		#endif
	}

/*
 *	The following loop gets the SWR reading for each frequency calculated above. "ix" keeps
 *	track of the index into the frequency and SWR arrays in the scan structure. This was
 *	changed in Version 02.7 to simplify the code. In previous versions, there was one loop
 *	for taking the readings an another loop to display them on the graph. Since I combined
 *	the 2 processes into one loop, it now looks like the plot gets drawn very slowly, which
 *	it is, however, the time between drawing the graph and beginning to draw the plot is
 *	almost zero. In other words, it takes the scame amount of total time, but it looks like
 *	it's taking longer.
 *
 *	Modified in Version 02.8: If in repeat mode, check the encoder switch and if it's pressed,
 *	set "stopRepeating" to "true". If it's already set to true, we stop testing the switch.
 */

	scan.xPos = scan.minFreq;

	for ( ix = 0; ix < MAX_PLOT_POINTS; ix++ )
	{
        if ( stopRepeating ) break;
        
		if ( mode == REPEAT_SCAN && !stopRepeating )
			if ( digitalRead ( SWITCH ) == LOW )		// Switch pushed?
				stopRepeating = true;					// Yes, stop after this cycle
			
		GetNextPoint ( scan.xPos, ix );					// Get next frequency/SWR pair

		scan.freq[ix] = scan.xPos;						// Save frequency
		scan.yPos     = scan.swr[ix];					// And SWR

		GraphPoints ( scan.xPos, scan.yPos, YELLOW );	// Add to the plot

		if ( scan.swr[ix] < scan.minSWR )				// New minimum SWR?
		{
			scan.minSWR   = scan.swr[ix];				// Remember new minimum
			scan.minIndex = ix;							// And where it is in the array
		}
		scan.xPos += scan.deltaX;						// Next frequency
	}

	if ( optStats && !stopRepeating )
		MyStats.LogScanStatsSummary();
	
	
/*
 *	Update the minimum SWR value for this band in the "minSWRs" array and in the appropriate
 *	spot in the EEPROM. Note: Before rewriting this code, I was under the impression that the
 *	saved minimum SWR for each band was the all time low. It is not; it is the minimum SWR
 *	from the last scan done!
 */

	int val = scan.minSWR * 100.0;						// Integer version of SWR
	minSWRs[activeBandIndex] = val;						// Update minimums array

	EEPROM.put ( SWR_MINS_ADDRESS + ( activeBandIndex * 2 ), val );	// Update minimum in EEPROM


/*
 *	Finally, paint the heading and if it's not off the scale, mark the minimum SWR
 *	on the graph with a red '+'. If the "AUTO_EXAMINE" symbol is defined, we don't bother
 *	with the heading as it's going to change almost immediately.
 */

	#ifndef	AUTO_EXAMINE
	
		PaintHeading ( 5, scan.swr[scan.minIndex], scan.freq[scan.minIndex], NO_PAGE );

	#endif

	if ( scan.minSWR <= 10.0 )					// Don't paint the '+' if off the chart
		MarkMinimum ( scan );

	DDS.down ();								// Put the DDS to sleep	
	scan.isValid = true;						// The "scan" structure contains valid data

	DEBUG_BEGIN
		DisplayScanStruct ( scan, "Main scan structure - End of DoNewScan" );
	DEBUG_END
}


/*
 *	"RepeatScan()" allows the operator to do a number of scans one after another. We ask
 *	how many cycles should be performed. The operator can set the repeat count in steps
 *	of 10. We will limit the number of repetitions to 100. The operator can stop the process
 *	anytime by pressing the encoder switch. That will cause the process to terminate at the
 *	end of the current scan.
 */

void RepeatScan ()
{
	char	buff[10];								// For displayed count

	tft.fillScreen ( BLACK );						// Fade to black
	itoa ( repeatCount, buff, 10 );					// Make repeat count ASCII
	
	PaintText ( 3, SCREEN_C,  2, 0, NORMAL,  CENTER, "Set repetition count:" );
	PaintText ( 3, SCREEN_C,  4, 0, HEADING, CENTER, buff );
	PaintText ( 2, SCREEN_C, 10, 0, NORMAL,  CENTER, msgPushButton );

	ResetEncoder ();								// Clear the encoder flags

	while ( digitalRead ( SWITCH ))					// While encoder switch not pushed
	{
		if ( encoderDirection != DIR_NONE )			// If the encoder moved
		{
			switch ( encoderDirection )				// Change repeat count
			{
				case DIR_CW:								// Next higher
					repeatCount += REPEAT_INCREMENT;		// Increment the index
					if ( repeatCount > MAX_REPEAT_COUNT )	// Upper limit?
						repeatCount  = MAX_REPEAT_COUNT;
					break;

				case DIR_CCW:								// Next lower
					repeatCount -= REPEAT_INCREMENT;		// Decrement the index
					if ( repeatCount < MIN_REPEAT_COUNT )	// Out of bounds
						repeatCount = MIN_REPEAT_COUNT;

					break;
			}

			ResetEncoder ();								// Clear encoder flags
			
			itoa ( repeatCount, buff, 10 );					// Make repeat count ASCII

			EraseText ( 3, SCREEN_C, 4, 0, BLACK,   CENTER, 10 );
			PaintText ( 3, SCREEN_C, 4, 0, HEADING, CENTER, buff );
		}
	}

	for ( int i = 0; i < repeatCount; i++ )
	{
		stopRepeating = false;						// Clear the stop flag
		scanCount = i + 1;							// Current scan counter
		DoNewScan ( REPEAT_SCAN );					// Do a scan

		delay ( SCAN_PAUSE );						// Brief time to read header

		if ( stopRepeating )						// Operator want to quit?
			break;									// Yes, so no more scans
	}
}


/*
 *	"Examine()" is added in Version 02.9. It will be called from the menu processing switches
 *	immediately after doing a live scan or a "View Plot". If the operator presses the encoder
 *	switch, control will be returned to the main menu just as in earlier versions upon the
 *	completion of one of the above commands.
 *
 *	But, if the operator rotates the encoder, a cursor will appear along the scan plot which
 *	can be moved left or right along the scan trace. As the cursor is moved, the heading will
 *	be updated to show the frequency at which the cursor is located and the SWR at that
 *	frequency.
 *
 *	Modified in Version 03.1. If the "AUTO_EXAMINE" symbol is defined in the header file,
 *	we automatically go into "Examine" mode without waiting for the encoder to be moved. The
 *	way the header data is displayed is also changed to make it paint faster and changed the
 *	starting position of the cursor line to the minimum SWR point.
 *
 *	Modified in Version 03.5 to add an optional (based on the setting of "HORIZ_INDEX" in the
 *	header file) cursor line.
 */

void Examine ()
{
	char 		shortBuff [10];						// For Frequency and SWR

	uint32_t	freq;								// Integer frequency
	uint16_t	swr;								// And SWR
	
	int			index;;								// Index to frequency/SWR pair
	
	int			xPos;								// X coordinate of current vertical index line
	int			lastX;								// X coordinate of previous vertical index line
	
	int			yPos;								// X coordinate of current horizontal index line
	int			lastY;								// X coordinate of previous horizontal index line

	uint16_t	vPixel[scan.graphHeight];			// Place to save vertical pixels
	uint16_t	hPixel[scan.graphWidth];			// Place to save horizontal pixels

	uint16_t	hColor;								// Color of horiaontal line varies
	bool		firstClick = true;					// Handle first encoder click differently

/*
 *	Figure out the coordinates of the starting point, which is where the minimum SWR occurrs.
 */

	lastX =  ( scan.freq[scan.minIndex] - scan.minFreq ) * ( scan.graphWidth ) /
						( scan.maxFreq - scan.minFreq ) + scan.xOrigin;

	lastY = scan.yOrigin - ( log10 ( scan.swr[scan.minIndex] ) * scan.graphHeight );

	index = scan.minIndex;						// Local copy of array index


/*
 *	Before we paint the index lines on the graph, we need to save the status of the pixels
 *	that are located where those lines will be so we can restore them later.
 */

	for ( int i = 0; i < scan.graphHeight; i++ )				// Save vertical pixels
		vPixel[i] = tft.readPixel ( lastX, scan.yOrigin-i );

	for ( int i = 0; i < scan.graphWidth; i++ )					// Save horizontal pixels
		hPixel[i] = tft.readPixel ( scan.xOrigin+i, lastY );


	#ifdef	AUTO_EXAMINE							// Don't wait for actual encoder movement

		encoderDirection = DIR_CW;					// But, instead, fake it!
		clickCount = 1;

	#endif
	
	while ( digitalRead ( SWITCH ))					// Do until the switch is pushed
	{
		if ( encoderDirection != DIR_NONE )			// If the encoder moved (or if we faked it)
		{
			if ( !firstClick )						// Don't move it on first click
				index += clickCount;				// Remember, clickCount can be + or -

			ResetEncoder ();						// Clear encoder flags

			if ( index > SCAN_INTERVALS )			// Upper limit?
				index  = SCAN_INTERVALS;			// Hold here

			if ( index < 0 )						// Out of bounds
				index = 0;							// Hold at zero

			if ( firstClick )						// Paint the heading titles
			{
				tft.fillRect ( scan.xOrigin, 0, scan.graphWidth, 16, BLACK );
				PaintText ( 2, 7, 0, 2, HEADING, LEFT, "SWR: " );
				PaintText ( 2, 18, 0, 2, HEADING, LEFT, "at Freq: " );
			}

/*
 *	Fill in the current SWR and frequency values
 */

			swr = scan.swr[index] * 100;
			FormatSWR ( swr, shortBuff );			// Add the SWR
			PaintText ( 2, 12, 0, 2, HEADING, LEFT, shortBuff );

			freq = scan.freq[index] / 1000;
			FormatFrequency ( freq, shortBuff, 3 );	// Add the frequency
			PaintText ( 2, 27, 0, 2, HEADING, LEFT, shortBuff );


/*
 *	Figure out the current coordinates
 */

			xPos = scan.graphWidth * (( scan.freq[index] - scan.minFreq ) /
												( scan.maxFreq - scan.minFreq ));
			xPos = xPos + scan.xOrigin;

			yPos = scan.yOrigin - ( log10 ( scan.swr[index] ) * scan.graphHeight );


/*
 *	Restore the pixels that were over written by the previous index lines. We don't
 *	need this on the first time through, since no lines have been painted.
 */

			if ( !firstClick )
			{
				for ( int i = 0; i < scan.graphHeight; i++ )			// Erase last cursor with the
					tft.drawPixel ( lastX, scan.yOrigin-i, vPixel[i] );	// original vertical pixels

				for ( int i = 0; i < scan.graphWidth; i++ )				// Erase last cursor with the
					tft.drawPixel ( scan.xOrigin+i, lastY, hPixel[i] );	// original horizontal pixels

			}

/*
 *	Now, save the pixels where we are about to draw the new lines
 */

			for ( int i = 0; i < scan.graphHeight; i++ )				// Save horizontal pixels
				vPixel[i] = tft.readPixel ( xPos, scan.yOrigin-i );

			for ( int i = 0; i < scan.graphWidth; i++ )					// Save vertical pixels
				hPixel[i] = tft.readPixel ( scan.xOrigin+i, yPos );
				

/*
 *	And finally, draw the new index lines; note, we only draw the horizontal index if the
 *	symbol "HORIZ_INDEX" is defined:
 */

			tft.drawLine ( xPos, scan.yOrigin-scan.graphHeight+2, xPos, scan.yOrigin, RED );

			#ifdef HORIZ_INDEX
			
				hColor = RED;							// Assume SWR >= 2:1

				if ( scan.swr[index] < 2.00 )			// If less than 2:1
					hColor = YELLOW;					// Yellow line

				if ( scan.swr[index] < 1.50 )			// Less that 1.5:1?
					hColor = GREEN;						// Then green line

				if ( scan.swr[index] <10 )				// Don't go off the chart
					tft.drawLine ( scan.xOrigin+scan.graphWidth, yPos, scan.xOrigin, yPos, hColor );

			#endif

			firstClick = false;				// Been through once
			
			lastX = xPos;					// Save coordinates for next pass
			lastY = yPos;
		}
	}
	
	ResetEncoder ();						// Clear encoder flags
}


/*
 *	"DoSingleFrequency()" is code that was moved out of the case statement in the
 *	"DoAnalysis()" function and made into a separate function. It computes the midpoint
 *	of the current active band and begins taking SWR readings every 250mS at that frequency.
 *
 *	The frequency can be changed using the encoder and the amount the frequency will change
 *	with each move of the encoder can be set for 100KHz, 10KHz or 1KHz steps if the "Fine
 *	Tuning" option is installed in the hardware. If the option is not installed, each
 *	click of the encoder will adjust the frequency by 5KHz as in the original software.
 *
 *	Modified in Version 02.4 so that if the current active band is 6 meters or "Custom",
 *	the frequency can also be adjusted in 1MHz steps.
 *
 *	Modified in Version 02.6 to add an analog SWR meter.
 *
 *	Modified in Version 03.6 to optionally add the raw pin data and ability to toggle the
 *	DDS on and off.
 */

void DoSingleFrequency ()
{
	uint16_t	localSWR;					// Integer version of SWR
	float		realSWR;					// Real SWR value
	char		buff[10];					// For displayed frequency & SWR
	float		realFreq;					// Frequency for the DDS
	uint32_t	tempFreq;					// Used for rounding

	DDS.up ();								// Wake up the DDS
	delay ( 10 );							// Give it a moment

	SWR_dataHeaderDone = false;				// Haven't painted the header for the detail data

/*
 *	Get the band center frequency (using limits set by the operator) and make it a multiple of 5KHz 
 */

	tempFreq =  activeBandTop;				// Only seems to work right
	tempFreq += activeBandBottom;			// When strung out into multiple
	tempFreq /= 2L;							// Statements like this!

	tempFreq = ( tempFreq / 5 ) * 5;

	realFreq = tempFreq * 1000;				// For the DDS

	DDS.setfreq ( realFreq );				// Send the frequency to the DDS (either one)

	tft.fillScreen ( BLACK );						// Fade to black

	PaintMeter ();									// Draw the SWR meter

	ShowDebugMode ();								// Show debug mode
	
	FormatFrequency ( tempFreq, buff, 3 );			// Format frequency into "buff"

	FT_pushCount = 2;								// Carat under last digit

	if ( FT_installed )
		activeFreqIncrement = freqIncrements[FT_pushCount];		// 1KHz

	else
		activeFreqIncrement = FIXED_FREQ_INCR;					// The old default (5KHz)


	PaintText ( 3, 15, 5, 0, NORMAL, RIGHT, "Frequency:" );		// Display the frequency
	DisplayFrequency ( 3, 16, 5, 0, HEADING, LEFT, buff );
	PaintText ( 3, 15, 7, 0, NORMAL, RIGHT, "SWR:" );			// And SWR heading

	while ( digitalRead ( SWITCH ))						// While encoder switch not operated
	{
		ReadFT ();										// "Fine Tune" button pushed?

		if ( newFT_Interrupt )							// Need to move the carat?
		{
			FormatFrequency ( tempFreq, buff, 3 );		// Format frequency into "buff"

			DisplayFrequency ( 3, 16, 5, 0, HEADING, LEFT, buff );
			activeFreqIncrement = freqIncrements[FT_pushCount];

			newFT_Interrupt = false;
		}

		if ( encoderDirection != DIR_NONE )			// If encoder changed
		{
			tempFreq += ( activeFreqIncrement * clickCount );			// "clickCount" is plus or minus

			if ( tempFreq > GetBandEdge ( activeBandIndex, UPPER ))		// Stay within band limits
				tempFreq = GetBandEdge ( activeBandIndex, UPPER );

			if ( tempFreq < GetBandEdge ( activeBandIndex, LOWER ))
				tempFreq = GetBandEdge ( activeBandIndex, LOWER );

			realFreq = tempFreq * 1000;				// For the DDS
			DDS.setfreq ( realFreq );				// Send the frequency to the DDS (either one)
     //Let it settle
      //If this is not done, the first readings are funky
      float TempReading = 0;
      for(int i = 0; i<5; i++)
      {
        TempReading = (float) analogRead  (ANALOG_FORWARD ); 
        TempReading = (float) analogRead  (ANALOG_REFLECTED ); 
      }
	  

			FormatFrequency ( tempFreq, buff, 3 );						// Format frequency into "buff"
			DisplayFrequency ( 3, 16, 5, 0, HEADING, LEFT, buff );		// And update the display
		}

		ResetEncoder ();						// Reset the encoder flags

		realSWR = ReadSWRValue ( false );		// Read new SWR ("false" says not doing a scan)
		
		localSWR = 100 * realSWR;				// Integer version

		MovePointer ( realSWR );

		FormatSWR ( localSWR, buff );						// Convert SWR to ASCII
		PaintText ( 3, 16, 7, 0, HEADING, LEFT, buff );		// And display it
		
		PaintSwrData ( realFreq, realSWR, FWD, REV );		// Also does the meter
		
		delay ( 250 );							// 1/4 second between readings

	}											// End of "while ( digitalRead ( SWITCH ) != LOW )"
	ResetFT ();									// And the "Fine Tune button
	DDS.down ();								// Put the DDS to sleep	

	if ( optStats )
		MyStats.LogScanStatsSummary();
}


/*
 *	"DoCalibration()" is completely overhauled in Version 03.6 based on work by Dave (M0WID).
 *
 *	Modified in Version 03.8 to compute the frequency at which to do the calibration
 *	based on the frequency limits set by the user as opposed to the legal band edge
 *	limits. This wasn't a big deal on any bands except the "Custom" band, where the
 *	selected calibration frequency was sometimes outside the user set limits.
 *
 *	Actually, there are now two versions of the function; one for Edwin's (PE1PWF) version of
 *	the analyzer and one for Glenn's (VK3PE) version.
 *
 *	Edwin's version uses a fudge factor in the SWR computation, which might seem a bit flakey,
 *	however, it really works throughout the frequency range of the analyzer (depending on which
 *	DDS you have).
 *
 *	Glenn's version is a little different. The primary calibration on his is done by adjusting
 *	a variable resistor on the board to balance the AD8307 log amps. What we do here is tweak
 *	the SWR formula to compensate for slight differences in the characteristics of specific
 *	AD8307s.
 *
 *	Both versions now include a display of the forward and reverse readings (averaged) and the
 *	difference between them.
 *
 *	The following is the version for Edwin's version of the analyzer:
 */

#ifdef PE1PWF_MOD

void DoCalibration ()
{
	int			i;							// Loop counter

	float		sum[2] = {0.0, 0.0};		// Sum of forward & reverse voltage readings

 	float		realSWR;					// Copy of SWR
 	float		adjustedSWR;				// SWR with calibration applied

 	double		realFreq;					// Frequency for the DDS
	uint32_t	tempFreq;					// Used for rounding

	char		buff[10];					// For displayed frequency & SWR

	uint16_t	samples = MAX_POINTS_PER_SAMPLE;
	
	SWR_dataHeaderDone  = false;			// Haven't painted the header for the detail data

	DDS.up ();								// Wake up the DDS
	delay ( 10 );							// Give it a moment
	ddsOn = true;							// Show it is on



/*
 *	Get the band center frequency and make it a multiple of 5KHz 
 */

	tempFreq  = GetActiveEdge ( UPPER );		// Only seems to work right
	tempFreq += GetActiveEdge ( LOWER );		// When strung out into multiple
	tempFreq /= 2L;								// Statements like this!
			
	tempFreq = ( tempFreq / 5 ) * 5;			// Round to nearest 5KHz

	realFreq = tempFreq * 1000;					// For the DDS

	DDS.setfreq ( realFreq );					// Send the frequency to the DDS (either one)

	tft.fillScreen ( BLACK );					// Fade to black
			
	FormatFrequency ( tempFreq, buff, 3 );		// Format frequency into "buff"

	PaintText ( 3, 13, 2, 1, NORMAL, RIGHT, "Frequency:" );	// Display the frequency
	PaintText ( 3, 14, 2, 1, HEADING, LEFT, buff );
	PaintText ( 3, 13, 5, 0, NORMAL, RIGHT, "SWR:" );		// And SWR heading

	ShowDebugMode ();										// Show debug mode
	
	while ( digitalRead ( SWITCH ))						// While encoder switch not operated
	{
		if ( encoderDirection != DIR_NONE )				// If encoder changed
		{
			calFactor += clickCount;

			if ( calFactor == 0 )						// Can't let it go to zero
				calFactor = 1;
		}										// End of if ( encoderDirection != DIR_NONE )

		ResetEncoder ();						// Reset the encoder flags
		ResetFT ();

		sum[0] = sum[1] = 0.0;					// Reset totaler

		for ( i = 0; i < samples; i++ )			// Take multiple samples at each frequency
		{
			sum[0] += (float) analogRead  ( ANALOG_FORWARD );	// Add forward voltage to the total
			sum[1] += (float) analogRead  ( ANALOG_REFLECTED );	// Same for reverse voltage
		}
	
		FWD = sum[0] / (float) samples;			// Average forward voltage
		REV = sum[1] / (float) samples;			// Average reverse voltage


		adjustedSWR = ((((( FWD + REV ) / ( FWD - REV )) - 1 ) * ((float) calFactor / 100 )) + 1 );

		adjustedSWR *= 100;

		DEBUG_BEGIN
			Serial.print( "FWD = " );
			Serial.print ( FWD );
			Serial.print ( ", REV = " );
			Serial.print ( REV );
			Serial.print ( ", calFactor = " );
			Serial.print ( calFactor );
			Serial.print( ", adjustedSWR = " );
			Serial.println ( adjustedSWR );
		DEBUG_END

		FormatSWR ( adjustedSWR , buff );					// Convert SWR to ASCII
		PaintText ( 3, 14, 5, 0, HEADING, LEFT, buff );		// And display it

		PaintSwrData ( realFreq, adjustedSWR, FWD, REV );	// Paint the details

		delay ( 250 );									// 1/4 second between readings

	}							// End of "while ( digitalRead ( SWITCH ) != LOW )"

		EEPROM.put ( SWR_CALIBRATION , calFactor );	// Save new calibration factor value
}
#endif


/*
 *	The following is the version of DoCalibration for Glenn's version of the analyzer:
 */

#ifdef AD8307_SWR

void DoCalibration ()
{
	int			i;							// Loop counter

	float		sum[2] = {0.0, 0.0};		// Sum of forward & reverse voltage readings

 	float		realSWR;					// Copy of SWR
 	float		adjustedSWR;				// SWR with calibration applied

 	double		realFreq;					// Frequency for the DDS
	uint32_t	tempFreq;					// Used for rounding

	char		buff[10];					// For displayed frequency & SWR

	bool		ddsOn;						// Is the DDS turned on or not?

	uint16_t	samples = MAX_POINTS_PER_SAMPLE * 4;	// Attempt to reduce the jitters
	
	SWR_dataHeaderDone  = false;			// Haven't painted the header for the detail data

	DDS.up ();								// Wake up the DDS
	delay ( 10 );							// Give it a moment
	ddsOn = true;							// Show it is on

/*
 *	Get the band center frequency and make it a multiple of 5KHz 
 */

	tempFreq  = GetActiveEdge ( UPPER );		// Only seems to work right
	tempFreq += GetActiveEdge ( LOWER );		// When strung out into multiple
	tempFreq /= 2L;								// Statements like this!
			
	tempFreq = ( tempFreq / 5 ) * 5;			// Round to nearest 5KHz

	realFreq = tempFreq * 1000;					// For the DDS

	DDS.setfreq ( realFreq );					// Send the frequency to the DDS (either one)

	tft.fillScreen ( BLACK );					// Fade to black
			
	FormatFrequency ( tempFreq, buff, 3 );		// Format frequency into "buff"

	PaintText ( 3, 13, 2, 1, NORMAL, RIGHT, "Frequency:" );	// Display the frequency
	PaintText ( 3, 14, 2, 1, HEADING, LEFT, buff );
	PaintText ( 3, 13, 5, 0, NORMAL, RIGHT, "SWR:" );		// And SWR heading
	
	ShowDebugMode ();										// Show debug mode

	while ( digitalRead ( SWITCH ))						// While encoder switch not operated
	{
		if ( encoderDirection != DIR_NONE )				// If encoder changed
		{
    		slope += ( float ) clickCount * 0.0001;		// Adjust the AD8307 slope factor
    		
    		if (slope < 0.02)							// Range limited between 0.02
    			slope = 0.02;

			if (slope > 0.03)							// and 0.03
				slope = 0.03;  
		}										// End of if ( encoderDirection != DIR_NONE )


		ResetEncoder ();						// Reset the encoder flags
		ResetFT ();

		sum[0] = sum[1] = 0.0;					// Reset totalers

		for ( i = 0; i < samples; i++ )			// Take multiple samples at each frequency
		{
			sum[0] += (float) analogRead  ( ANALOG_FORWARD );	// Add forward voltage to the total
			sum[1] += (float) analogRead  ( ANALOG_REFLECTED );	// Same for reverse voltage
		}
	
		FWD = sum[0] / (float) samples;			// Average forward voltage
		REV = sum[1] / (float) samples;			// Average reverse voltage
		
	   	returnLoss = ( FWD - REV ) * ( vRef / ( 1023.0 * slope ));		// Return Loss calculation
    	reflectionCoefficient = pow ( 10.0,- ( returnLoss / 20.0 ));	// Calculate reflection 
    		                   
 		adjustedSWR = ( 1 + reflectionCoefficient ) /
 								( 1 - reflectionCoefficient ) * 100.0;		// Calculate VSWR
 
		if ( adjustedSWR > 9999 )				// Avoid loss of leading digits in display
			adjustedSWR = 0;					// giving misleading SWR display

		DEBUG_BEGIN
			Serial.print( "FWD = " );
			Serial.print ( FWD );
			Serial.print ( ", REV = " );
			Serial.print ( REV );
	        Serial.print ( ", slope = " );
       		Serial.print ( slope, 5 );
       		Serial.print ( ", returnLoss = " );
       		Serial.print ( returnLoss );
			Serial.print( ", adjustedSWR = " );
			Serial.println ( adjustedSWR );
		DEBUG_END

		FormatSWR ( adjustedSWR , buff );					// Convert SWR to ASCII
		PaintText ( 3, 14, 5, 0, HEADING, LEFT, buff );		// And display it
		
		PaintSwrData ( realFreq, adjustedSWR, FWD, REV );	// Paint the details
		
		delay ( 250 );								// 1/4 second between readings

	}							// End of "while ( digitalRead ( SWITCH ) != LOW )"

	EEPROM.put ( SLOPE_CALIBRATION, slope );    
}
#endif


/*
 *	"SaveScan()" saves the last scan performed or the scan last viewed with "View Plot" or
 *	"View Table" to a new file on the SD card. Of course if you're saving a scan done with
 *	one of the "View" options, you're just saving a second copy of the same data.
 */

void SaveScan ()
{
	char	tempNum[5];						// ASCII version of number part of file name
	char	fileName[13];					// Used to construct the file name
	char	workBuff[50];					// Used to build display messages

	if ( !scan.isValid )					// If scan structure not valid!
	{
		PaintText ( 2, SCREEN_C, 5, 0, ERR, CENTER, "No acvtive scan!" );
		PaintText ( 2, SCREEN_C, 7, 0, ERR, CENTER, "Unable to save data!" );
		PaintText ( 2, SCREEN_C, 10, 0, NORMAL, CENTER, msgPushButton );
		return;
	}

	strcpy ( fileName, "SCAN" );				// All file names are "SCAN" plus some number

	itoa ( nextSD_FileNumber, tempNum, 10 );	// ASCII File number

	if ( nextSD_FileNumber < 10 )				// Single digit?
	strcat ( fileName, "0" );					// Add leading zero

	strcat ( fileName, tempNum );				// Make complete file name
	strcat ( fileName, ".CSV" );				// Plus "CSV" extension

	root = SD.open ( fileName, FILE_WRITE );	// Open the file for writing

	if ( !root )								// Did it open sucessfully?
	{
		Display_SD_Err_4 ();					// Error message
	       return;
	}

	strcpy ( workBuff, "Write new file: " );
	strcat ( workBuff, fileName );

	PaintText ( 2, SCREEN_C, 5, 0, NORMAL, RIGHT, workBuff );

	WriteScanData ( root );						// Opened ok, so write the scan data

	strcpy ( workBuff, "File named: " );
	strcat ( workBuff, fileName );
	strcat ( workBuff, " created!" );

	PaintText ( 2, SCREEN_C,  7, 0, NORMAL, CENTER, workBuff );
	PaintText ( 2, SCREEN_C, 10, 0, NORMAL, CENTER, msgPushButton );

	nextSD_FileNumber++;									// Update the number for the next new file

	if ( nextSD_FileNumber > 99 )							// 2 digit limit
		nextSD_FileNumber = 0;								// Reset to zero

	EEPROM.put ( NEXT_SD_FILE_NUMBER, nextSD_FileNumber );	// And save it in the EEPROM
}


/*
 *	"ViewOldPlot()" reads the data saved in a file on the SD card and displays it identically
 *	to when it was originally performed.
 *
 *	Modified in Version 03.2 to label to the plot with the file name from whence it came.
 */

void ViewOldPlot ()
{
	int pairCount;						// Number of SWR/frequency pairs
			
	scan.isValid = false;				// No valid scan in memory

	CountFiles ();						// How many files on the card (answer in fileCount)

	if ( fileCount < 0 )				// File system error
		return;

	if ( fileCount == 0 )				// If no files on the card
	{
		Display_SD_Err_2 ();			// Send error message
		return;
	}

	SelectFile();								// Display list of available files

	root = SD.open ( mySDFiles[useFile], FILE_READ );	// Open the file for reading

	if ( !root )										// Did it open sucessfully?
	{
		Display_SD_Err_4 ();							// No, send error message
	        return;
	}
			
	memset ( scan.swr,  0, sizeof ( scan.swr ));		// Zero out the swr array
	memset ( scan.freq, 0, sizeof ( scan.freq));		// And the frequency array

	scan.minFreq = 0;
	scan.maxFreq = 0;

	strcpy ( scan.fileName, root.name () );
	scan.fileName[6] = '\0';
	
	pairCount = ReadScanDataFile ( &scan );

/*
 *	Initialize graphics structure values. Assumes minimum SWRs are in the vswrs[] array
 */

	scan.lastX = scan.minFreq;					// Copy of minimum frequency
	scan.lastY = scan.maxSWR;					// And maximum SWR

	GraphAxis ();								// Draw the lines & labels

	scan.deltaX = ( scan.maxFreq - scan.minFreq ) / SCAN_INTERVALS;	// Interval between lines

	for ( int k = 0; k < pairCount; k++ )
		GraphPoints ( scan.freq[k] , scan.swr[k], YELLOW );

/*
 *	Finally, paint the heading and if it's not off the scale, mark the minimum SWR
 *	on the graph with a red '+'. If the "AUTO_EXAMINE" symbol is defined, w don't bother
 *	with the heading as it's going to change almost immediately.
 */

	#ifndef	AUTO_EXAMINE
	
		PaintHeading ( 5, scan.swr[scan.minIndex], scan.freq[scan.minIndex], NO_PAGE );

	#endif

	if ( scan.minSWR <= 10.0 )				// Don't paint the '+' if off the chart
		MarkMinimum ( scan );
	
	scan.isValid = true;					// The "scan" structure contains valid data

	DEBUG_BEGIN
		DisplayScanStruct ( scan, "Main scan structure - End of ViewOldPlot" );
	DEBUG_END

/*
 *	Added in Version 03.2: Paint the file name on the scan plot if "LABEL_SCAN" is defined.
 */

	#ifdef LABEL_SCAN
		PaintText ( 2, SCREEN_C, 1, 3, SCAN_LBL, CENTER, scan.fileName );
	#endif
}


/*
 *	"PlotOverlay()" is used to display the contents of a saved scan on top of another one. The
 *	primary scan can either one kust performed live or the contents of another saved file that
 *	was just displayed using the "View Plot" or "View Table" options. The primary scan is
 *	displayed in yellow and the overlay scan in white. The header information on the page shows
 *	the minimum SWR and frequency for the overlay scan.
 */

void PlotOverlay ()
{
	scanStruct		ovly;					// Separate scan structure for overlays
	int	pairCount = 0;						// Number of SWR/frequency pairs
	
	ovly.isValid = false;					// No valid data in "ovly" structure

	if ( !scan.isValid )					// Need to have valid data in the "scan" structure
	{
		PaintText ( 2, SCREEN_C, 5, 0, ERR, CENTER, "No acvtive scan!" );
		PaintText ( 2, SCREEN_C, 7, 0, ERR, CENTER, "Unable to do overlay!" );
		PaintText ( 2, SCREEN_C, 10, 0, NORMAL, CENTER, msgPushButton );
		return;
	}

	CountFiles ();						// How many files on the card (answer in fileCount)

	if ( fileCount < 0 )				// File system error
		return;

	if ( fileCount == 0 )				// If no files on the card
	{
		Display_SD_Err_2 ();			// Send error message
		return;
	}

	SelectFile ();								// Display available files and get number of them

	root = SD.open (mySDFiles[useFile], FILE_READ );	// Open the selected file for reading
	if ( !root )										// Did it open sucessfully?
	{
		Display_SD_Err_4 ();							// No, send error message
		return;
	}

	memset ( ovly.swr,  0, sizeof ( ovly.swr ));		// Zero out the swr array
	memset ( ovly.freq, 0, sizeof ( ovly.freq));		// And the frequency array

	ovly.minFreq = 0;
	ovly.maxFreq = 0;

	pairCount = ReadScanDataFile ( &ovly );				// Read the overlay file

	if ( scan.minFreq != ovly.minFreq || scan.maxFreq != ovly.maxFreq )		// Ranges the same?
	{
		tft.fillScreen ( BLACK );						// Fade to black
				
		PaintText ( 2, SCREEN_C, 3, 1, ERR, CENTER, "Overlay frequency range" );
		PaintText ( 2, SCREEN_C, 4, 1, ERR, CENTER, "different than scan range!" );
		PaintText ( 2, SCREEN_C, 6, 1, ERR, CENTER, "Unable to do overlay!" );
		PaintText ( 2, SCREEN_C, 12, 0, NORMAL, CENTER, msgPushButton );
		return;
	}

	strcpy ( ovly.fileName, root.name () );
	ovly.fileName[6] = '\0';

	GraphAxis ();									// Paint the graph layout

	for ( int k = 0; k < pairCount; k++ )
		GraphPoints ( scan.freq[k] , scan.swr[k], YELLOW );		// Scan data

	ovly.lastX = ovly.minFreq;						// Copy minimum frequency
	ovly.lastY = ovly.maxSWR;						// And maximum SWR

	for ( int k = 0; k < pairCount; k++ )
		GraphPoints ( ovly.freq[k] , ovly.swr[k], WHITE );		// Overlay data

	PaintHeading ( 5, ovly.minSWR, ovly.freq[ovly.minIndex], NO_PAGE );

	if ( ovly.minSWR <= 10.0 )						// Don't paint the '+' if off the chart
		MarkMinimum ( ovly );

	if ( scan.minSWR <= 10.0 )						// Don't paint the '+' if off the chart
		MarkMinimum ( scan );

	ovly.isValid = true;				// The "ovly" structure contains valid data

	DEBUG_BEGIN
		DisplayScanStruct ( ovly, "Overlay scan structure - End of PlotOverlay" );
	DEBUG_END

/*
 *	Added in Version 03.2: Paint the file names (or "LIVE") of the original scan and the
 *	overlay on the scan plot if the "LABEL_SCAN" symbol is defined.
 */
	#ifdef	LABEL_SCAN
		PaintText ( 2, 19, 1, 3, SCAN_LBL, RIGHT, scan.fileName );
		PaintText ( 2, 20, 1, 3, OVLY_LBL, LEFT,  ovly.fileName );
	#endif
}


/*
 *	"ViewTable()" is used to display the saved scan data for a previous plot stored on the
 *	SD card. It shows the SWR/frequency pairs in a tablular format plotted top to bottom then
 *	left to right. There are four pages to the display, and rotating the encoder knob will
 *	scroll through the pages.
 */

void ViewTable ()
{
	int	pairCount = 0;					// Number of SWR/frequency pairs

	CountFiles ();						// How many files on the card (answer in fileCount)

	if ( fileCount < 0 )				// File system error
		return;

	if ( fileCount == 0 )				// If no files on the card
	{
		Display_SD_Err_2 ();			// Send error message
		return;
	}

	SelectFile();						// Get file to display

	root = SD.open ( mySDFiles[useFile], FILE_READ );	// Open it for reading

	if ( !root )										// Did it open sucessfully?
	{
		Display_SD_Err_4 ();							// No, send error message
		return;
	}

	scan.isValid = false;								// No valid scan in memory
			
	memset ( scan.swr,  0, sizeof ( scan.swr ));		// Zero out the swr array
	memset ( scan.freq, 0, sizeof ( scan.freq));		// And the frequency array

	scan.minFreq = 0;
	scan.maxFreq = 0;
			
	pairCount = ReadScanDataFile ( &scan );

	ShowAndScroll ();						// Display the data
	scan.isValid = true;					// Data is valid
}


/*
 *	"PlotToSerial()" sends the data from a savewd scan on the SD card to the Arduino serial
 *	monitor. The data displayed on the serial monitor is an exact image of the contents
 *	of the SD card file. It is in a CSV format which may be copied and pasted into Excel
 *	or any other spreadsheed program for further analysis.
 *	
 *	Unfortunately, on the Mega, there is no way to determine whether or not the USB port
 *	is actually connected to a computer, so the data will be sent blindly, and the display
 *	will indicate that it was successful regardless of whether or not the data actually
 *	went to anyplace other than thin air!
 *	
 *	Greatly simplified in Version 02.7. Instead of disecting the data in the file, then
 *	reassembling it to send to the serial monitor, we just copy it verbatum (and get the
 *	exact same result)!
 */

void PlotToSerial ()
{
	char c;								// We read the file one character as a time

	CountFiles ();						// How many files on the card? (answer in fileCount)

	if ( fileCount < 0 )				// File system error
		return;

	if ( fileCount == 0 )				// If no files on the card
	{
		Display_SD_Err_2 ();			// Send error message
		return;
	}

	SelectFile ();						// Display the available files and select one

	root = SD.open ( mySDFiles[useFile], FILE_READ );		// Open the selected file
	if ( !root )											// Did it open sucessfully?
	{
		Display_SD_Err_4 ();								// No, send error messsage
		return;
	}

	tft.fillScreen ( BLACK );					// Fade to black

	PaintText ( 2, SCREEN_C, 5, 0, NORMAL, CENTER, "Writing to Serial port" );

	while ( root.available () )					// As long as we don't run out of file
	{
		c = root.read ();						// Read next character
		Serial.print ( c );						// Write to serial monitor
	}

	PaintText ( 2, SCREEN_C, 7, 0, NORMAL, CENTER, "Serial Output Completed!" );
}


/*
 *	"DrawBarChart()" draws the new (Version 02.9) SWR minimums bar chart. When we added the
 *	capability to add 6 meters and the "Custom" band, there was no longer room to display
 *	all of the bars on the screen in the original horizontal format, so we changed it to a
 *	vertical format.
 *
 *	At the same time, the vertical (SWR) scale is converted to a logarithmic scale as were
 *	the scan graphs a few versions back. If the SWR is less than 1.5:1, the bar is painted
 *	green. If the SWR is between 1.5:1 and 2:1, the bar is yellow. If the SWR is over 2:1,
 *	the bar is painted red.
 */

void DrawBarChart ()
{

	int 	horizLines[]  = { 0, 38, 78, 124, 182, 220, 260 };			// Blue line Y coordinates
	int		greyLines[]   = { 21, 53, 66, 103, 157, 202, 235, 248 };	// Grey line Y coordinates
	
	char* 	labelsY[] = { "1.00", "1.40", "2.00", "3.00", "5.00", "7.00", "10.0" };

	int		lineY;							// Current Y coordinate
	int		color;							// Bar color

	char	buff[10];

	tft.fillScreen ( BLACK );				// Fade to black


/*
 *	Plot the horizontal lines and label them. Note the Y origin is defined as the lower left
 *	hand corner of the graph, so everything above that is referenced negatively.
 *
 *	I learned something while doing this. If no background color is specified in a call to
 *	"tft.setTextColor()", the background is transparent.
 */

	for ( int k = 0; k < ELEMENTS ( horizLines ); k++ )
	{
		lineY = bar.yOrigin - horizLines[k];				// Vertical line coordinate

		tft.drawLine ( bar.xOrigin, lineY, bar.xOrigin + bar.graphWidth, lineY, BLUE );

		tft.setTextSize  ( 1 );								// Tiny text
		tft.setTextColor ( WHITE );							// Axis label color
		tft.setCursor    ( bar.xOrigin - 40, lineY - 4 );	// Position cursor		
	   	tft.println      ( labelsY[k] );					// Print vertical axis label
	}

	for ( int k = 0; k < ELEMENTS ( greyLines ); k++ )
	{
		lineY = bar.yOrigin - greyLines[k] ;

		tft.drawLine ( bar.xOrigin, lineY, bar.xOrigin + bar.graphWidth, lineY, DKGREY );
	}


/*
 *	Draw the vertical lines on the left and right sides
 */

	tft.drawLine ( bar.xOrigin, upperPlotMargin, bar.xOrigin, bar.yOrigin, BLUE );
		
	tft.drawLine ( bar.xOrigin + bar.graphWidth, bar.yOrigin,
						bar.xOrigin + bar.graphWidth, bar.yOrigin - bar.graphHeight, BLUE );


	bar.X = bar.xOrigin + ( bar.Space / 2 );			// X coordinate of left side of 1st bar
	
/*
 *	Draw the bars and label them:
 */

	for ( int k = 0; k < bandCount; k++ )
	{
		bar.currentSWR = (float) minSWRs[k] / 100;					// Convert SWR to real value
		
		int height = log10 ( bar.currentSWR ) * bar.graphHeight;	// Computes bar height
		
		if ( height == 0 )					// Forces at a least a 1 pixel high bar;
			height = 1;						// just looks better than a blank spot

		if ( height > bar.graphHeight )		// Top limit
			height = bar.graphHeight;

		color = RED;						// Default bar color is red

		if ( bar.currentSWR < 1.5 )			// If SWR is less than 1.5:1
			color = GREEN;					// Paint the bar green

		else if ( bar.currentSWR < 2.0 )	// If the SWR is less than 2:1
			color = YELLOW;					// Paint the bar yellow

		tft.fillRect ( bar.X, bar.yOrigin - height, bar.Width, height, color );

		tft.setCursor ( bar.X - 6, bar.yOrigin + 5 );		// Band label at the bottom of the bar
		tft.print ( menuBands[k] );

		if ( minSWRs[k] != 0 )						// Real SWR there?
			FormatSWR ( minSWRs[k], buff );			// Yes, convert to ASCII

		else										// No recorded SWR for this band
			strcpy ( buff, "  N/A" );				// So we mark it "N/A"

		tft.setCursor ( bar.X-6, bar.yOrigin - height - 15 );	// SWR label at the top of the bar

		tft.print ( buff );							// Paint it
		
		bar.X = bar.X + bar.Width + bar.Space;		// Next bar
	}
}


/*
 *	"DeleteSingleFile()" deletes a single file from the SD card. A list of the files on the
 *	card is displayed, and the operator can choose the one to be deleted. If the operator
 *	their mind, the operation can be cancelled.
 *
 *	Note that if there are more than 20 files on the card, only the first 20 will be
 *	displayed.
 */

void DeleteSingleFile ()				// Delete Single File
{
	char	workBuff[50];				// For building display messages

    CountFiles ();						// How many files on the SD card? (answer in fileCount)

	if ( fileCount < 0 )				// File system error
		return;

	if ( fileCount == 0 )				// If no files on the card
	{
		Display_SD_Err_2 ();			// Send error message
		return;
	}

	SelectFile ();								// Display the file list and get the count

	ResetEncoder ();							// No encoder movement yet

	if ( ConfirmDelete ( useFile ))				// When operator confirms delete action
	{
		tft.fillScreen ( BLACK );				// Fade to black

		strcpy ( workBuff, "Deleting file: " );
		strcat ( workBuff, mySDFiles[useFile] );
		PaintText ( 2 , SCREEN_C, 5, 0, NORMAL, CENTER, workBuff );

		SD.remove ( mySDFiles[useFile] );		// Delete it

		strcpy ( workBuff, mySDFiles[useFile] );
		strcat ( workBuff, " Successfully deleted!" );
		PaintText ( 2, SCREEN_C, 7, 0, NORMAL, CENTER, workBuff );

		PaintText ( 2, SCREEN_C, 10, 0, NORMAL, CENTER, msgPushButton );
	}
}


/*
 *	"DeleteAllFiles()" allows the operator to delete all files from the SD card. A list of
 *	the files on the card is displayed, and the operator can choose the one to be deleted.
 *	If the operator their mind, the operation can be cancelled.
 *
 *	Note that if there are more than 20 files on the card, only the first 20 will be
 *	displayed and deleted.
 */

void DeleteAllFiles ()
{
	char	workBuff[50];				// For building display messages

	CountFiles ();						// How many files on the SD card? (answer in fileCount)

	if ( fileCount < 0 )				// File system error
		return;

	if ( fileCount == 0 )				// If no files on the card
	{
		Display_SD_Err_2 ();			// Send error message
		return;
	}

	ShowFiles ();						// Display the file list and get the count

	delay ( 3000 );						// Time to actually read the list

	if ( ConfirmDeleteAll ())			// When operator confirms delete action
	{
		tft.fillScreen ( BLACK );		// Fade to black

		for ( int i = 0; i < fileCount; i++ )		// Loop through them all
		{
			strcpy ( workBuff, "Deleting file: " );
			strcat ( workBuff, mySDFiles[i] );
			PaintText ( 2 , SCREEN_C, 5, 0, NORMAL, CENTER, workBuff );

			SD.remove ( mySDFiles[i] );			// Delete it
			delay ( 250 );						// Time to read
		}

		PaintText ( 2, SCREEN_C, 7, 0, NORMAL, CENTER, "All Files Deleted" );
		PaintText ( 2, SCREEN_C, 9, 0, NORMAL, CENTER, msgPushButton );
	}
}


/*
 *	"ResetFileSeqNumber()" can be used to set the next file sequence number back to zero,
 *	but only if there are no files that start with the string "SCAN" remaining on the
 *	SD card.
 */

void ResetFileSeqNumber ()
{
   	CountFiles ();							// How many files on the card? (answer in fileCount)

	if ( fileCount < 0 )					// File system error
		return;

	if ( fileCount != 0 )					// If there are files on the card
    {
		tft.fillScreen ( BLACK );			// Fade to black

		PaintText ( 2, SCREEN_C,  5, 0, ERR,    CENTER, "There are still files on the SD Card!" );
		PaintText ( 2, SCREEN_C,  7, 0, ERR,    CENTER, "Unable to reset the sequence number." );
		PaintText ( 2, SCREEN_C, 10, 0, NORMAL, CENTER, msgPushButton );
		return;
    }

    nextSD_FileNumber = 0;									// No files, then reset to zero
	EEPROM.put ( NEXT_SD_FILE_NUMBER, nextSD_FileNumber );	// And save it in the EEPROM

	tft.fillScreen ( BLACK );								// Fade to black

	PaintText ( 2, SCREEN_C, 5, 0, NORMAL, CENTER, "File sequence number reset to '0'!" );
	PaintText ( 2, SCREEN_C, 8, 0, NORMAL, CENTER, msgPushButton );
}


/*
 *	"EraseEEPROM()" sets the contents of almost the entire EEPROM memory to zeroes. While
 *	anyone can use it, it was added primarily as a tool for the developers (when they screw
 *	up the code) or for when a new release of the software requires it to be cleared as was
 *	the case in Version 02.0.
 *
 *	Note that it does not erase the saved next file sequence number as that would end up
 *	causing future save requests to overwrite existing files. 
 */

void EraseEEPROM ()
{
	int			epromAddress;				// Address in the EEPROM
	int			epromSize = 1024;			// Number of bytes in Mega EEPROM
	uint16_t	data = 0;					// What to write at each address

	tft.fillScreen ( BLACK );				// Fade to black

	PaintText ( 2, SCREEN_C, 4, 0, WARNING, CENTER, "Really erase the EEPROM?" );

	if ( ConfirmAction ( "Erase" ))							// Erase selected?
	{
		for ( epromAddress = 0; epromAddress < epromSize; epromAddress += 2 )	// Zap it
		{
			if (epromAddress != NEXT_SD_FILE_NUMBER )	// Do not reset file sequence number
				EEPROM.put ( epromAddress, data );
		}

		PaintText ( 2, SCREEN_C, 6, 1, NORMAL, CENTER, "EEPROM Data Erased!" );
		PaintText ( 2, SCREEN_C, 8, 1, NORMAL, CENTER, msgPushButton );
	}
}


/*
 *	"ReadEEPROM()" sends the current contents of the EEPROM to the serial port in a human
 *	readable format. 
 */

void ReadEEPROM ()
{
	int			address;					// Current address being read
	uint16_t	value;						// Contents at that address
	uint32_t	bigValue;					// For DDS calibration factor
	float		floatValue;					// For slope calibration factor
	char		buff[10];					// For slope in ASCII

	tft.fillScreen ( BLACK );				// Fade to black

	PaintText ( 2, SCREEN_C, 6, 0, NORMAL, CENTER, "Sending data to serial monitor" );

	for ( address = 0; address < EEPROM_NEXT_ADDR; address +=2 )	// All 2 byte integers (except FC & slope)
	{
		EEPROM.get ( address, value );					// Read the address

		Serial.print ( F ( "Address: " ) );					// Display the information

		if ( address < 100 )
			Serial.print  ( F ( "0" ) );						// 2 digit address?

		if ( address < 10 ) 							// Single digit?
			Serial.print ( F ( "0" ) );						// Leading zero

		Serial.print ( address );
		Serial.print ( F ( "\tData: " ) );
		Serial.print ( value );

		switch ( address )
		{
			case SWR_MINS_SET:
				Serial.print ( F ( "\t\tSWR Minimums valid indicator" ) );
				break;

			case SWR_MINS_ADDRESS:
				Serial.print ( F ( "\tStart of saved SWR minimums" ) );
				break;

			case ACTIVE_BAND_SET:
				Serial.print ( F ( "\t\tActive band information valid indicator" ) );
				break;

			case ACTIVE_BAND_INDEX:
				Serial.print ( F ( "\t\tSaved active band index =" ) );
				Serial.print ( menuBands[value] );
				break;

			case ACTIVE_BAND_BOTTOM:
				Serial.print ( F ( "\tLower frequency = " ) );
				Serial.print ( value );
				break;

			case ACTIVE_BAND_TOP:
				Serial.print ( F ( "\tUpper frequency = " ) );
				Serial.print ( value );
				break;

			case NEXT_SD_FILE_NUMBER:
				Serial.print ( F ( "\t\tNext SD File Sequence number = " ) );
				Serial.print ( value );
				break;

			case SWR_CALIBRATION:
				Serial.print ( F ( "\tPE1PWF SWR Calibration Factor = " ) );
				Serial.print ( value );
				break;

			case FREQ_CALIBRATION:
				EEPROM.get ( FREQ_CALIBRATION, bigValue );			// DDS calibration is 4 bytes
				Serial.print ( F ( "\tDDS Calibration Factor = " ) );
				Serial.print ( bigValue );
				break;

			case OPTION_FLAGS:
				if ( value < 10 ) Serial.print ( F( "\t" ) );
				Serial.print ( F( "\tOptions = " ) );
				if ( value & OPT_DBG_MODE )			Serial.print ( F( "Debug-Mode ") );
				if ( value & OPT_STATS )			Serial.print ( F( "Statistics ") );
				if ( value & OPT_STATS_POINT )		Serial.print ( F( "Show-Point-Stats ") );
				if ( value & OPT_STATS_LOG_IND )	Serial.print ( F( "Show-Indiv-Readings ") );
				break;

			case DDS_IN_USE:
				Serial.print ( F ( "\tDDS Type = " ) );
				Serial.print ( value );
				break;

			case SLOPE_CALIBRATION:
				EEPROM.get ( SLOPE_CALIBRATION, floatValue );	// Slope is floating point
				Serial.print ( F ( "\tAD8307 Slope Factor = " ) );
				FormatFloat ( floatValue, 1, 5, buff );
				Serial.print ( buff );
				break;

			case SERIAL_BAUD_RATE:
				EEPROM.get ( SERIAL_BAUD_RATE, bigValue );			// Serial baud rate is 4 bytes
				Serial.print ( F( "\tSerial monitor baud rate = " ) );
				Serial.print ( bigValue );
				break;

		}
		Serial.println ();
	}

	PaintText ( 2, SCREEN_C,  9, 0, NORMAL, CENTER, "EEPROM Data Transmitted" );
	PaintText ( 2, SCREEN_C, 12, 0, NORMAL, CENTER, msgPushButton );
}

/*
 *	"DoSetOptions()" shows a submenu of program options the user can set which dynamically
 *	alter the program behavior and can reduce the number of compile-time options.
 */
 
void DoSetOptions ()
{
	char*		values[ ELEMENTS ( menuOptions ) ];		// Values of each item in the menu
	int			valueCount = ELEMENTS ( menuOptions );	// Count of values
	int			valueCol = 27;							// Column of menu values
	int			oldColumn = menuCol;					// Column of currently selected item
	int			oldIndex  = menuIndex;					// Index of currently selected item
	int			row;									// Row of newly selected item
	char		buff[13];								// For string conversion

	tft.fillScreen ( BLACK );						// Fade to black
	
	PaintText ( 2, 0, 0, 1, HEADING, LEFT, "Set Program Options" );

	menuCol = 0;

	// Initialize the option values array
	values[0] = optionFlags & OPT_DBG_MODE ? strYes : strNo;
	values[1] = optionFlags & OPT_STATS ? strYes : strNo;
	values[2] = optionFlags & OPT_STATS_POINT ? strYes : strNo;
	values[3] = optionFlags & OPT_STATS_LOG_IND ? strYes : strNo;
	strcpy ( buff, " " );
	ltoa ( baudRate, &buff[1], 10 );
	strcat ( buff, " " );
	values[4] = buff;
	values[5] = '\0';
	
	ShowSubMenu ( menuOptions, ELEMENTS ( menuOptions ), values, valueCol );	// Display the menu choices
	row = subMenuIndex + 1;

	// Select a menu option and change the setting for that option
	
	while ( !exitFlag )
	{
		while ( digitalRead ( SWITCH ))			// While encoder switch not operated
		{
			if ( encoderDirection != DIR_NONE )	// Has the encoder moved?
			{
				PaintText ( 2, menuCol, row, 1, NORMAL, LEFT, menuOptions[subMenuIndex] );	// Un-highlight the current menu item

				subMenuIndex += clickCount;
				if ( subMenuIndex >= valueCount )	// Out of bounds
					subMenuIndex = 0;				// Back to the beginning
				else if ( subMenuIndex < 0 )		// Out of bounds
					subMenuIndex = valueCount - 1;	// Last one
					
				row = subMenuIndex + 1;
				PaintText ( 2, menuCol, row, 1, HIGHLIGHT, LEFT, menuOptions[subMenuIndex] );	// Highlight the selected menu item
				
				ResetEncoder ();					// Clear encoder flags
			}
		}
		
		delay ( 250 );								// Debounce
		
		PaintText ( 2, menuCol, row, 1, NORMAL, LEFT, menuOptions[subMenuIndex] );	// Un-highlight the selected menu item
		PaintText ( 2, valueCol, row, 1, HIGHLIGHT, LEFT, values[subMenuIndex] );	// Highlight the selected value
	
		switch ( subMenuIndex )							// What does operator want to do?
		{
			case 0:
				optDebugMode = SetOptionFlag ( valueCol, row, OPT_DBG_MODE, &values[subMenuIndex] );
				ShowDebugMode();
				break;
				
			case 1:
				optStats = SetOptionFlag ( valueCol, row, OPT_STATS, &values[subMenuIndex] );
				break;
	
			case 2:
				optStatsPoint = SetOptionFlag ( valueCol, row, OPT_STATS_POINT, &values[subMenuIndex] );
				break;
	
			case 3:
				optStatsLogIndiv = SetOptionFlag ( valueCol, row, OPT_STATS_LOG_IND, &values[subMenuIndex] );
				break;
	
			case 4:
				// Set serial baud rate
				int index;
				if ( !validateBaudRate ( baudRate, &index ) )
				{
					baudRate = 9600;
					validateBaudRate ( baudRate, &index );
				}

				baudRate = SetOptionInt32 ( valueCol, row, SERIAL_BAUD_RATE, baudRates, ELEMENTS ( baudRates ), index, &values[subMenuIndex] );

				Serial.flush ();						// Flush the serial buffer
				Serial.end ();							// Close the device
				delay ( 100 );							// Give it some time
				Serial.begin ( baudRate );				// Restart the serial interface
				delay ( 500 );							// Give it half a second to stabilize
				
				break;

			case 5:
			default:										// Go back to main menu
				exitFlag = true;							// Without waiting for encoder switch push
				break;
		}													// End of switch ( subMenuIndex )
		
		if ( !exitFlag )	// avoids flicker
			PaintText ( 2, menuCol, row, 1, HIGHLIGHT, LEFT, menuOptions[subMenuIndex] );	// Highlight the selected menu item
		
	}														// End of debug setting menu

	menuCol = oldColumn;					// Restore previous menu column
	menuIndex = oldIndex;					// Restore previous selected item

}											// End of DoSetOptions

/*
 *	"SetOptionFlag()" toggles an on/off option flag and stores the result in EEPROM,
 *	highlighting the value as required and returning the new setting.
 */
 
uint8_t SetOptionFlag ( int col, int row, uint16_t bitMask, char **valueStr )
{
	
	while ( digitalRead ( SWITCH ))				// While encoder switch not operated
	{
		if ( encoderDirection != DIR_NONE )		// Has the encoder moved?
		{
			if (optionFlags & bitMask)			// Yes, toggle the flag
				optionFlags &= ~bitMask;		// Turn it off
			else
				optionFlags |= bitMask;			// Turn it on
				
			*valueStr = optionFlags & bitMask ? strYes : strNo;			// Set value in referenced parameter
			EraseText ( 2, col, row, 1, BLACK, LEFT, 5 );				// Erase the old value
			PaintText ( 2, col, row, 1, HIGHLIGHT, LEFT, *valueStr );	// Highlight the new value
			
			ResetEncoder ();
		}
	}
	
	EEPROM.put ( OPTION_FLAGS, optionFlags );
	
	PaintText ( 2, col, row, 1, NORMAL, LEFT, *valueStr );	// Un-highlight the value

	delay ( 250 );								// Avoid long switch press issue

	return optionFlags & bitMask;				// Return the setting
}

/*
 *	"SetOptionInt32()" picks one value of a int32_t set and stores the result in EEPROM,
 *	highlighting the value as required and returning the index to the new value.
 */
 
int32_t SetOptionInt32 ( int col, int row, int optionId, int32_t values[], int valueCount, int valueIndex, char **valueStr )
{
	int32_t		value = values[valueIndex];		// Value to be returned
	char		*pvs = *valueStr;				// Pointer to string value parameter
		
	while ( digitalRead ( SWITCH ))				// While encoder switch not operated
	{
		if ( encoderDirection != DIR_NONE )		// Has the encoder moved?
		{
			valueIndex += clickCount;
			if ( valueIndex >= valueCount )		// Out of bounds
				valueIndex = 0;					// Back to the beginning
			else if ( valueIndex < 0 )			// Out of bounds
				valueIndex = valueCount - 1;	// Last one

			value = values[valueIndex];							// Set value to be returned
			strcpy ( pvs, " " );								// Convert value to text and pad
			ltoa ( value, &pvs[1], 10 );
			strcat ( pvs, " " );
			EraseText ( 2, col, row, 1, BLACK, LEFT, 12 );		// Erase the old value
			PaintText ( 2, col, row, 1, HIGHLIGHT, LEFT, pvs );	// Highlight the new value
			
			ResetEncoder ();
		}
	}
	
	EEPROM.put ( optionId, value );
	
	PaintText ( 2, col, row, 1, NORMAL, LEFT, pvs );	// Un-highlight the value

	delay ( 250 );								// Avoid long switch press issue

	return value;								// Return the value
}

/******************************  FREQUENCY CALIBRATION FUNCTIONS  ******************************
 *
 *	These functions are all part of the frequency calibration function.
 */

/*
 *	"DoFreqCal()" was added in Version 03.3.
 *
 *	This works similarly to the SWR calibration function. Instead of displaying the frequency
 *	and SWR on the screen however, we display the frequency and the calibration factor. Turning
 *	the encoder will change the calibration factor until the DDS is sending the correct
 *	frequency. Pushing the encoder button will save the calibration factor.
 *
 *	Normally, the calibration factor is either 125MHz or 180MHz, depending on whether the AD9850
 *	or AD9851 DDS in in use. See the definitions of "CAL_9850" and "CAL_9851" in the My_Header.h"
 *	file and the "Hacker's Guide". Note that there are some AD9851 modules out there that need
 *	to have the calibration factor set to 120MHz. If you're having problems with an AD9851 DDS,
 *	try changing the calue of "CAL_9851" to "120000000UL" in the header file.
 *
 *	Please read the "User Manual" for a detailed explanation of how to use this capability.
 */

void DoFreqCal ()
{
	char		buff[20];					// For displayed frequency & calibration constant
	float		realFreq;					// Frequency for the DDS
	uint32_t	tempFreq;					// Used for rounding
	uint32_t	calConstant;				// Calibration constant

	DDS.up ();								// Wake up the DDS
	delay ( 10 );							// Give it a moment

/*
 *	Get the band center frequency (using limits set by the operator) and make it a multiple of 5KHz 
 */

	tempFreq =  activeBandTop;				// Only seems to work right
	tempFreq += activeBandBottom;			// When strung out into multiple
	tempFreq /= 2L;							// Statements like this!

	tempFreq = ( tempFreq / 5 ) * 5;

	tft.fillScreen ( BLACK );						// Fade to black

	FormatFrequency ( tempFreq, buff, 3 );			// Format frequency into "buff"

	FT_pushCount = 2;								// Carat under last digit

	if ( FT_installed )
		activeFreqIncrement = freqIncrements[FT_pushCount];		// 1KHz

	else
		activeFreqIncrement = FIXED_FREQ_INCR;					// The old default (5KHz)

	PaintText ( 3, 15, 2, 1, NORMAL, RIGHT, "Frequency:" );		// Display the frequency
	DisplayFrequency ( 3, 16, 2, 1, HEADING, LEFT, buff );

	while ( digitalRead ( SWITCH ))						// While encoder switch not operated
	{
		ReadFT ();										// "Fine Tune" button pushed?

		if ( newFT_Interrupt )							// Need to move the carat?
		{
			FormatFrequency ( tempFreq, buff, 3 );		// Format frequency into "buff"

			DisplayFrequency ( 3, 16, 2, 1, HEADING, LEFT, buff );
			activeFreqIncrement = freqIncrements[FT_pushCount];

			newFT_Interrupt = false;
		}

		if ( encoderDirection != DIR_NONE )			// If encoder changed
		{
			tempFreq += ( activeFreqIncrement * clickCount );			// "clickCount" is plus or minus

			if ( tempFreq > GetBandEdge ( activeBandIndex, UPPER ))		// Stay within band limits
				tempFreq = GetBandEdge ( activeBandIndex, UPPER );

			if ( tempFreq < GetBandEdge ( activeBandIndex, LOWER ))
				tempFreq = GetBandEdge ( activeBandIndex, LOWER );

			ResetEncoder ();						// Reset the encoder flags

			FormatFrequency ( tempFreq, buff, 3 );						// Format frequency into "buff"
			DisplayFrequency ( 3, 16, 2, 1, HEADING, LEFT, buff );		// And update the display
		}

		realFreq = tempFreq * 1000;				// For the DDS
		DDS.setfreq ( realFreq );				// Send the frequency to the DDS (either one)

	}											// End of "while ( digitalRead ( SWITCH ) != LOW )"

	EraseText ( 3 ,16, 3, 1, BLACK, LEFT, 7 );	// Erase the carat under the frequency


/*
 *	Ok, now the operator has selected the frequency to use for the calibration, so now we
 *	display the current setting of the DDS calibration factor. 
 */

	#ifdef	AD9850_DDS
		calConstant = CAL_9850;							// Constant for AD9850
	#endif

	#ifdef	AD9851_DDS
		calConstant = CAL_9851;							// Constant for AD9851
	#endif

	calPushCount = 1;									// Start with 100 Hz tuning
	activeCalIncrement = calIncrements[calPushCount];	// Whether FT button installed or not

	FormatCalConstant ( calConstant, buff );

	PaintText ( 3, SCREEN_C, 3, 2, NORMAL, CENTER, "Calibration Constant" );
	DisplayCalibration ( 3, 8 ,4, 2, HEADING, LEFT, buff );

	while ( digitalRead ( SWITCH ))						// While encoder switch not operated
	{
		ReadFT ();										// "Fine Tune" button pushed?

		if ( newFT_Interrupt )							// Need to move the carat?
		{			
			FormatCalConstant ( calConstant, buff );	// Format the calibration factor
			DisplayCalibration ( 3, 8 ,4, 2, HEADING, LEFT, buff );
			activeCalIncrement = calIncrements[calPushCount];

			newFT_Interrupt = false;
		}

		if ( encoderDirection != DIR_NONE )			// If encoder changed
		{			
			calConstant += ( activeCalIncrement * clickCount );		// "clickCount" is plus or minus

			ResetEncoder ();						// Reset the encoder flags

			FormatCalConstant ( calConstant, buff );	// Format the calibration factor
			DisplayCalibration ( 3, 8 ,4, 2, HEADING, LEFT, buff );

			DDS_Calibrate = calConstant;			// Update real copy
			DDS.calibrate ( DDS_Calibrate );		// And set in the DDS
			
			realFreq = tempFreq * 1000;				// For the DDS
			DDS.setfreq ( realFreq );				// Send the frequency to the DDS (either one)
		}
	}												// End of "while ( digitalRead ( SWITCH ) != LOW )"

	EEPROM.put ( FREQ_CALIBRATION, DDS_Calibrate);	// Save for next time
	DDS.down ();									// Put the DDS to sleep	
}


/*
 *	"DisplayCalibration()" is used in the "DoFreqCal()" to display the calibration
 *	constant with the carat under the 1KHz, 100Hz, 10Hz or 1MHz digit based on the
 *	value of "calPushCount", which is similar to "FT_Pushcount".

 *	It uses the same parameter list as "PaintText()". And will put the frequency on the
 *	display exactly as would "PaintText()" except it will place a carat ('^') under the
 *	digit which will be changed based on the current value of "activeFreqIncrement".
 */

void DisplayCalibration ( int sz, int cl, int rw, int spacing, int style, int align, char* txt )
{
	EraseText ( sz ,cl, rw + 1, spacing, BLACK, align, strlen ( txt ) + 2 );	// Erase the carat

	PaintText ( sz, cl, rw, spacing, style, align, txt );						// Paint frequency

	if ( calPushCount == 0 )													// Allow MHz tuning
		PaintText ( sz, cl + 6, rw + 1, spacing, style, align, "^" );

	else
		PaintText ( sz, cl + calPushCount + 7, rw + 1, spacing, style, align, "^" );	// Repaint the carat
}


/*
 *	"FormatCalConstant()" is used to convert the "calConstant" into a string in the format
 *	"nnn,nnn,nnn". The "calConstant" is a frequency that gets loaded into the DDS to 
 *	compensate for any slight inaccuracies in the crystal frequency.
 *
 *	The parameters are "freq", the frequency to be formatted, "buff", which is the buffer
 *	where the formatted number will be built.
 */

void FormatCalConstant ( uint32_t freq, char* buff )
{
	uint32_t	MHz;							// MHz part of frequency
	uint32_t	KHz;							// Entire KHz part

	uint32_t	Divisor = 100000UL;				// Used in breaking down KHz

	char		Digit;							// Going to break KHz into single digits
	int			iX;								// Buffer position

	MHz = freq / 1000000UL;						// Isolate MHz
	KHz = freq % 1000000UL;						// Isolate KHz

	memset ( buff, 0, sizeof ( buff ));			// Zap the buffer

	ltoa ( MHz, buff, DEC );					// Copy MHz part into the buffer
	strcat ( buff, ",000,000" );				// Rest of the string

/*
 *	This might be some of the ugliest code I've ever written! But after playing around with
 *	trying to do it with "sprintf" for most of an evening, I gave up! Sorry!
 */

	iX = 4;										// Next position in the buffer
	Divisor = 100000UL;
	
	while ( Divisor > 0 )						// Don't want to do that!
	{
		Digit = ( KHz / Divisor );				// Get the next digit
		KHz = KHz % Divisor;					// Now 1 less remaining

		if ( iX == 7 )							// Jump over the 2nd comma
			iX++;

		buff[iX++] |= Digit;					// Add digit to the buffer
		
		Divisor /= 10;							// Decrement the Divisor
	}

	buff[iX] = '\0';							// Add a null terminator
}


/******************************  FORMATTING & DISPLAY FUNCTIONS  ******************************
 *
 *	The functions in this section all have something to do with formatting specific types
 *	of data (e.g. SWRs or Frequencies) or displaying specific things on the dislply (e.g.
 *	scan plots or the analog SWR meter.
 */

/*
 *	Splash() displays the initial message on boot up and leaves it on the
 *	display for 3 seconds.
 */

void Splash ()
{
	char	bottomLine[50];
	bool	DDS_installed = false;

	memset ( bottomLine, 0, 50 );						// Clear the buffer
	
	tft.fillScreen ( BLACK );							// Fade to black

	PaintText ( 3, SCREEN_C,  0, 1, SPECIAL, CENTER, "Milford Amateur Radio Club" );
	PaintText ( 3, SCREEN_C,  1, 1, SPECIAL, CENTER, "Antenna Analyzer" );
	PaintText ( 2, SCREEN_C,  5, 0, ERR,     CENTER, "Upgraded Software Version 3.8" );
	PaintText ( 1, SCREEN_C, 14, 0, HEADING, CENTER, "Original Design By:" );
	PaintText ( 2, SCREEN_C,  8, 0, NORMAL,  CENTER, "Jack-W8TEE & Farrukh-K2ZIA" );
	PaintText ( 1, SCREEN_C, 20, 0, HEADING, CENTER, "Modified Software By:" );
	PaintText ( 2, SCREEN_C, 11, 0, NORMAL,  CENTER, "John-WA2FZW, Edwin-PE1PWF," );
	PaintText ( 2, SCREEN_C, 13, 0, NORMAL,  CENTER, "Dave-M0WID, Bill-AB1XB, George-KB1HFT" );
	PaintText ( 1, SCREEN_C, 30, 0, HEADING, CENTER, "Additional Testing By:" );
	PaintText ( 2, SCREEN_C, 16, 0, NORMAL,  CENTER, "Jim-G3ZQC, Dick-K2RH & Glenn-VK3PE" );

	if ( ddsType == DDS_TYPE_9850 )					// If AD9850 DDS installed
	{
		strcat ( bottomLine, "Using AD9850 DDS" );
		DDS_installed = true;
	}

	if ( ddsType == DDS_TYPE_9851 )					// If AD9851 DDS installed
	{
		strcat ( bottomLine, "Using AD9851 DDS" );
		DDS_installed = true;
	}

	if ( DDS_installed == false )			// If no DDS defined
	{
		delay ( 1000 );

		while ( true )						// Display error message forever
		{
			tft.fillScreen ( BLACK );		// Fade to black
			delay ( 500 );
			PaintText ( 3, SCREEN_C, 3, 1, ERR, CENTER, "FATAL ERROR!" );
			PaintText ( 3, SCREEN_C, 5, 1, ERR, CENTER, "NO DDS TYPE DEFINED!" );
			delay ( 500 );
		}
	}

#ifdef AD8307_SWR

	strcat ( bottomLine, " & AD8307 Detector" );
	
#endif

#ifdef PE1PWF_MOD

	strcat ( bottomLine, " & PE1PWF Modification" );

#endif

	PaintText ( 2, SCREEN_C, 18, 0, WARNING, CENTER, bottomLine );

	delay ( 3000 );							// Wait 3 seconds
}


/*
 *	"FormatFrequency()" is used in the table and plot display functions to format the
 *	frequency. It will be displayed as M.KKK, or MM.KKK or M.KK or MM.KK, depending
 *	on whether or not the MHz part of the frequency is a single digit or a two digit
 *	number, and the value of the "decimals" parameter (which I added). The 3rd parameter
 *	specifies whether to send back 2 decimal places or 3. If anything other than 2
 *	is specified, it will default to 3.
 *
 *	This has been modified from the original, which was always displaying the frequency
 *	in a format of NN.NN, regardless of how many digits should have been in the MHz part
 *	of the number.
 *
 *	The parameters are "freq", the frequency to be formatted, "buff[]", which is the buffer
 *	where the formatted number will be built and "decimals" which allows the caller to
 *	specify that only 2 decimal places be returned (used in plot display code). Note,
 *	that any value of the "decimals" parameter other then "2" is ignored and 3 decimal
 *	places will be returned.
 */

void FormatFrequency ( uint32_t freq, char buff[], int decimals )
{
	int			MHz;							// MHz part of frequency
	int			KHz;							// Entire KHz part

	int			KHz_Digit_1;					// Going to break KHz into 3 digits
	int			KHz_Digit_2;
	int			KHz_Digit_3;

	int			Digit = 0;						// Where are we in the buffer

	MHz = freq / 1000;							// Isolate MHz
	KHz = freq % 1000;							// Isolate KHz


/*
 *	The following convoluted math does a series of divisions and modulo functions
 *	to break the KHz part of the frequency into single digits.
 */

	KHz_Digit_1 = KHz / 100;					// Isolate 1st digit
	KHz_Digit_2 = KHz % 100;					// Isolate the rest of the number
	KHz_Digit_3 = KHz_Digit_2 % 10;				// Isolate the 3rd digit
	KHz_Digit_2 = KHz_Digit_2 / 10;				// And finally determine the 2nd digit

	if ( MHz < 10 )								// Single digit MHz?
	{
		buff[0] = ' ';							// Leading space character
		Digit = 1;								// Yes, next character goes into [1]
		itoa ( MHz, &buff[Digit++], 10 );		// Convert MHz to character(s)
	}

	else
	{
		itoa ( MHz, buff, 10 );					// Convert MHz to character(s)
		Digit = 2;								// Decimal goes into [2]
	}

/*
 *	Rather than a bunch of itoa functions to put the single digit parts of the
 *	KHz into the buffer, I took a shortcut and just or'd the digits with an ASCII
 *	zero character.
 */

	buff[ Digit++ ] = '.';						// Decimal point
	buff[ Digit++ ] = KHz_Digit_1 | '0';		// KHz hundreds
	buff[ Digit++ ] = KHz_Digit_2 | '0';		// KHz tens

	if ( decimals == 2 )						// 2 decimals is the only value we really deal with
	{
		buff[ Digit ] = '\0';					// Stop at 2 decimal places
		return;
	}

	buff[ Digit++ ] = KHz_Digit_3 | '0';		// Otherwise, add the KHz units
	buff[ Digit ] = '\0';						// Null terminator
}


/* 
 *	"FormatFloat()" is used to format a general floating "value". the "integer" value determines
 *	the number of digits in the integer part, the "decimals" value determines the number of
 *	digits in the decimal part "buff[]", which is the buffer where the formatted number will be built.
 */

void FormatFloat ( float value, uint8_t integer, uint8_t decimals, char* buff )
{
	dtostrf ( value, integer+decimals+1, decimals, buff);
}
 

/*
 *	"PaintSwrData()" puts the FWD and REV readings on the screen along with the difference
 *	between them if the symbol "VIEW_PIN_DATA" is defined.
 */

void PaintSwrData ( float freq, float swr, float fwd, float rev )
{
	float	diff;
	char	buff[10];

	#if ( !defined ( VIEW_PIN_DATA ))
		return;
	#endif

	if ( !SWR_dataHeaderDone )
	{
		PaintText ( 2, 10, 13, 0, NORMAL, RIGHT, " Fwd" );		// Headings for raw data
		PaintText ( 2, 20, 13, 0, NORMAL, RIGHT, " Rev" );
		PaintText ( 2, 30, 13, 0, NORMAL, RIGHT, "Diff" );
 		PaintText ( 2, SCREEN_C, 18, 0, NORMAL, CENTER, "DDS On " );	// DDS Status
		ddsOn = true;
		SWR_dataHeaderDone = true;
	}

	FormatFloat ( fwd, 4, 1, buff );				// Convert FWD to ASCII
	PaintText ( 2, 4, 15, 0, HEADING, LEFT, buff ); // And display it
    
	FormatFloat ( rev, 4, 1, buff );				// Convert FWD to ASCII
	PaintText ( 2, 14, 15, 0, HEADING, LEFT, buff ); // And display it

	diff = fwd - rev;								// Calculate difference
		
	FormatFloat (diff, 4, 1, buff);					// Convert difference to ASCII
	PaintText ( 2, 24, 15, 0, HEADING, LEFT, buff ); // And display it

	ReadFT ();

	if ( longFT_Interrupt )
	{
		if ( ddsOn )
		{
			DDS.down ();
			ddsOn = false;
			PaintText ( 2, SCREEN_C, 18, 0, ERR, CENTER, "DDS Off" );		// DDS Status
		}

		else
		{
			DDS.up ();
			delay ( 10 );
      		DDS.setfreq ( freq );       // Send the frequency to the DDS (either one)
 			ddsOn = true;
 			PaintText ( 2, SCREEN_C, 18, 0, NORMAL, CENTER, "DDS On " );	// DDS Status
		}

		ResetFT ();

	}										// End of if ( newFT_Interrupt )
}


/*
 *	"FormatSWR()" is used in the table and plot display functions to format the SWR. It will
 *	be displayed as "NN.NN" or " N.NN", depending on whether or not the integer part of the
 *	SWR is a single digit or a two digit number.
 *
 *	The parameters are "s", the SWR to be formatted, "buff[]", which is the buffer
 *	where the formatted number will be built.
 *
 *	Modified in Version 03.6 to test for bogus SWR values and display "N/A" in place of
 *	a goofy meaningless number.
 */

void FormatSWR ( uint16_t swr, char* buff )
{
	int		Integer;							// Integer part of SWR
	int		Decimal;							// Entire fractional part

	int		Dec_Digit_1;						// Going to break decimal part into 2 digits
	int		Dec_Digit_2;

	int		Digit = 0;							// Where are we in the buffer

	if ( swr < 100 || swr > 2000 )				// Less than 1.0 or more than 20.0?
	{
		strcpy ( buff, " N/A " );
		return;
	}

	Integer = swr / 100;						// Isolate the integer part
	Decimal = swr % 100;						// And the fractional part

/*
 *	The following convoluted math does a series of divisions and modulo functions
 *	to break the decimal part of the SWR into single digits.
 */

	Dec_Digit_1 = Decimal / 10;					// Isolate 1st digit
	Dec_Digit_2 = Decimal % 10;					// Isolate 2nd digit

	Digit = 0;

	if ( Integer < 10 )							// Single digit integer part?
	{
		buff[0] = ' ';							// Yes, need leading space
		Digit = 1;								// Yes, next character goes into [1]
	}

	itoa ( Integer, &buff[Digit], 10 );			// Convert the integer part to character(s)

/*
 *	Rather than a bunch of itoa functions to put the single digit parts of the
 *	KHz into the buffer, I took a shortcut and just or'd the decimal part digits
 *	with an ASCII zero character.
 */

	buff[ 2 ] = '.';						// Decimal point
	buff[ 3 ] = Dec_Digit_1 | '0';			// Tenths
	buff[ 4 ] = Dec_Digit_2 | '0';			// Hundredths
	buff[ 5 ] = '\0';						// Null terminator
}


/*
 *	"PaintText()" is a new function added in Version 01.2. Most (but not all) of the text
 *	displayed by the program is done in text size 2. Text size 1 is 8 pixels high by 6 
 *	pixels wide, including one column of pixels painted in the background color to the right
 *	and one row below the character itself for character and line spacing. The text size
 *	specified in the "tft.setTextSize()" function simply multiplies the size of each
 *	character by the value of the size parameter. Thus, text size 2 characters are 16
 *	pixels high and 12 pixels wide, including the inter-character and inter-row spacing.
 *
 *	Throughout the original code, there are wierd computations to figure out where to paint
 *	a text item on the display typically based on a loop index value and some "spacing"
 *	type value. I decided that it would be so much simpler to let the loop indicies determine
 *	the row an/or column in which to place the text in terms of the number of rows and
 *	columns based on the text size. For example, using size 2 text, there are 33.3 columns
 *	and 15 rows available starting from the upper leftmost corner of the display.
 *
 *	Also, there are 6 basic color schemes used throughout the program:
 *
 *		NORMAL		Green text/Black background
 *		HIGHLIGHT	Blue text/White background
 *		WARNING		Yellow text/Black background
 *		ERROR		Red text/Black background
 *		HEADING		White text/Black background
 *		SPECIAL		Magenta text/Black background (splash screen only)
 *		SCAN_LBL	Added in V03.2 for labelling plots (Yellow/Transparent)
 *		OVLY_LBL	Added in V03.2 for labelling overlay plots (White/Transparent)
 *		DEBUG		Added in V03.4 for debugg mode indicator (Red/Transparent)
 *
 *	So, we will define those color schemes and this function will set the proper colors based
 *	on one of the above values being provided in the parameter list.
 *
 *	The function will also recognize an alignment parameter which will be "LEFT", "CENTER"
 *	or "RIGHT". the value of that parameter will be ground into the positioning computation.
 *	If "LEFT", the text will begin in the (text) column specified in the parameter list. If
 *	"RIGHT", the text will end in the (text) column specified. In the case of "CENTER" it's
 *	a bit trickier. The text will be positioned such that the middle pixel in the string is
 *	in the middle of the specified (text) column.
 *
 *	The caller can also specify one of the values, "SCREEN_L", "SCREEN_C" or "SCREEN_R"
 *	in the "cl" parameter. These cause the text to be aligned based on the edges of the
 *	screen or the center pixel.
 *
 *	The only caveat here is that the calling function will have to specify row and column 
 *	based on the text size to be used.
 *
 *	Sounds complicated, but it's going to make the rest of the code look a lot simpler and
 *	more efficient.
 *
 *	Modified in Version 02.0 to add the spacing parameter, which allows the caller to specify
 *	a number of pixels to add to the vertical character size to provide extra space between
 *	lines. The number specified is added to the text size 1 base height before computing the
 *	height for the text size specified. So in other words, size 1 text will get one extra pixel,
 *	size 2 text gets 2 extra pixels, etc.
 */

void PaintText ( int sz, int cl, int rw, int spacing, int style, int align, const char* txt )
{
int		fontWidth  = sz * SZ_1_W;					// Width of a single character in specified size
int		fontHeight = ( sz + spacing ) * SZ_1_H;		// Height of a single character + spacing

int		maxColumn  = displayW / fontWidth;			// Maximum number of columns based on character size
int		maxRow     = displayH / fontHeight;			// Naximum number of rows

int		horiz;										// Horizontal cursor address (pixel column)
int		vert;										// Vertical cursor address (pixel row)

int		stringPixels;								// String length in pixels

/*
 *	We'll do the easy stuff first! Note, the order of things in the "switch" puts the
 *	ones we expect to be used most often near the top.
 */

	switch ( style )								// Determine the text color scheme
	{
		default:									// Default is "NORMAL"
		case NORMAL:
			tft.setTextColor ( GREEN, BLACK );		// Green on black
			break;

		case HEADING:								// White on black
			tft.setTextColor ( WHITE, BLACK );
			break;

		case HIGHLIGHT:								// Blue on white
			tft.setTextColor ( BLUE, WHITE );
			break;

		case WARNING:								// Yellow on black
			tft.setTextColor ( YELLOW, BLACK );
			break;

		case SCAN_LBL:
			tft.setTextColor ( YELLOW );			// Yellow on whatever is already there
			break;

		case OVLY_LBL:
			tft.setTextColor ( WHITE );				// White on whatever is already there
			break;

		case ERR:									// Red on black
			tft.setTextColor ( RED, BLACK );
			break;

		case SPECIAL:
			tft.setTextColor ( MAGENTA, BLACK );	// Magenta on black
			break;

		case DEBUG:
			tft.setTextColor ( RED );				// Red on whatever is already there
	}

	stringPixels = strlen ( txt ) * fontWidth;		// String length in pixels

/*
 *	If the caller specified one of the "SCREEN_x" parameters in the "cl" parameter, we
 *	need to do some special computations to figure out the "horiz" cursor position for the
 *	first text character.
 */

	if ( cl < 0 )							// Special case?
	{
		switch ( cl )						// Compute first pixel column 
		{
			case SCREEN_L:					// Screen left
				horiz = 0;					// Extreme left pixel
				break;

			case SCREEN_C:									// Center screen
				horiz = (( displayW - stringPixels ) / 2 );	// Center pixel - half of string length	
				break;

			case SCREEN_R:								// Right side of screen
				horiz = displayW - stringPixels;		// Right side - length of string
				break;
		}
	}

	else										// Real column number in "cl" parameter
	{
		horiz = cl * fontWidth;					// Horizontal cursor address (column)

		switch ( align )
		{
			default:							// If garbage in the parameter, make it "LEFT"
			case LEFT:
				break;							// That was pretty simple!

		case CENTER:
			horiz = horiz - (( stringPixels + fontWidth ) / 2 );
			break;

		case RIGHT:
			horiz = horiz - stringPixels;
			break;
		}
	}

	DEBUG_BEGIN
		if ( horiz > ( displayW - fontWidth ) || vert > ( displayH - fontHeight ))	// Off the screen?
			Serial.print ( "Illegal cursor address" );
	DEBUG_END

	vert  = rw * fontHeight;				// Vertical cursor address (row)

	tft.setTextSize ( sz );					// Set the text size
	tft.setCursor ( horiz, vert );			// Set the cursor position
	tft.print ( txt );						// Display the text

	tft.setTextColor ( GREEN, BLACK );		// Leave color set to defaults
}


/*
 *	"EraseText()" is similar to "PaintText()", except that it can be used to erase
 *	a small portion of the screen, in any color desired! The parameters are all the
 *	same as those for "PaintText" with two exceptions:
 *
 *		The "style" parameter is replaced by "color" to be used in the fill
 *		The "txt" parameter is replaced by the character sized width to fill.
 */

void EraseText ( int sz, int cl, int rw, int spacing, int color, int align, int fillWidth )
{
int		fontWidth  = sz * SZ_1_W;					// Width of a single character in specified size
int		fontHeight = ( sz + spacing ) * SZ_1_H;		// Height of a single character + spacing

int		maxColumn  = displayW / fontWidth;			// Maximum number of columns based on character size
int		maxRow     = displayH / fontHeight;			// Naximum number of rows

int		horiz;								// Horizontal cursor address (pixel column)
int		vert;								// Vertical cursor address (pixel row)

int		stringPixels;						// Fill length in pixels

	stringPixels = fillWidth * fontWidth;	// fillWidth length in pixels

/*
 *	If the caller specified one of the "SCREEN_x" parameters in the "cl" parameter, we
 *	need to do some special computations to figure out the "horiz" cursor position.
 */

	if ( cl < 0 )							// Special case?
	{
		switch ( cl )						// Compute first pixel column 
		{
			case SCREEN_L:					// Screen left
				horiz = 0;					// Extreme left pixel
				break;

			case SCREEN_C:									// Center screen
				horiz = (( displayW - stringPixels ) / 2 );	// Center pixel - half of string length	
				break;

			case SCREEN_R:									// Right side of screen
				horiz = displayW - stringPixels;			// Right side - length of string
				break;
		}
	}

	else										// Real column number in "cl" parameter
	{
		horiz = cl * fontWidth;					// Horizontal cursor address (column)

		switch ( align )
		{
			default:							// If garbage in the parameter, make it "LEFT"
			case LEFT:
				break;							// That was pretty simple!

		case CENTER:
			horiz = horiz - (( stringPixels + fontWidth ) / 2 );
			break;

		case RIGHT:
			horiz = horiz - stringPixels;
			break;
		}
	}

	DEBUG_BEGIN
		if ( horiz > ( displayW - fontWidth ) || vert > ( displayH - fontHeight ))	// Off the screen?
			Serial.print ( "Illegal cursor address" );
	DEBUG_END

	vert  = rw * fontHeight;				// Vertical cursor address (row)

	tft.fillRect ( horiz, vert, stringPixels, fontHeight, color );
}


/*
 *	"GraphAxis()" displays the axes for a graph.
 *
 *	Re-written in Version 02.4. In the original, almost all of the parameters passed in the
 *	argument list were individual members of the graphics structure. Those parameters that 
 *	were not part of the graphics structure were titles and colors that never changed.
 *
 *	This version uses the elements in the graphics structure directly and hard-codes the
 *	titles and colors.
 *
 *	In Version 02.7, we changed the vertical scale from a linear one to a logarithmic one,
 *	which also allows us to display an SWR value of up to 10:1 instead of only 3:1.
 *
 *	Modified in Version 02.9 to add un-labelled grey lines between the blue labeled ones.
 */

void GraphAxis ()

{
	char 		freqBuff[10];			// ASCII Frequency
	uint32_t 	tempFreq;				// Used for displaying the frequency
	float		i;						// Floating point loop counter!
	float		temp;					// Used in intermediate computations

	int 	vertLines[]   = { 60, 130, 200, 270, 340, 410 };	// X-axis vertical line locations
	int 	horizLines[]  = { 0, 38, 78, 124, 182, 220, 260 };	// Y-axiz horizontal line locations
	int		greyLines[]   = { 21, 53, 66, 103, 157, 202, 235, 248 };	// Intermediate lines
	
	char* 	labelsY[] = { "1.00", "1.40", "2.00", "3.00", "5.00", "7.00", "10.0" };

	int		lineY;

	tft.fillScreen ( BLACK );								// Fade to black

	for ( int k = 0; k < ELEMENTS ( horizLines ); k++ )
	{
		lineY = scan.yOrigin - horizLines[k] ;

		tft.drawLine ( scan.xOrigin, lineY, scan.xOrigin + scan.graphWidth, lineY, BLUE );

		tft.setTextSize ( 1 );							// Tiny text
		tft.setTextColor ( WHITE, BLACK );				// Axis label color & Background color
		tft.setCursor  ( scan.xOrigin - 40, lineY );	// Position cursor		
	   	tft.println ( labelsY[k] );						// Print vertical axis label
	}

	for ( int k = 0; k < ELEMENTS ( greyLines ); k++ )
	{
		lineY = scan.yOrigin - greyLines[k] ;

		tft.drawLine ( scan.xOrigin, lineY, scan.xOrigin + scan.graphWidth, lineY, DKGREY );
	}
	
	hedge = scan.minFreq;								// Horizontal edge?
	bump = ( scan.maxFreq - scan.minFreq ) / 5.0;		// Distance between vertical lines?

/*
 *	Draw X scale
 */

	for ( int i = 0; i < ELEMENTS ( vertLines ); i++ )		// X coordinates
	{
		temp = vertLines[i];								// Get next location

		tft.drawLine ( temp, upperPlotMargin, temp, scan.yOrigin, BLUE );	// Draw vertical line

		tft.setTextSize ( 1 );								// Tiny text
		tft.setTextColor ( WHITE );							// Axis label color & background color
		tft.setCursor ( temp - 10, scan.yOrigin + 10 );		// Location for X axis label

		tempFreq = (uint32_t) ( hedge * .001 );				// Floating frequency / 1000 -> integer

		FormatFrequency ( tempFreq, freqBuff, 2 );		// Convert frequency to ASCII

		hedge += bump;									// Next place for a label
		tft.println ( freqBuff );						// Put frequency label on X axis
	}

	scan.lastX = temp;									// X coordinate of rightmost vertical line


/*
 *	Paint the frequency and SWR axis labels
 */

	tft.setTextSize ( 1 );								// Tiny text
	tft.setTextColor ( RED, BLACK );
	tft.setCursor ( 440, scan.yOrigin + 10 );			// Place for "FREQ" label
	tft.println ( "FREQ" );								// Paint it

	tft.setCursor ( 22, upperPlotMargin - 20 );			// Place for "SWR" label
	tft.println ( "VSWR" );								// Paint it

	ShowDebugMode();
}


/*
 *	"GraphPoints()" plots the data points for a scan. The parameters are:
 *
 *		float	x				X value of data point (frequency)
 *		float 	y				Y value of  data point SWR)
 *		u_int	pcolor			Plot color
 *
 *	Everything else is setup in the "scan" structure.
 */

void GraphPoints ( float x, float y, unsigned int pcolor)
{
	x =  ( x - scan.minFreq ) * ( scan.graphWidth ) / ( scan.maxFreq - scan.minFreq )
											+ scan.xOrigin;				// X Value -> coordinate

	y = scan.yOrigin - ( log10 ( y ) * scan.graphHeight );

	if  ( y >= upperPlotMargin && x > scan.xOrigin )
	{
		tft.drawLine ( scan.lastX, scan.lastY, x, y, pcolor );			// Note, the plot
		tft.drawLine ( scan.lastX, scan.lastY + 1, x, y + 1, pcolor );	// is actually three
		tft.drawLine ( scan.lastX, scan.lastY - 1, x, y - 1, pcolor );	// pixels wide
	}

	scan.lastX = x;					// Save old coordinates so we know where the next line starts
	scan.lastY = y;
}


/*
 *	MarkMinimum()" plots a red '+' character at the minimum SWR point on the graph. It takes a
 *	"scanStruct" as a parameter, as it can be used both for the main plot or the overlay plot.
 *
 *	It computes the X and Y coordinates for where to put the '+' based on the minimum SWR
 *	value and the frequency at which it ocurred. The coordinate values are then adjusted
 *	based on the geometry of the font for the '+', since we want the middle of the character
 *	on the appropriate spot, and the cursor location is for the top left corner of the
 *	character.
 */

void MarkMinimum ( scanStruct markIt )
{
	int x =  ( markIt.freq[markIt.minIndex] - markIt.minFreq ) * ( markIt.graphWidth ) /
						( markIt.maxFreq - markIt.minFreq ) + markIt.xOrigin;
						
	int	y = markIt.yOrigin - ( log10 ( markIt.minSWR ) * markIt.graphHeight );

	x = x - 4;							// Compensation to put the middle of the '+' at the
	y = y - 7;							// desired coordinates

	tft.setCursor ( x, y );
	tft.setTextSize ( 2 );
	tft.setTextColor ( RED );
	tft.print ( "+" );
}


/*
 *	"PaintMeter()" draws the analog SWR meter on the display. For now, the size and location
 *	are fixed. Perhaps in the future, we will allow it's position to be set dynamically.
 */

void PaintMeter ()
{
	int 	tickX[]  = { 0, 56, 96, 153, 224, 320 };
	int		labelX[] = { 0, 49, 94, 151, 222, 312 };
	char* 	labels[] = { "1.0", "1.5", "2", "3", "5", "10" };

/*
 *	Draw the meter bezel and the meter face:
 */

	tft.fillRect ( meter.startX,   meter.startY,   meter.frameW,    meter.frameH,    DKGREY );
	tft.fillRect ( meter.startX+8, meter.startY+8, meter.frameW-16, meter.frameH-16, WHITE );

/*
 *	Draw the OK, Warning & Bad rectangles
 */

	tft.fillRect ( meter.scaleX,    meter.scaleY-10, 56,                10, GREEN );	// SWR OK
	tft.fillRect ( meter.scaleX+56, meter.scaleY-10, 96-56,             10, YELLOW );	// SWR Caution
	tft.fillRect ( meter.scaleX+96, meter.scaleY-10, meter.scaleLen-96, 10, RED );		// SWR Bad

	tft.fillRect ( meter.scaleX, meter.scaleY, meter.scaleLen, 3, BLACK );		// Horizontal scale line

	tft.setTextColor ( BLACK );				// Black labels
	tft.setTextSize ( 1 );					// Set the text size

/*
 *	Draw the tick marks and label them
 */
	for ( int i = 0; i < 6; i++ )
	{
		tft.drawLine ( meter.scaleX + tickX[i], meter.scaleY,
					   meter.scaleX + tickX[i], meter.scaleY - 15, BLACK );
						
		tft.setCursor ( meter.scaleX + labelX[i], meter.scaleY - 25 );
		tft.print ( labels[i] );								// Display the text
	}
}


/*
 *	"MovePointer()" draws and moves the SWR meter pointer.
 */

void MovePointer ( float swr )
{
	int		pointerTop = meter.scaleY + 4;				// Y coordnate of the top of the pointer
	int		pointerW   = 3;								// Pointer width
	int		pointerL   = 37;							// Length of pointer
	
	float	logSWR;										// Meter scale is logarithmic
	float	newX;										// new X position of needle

	if ( isnan ( swr ) || swr < 1.0 || swr > 20.0 )		// Test for nonsense values
		logSWR = 0.0;

	else
		logSWR = log10 ( swr );							// Meter scale is logarithmic

	newX = ( logSWR * meter.scaleLen );					// Use it as a percentage of the scale length

	if ( newX > meter.scaleLen )						// Too big?
		newX = meter.scaleLen;							// Limit it

	if ( newX < 0 )										// Too small?
		newX = 0;

	newX += meter.scaleX;								// Then add in the scale start offset

	tft.fillRect ( meter.lastPtr, pointerTop, pointerW, pointerL, RED );	// Paint old pointer

	if ( abs ( newX - meter.lastPtr ) > 0.2 )			// Prevents the jitters
	{
		if ( newX > meter.lastPtr )						// Move to the right
		{

/*
 *	These loops actually walk the pointer across the face of the meter as opposed to just
 *	erasing the old one and then painting a new one. The effect is that the operator sees
 *	the needle movement much the same way as a physical meter would behave.
 *
 *	One loop for right movement and one for left movement.
 */
			for ( int i = meter.lastPtr; i < newX; i++ )
			{
				tft.fillRect ( i - 2, pointerTop, pointerW + 2, pointerL, WHITE );	// Erase old
				tft.fillRect ( i - 1, pointerTop, pointerW, pointerL, RED );		// Paint new

				if ( i >= meter.scaleLen + meter.scaleX )	// Stay in bounds
					break;
			}
		}

		if ( newX < meter.lastPtr )
		{
			for ( int i = meter.lastPtr; i > newX; i-- )		// Move left
			{
				tft.fillRect ( i - 2, pointerTop, pointerW + 2, pointerL, WHITE );
				tft.fillRect ( i - 1, pointerTop, pointerW, pointerL, RED );

				if ( i <= meter.scaleX )						// Stay in bounds
					break;
			}
		}
	}

	meter.lastPtr = newX;						// New becomes old!
}


/*
 *	"ShowAndScroll()" is used to display and scroll the scan data in tabular form.
 *	The parameters are the (global) swr data array and the (global) frequency data array.
 */

void ShowAndScroll()
{
	int page = 0;						// Start with the first page in each array

	DrawTable ( page );					// Display the table starting with the first page

	while ( true )						// Wait for button push or scroll request
	{
		if ( digitalRead ( SWITCH ) == LOW )	// Return to main menu?
			break;								// Yes

		if ( encoderDirection != DIR_NONE )		// Has the encoder moved?
		{
			switch ( encoderDirection )
			{
				case DIR_CW:					// If clockwise rotation
					page++;						// Next page
					break;

				case DIR_CCW:					// If counter clockwise rotation
					page--;						// Previous page
					break;
			}

			if ( page >= 0 && page <= 3)		// Page number within limits?
				DrawTable ( page );				// Yes, then display the requested page

			else if ( page < 0)					// Don't let the page number go out of limits
				page = 0;

			else if ( page > 3 )				// Don't let the page number go out of limits
				page = 3;

			ResetEncoder ();					// No encoder movement
		}
	}											// end while (true)
}


/*
 *	"DrawTable()" displays a saved scan in tabular format. The "page" parameter lets
 *	us compute which is the first swr/frequency combination to display (upper left-most)
 *	as follows:
 *	
 *			Page	First	Last	Lines
 *			Number	Index	Index	On Page
 *
 *			  0		  0		 26		  9
 *			  1		 27		 53		  9
 *			  2		 54		 80		  9
 *			  3      81		100		  7/6		// Special case because 101 / 3 means 2 extra
 *
 *	The data is displayed as SWR/frequency pairs from top to bottom, left to right. If
 *	the "page" is less than zero or greater than "3", we ignore the request.
 *
 *	Eventually, I should compute the number of pages and all the other stuff based on the
 *	setting of "MAX_PLOT_POINTS", but this is good enough for now.
 */

void DrawTable ( int page )
{
	char		buff[15];					// Place to build an SWR/Frequency pair
	int			line;						// Line loop counter
	int			column;						// Column loop counter
	int			x_pos;						// X value of column start
	int			y_pos;						// Y value for each line
	int			index;						// First SWR/freq index on the page
	int			max_lines = 9;				// Line limit for page (only 7 for page 3)
	uint16_t	swr;						// Integer SWR
	uint32_t	freq;						// Occurs at frequency

	if ( page < 0 || page > 3 )				// Illegal page number, ignore
		return;


/*
 * 	Display the page header in white text just like it is displayed on the plots, except
 * 	we added the page number being displayed.
 */

	tft.fillScreen ( BLACK );							// Fade to black
	
	PaintHeading ( 0, scan.minSWR, scan.freq[scan.minIndex], page + 1 );

/*
 *	The following switch determines  the first data pair to be displayed on the page. For
 *	page 3, it also reduces the number of lines from 9 to 7.
 */

	switch	( page )
	{
		case 0:
			index = 0;							// Starts with 1st pair
			break;

		case 1:
			index = 27;							// Starts with 28th pair
			break;

		case 2:
			index = 54;							// Starts with 55th pair
			break;

		case 3:
			index = 81;							// Starts with 80th pair
			max_lines = 7;						// Only 7 lines on last page
			break;

		default:								// Shouldn't be needed
			return;
	}

/*
 *	Display the data starting with the first pair previously selected based on the
 *	page number. The outer loop does the data by column, and the inner loop does lines.
 *	In the original, the data was painted from left to right, then by line, which is
 *	not how people like to read tabular data.
 *
 *	Note the test for the combination of page 3 and column 2. This takes care of the
 *	problem caused by the fact that 101 scan points displayed 3 on a line leaves two
 *	extra. We put the extra ones in the first and second column on the last page.
 */

	for ( column = 0; column < 3; column++ )			// Paint by columns
	{
		if ( page == 3 && column == 2 )
			max_lines--;

		x_pos = column * 14;							// X position for column

		for ( line = 0; line < max_lines; line++ )		// Do each line in this column
		{
			y_pos = line +2;

			if ( scan.swr[index] != 0.0 )				// Fudge for older saved files (with only 100 points)
			{
				swr = scan.swr[index] * 100;
				FormatSWR ( swr, buff );
				PaintText ( 2, x_pos, y_pos, 1, NORMAL, LEFT, buff );

				freq = scan.freq[index] / 1000.0;
				FormatFrequency ( freq, buff, 3 );
				PaintText ( 2, x_pos+6, y_pos, 1, NORMAL, LEFT, buff );
			}

			index++;									// Next pair
		}
	}
}


/*
 *	"DisplayFrequency()" is used in the "Single Frequency" SWR measurement process and in
 *	the "SetBandEdge()" functions. Both are places where the operator can vary the frequency
 *	by moving the encoder. 
 *	
 *	It uses the same parameter list as "PaintText()". And will put the frequency on the
 *	display exactly as would "PaintText()" except it will place a carat ('^') under the
 *	digit which will be changed based on the current value of "activeFreqIncrement".
 */

void DisplayFrequency ( int sz, int cl, int rw, int spacing, int style, int align, char* txt )
{
	EraseText ( sz ,cl, rw + 1, spacing, BLACK, align, strlen ( txt ) + 2 );	// Erase the carat

	PaintText ( sz, cl, rw, spacing, style, align, txt );						// Paint frequency

	if ( FT_pushCount == 3 )													// Allow MHz tuning
		PaintText ( sz, cl + 1, rw + 1, spacing, style, align, "^" );

	else
		PaintText ( sz, cl + FT_pushCount + 3, rw + 1, spacing, style, align, "^" );	// Repaint the carat
}


/*
 *	"PaintHeading()" displays the heading for the plot and table outputs. The first three
 *	parameters are the column number where the heading is to start, the minimum SWR and
 *	frequency at which it occurs. The fourth parameter is either a page number (for the
 *	table display) starting with page 0, or "NO_PAGE" (-1) to indicate that the page number
 *	should not be displayed.
 */

void PaintHeading ( int cl, float s, float f, int page )
{
	char buff[42];								// Place to build the heading

	uint32_t	freq = f / 1000.0;
	uint16_t	swr  = s * 100.0;

	memset ( buff, 0, 42 );						// Clear the buffer

	strcat ( buff, "Min SWR: " );				// Start the heading
	FormatSWR ( swr, &buff[8] );				// Add the SWR
	strcat ( &buff[13], " at Freq: " );			// More fixed text
	FormatFrequency ( freq, &buff[23], 3 );		// Add the frequency

	if ( page != NO_PAGE )						// Need page number?
	{
		strcat ( &buff[29], "    Page: " );		// Text part
		buff[39] = page  | '0';					// Only works up to page 9
	}

	PaintText ( 2, cl, 0, 2, HEADING, LEFT, buff );
}


/*
 *	"PaintFooter()" puts the currently selected band and scan limit frequencies at the bottom
 *	of the screen when any of the munu screens are active.
 */

void PaintFooter()
{
	char	buffer[40];									// Place to build the output string
	char	freqBuff[10];								// For frequencies

	strcpy ( buffer, "Band:" );							// Heading
	strcat ( buffer, menuBands[activeBandIndex] );		// Current band

	strcat ( buffer, "Range: " );						// Scan range header
	FormatFrequency ( activeBandBottom, freqBuff, 3 );	// Low frequency
	strcat ( buffer, freqBuff );

	strcat ( buffer, " - " );
	FormatFrequency ( activeBandTop, freqBuff, 3 );		// High frequency
	strcat ( buffer, freqBuff );

	PaintText ( 2, SCREEN_C, 19, 0, HEADING, CENTER, buffer );	// Paint it
}


/******************************* DDS RELATED FUNCTIONS ************************************
 *
 *	The functions in this section manipulate the DDS (either the AD9850 or AD9851)
 */

/*
 *	"GetNextPoint()" reads the current SWR for a given frequency. The parameters are:
 *
 *		float currentFreq     the frequency being tested
 *		int index             the scan point index into the array
 *
 *	It returns the SWR reading in the SWR array in the "scan" structure.
 */

void GetNextPoint ( float currentFreq, int index )
{
	DDS.setfreq ( currentFreq );		// Sent the frequency to the DDS (either one)
 //Let it settle
  float TempReading = 0;
  for(int i = 0; i<5; i++)
  {
    TempReading = (float) analogRead  (ANALOG_FORWARD ); 
    TempReading = (float) analogRead  (ANALOG_REFLECTED ); 
  }

	scan.swr[index] = ReadSWRValue ( true );	// Get the SWR reading ("true" says doing a scan)
}


/*
 *	"ReadSWRValue()" reads the SWR bridge forward and reverse voltages "MAX_POINTS_PER_SAMPLE"
 *	times and computes the average values for forward and reverse voltage.
 *
 *	The functions assumes that the DDS has been set to the desired frequency for which the
 *	reading is to be taken.
 *
 *	Modified in Version 02.5 to add support for the AD8307 detector circuit.
 *
 *	Modified in Version 02.7 to return the SWR as a floating point value instead of returning
 *	the unsigned integer version. Also added the parameter "doingScan". If that is true, we
 *	are taking the reading as part of doing a scan, and don't want to paint the needle on
 *	the SWR meter used in the single frequency readings.
 *
 *	Modified in Version 03.1 to include a software modifable calibration factor if the PE1PWF
 *	hardware modifications are installed.
 *
 *	Modified in Version 03.6 to use the "slope" factor to tweak the formula used if the
 *	VK3PE board is being used.
 */

float ReadSWRValue ( bool doingScan )
{
	int		i;							// Loop counter
	float	sum[2] = {0.0, 0.0};		// Sum of forward & reverse voltage readings

	float	VSWR;						// Computed SWR

	uint16_t	samples = MAX_POINTS_PER_SAMPLE;

    if ( optStats && optStatsLogIndiv )
	{
		// divide the output listing for readability
		Serial.println(F(" "));
		Serial.println(F("========"));
	}
	
	sum[0] = sum[1] = 0.0;
	
    if ( optStats )
		MyStats.ResetStatsCounters();
    
    for ( i = 0; i < samples; i++ )			// Take multiple samples at each frequency
	{
        uint16_t fwdRdg = analogRead  ( ANALOG_FORWARD );
        uint16_t revRdg = analogRead  ( ANALOG_REFLECTED );
        
        sum[0] += (float)fwdRdg;                            // Add forward voltage to the total   
        sum[1] += (float)revRdg;                            // Same for reverse voltage        
       
        if ( optStats )
		{
			MyStats.CollectStatsDataPerPoint(fwdRdg, revRdg, i);
			if ( optStats && optStatsLogIndiv )
			{
				MyStats.LogIndivReadings(fwdRdg, revRdg);
				// Return if button is being pressed at the end of this group of readings
				if ( !digitalRead ( SWITCH ))
				{
					stopRepeating = true;
					return 0.0;
				}                              
			}
		}
	}  

	FWD = sum[0] / (float) samples;			// Average forward voltage
	REV = sum[1] / (float) samples;			// Average reverse voltage

	if ( optStats )
	{
		MyStats.CalculateStats(FWD, REV);
		if ( optStatsPoint )
			MyStats.LogSWR_PointStats();
	}
  	
	
	DEBUG_BEGIN
		Serial.print ( "In ReadSWRValue - FWD = " );
		Serial.print ( FWD );
		Serial.print ( ",  REV = " );
		Serial.print ( REV );
	DEBUG_END

#ifdef	AD8307_SWR							// If using the AD8307 detector

	returnLoss = ( FWD - REV ) * ( vRef / ( 1023.0 * slope ));				// Return Loss calculation
    reflectionCoefficient = pow ( 10.0, - ( returnLoss / 20.0 ));   		// Calculate reflection      
    VSWR = ( 1 + reflectionCoefficient ) / ( 1 - reflectionCoefficient );	// Calculate VSWR

	DEBUG_BEGIN
		Serial.print ( ",  Return loss: " );
		Serial.print ( returnLoss );
	DEBUG_END		

#else									// If using the standard diode bridge


/*
 *	Two different ways to compute the VSWR The first one is the commonly accepted method
 *	of performing the computation.
 *
 *	Modified in Version 03.1 to add a "calFactor" that can be set by the user in the
 *	SWR computation if the PE1PWF hardware modifications are installed.
 */

		#ifndef PE1PWF_MOD							// Unmodified analyzer
		
			VSWR = ( FWD + REV ) / ( FWD - REV );

		#else										// Edwin's version

			VSWR = ((((( FWD + REV ) / ( FWD - REV )) - 1 ) * ( (float) calFactor / 100 )) + 1 );

		#endif

#endif	// AD8307_SWR

	DEBUG_BEGIN
		Serial.print ( " SWR = " );
		Serial.println ( VSWR );
	DEBUG_END

	if ( optStats )
		MyStats.FindMaxMinSWR ( VSWR );

	return ( VSWR );							// Return floating point value
}


/******************************  FILE HANDLING FUNCTIONS  ******************************
 *
 *	The functions listed in this section all have to do with the processing of read, write
 *	and other miscellaneous functions for the SD card file system. They are all used primarly
 *	by the functions in the menu handling section.
 */

/*
 *	"Mount_SD()" attempts to initialize the SD card. It returns "true" or "false" in the
 *	"SD_Status" global variable depending on whether or not the card was successfully
 *	initialized. If it was not successful during startup, the operator can insert a card
 *	and invoke the function from the "Maintenance" menu later on. Eventually, I'll get
 *	around to using "SD_Status" to enable or disable the file system related functions.
 *
 *	Note, for some reason, it the card is removed while the analyzer is in use, re-inserting
 *	the card and using "Mount SD" does not work. I still haven't figured out why not.
 */

void Mount_SD ()
{
	root = SD.open ( "/" );							// Try to open the root directory
	if ( !root )									// If that failed, we need to initialize the card
	{
 		if ( !SD.begin ( SD_SS ))					// If the startup fails
		{
			Display_SD_Err ( "WARNING: No SD Card Loaded!" );	// Show error message
			PaintText ( 2, SCREEN_C, 7, 0, WARNING, CENTER, "File functions are inoperative!" );
			SD_Status = false;
			delay ( 3000 );							// Give operator 3 seconds to read the message
			return;
		}
	}

	SD_Status = true;								// Card is successfully mounted!
}


/*
 *	"CountFiles()" counts the number of files currently on the SD card and returns the
 *	number of files on the card in the global variable "fileCount".
 *
 *	Modified in version 02.9 to only count files whose name starts with "SCAN".
 */

void CountFiles()
{
	int		k;								// Loop index
	char	buff[15];						// For file name
	fileCount = 0;							// Reset the file counter

	root = SD.open ( "/" );					// Open the root directory

	if ( (bool) root == false )
	{
		Display_SD_Err ( "WARNING: No SD Card Loaded!" );
		PaintText ( 2, SCREEN_C,  7, 0, WARNING, CENTER, "Function is inoperative!" );
		PaintText ( 2, SCREEN_C, 10, 0, NORMAL,  CENTER, msgPushButton );

		fileCount = -1;							// Error indication
		return;
	}

	root.rewindDirectory ();					// Begin at the start of the directory

	while ( true )
	{
		File entry =  root.openNextFile ();		// Get the next file
		if ( ! entry )							// No file there?
			break;								// That's all folks!

		strcpy ( buff, entry.name () );			// Get the file name
		buff[4] = '\0';							// We are only interested in the 1st 4 characters
	
		if ( strcmp ( buff, "SCAN" ) == 0 )		// Is it a "SCAN" file?
			fileCount++;						// Yes, count it

		entry.close();							// And close the file
	}

	root.close();								// Close up shop
}


/*
 *	"ShowFiles()" displays a list of the files currently on the SD card and returns the
 *	number of files on the card.
 *
 *	It was modified in Version 01.2 to set the global variable "fileCount" to the number
 *	of file names read into memory. A test was also added to make sure the "MAX_FILES"
 *	limit was not exceeded.
 */

void ShowFiles()
{
	int		k;								// Loop index
	char	buff[15];						// For file name

	fileCount = 0;							// Reset the counter

	int firstRow   = 2;						// First row to display files on
	int	displayRow = firstRow;				// Row position of 1st file name
	int	displayColumn;						// Column for file name

	root = SD.open ( "/" );					// Open the root directory
	if ( !root )							// Did it open sucessfully?
	{
		PaintText ( 2, SCREEN_C , 8, 0, ERR, CENTER, "File open failure!" );
		PaintText ( 2, SCREEN_C, 10, 0, ERR, CENTER, "Is SD card inserted?" );
		PaintText ( 2, SCREEN_C, 13, 0, NORMAL, CENTER, msgPushButton );

		SD_Status = false;					// Card not mounted
		fileCount = -1;						// Another error indicator
		return;
	}

	root.rewindDirectory ();				// Begin at the start of the directory

	while (true)
	{
		File entry =  root.openNextFile ();		// Get the next file
		if ( ! entry )							// No file there?
			break;								// That's all folks!

		strcpy ( buff, entry.name () );			// Get the file name
		buff[4] = '\0';							// We are only interested in the 1st 4 characters
	
		if ( strcmp ( buff, "SCAN" ) == 0 )						// Is it a "SCAN" file?
			strcpy ( mySDFiles[fileCount++],  entry.name() );	// Yes, then add the name to the list

		entry.close();								// And close the file

		if ( fileCount == MAX_FILES )				// Reached array limit?
		{
			tft.fillScreen ( BLACK );				// Fade to black
			PaintText ( 2, SCREEN_C, 5, 0, WARNING, CENTER, "Number of files exceeds 20" );
			PaintText ( 2, SCREEN_C, 7, 0, WARNING, CENTER, "List is truncated!" );
			delay ( 3000 );
			tft.fillScreen ( BLACK );			// Fade to black
			break;
		}
	}

	tft.fillScreen ( BLACK );					// Fade to black
	PaintText ( 2, 1, firstRow, 1, NORMAL, LEFT, "SD Files:" );

	if ( fileCount !=0 )						// Any files to display?
	{
		SortFiles ();							// Put the list in order before we display them

		for ( k = 0; k < fileCount; k++ )
		{
			if ( k < 10 )								// Compute column
				displayColumn = 13 ;
			else
				displayColumn = 26;

			displayRow = ( k % 10 ) + firstRow;			// And row

			PaintText ( 2, displayColumn, displayRow, 1, NORMAL, LEFT, mySDFiles[k] );
		}
	}

	root.close();								// Close up shop
}


/*
 *	"SelectFile()" displays the list of files on the SD card, then waits for the operator 
 *	to select one. It is used by all the functions that have to do with processing data
 *	read from a file and in the delete functions.
 *
 *	It returns the index of the selected file in the global variable "useFile"
 *
 *	Modified in Version 01.2 to display more then 10 files by using 2 columns instead of
 *	just one column as did the original software. Also changed to use the "PaintText()"
 *	function instead of pixel manipulation.
 */

int SelectFile()
{
	int		edge;								// Index of selected file

	int 	firstRow      = 2;					// First row to display files on
	int		displayRow    = firstRow;			// Row position of 1st file name
	int		displayColumn = 13;					// (first) Column for file name

	edge = 0;

	ShowFiles ();									// Display files

	PaintText ( 2, displayColumn, displayRow, 1, HIGHLIGHT, LEFT, mySDFiles[0] );	// Highlight 1st file

	while ( digitalRead ( SWITCH ) == HIGH )		// Wait for encoder switch change
	{
		if ( encoderDirection != DIR_NONE )			// If it has rotated...
		{
			PaintText ( 2, displayColumn, displayRow, 1, NORMAL, LEFT, mySDFiles[edge] );

			switch ( encoderDirection )
			{
				case DIR_CW:					// CW -> Next one in the list
					edge++;						// Increased edge value
					break;

				case DIR_CCW:					// CCW -> Previous entry
					edge--;						// Decreased edge value
					break;

				default:
					DEBUG_BEGIN
						Serial.print( "Souldn't be here, edge = " );
						Serial.println ( edge );
					DEBUG_END
	         		break;
				}

			if ( edge >= fileCount )					// Limit checks
				edge = 0;								// First file

			if ( edge < 0 )
				edge = fileCount - 1;					// Last file

			if ( edge < 10 )							// Compute column
				displayColumn = 13 ;

			else
				displayColumn = 26;

			displayRow = ( edge % 10 ) + firstRow;		// And row

			PaintText ( 2, displayColumn, displayRow, 1, HIGHLIGHT, LEFT, mySDFiles[edge] );

			ResetEncoder ();							// Reset the encoder
		}
	}

	useFile = edge;    // Return the index for the selected file name in "useFile"
}


/*
 *	Since the file handling system for the SD card seems to be smart enough to save new
 *	files in the first free space of sufficient size that it finds on the card, the files
 *	may not be stored in sequential number on the card.
 *
 *	So, we'll sort them before we display them! Since we're not dealing with thousands of
 *	items in the array, we'll use the most inefficient approach possible, the horrible
 *	"Bubble Sort" algorithm. There is a noticable delay with about 15 files, but it's not
 *	too too bad.
 */

void SortFiles ()
{
	int		loop1;									// Loop counter
	int		loop2;									// Same
	char	holdBuff[NAME_LENGTH];					// Used in the swap

	for ( loop1 = 0; loop1 < fileCount-1; loop1++ )
	{
		for ( loop2 = 0; loop2 < ( fileCount - ( loop1 + 1 )); loop2++ )
		{
			if ( strcmp ( mySDFiles[loop2], mySDFiles[loop2+1] ) > 0 )		// If  2nd < 1st
			{
				strcpy ( holdBuff, mySDFiles[loop2] );						// Swap them
				strcpy ( mySDFiles[loop2], mySDFiles[loop2+1] );
				strcpy ( mySDFiles[loop2+1], holdBuff );
			}
		}
	}
}


/*
 *	ConfirmDelete() is used in the "Delete File" section of the "DoMaintenance" function. It 
 *	gives the operator the choice of confirming that the selected file should actually
 *	be deleted or to cancel the operation.
 *
 *	The only parameter is the index number of the file to be deleted (from the list), and
 *	it returns 1 if the file is to actually be deleted, or zero if the operator selects
 *	the "Cancel" option.
 */

int ConfirmDelete ( int fnToDelete )
{
	tft.fillScreen ( BLACK );					// Fade to black

	PaintText ( 2, 20, 4, 0, NORMAL, RIGHT, "File to delete:" );
	PaintText ( 2, 21, 4, 0, HEADING, LEFT, mySDFiles[fnToDelete] );

	return ConfirmAction ( "Delete" );
}


/*
 *	ConfirmDeleteAll() is used in the "Delete All" section of the "DoMaintenance" function. It 
 *	gives the operator the choice of confirming that all the selected files on the SD card
 *	should actually be deleted or to cancel the operation.
 *
 *	It returns 1 if the files are to actually be deleted, or zero if the operator selects
 *	the "Cancel" option.
 */

int ConfirmDeleteAll ()
{
	int		selection = 0;						// "Delete" or "Cancel"?

	tft.fillScreen ( BLACK );					// Fade to black

	PaintText ( 2, 20, 4, 0, WARNING, CENTER, "Really delete all files?" );

	return ConfirmAction ( "Delete" );
}												// End of "ConfirmDeleteAll()"


/*
 *	"ReadScanDataFile()" reads the data from a saved scan file on the SD card. It
 *
 *	The only input parameter is:
 *
 *		scanStruct*	newScan
 *
 *	It sets up the scan structure just as if it had been created by doing a "New Scan".
 *	The structure can be one used to do any of the "View Plot", "View Table" or "Overlay"
 *	functions.
 *	
 *	It returns the number of SWR/frequency pairs read from the file.
 */

int ReadScanDataFile ( scanStruct* readIt )
{
	char	temp;
	char	buff[10];			// Buffer to build SWR and frequency strings
	int		i;					// Buffer index
	int		index = 0;			// Number of SWR/frequency pairs read

	i = 0;

/*
 *	The function looks like it does the same thing twice, but not really. This section of
 *	the code reads the first line in the file, which contains the minimum and maximum
 *	frequencies for the saved scan. The second section handles the SWR/frequency pairs.
 */

	while ( root.available () )						// While the file is good to read
	{
    	buff[i] = root.read();						// Read the data one byte at a time

    	if ( buff[i] != ',' && buff[i] != '\n' )	// Look for comma separator or \n terminator
			i++;									// Nope, next character

		else										// Character is a comma or new-line
		{
			if (buff[i] == ',')								// If separator character
			{												// Buff contains minimum frequency
				buff[i] = '\0';								// Add a null terminator
				readIt->minFreq = 1000 * atof ( buff );		// Convert character string to floating point
				i = 0;										// Reset the buffer index
			}

			else											// Must have been a new-line
			{
				buff[i] = '\0';								// So add null terminator
				readIt->maxFreq = 1000 * atof ( buff );		// And set the maximum frequency
				i = 0;										// Reset the buffer index
        		break;
			}
		}
	}

	i = 0;											// Reset the buffer index

	while ( root.available () )						// As long as we don't run out of file
	{
		buff[i] = root.read ();						// Read next character

		if ( buff[i] != ',' && buff[i] != '\n' )	// Separator or new-line?
			i++;									// Nope, next character

		else										// It is a comma or new-line
		{
			if ( buff[i] == ',' )					// Is it a comma?
			{ 
				buff[i] = '\0';										// Yes, add null terminator
				readIt->swr[index] = ((float) atol ( buff )) / 100;	// SWR
        		i = 0;												// Reset buffer index
			}

			else												// Must have been a new-line
			{
				buff[i] = '\0';									// Add null terminator
				readIt->freq[index++] = 1000 * atol ( buff );	// Second half of the pair is a frequency
				i = 0;											// Reset buffer index
			}
		}
	}

	root.close();						// Close the file


/*
 *	Ok, we've read all the data from the file, now to finish setting up the scan structure
 *	so it looks like we just did a scan.
 *
 *	First, find the minimum SWR and the frequency where it occurred.
 */

	readIt->minSWR = 100.0;				// Arbitrary big number
	
	for ( i = 0; i < index; i++ )
	{
		if ( readIt->swr[i] < readIt->minSWR )
		{
			readIt->minSWR = readIt->swr[i];
			readIt->minIndex = i;
		}
	}
	return index;						// Return count of SWR/frequency pairs
}


/*
 *	"WriteScanData()" is used to write the results of the last scan performed to a new
 *	file on the SD card. It MUST be called after doing a new scan.
 *
 *	The only input parameter is a currently opened (for writing) file. It returns
 *	nothing.
 */

void WriteScanData ( File root )
{
	char		temp[10];
	int			i;
	uint32_t	xValMin, xValMax;				// Frequency range
	uint16_t	swr;							// Integer version of SWR
	uint32_t	freq;							// And frequency

	xValMin = scan.minFreq / 1000.0;			// Convert high and low frequencies to integers
	xValMax = scan.maxFreq / 1000.0;			// divided by 1000

	ltoa ( xValMin, temp, 10 );					// Convert minimum frequency to ASCII
	root.print ( temp );						// And write it to the file

	root.print ( "," );							// Write comma field separator

	ltoa( xValMax, temp, 10 );					// Convert maximum frequency to ASCII
	root.println ( temp );						// Write it to the file (with new-line terminator)

	for ( i = 0; i < MAX_PLOT_POINTS; i++ )		// Loop through the SWR/frequency data
	{
		swr = scan.swr[i] * 100;				// Integer version

		if ( scan.swr[i] == 0 )					// Skip zero SWR values??
			continue;

		utoa ( swr, temp, 10 );					// Convert SWR to ASCII

		root.print ( temp ); 					// Put it in the file

		root.print ( "," );						// Write comma field separator

		freq = scan.freq[i] / 1000;				// Integer version of frequency	
		ltoa (freq, temp, 10 );					// Convert frequency to ASCII
		root.println ( temp );					// Add data and newline
	}

	root.close();								// Close up shop
}


/*
 *	"Display_SD_Err()" is code that was originally inline in "setup()". In Version 01.1,
 *	I made it a separate function so that it can be used from other places in the
 *	program should a file system problem occur.
 *
 *	The parameter "text" contains the error message.
 *
 *	In a future version, I'll set some kind of flag in here that can be used to disable
 *	all the menu items that rely on the presence of the SD card.
 *
 *	It blanks the screen, displays the error message and pauses for 3 seconds to give
 *	the operator an opportunity to read it!
 *
 *	"Display_SD_Err_2()" and "Display_SD_Err_4()" display a set of 3 messages.
 */

void Display_SD_Err ( char* text )
{
	tft.fillScreen ( BLACK );							// Fade to black
	PaintText ( 2, SCREEN_C, 4, 0, WARNING, CENTER, text );
}


void Display_SD_Err_2 ()									// Specifically for error #2
{
	Display_SD_Err ( "There are no files on the SD Card!" );
	PaintText ( 2, SCREEN_C, 7, 0, WARNING, CENTER, "Function is inoperative!" );
	PaintText ( 2, SCREEN_C, 10, 0, NORMAL, CENTER, msgPushButton );
}


void Display_SD_Err_4 ()									// Specifically for error #4
{
		tft.fillScreen ( BLACK );							// Fade to black

		PaintText ( 2, SCREEN_C , 8, 0, ERR, CENTER, "File open error!" );
		PaintText ( 2, SCREEN_C, 10, 0, ERR, CENTER, "Is SD card inserted?" );
		PaintText ( 2, SCREEN_C, 13, 0, NORMAL, CENTER, msgPushButton );

		SD_Status = false;							// Card not mounted
}


/********************************** INTERRUPT RELATED FUNCTIONS *************************
 *
 *	The functions in this section all have to do with the processing of interrupts from the
 *	encoder or the "FineTune" button (if installed). Starting with Version 03.6, the "Fine Tune"
 *	button no longer uses interrupts.
 */

/*
 *	"ReadEncoder()" is the interrupt service routine for the rotary encoder. It is never
 *	called directly, but rather is invoked via the Arduino's interrupt handling mechanism
 *	whenever the encoder is moved.
 *
 *	It doesn't directly affect anything in the program, but simply sets the "encoderDirection"
 *	variable to indicate the last direction of rotation, and the "clickCount" variable which 
 *	tracks how many clicks have happened since it was last reset.
 *
 *	It should be noted, that the things that rely on reading the encoder within the rest of
 *	the program only do so when they are expecting it to move. That is to say, movements of
 *	the encoder when not expected are basically ignored. Code that looks for movement of the
 *	encoder sets the value of "encoderDirection" to zero, then waits to see if it moves, or
 *	if the encoder switch is pushed to move onto the next task to be performed.
 *
 *	This was modified in Version 01.2 to eliminate the redundent "aVal" variable and to add
 *	the "clickCount" variable. The "encoderDirection" variable now uses the standard values
 *	of "DIR_CW" and "DIR_CCW" defined in the "Rotary" library. Also added in Version 01.2
 *	is the "encoderReset()" function which will clear out both variables.
 *
 *	Note that not all functions in the rest of the program use the "clickCount".
 */

void ReadEncoder()
{
	unsigned char result = rotary.process();

	if (result == DIR_CW)
	{
		encoderDirection = DIR_CW;						// Which way the encoder moved
		clickCount++;									// Positive click
	}

	else if (result == DIR_CCW)
	{
		encoderDirection = DIR_CCW;						// Ditto
		clickCount--;									// Negative click
	}

	if ( clickCount == 0 )								// Net movement zero (can happen)
		encoderDirection = DIR_NONE;					// Then no movement
}


/*
 *	"ResetEncoder()" clears the "encoderDirection" and "clickCount" variables
 */

void ResetEncoder ()
{
	clickCount       = 0;
	encoderDirection = DIR_NONE;
}


/*
 *	"ReadFT()" gets called by those functions that allow the carat character to be moved when
 *	setting frequencies.
 *
 *	It first sets checks to see if "FT_Time" is zero or not. If it is zero, there is no
 *	pending interrupt to process. If it is non-zero, that means we saw an interrupt, but
 *	before we process it, we need to see if the debounce time (currently 50mS) has
 *	expired. If not, we just return.
 *
 *	If the time has expired, we process the interrupt.
 *
 *	Modified in Version 03.2 to also maintain the variables used in the frequency calibration
 *	function.
 *
 *	Modified in Version 03.6 to eliminate the use of interrupts from the "Fine Tune" button so
 *	we could differentiate between long and short button pushes. Short pushes are used to
 *	move the carat when adjusting numbers and long pushes are used to toggle the DDS on and off
 *	in the "Frequency" and "SWR Calibrate" functions.
 */

void ReadFT ()
{
	#if ( !defined ( FT_INSTALLED ))
		return;
	#endif

	if ( digitalRead ( FINE_TUNE_PIN ))			// If high, nothing to do
		return;
	
	FT_Time = millis ();						// Record start time
	delay ( 50 );								// Debounce time

	while ( !digitalRead ( FINE_TUNE_PIN ))	{}	// Wait for pin to go high again

	if (( millis() - FT_Time ) > 750 )			// Long button push
	{
		longFT_Interrupt = true;				// Indicate that
		FT_Time = 0;							// Clear time stamp
		return;									// And return
	}

	newFT_Interrupt = true;						// Log a new short push
	FT_pushCount++;								// And bump both click counters
	calPushCount++;
	
	FT_Time = 0;

/*
 *	We make a special case for 6 meters and the "Custom" band because of their range, we allow
 *	tuning at the MHz level. For all other bands, fine tuning is limited to KHz only.
 */

	if ( strcmp ( menuBands[activeBandIndex], STRING_6M ) == 0 ||
		 strcmp ( menuBands[activeBandIndex], STRING_CUSTOM ) == 0 )
		 	FT_pushCount &= 3;

	else if ( FT_pushCount > 2 )				// If it's more than 2
		FT_pushCount = 0;						// Set it back to 0

	calPushCount &= 3;							// Always 0 to 3 regardless of band
}


/*
 *	"ResetFT()" simply clears the interrupt indicators and the time.
 */

void ResetFT ()									// Resets the fine tune variables
{
	newFT_Interrupt  = false;					// Clear the short interrupt indicator
	longFT_Interrupt = false;					// Clear the long interrupt indicator
	FT_Time         	= 0;					// Clear the interrupt time
}


/********************************** MISCELLANEOUS FUNCTIONS ********************************
 *
 *	These really don't fit in any of the other catedories.
 */

/*
 *	"ConfirmAction()" is used whenever we need the operator to confirm that something like
 *	deleting a file or erasing the EEPROM should actually be performed or cancelled.
 *
 *	The parameter "action" is a string to be displayed on the "yes, do it" choice (the other
 *	choice will always be "Cancel"). It returns 0 if the action is to be cancelled, and 1 if
 *	the action is to be performed.
 */

 int ConfirmAction ( char* action )
 {
 	int	selection = 0;							// 0 for cancel, 1 for do it
 	
	PaintText ( 2, 19, 6, 0, NORMAL,    RIGHT, action );
	PaintText ( 2, 20, 6, 0, HIGHLIGHT, LEFT,  "Cancel" );

	ResetEncoder ();							// Clear the encoder flags

	while ( digitalRead ( SWITCH ))				// Do until button operated
	{
		selection &= 1;							// Only options are 0 and 1

		if ( encoderDirection != DIR_NONE )		// Encoder moved?
		{
			selection++;						// Direction doesn't matter, only 2 choices
			selection &= 1;						// Only 0 or 1

			EraseText ( 2, SCREEN_C, 6, 0, BLACK, CENTER, 22 );

			if ( selection )					// Selection = 1
			{
				PaintText ( 2, 19, 6, 0, HIGHLIGHT, RIGHT, action );
				PaintText ( 2, 20, 6, 0, NORMAL,    LEFT,  "Cancel" );
			}

			else
			{
				PaintText ( 2, 19, 6, 0, NORMAL,    RIGHT, action );
				PaintText ( 2, 20, 6, 0, HIGHLIGHT, LEFT,  "Cancel" );
			}

			ResetEncoder ();					// Clear encoder flags
		}
	}

	return selection;
}


/*
 *	"DisplayScanStruct()" is a debugging tool. It shows the entire contents of the "scan"
 *	structure on the serial monitor. The only parameters are a scan structure, which could either
 *	be the main "scan" structure, or the "ovly" structure and a title to be displayed.
 */

void DisplayScanStruct ( scanStruct debugStruct, char* Title )
{
	Serial.println ();
	Serial.println ( Title );
	
	Serial.print ( F ( "\nxOrigin    =  " ) );
	Serial.println ( debugStruct.xOrigin );
	
	Serial.print ( F ( "yOrigin     = " ) );
	Serial.println ( debugStruct.yOrigin );
	
	Serial.print ( F ( "graphWidth  = " ) );
	Serial.println ( debugStruct.graphWidth );
	
	Serial.print ( F ( "graphHeight = " ) );
	Serial.println (debugStruct.graphHeight );

	Serial.print ( F ( "minFreq     = " ) );
	Serial.println ( debugStruct.minFreq );
	
	Serial.print ( F ( "maxFreq     = " ) );
	Serial.println ( debugStruct.maxFreq );
	
	Serial.print ( F ( "minSWR      = " ) );
	Serial.println ( debugStruct.minSWR );
	
	Serial.print ( F ( "maxSWR      = " ) );
	Serial.println ( debugStruct.maxSWR );

	Serial.print ( F ( "minIndex    = " ) );
	Serial.println ( debugStruct.minIndex );

	Serial.print ( F ( "xPos        = " ) );
	Serial.println ( debugStruct.xPos );
	
	Serial.print ( F ( "yPos        = " ) );
	Serial.println ( debugStruct.yPos );
	
	Serial.print ( F ( "lastX       = " ) );
	Serial.println ( debugStruct.lastX );
	
	Serial.print ( F ( "lastY       = " ) );
	Serial.println ( debugStruct.lastY );
	
	Serial.print ( F ( "deltaX      = " ) );
	Serial.println ( debugStruct.deltaX );

	Serial.print ( F ( "isValid     = " ) );
	Serial.println ( debugStruct.isValid );

	Serial.print ( F ( "fileName    = " ) );
	Serial.println ( debugStruct.fileName );
}


/*
 *	"ShowDebugMode()" was added in Version 03.4 as part of the conversion to dynamic debugging.
 *	It displays a red "DB" in the upper right hand corner of the screen if dynamic debugging is
 *	turned on.
 */

void ShowDebugMode ()
{
	if ( optDebugMode )
		PaintText ( 2, SCREEN_R, 0, 2, ERR, RIGHT, "DB" );
	else
		EraseText ( 2, SCREEN_R, 0, 2, BLACK, RIGHT, 2 );
}


/*
 *	"BatteryCheck" is added in Version 03.8. This is  functional code, but might
 *	need to be modified to suit the hardware configuration of your particular unit.
 *	Detailed instructions of how to do it are found in the "Hacker's Guide".
 *
 *	Note that the function doesn't even get compiled unless the "DO_BATT_CK" symbol
 *	is defined in the header file
 */

#if ( defined ( DO_BATT_CK ))			// If battery check is enabled

void BatteryCheck ()					// See if we're good still
{
uint16_t voltage;							// Current battery voltage

	voltage = analogRead ( BATT_CHECK_PIN );

	if ( voltage > LOW_BATTERY )
		return;

	DEBUG_BEGIN
		Serial.print ( "Battery Voltage = " );
		Serial.println ( voltage );
	DEBUG_END

	while ( true )
	{
		tft.fillScreen ( BLACK );				// Fade to black
		PaintText ( 4, SCREEN_C, 4, 0, ERR, CENTER, "Low Battery!" );
		delay ( 1000 );
		tft.fillScreen ( BLACK );				// Fade to black
		delay ( 1000 );
	}


}

#endif
