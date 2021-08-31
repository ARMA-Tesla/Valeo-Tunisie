#include <stdlib.h>
#include <stdio.h>
#include <userint.h>
#include "niDMMErrorHandler.h"


void ErrorHandler(ViSession session)
{
   ViChar *buffer = VI_NULL;
   ViChar *pos = VI_NULL;
   ViStatus errorCode = VI_SUCCESS;
   ViInt32 bufferSize = 0;

   // Call niDMM_GetError, pass VI_NULL for the buffer in order to retrieve
   // the length of the error message.
   bufferSize = niDMM_GetError(session, &errorCode, 0, VI_NULL);
   
   if (bufferSize > 0)
   {
      // Allocate a buffer to store the error message.
      // The additional 256 bytes are for the static text before the description.
      pos = buffer = malloc(bufferSize + 256);
      pos += sprintf(pos, "Code:%d (0x%X)\n", errorCode, errorCode);
      niDMM_GetError(session, &errorCode, bufferSize, pos);
   }
   else
   {
      // A return code from niDMM_GetError() <= 0 indicates a problem.
      // This is expected when the session is invalid (IVI spec requires
      // niDMM_GetError() to fail).
      // If that happens, use niDMM_GetErrorMessage to retrieve the error message
      // for the returned error code.
      errorCode = bufferSize;

      // Call niDMM_GetErrorMessage, pass VI_NULL for the buffer in order to retrieve
      // the length of the error message.
      bufferSize = niDMM_GetErrorMessage(VI_NULL, errorCode, 0, VI_NULL);
      pos = buffer = (ViChar *)malloc(bufferSize);
     
      // Get the error description
      niDMM_GetErrorMessage(VI_NULL, errorCode, bufferSize, pos);
   }
   
   MessagePopup("Error!", buffer);
   free(buffer);
}
