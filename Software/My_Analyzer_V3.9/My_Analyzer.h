/*
 * 	My_Analyzer.h
 *
 *	This source file is under General Public License version 3. Please feel free to
 *	distribute it, hack it, or do anything else you like to it. I would ask, however
 *	that is you make any cool improvements, you let me know via any of the websites
 *	on which I have published this or by email at: WA2FZW@ARRL.net.
 *	
 *	This header file is used by the W8TEE/K2ZIA Antenna Analyzer in conjunction with
 *	the upgraded software by WA2FZW, with much help from G3ZQC, PE1PWF, K2RH and VK3PE.
 *	The definitions in here include the definitions of things needed to convert from
 *	the AD9850 DDS to the AD9851 DDS and things that allow the 6 meter band and "Custom"
 *	(i.e. any frequency range the owner wants) to be added or not.
 *
 *	In Version 02.5, we add support for Glenn's (VK3PE) AD8307 detector circuit in
 *	place of the germanium diodes.
 */

#ifndef My_Analyzer_h		// Prevent double #include
#define My_Analyzer_h

/*
 *	Include all the necessary libraries. For the non-standard ones, use the links provided to
 *	find the ones that work with this code, as there are some different versions with identical
 *	or similar names that might not work correctly.
 *
 *	Note a slight changes in Version 03.3; The "AD985XSPI" library replaces the "AD9850SPI"
 *	library.
 */

#include <AD985XSPI.h>			// https://groups.io/g/SoftwareControlledHamRadio/files/?ord=created&d=desc
#include <SPI.h>				// Use the standard IDE library instead
#include <Adafruit_GFX.h>		// https://github.com/adafruit/Adafruit-GFX-Library
#include <MCUFRIEND_kbv.h>		// https://github.com/prenticedavid/MCUFRIEND_kbv
#include <Rotary.h>				// https://github.com/brianlow/Rotary
#include <EEPROM.h>				// Standard with IDE
#include <SD.h>					// Standard with IDE
#include <Wire.h>				// Standard with IDE
#include <math.h>				// Standard with IDE


/*
 *	There are three optional configurations of the analyzer now. The original design uses
 *	the AD9850 DDS (Type II) board. Subsequent modifications were made to allow use of the
 *	AD9851 DDS, and most recently, the addition of support for the VK3PE AD8307 log
 *	amplifiers.
 *
 *	Before compiling for your particular analyzer, you must un-comment the #define statement
 *	for either "AD9850_DDS" or "AD9851_DDS", whichever applies to the DDS module that you are
 *	using.
 */

//	#define AD9850_DDS				// Un-comment the line that defines which DDS module you have
	#define AD9851_DDS				// And comment out the other one

/*
 *	If you have installed Edwin's (PE1PWF) or Glenn's (VK3PE) modifications uncomment the
 *	appropiate definition. Don't uncommentboth of them!
 */

//	#define AD8307_SWR				// Using AD8307 detectors
	#define	PE1PWF_MOD				// Enable if Edwin's hardware mods are installed


/*
 *	Added in Version 03.3, we define 2 DDS calibration factors; one for the AD9850 module and
 *	one for the AD9851 module. The normal calibration factors for these are 125MHz and 180MHz
 *	respectively, however we already know there are a few AD9851 modules in use that have an
 *	issue with operating at 180MHz and will only work if the calibration is set to 120MHz. If
 *	this applies to your DDS, change the definition of "CAL_9851" to "120000000UL".
 *
 *			DO NOT; I REPEAT, DO NOT COMMENT OUT EITHER OF THESE LLINES
 */

#define	CAL_9850	125000000UL
#define	CAL_9851	180000000UL



/*
 *	Definitions of the Arduino pins used for the encoder and its built-in switch. I moved
 *	these here from the main program in Version 03.2 to make them easier to find.
 *
 *	If you find that your particular encoder works backwards, simply flip the definitions
 *	of PIN_A and PIN_B and all will be well. Many of the encoders on eBay are wired backwards,
 *	as are the dozen or so I have in my parts collection.
 */

#define PIN_A		18							// Encoder hookup; reverse A and B if
#define PIN_B		19							// it works backwards
#define SWITCH      20


/*
 *	The new code has all of the standard HF ham bands (160 meters through 10
 *	meters) permanently built in. However, if your analyzer uses the AD9851 DDS
 *	module, it is possible to add the 6 meter band as well, by enabling the
 *	definition of "ADD_6_METERS". Note, however that this is conditionalized
 *	upon whether or not "AD9851_DDS" is defined as 6 meters won't really work
 *	well without the higher frequency DDS.
 *
 *	If "ADD_6_METERS" is defined, you can also set the values of "LOW_6M_EDGE"
 *	and "HIGH_6M_EDGE" to values other than 50MHz to 54MHz as more useful
 *	limits might be more appropriate for what you do on 6 meters.
 */

#ifdef AD9851_DDS					// You can only add 6 meters if you have the AD9851

#define ADD_6_METERS				// Un-comment to enable 6 meter operation

	#define	LOW_6M_EDGE		50000U	// 50 MHz / 1000
	#define	HIGH_6M_EDGE	54000U	// 54 MHz / 1000

