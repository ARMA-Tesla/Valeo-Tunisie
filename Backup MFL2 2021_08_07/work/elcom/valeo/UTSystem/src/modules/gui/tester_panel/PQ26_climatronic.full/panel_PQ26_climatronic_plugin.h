#include <plugins/el_plugin.h>
#include <guid.h>

#if defined(WINDOWS)
	#include <windows.h>
#endif

DEFINE_PLUGIN_EXPORTS();

DEFINE_PLUGIN_INFO( 1,							/*!< The build number. */
					1,							/*!< The major version (e.g. 1.xx).	*/
					0,							/*!< The minor version (e.g. 0.10). */
					TRUE,						/*!< Does this plugin show its arguments to the public? */
					"PQ26 CLIMATRONIC GUI",	  	/*!< The plugin's name. */
					"LARISYSNA",				/*!< The plugin's vendor. */
					"Air-condition control panel",	/*!< The plugin's general description. */
					"",							/*!< The plugin's additional description. */
					"www.larisys.com", 			/*!< The plugin vendor's homepage. */
					"araibia.mohamed@larisysna.com",	/*!< The plugin vendor's email address. */
					PLUGIN_PANEL_PQ26_CLIMATRONIC,    	/*!< The plugin's UUID. */
					PLUGIN_TESTER				/*!< Target */
                    );				            
