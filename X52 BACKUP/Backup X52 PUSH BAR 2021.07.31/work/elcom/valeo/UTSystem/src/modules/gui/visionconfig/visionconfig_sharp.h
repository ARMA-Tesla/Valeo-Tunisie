#if !defined(__VISIONCONFIG_SHARP_H__)
#define __VISIONCONFIG_SHARP_H__

SElExceptionPtr	OpenSharpCamera(void);
SElExceptionPtr	CloseSharpCamera(void);

SElExceptionPtr CameraSharpStart(void);
SElExceptionPtr CameraSharpStop(void);

#endif
