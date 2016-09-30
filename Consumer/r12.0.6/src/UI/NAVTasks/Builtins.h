// Builtins.h : implementation file
//

#ifndef __Builtins_h__
#define __Builtins_h__

extern bool Computer_Load(IScanTask* pThis);
extern bool HardDisks_Load(IScanTask* pThis);
extern bool RemovableDrives_Load(IScanTask* pThis);
extern bool Floppy_Load(IScanTask* pThis);
extern bool Folders_Load(IScanTask* pThis);
extern bool Files_Load(IScanTask* pThis);
extern bool Drives_Load(IScanTask* pThis);

#endif __Builtins_h__
