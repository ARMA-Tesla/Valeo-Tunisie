#include <plugins/el_plugin.h>
#include <guid.h>

#if defined(WINDOWS)
	#include <windows.h>
#endif

DEFINE_PLUGIN_EXPORTS();


DEFINE_PLUGIN_INFO( 1,									/*!< The build number. */
					1,									/*!< The major version (e.g. 1.xx).	*/
					20,									/*!< The minor version (e.g. 0.10). */
					TRUE,								/*!< Does this plugin show its arguments to the public? */
					"Test Sequence",  					/*!< The plugin's name. */
					"AE, s.a.s.",						/*!< The plugin's vendor. */
					"Test Sequence for CMFB PIANO",		/*!< The plugin's general description. */
					"",									/*!< The plugin's additional description. */
					"http://ae.italy.it",		      	/*!< The plugin vendor's homepage. */
					"araibia@ae-italy.it",				/*!< The plugin vendor's email address. */
					PLUGIN_TEST_CMFB_PIANO,            	/*!< The plugin's UUID. */
					PLUGIN_AUTOMAT						/*!< Target */
                    );				            
