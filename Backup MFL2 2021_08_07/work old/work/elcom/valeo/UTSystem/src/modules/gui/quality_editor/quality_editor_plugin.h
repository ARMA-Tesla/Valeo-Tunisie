#include <plugins/el_plugin.h>
#include <guid.h>

#if defined(WINDOWS)
	#include <windows.h>
#endif

DEFINE_PLUGIN_EXPORTS();


DEFINE_PLUGIN_INFO( 1,									/*!< The build number. */
					1,									/*!< The major version (e.g. 1.xx).	*/
					0,									/*!< The minor version (e.g. 0.10). */
					TRUE,								/*!< Does this plugin show its arguments to the public? */
					"Quality Editor",	  				/*!< The plugin's name. */
					"ELCOM, a.s.",						/*!< The plugin's vendor. */
					"Quality panel",					/*!< The plugin's general description. */
					"",									/*!< The plugin's additional description. */
					"http://dvi.elcom.cz",		      	/*!< The plugin vendor's homepage. */
					"zdenek.rykala@elcom.cz",			/*!< The plugin vendor's email address. */
					PLUGIN_QUALITY_EDITOR,              /*!< The plugin's UUID. */
					""									/*!< Target */
                    );				            