#endif


/*
 *	Regardless of which DDS module you have in your analyzer, you can add a "Custom"
 *	band definition. And as was the case with the inclusion of the 6 meter band, you
 *	can define the upper and lower limits however you like. We have them set for a
 *	range of 1MHz to 30MHz as the default, but be warned, that you're not going to
 *	see a lot of bandwidth detail on a graph of the SWR values (or maybe no dips at
 *	all) with that wide a range. Also if using the single frequency SWR option, it
 *	may take a few million turns of the encoder to get to where you want!
 *
 *	There is a more practical use for this option, however. If you're working with
 *	some kind of multiband antenna such as a tribander, you might want to set the
 *	band limits to 14MHz and 29.7MHz.
 *
 *	VERY IMPORTANT:	Be aware that when you use this feature to perform a full scan,
 *					you will be transmitting a low power signal across all frequencies
 *					between the band limits.
 */

	#define ADD_CUSTOM					// Un-comment to enable the "Custom" band

	#ifdef	ADD_CUSTOM					// If "Custom" is enabled

		#define LOW_C_EDGE	   100U		// Changed to 100KHz in Version 03.2
		#define	HIGH_C_EDGE	 65500U		// 65.5MHz / 1000

	#endif

/*
 *	These definitions are added in Version 02.8. They are used in the "RepeatScan()" and
 *	"DoNewScan()" functions.
 */

#define	DEFAULT_COUNT		 50				// Initial default repeat count
#define	REPEAT_INCREMENT	 10				// How much to change the count for each encoder click
#define	MIN_REPEAT_COUNT	 10				// Minimum repetition count (don't set any lower than 10)
#define	MAX_REPEAT_COUNT	100				// Maximum repetition count

#define	REPEAT_SCAN			  1				// For "DoNewScan()" - In repeat mode
#define	SINGLE_SCAN			  0				// For "DoNewScan()" - In single scan mode


/*
 *	Modified in Version 3.9
 *
 *		"SCAN_PAUSE" is now the default delay (in seconds) between repeated scans.
 *		"MAX_SCAN_PAUSE" is the maximum delay (in seconds) allowed. Obviously, the
 *		minimum will be '1'.
 *
 *		The code now allows the user to set the delay as opposed to it being a fixed
 *		value in previous releases.
 */

#define SCAN_PAUSE			 1				// Length of pause between scans
#define MAX_SCAN_PAUSE		60				// MAximum pause between scans


/*
 *	A slight modification in Version 02.9. Instead of the cute trick of using 2 pins tied
 *	together on the Arduino to detect the presence of the "Fine Tune" button, the folowing
 *	definition will tell the software whether or not it is installed. We discovered that for
 *	some reason, the button was drawing 80mA in the original configuration. That's twice
 *	the rating for a pin on the Arduino. Without the pins tied together, it draws about 10mA.
 *
 *	Comment out the following definition if you do not have the "Fine Tune" button installed.
 */

	#define	FT_INSTALLED


/*
 *	Un-commenting the following definition will allow the "Examine" feature to start immediately
 *	after doing or viewing a scan as opposed to requiring a movement of the encoder to start it
 *	going.
 */

	#define AUTO_EXAMINE


/*
 *	The following definition allows a horizontal index line to be displayed in the "Examine"
 *	feature. If you don't want to see the horizontal index, comment it out. 
 */

	#define HORIZ_INDEX


/*
 *	The definition of "LABEL_SCAN" was supposed to be added here in Version 03.2 to turn
 *	the labeling of the scan graphs on or off, but somehow I forgot it! It's added in
 *	Version 03.3. If you don't like the labels, comment it out.
 */

	#define LABEL_SCAN


/*
 *	New in Version 03.5, we give you the option to skip setting the band and frequency range
 *	on startup UNLESS the information is not already stored in the EEPROM. So if you've erased
 *	the EEPROM contents, you will still be prompted for the band and frequency information at
 *	startup regardless of whether or not you've enabled the option. If you want to be required
 *	to set the band and range at startup, comment the following definition out:
 */

	#define	SKIP_BAND_SELECT


/*
 *	Added in Version 03.6 is the option to display the forward and reverse Arduino pin
 *	readings on the "Frequency" and the "SWR Calibration" displays. Also by pressing the
 *	"Fine Tune" button for more than one second, the DDS can be toggled on and off when
 *	using these functions.
 *
 *	If you want to enable this option, un-comment the following definition:
 */

	#define	VIEW_PIN_DATA


/*
 *	Added in Version 03.7.3. The following definitions are associated with the prototype
 *	function to do a "BatteryCheck".
 *
 *	Please note that the code associated with this in the .ino file is not intended to
 *	actually work as-is, but rather it is included to give folks an idea of how to
 *	implement the capability. Because there are so many different ways that folks are
 *	powering the AA and the different configurations might start to fail at different
 *	voltage levels, it is impossible for me to implement a standard solution.
 */

//	#define	DO_BATT_CK					// Un-comment if you want to use the capability

#define	BATT_CHECK_PIN	 A0				// Or whichever analog pin you're using
#define	LOW_BATTERY		512				// Pin reading that will trigger the warning
										// Up to you to determine the right value

#endif
