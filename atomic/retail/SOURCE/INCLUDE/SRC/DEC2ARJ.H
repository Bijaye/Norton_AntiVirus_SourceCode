#ifndef _DEC2ARJ_H_
#define _DEC2ARJ_H_

#define DEC_TYPE_ARJ    'ARJ'


// Exported functions.
#ifdef DEC2ARJ_CPP
#define DEC2ARJ_IE __declspec(dllexport) 
#else
#define DEC2ARJ_IE __declspec(dllimport) 
#endif

DEC2ARJ_IE HRESULT DecNewARJEngine(IDecEngine** ppEngine);
DEC2ARJ_IE HRESULT DecDeleteARJEngine(IDecEngine* pEngine);

#endif